// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// USkillSystem.cpp
//
// Implements gesture → star-gate → cooldown → success-roll → montage pipeline.
// GDD Section 4.1–4.5

#include "USkillSystem.h"
#include "MGestureRecognizer.h"
#include "MSoccerPlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
USkillSystem::USkillSystem()
    : bAnySkillExecuting(false)
    , ActiveSkillID(ESkillID::None)
    , PlayerSkillStars(3)
    , DefenderInfluenceRadius(150.0f)
{
    PrimaryComponentTick.bCanEverTick = true;
}

// ---------------------------------------------------------------------------
// BeginPlay
// ---------------------------------------------------------------------------
void USkillSystem::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<AMSoccerPlayerCharacter>(GetOwner());

    // Create gesture recognizer
    GestureRecognizer = NewObject<UMGestureRecognizer>(this);

    // Bind gesture events
    GestureRecognizer->OnGestureRecognized.AddDynamic(
        this, &USkillSystem::HandleGestureRecognized_Internal);

    // Build fallback skill definitions for 5 starter skills
    InitDefaultSkillData();
}

// ---------------------------------------------------------------------------
// TickComponent — poll cooldown updates
// ---------------------------------------------------------------------------
void USkillSystem::TickComponent(float DeltaTime, ELevelTick TickType,
                                  FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!OwnerCharacter) return;

    BroadcastCooldowns(GetWorld()->GetTimeSeconds());
}

// ---------------------------------------------------------------------------
// FeedJoystickSample
// ---------------------------------------------------------------------------
void USkillSystem::FeedJoystickSample(const FGestureSample& Sample)
{
    if (GestureRecognizer && !bAnySkillExecuting)
    {
        GestureRecognizer->AddSample(Sample);
    }
}

// ---------------------------------------------------------------------------
// InjectShootCancelGesture (FakeShot)
// ---------------------------------------------------------------------------
void USkillSystem::InjectShootCancelGesture()
{
    if (GestureRecognizer && !bAnySkillExecuting)
    {
        GestureRecognizer->InjectDiscreteGesture(EGestureType::ShootCancel);
    }
}

// ---------------------------------------------------------------------------
// Internal gesture callback (bound to GestureRecognizer.OnGestureRecognized)
// ---------------------------------------------------------------------------
void USkillSystem::HandleGestureRecognized_Internal(EGestureType Gesture)
{
    const ESkillID SkillID = GestureToSkill(Gesture);
    if (SkillID != ESkillID::None)
    {
        TryExecuteSkill(SkillID);
    }
}

// ---------------------------------------------------------------------------
// TryExecuteSkill — main pipeline
// ---------------------------------------------------------------------------
ESkillExecutionResult USkillSystem::TryExecuteSkill(ESkillID SkillID)
{
    FSkillExecutionEvent Event;
    Event.SkillID            = SkillID;
    Event.ExecutionTimestamp = GetWorld()->GetTimeSeconds();

    // --- 1. Possession check ------------------------------------------------
    if (!OwnerCharacter || !OwnerCharacter->HasBallPossession())
    {
        Event.Result = ESkillExecutionResult::NoBallPossession;
        OnSkillExecuted.Broadcast(Event);
        return Event.Result;
    }

    // --- 2. Already executing check -----------------------------------------
    if (bAnySkillExecuting)
    {
        Event.Result = ESkillExecutionResult::OnCooldown;
        OnSkillExecuted.Broadcast(Event);
        return Event.Result;
    }

    // --- 3. Load skill definition -------------------------------------------
    bool bFound = false;
    const FSkillDefinition Def = GetSkillDefinition(SkillID, bFound);
    if (!bFound)
    {
        Event.Result = ESkillExecutionResult::NotRecognized;
        OnSkillExecuted.Broadcast(Event);
        return Event.Result;
    }

    // --- 4. Star gate check -------------------------------------------------
    if (!CheckStarGate(Def))
    {
        Event.Result = ESkillExecutionResult::StarGateFailed;
        OnSkillExecuted.Broadcast(Event);
        UE_LOG(LogTemp, Warning,
            TEXT("[USkillSystem] Skill %s blocked: requires %d★, player has %d★"),
            *Def.DisplayName, Def.RequiredStars, PlayerSkillStars);
        return Event.Result;
    }

    // --- 5. Cooldown check --------------------------------------------------
    if (!CheckCooldown(SkillID, Def))
    {
        Event.Result = ESkillExecutionResult::OnCooldown;
        Event.bExecutionSucceeded = false;
        OnSkillExecuted.Broadcast(Event);
        return Event.Result;
    }

    // --- 6. Movement check (if required) ------------------------------------
    if (Def.bRequiresMovement && OwnerCharacter)
    {
        if (OwnerCharacter->GetVelocity().Size2D() < 50.0f)
        {
            Event.Result = ESkillExecutionResult::Blocked;
            OnSkillExecuted.Broadcast(Event);
            return Event.Result;
        }
    }

    // --- 7. Success chance roll (modified by defender proximity) -----------
    const float SuccessChance = ComputeSuccessChance(Def);
    const bool  bSuccess      = (FMath::FRand() <= SuccessChance);

    // --- 8. Record execution time (start cooldown) -------------------------
    FSkillRuntimeState& State = GetOrCreateState(SkillID);
    State.LastExecutedTime    = GetWorld()->GetTimeSeconds();
    State.bIsExecuting        = true;

    if (bSuccess)
    {
        State.SuccessCount++;
        ActiveSkillID      = SkillID;
        bAnySkillExecuting = true;

        PlaySkillMontage(SkillID, Def);
        OnSkillSucceeded(SkillID);

        Event.Result             = ESkillExecutionResult::Success;
        Event.bExecutionSucceeded = true;
    }
    else
    {
        // Partial cooldown on failure (half)
        State.LastExecutedTime = GetWorld()->GetTimeSeconds() - (Def.CooldownSeconds * 0.5f);
        State.bIsExecuting     = false;

        OnSkillFailed(SkillID);
        Event.Result              = ESkillExecutionResult::Blocked;
        Event.bExecutionSucceeded = false;
    }

    OnSkillExecuted.Broadcast(Event);
    return Event.Result;
}

// ---------------------------------------------------------------------------
// IsSkillOnCooldown / GetCooldownRemaining
// ---------------------------------------------------------------------------
bool USkillSystem::IsSkillOnCooldown(ESkillID SkillID) const
{
    return GetCooldownRemaining(SkillID) > 0.0f;
}

float USkillSystem::GetCooldownRemaining(ESkillID SkillID) const
{
    const uint8 Key = static_cast<uint8>(SkillID);
    const FSkillRuntimeState* StatePtr = SkillStates.Find(Key);
    if (!StatePtr) return 0.0f;

    bool bFound = false;
    const FSkillDefinition Def = GetSkillDefinition(SkillID, bFound);
    if (!bFound) return 0.0f;

    const float Elapsed = GetWorld()->GetTimeSeconds() - StatePtr->LastExecutedTime;
    return FMath::Max(0.0f, Def.CooldownSeconds - Elapsed);
}

// ---------------------------------------------------------------------------
// GetSkillDefinition — DataTable lookup with fallback
// ---------------------------------------------------------------------------
FSkillDefinition USkillSystem::GetSkillDefinition(ESkillID SkillID, bool& bFound) const
{
    // Try DataTable first
    if (SkillDataTable)
    {
        const FString RowName = UEnum::GetValueAsString(SkillID);
        const FSkillDefinition* Row =
            SkillDataTable->FindRow<FSkillDefinition>(FName(*RowName), TEXT(""));
        if (Row)
        {
            bFound = true;
            return *Row;
        }
    }

    // Fallback to hardcoded defaults
    const FSkillDefinition* Fallback = FallbackSkillDefs.Find(SkillID);
    if (Fallback)
    {
        bFound = true;
        return *Fallback;
    }

    bFound = false;
    return FSkillDefinition();
}

// ---------------------------------------------------------------------------
void USkillSystem::SetPlayerSkillStars(int32 Stars)
{
    PlayerSkillStars = FMath::Clamp(Stars, 1, 5);
}

// ---------------------------------------------------------------------------
// OnSkillMontageEnded — called from BP AnimNotify or anim finished callback
// ---------------------------------------------------------------------------
void USkillSystem::OnSkillMontageEnded(ESkillID FinishedSkill)
{
    if (ActiveSkillID == FinishedSkill)
    {
        bAnySkillExecuting = false;
        ActiveSkillID      = ESkillID::None;
    }

    FSkillRuntimeState& State = GetOrCreateState(FinishedSkill);
    State.bIsExecuting = false;
}

// ===========================================================================
// BLUEPRINT NATIVE EVENT DEFAULTS
// ===========================================================================

void USkillSystem::OnSkillSucceeded_Implementation(ESkillID SkillID)
{
    UE_LOG(LogTemp, Log, TEXT("[USkillSystem] Skill SUCCEEDED: %s"),
        *UEnum::GetValueAsString(SkillID));
    // TODO Milestone 3: apply speed burst, evade physics, unlock defender
}

void USkillSystem::OnSkillFailed_Implementation(ESkillID SkillID)
{
    UE_LOG(LogTemp, Log, TEXT("[USkillSystem] Skill FAILED: %s"),
        *UEnum::GetValueAsString(SkillID));
    // TODO Milestone 3: stumble anim, brief speed penalty
}

// ===========================================================================
// PRIVATE HELPERS
// ===========================================================================

// Gesture → Skill mapping (GDD Section 4.2)
ESkillID USkillSystem::GestureToSkill(EGestureType Gesture) const
{
    switch (Gesture)
    {
        case EGestureType::Circle:           return ESkillID::StepOver;
        case EGestureType::Spin360:          return ESkillID::Roulette;
        case EGestureType::HoldSideways:     return ESkillID::BallRoll;
        case EGestureType::ShootCancel:      return ESkillID::FakeShot;
        case EGestureType::FlickAtDefender:  return ESkillID::Nutmeg;
        case EGestureType::DoubleCircle:     return ESkillID::DoubleStepOver;
        case EGestureType::BackwardFlick:    return ESkillID::HeelToHeel;
        case EGestureType::HorizontalSwipe:  return ESkillID::LaCroqueta;
        case EGestureType::SwipeUpDouble:    return ESkillID::SombreroFlick;
        default:                             return ESkillID::None;
    }
}

bool USkillSystem::CheckStarGate(const FSkillDefinition& Def) const
{
    return PlayerSkillStars >= Def.RequiredStars;
}

bool USkillSystem::CheckCooldown(ESkillID SkillID, const FSkillDefinition& Def) const
{
    return GetCooldownRemaining(SkillID) <= 0.0f;
}

float USkillSystem::ComputeSuccessChance(const FSkillDefinition& Def) const
{
    float Chance = Def.BaseSuccessChance;

    // Defender proximity penalty: within DefenderInfluenceRadius → halve success
    const float NearestDist = FindNearestDefenderDistance();
    if (NearestDist < DefenderInfluenceRadius)
    {
        const float Penalty = FMath::Lerp(0.5f, 0.0f,
            NearestDist / DefenderInfluenceRadius);
        Chance *= (1.0f - Penalty);
    }

    // Star bonus: each star above minimum adds 3%
    bool bFound = false;
    const int32 ExtraStars = PlayerSkillStars - Def.RequiredStars;
    Chance += ExtraStars * 0.03f;

    return FMath::Clamp(Chance, 0.0f, 1.0f);
}

float USkillSystem::FindNearestDefenderDistance() const
{
    // TODO Milestone 3: query AISightPerceptionComponent for nearest opponent
    // Placeholder: return large distance (no penalty) until AI is online
    return 999.0f;
}

void USkillSystem::PlaySkillMontage(ESkillID SkillID, const FSkillDefinition& Def)
{
    TObjectPtr<UAnimMontage>* MontagePtr = SkillMontages.Find(SkillID);
    if (!MontagePtr || !(*MontagePtr)) return;

    if (UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance())
    {
        AnimInst->Montage_Play(*MontagePtr);
    }
}

void USkillSystem::BroadcastCooldowns(float CurrentTime)
{
    for (auto& KV : SkillStates)
    {
        const ESkillID SkillID = static_cast<ESkillID>(KV.Key);
        bool bFound = false;
        const FSkillDefinition Def = GetSkillDefinition(SkillID, bFound);
        if (!bFound) continue;

        const float Remaining = FMath::Max(0.0f,
            Def.CooldownSeconds - (CurrentTime - KV.Value.LastExecutedTime));

        if (Remaining > 0.0f)
        {
            OnSkillCooldownUpdated.Broadcast(SkillID, Remaining);
        }
    }
}

FSkillRuntimeState& USkillSystem::GetOrCreateState(ESkillID SkillID)
{
    const uint8 Key = static_cast<uint8>(SkillID);
    if (!SkillStates.Contains(Key))
    {
        SkillStates.Add(Key, FSkillRuntimeState());
    }
    return SkillStates[Key];
}

// ---------------------------------------------------------------------------
// InitDefaultSkillData — 5 starter skills (GDD Table 4.2)
// Used as fallback when no DataTable is assigned.
// ---------------------------------------------------------------------------
void USkillSystem::InitDefaultSkillData()
{
    auto Add = [&](ESkillID ID, FString Name, int32 Stars,
                   EGestureType Gesture, float Cooldown, float SuccessChance)
    {
        FSkillDefinition Def;
        Def.SkillID          = ID;
        Def.DisplayName      = Name;
        Def.RequiredStars    = Stars;
        Def.RequiredGesture  = Gesture;
        Def.CooldownSeconds  = Cooldown;
        Def.BaseSuccessChance = SuccessChance;
        Def.MontageSlot      = TEXT("UpperBody");
        FallbackSkillDefs.Add(ID, Def);
    };

    //          ID                    DisplayName        Stars  Gesture                        CD    Chance
    Add(ESkillID::StepOver,    TEXT("Step Over"),        3,  EGestureType::Circle,          3.0f, 0.85f);
    Add(ESkillID::BallRoll,    TEXT("Ball Roll"),         3,  EGestureType::HoldSideways,    2.5f, 0.90f);
    Add(ESkillID::FakeShot,    TEXT("Fake Shot"),         2,  EGestureType::ShootCancel,     2.0f, 0.92f);
    Add(ESkillID::Nutmeg,      TEXT("Nutmeg"),            3,  EGestureType::FlickAtDefender, 4.0f, 0.55f);
    Add(ESkillID::Roulette,    TEXT("Roulette"),          4,  EGestureType::Spin360,         3.5f, 0.78f);
}

// NOTE: The private method HandleGestureRecognized_Internal needs to be declared
// in the header for DYNAMIC delegate binding. Add to the private section of
// USkillSystem.h:
//   UFUNCTION()
//   void HandleGestureRecognized_Internal(EGestureType Gesture);
