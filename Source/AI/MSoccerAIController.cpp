// Copyright (c) 2026 Soccer Mobile Pro. All Rights Reserved.
#include "MSoccerAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

AMSoccerAIController::AMSoccerAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    // ----- Perception -------------------------------------------------------
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*PerceptionComp);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius                = 2000.0f;  // 20 m sight range
    SightConfig->LoseSightRadius            = 2400.0f;  // 24 m lose-sight range
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->SetMaxAge(5.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies  = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    PerceptionComp->ConfigureSense(*SightConfig);

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 1500.0f;  // 15 m hearing range
    HearingConfig->SetMaxAge(3.0f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies  = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    PerceptionComp->ConfigureSense(*HearingConfig);

    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

    // ----- Blackboard & BT components created by parent AAIController ------
    bWantsPlayerState = false;
}

// ---------------------------------------------------------------------------
// BeginPlay
// ---------------------------------------------------------------------------

void AMSoccerAIController::BeginPlay()
{
    Super::BeginPlay();
    // BT startup deferred to OnPossess so we have a valid pawn.
}

// ---------------------------------------------------------------------------
// AAIController overrides
// ---------------------------------------------------------------------------

void AMSoccerAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
    else
    {
        // TODO Milestone-3: log warning and assign default BT_SoccerPlayer_Base
        UE_LOG(LogTemp, Warning,
               TEXT("AMSoccerAIController::OnPossess — No BehaviorTreeAsset assigned on %s."),
               *GetNameSafe(this));
    }

    // Set initial tactical state on Blackboard
    SetTacticalState(TacticalState);

    // Apply formation anchor if data is already present
    if (CurrentFormation)
    {
        UpdateTeamFormation(CurrentFormation);
    }
}

void AMSoccerAIController::OnUnPossess()
{
    Super::OnUnPossess();
    // Cancel any in-flight EQS requests
    // TODO Milestone-3: call UEnvQueryManager::AbortQuery once request IDs are tracked
}

// ---------------------------------------------------------------------------
// SetTacticalState
// ---------------------------------------------------------------------------

void AMSoccerAIController::SetTacticalState(EAITacticalState NewState)
{
    if (TacticalState == NewState)
    {
        return;
    }

    TacticalState = NewState;

    // Mirror to Blackboard for BT decorator queries
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        static const FName Key_TacticalState(TEXT("TacticalState"));
        // Store as FName so BT decorators can do string comparison
        const UEnum* Enum = StaticEnum<EAITacticalState>();
        const FName StateName = Enum
            ? FName(*Enum->GetNameStringByValue(static_cast<int64>(NewState)))
            : FName(TEXT("Unknown"));
        BB->SetValueAsName(Key_TacticalState, StateName);

        OnTacticalStateChanged.Broadcast(this, StateName);
    }
}

// ---------------------------------------------------------------------------
// UpdateTeamFormation  (TODO Milestone-3: wire to APitchBounds)
// ---------------------------------------------------------------------------

void AMSoccerAIController::UpdateTeamFormation(UFormationDefinition* InFormation)
{
    if (InFormation)
    {
        CurrentFormation = InFormation;
    }

    if (!CurrentFormation)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("AMSoccerAIController::UpdateTeamFormation — no formation set on %s."),
               *GetNameSafe(this));
        return;
    }

    // Resolve anchor world location and push to Blackboard
    const FVector AnchorWorld = GetFormationAnchorWorldLocation();
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        static const FName Key_FormationAnchor(TEXT("FormationAnchorLocation"));
        BB->SetValueAsVector(Key_FormationAnchor, AnchorWorld);
    }

    // TODO Milestone-3: if team is mirrored (attacking right→left), flip NormalizedX
    // TODO Milestone-3: apply CompactShape / WidthStretch overrides from game mode
}

// ---------------------------------------------------------------------------
// GetFormationAnchorWorldLocation
// ---------------------------------------------------------------------------

FVector AMSoccerAIController::GetFormationAnchorWorldLocation() const
{
    if (!CurrentFormation)
    {
        return FVector::ZeroVector;
    }

    const FFormationSlot* Slot = CurrentFormation->GetSlotForRole(AssignedRole);
    if (!Slot)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("AMSoccerAIController::GetFormationAnchorWorldLocation — "
                    "No slot for role %d in formation %s."),
               static_cast<int32>(AssignedRole),
               *CurrentFormation->GetName());
        return FVector::ZeroVector;
    }

    // Apply depth offset based on current tactical state
    float DepthOffset = 0.0f;
    switch (TacticalState)
    {
        case EAITacticalState::Attacking:  DepthOffset = +Slot->TacticalDepthOffset; break;
        case EAITacticalState::Defending:  DepthOffset = -Slot->TacticalDepthOffset; break;
        default: break;
    }
    const float AdjustedX = FMath::Clamp(Slot->NormalizedX + DepthOffset, 0.0f, 1.0f);

    return NormalizedPitchToWorld(AdjustedX, Slot->NormalizedY);
}

// ---------------------------------------------------------------------------
// RequestFindOpenPosition
// ---------------------------------------------------------------------------

void AMSoccerAIController::RequestFindOpenPosition()
{
    if (!EQS_FindOpenPosition)
    {
        // TODO Milestone-3: assign default EQS_FindOpenPosition asset
        UE_LOG(LogTemp, Warning,
               TEXT("AMSoccerAIController::RequestFindOpenPosition — "
                    "EQS_FindOpenPosition asset not assigned on %s."),
               *GetNameSafe(this));
        OnEQSQueryComplete.Broadcast(false, FVector::ZeroVector);
        return;
    }

    // TODO Milestone-3: construct FEnvQueryRequest with relevant context
    // (FormationAnchorLocation, OpponentPositions, BallLocation)
    // and call UEnvQueryManager::RunQuery.
    //
    // Stub: broadcast a fake success using the formation anchor.
    const FVector AnchorWorld = GetFormationAnchorWorldLocation();
    OnEQSQueryComplete.Broadcast(true, AnchorWorld);

    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        static const FName Key_DesiredMoveLocation(TEXT("DesiredMoveLocation"));
        BB->SetValueAsVector(Key_DesiredMoveLocation, AnchorWorld);
    }
}

// ---------------------------------------------------------------------------
// RequestFindPassTarget
// ---------------------------------------------------------------------------

void AMSoccerAIController::RequestFindPassTarget(float MaxPassDistance)
{
    if (!EQS_FindPassTarget)
    {
        // TODO Milestone-3: assign default EQS_FindPassTarget asset
        UE_LOG(LogTemp, Warning,
               TEXT("AMSoccerAIController::RequestFindPassTarget — "
                    "EQS_FindPassTarget asset not assigned on %s."),
               *GetNameSafe(this));
        OnEQSQueryComplete.Broadcast(false, FVector::ZeroVector);
        return;
    }

    // TODO Milestone-3: populate FEnvQueryRequest with:
    //   - MaxPassDistance context float
    //   - TeamMembersGenerator (all friendly pawns within range)
    //   - LaneFreedomTest, DistanceTest, FacingAngleTest
    // and bind OnFindPassTargetComplete as the result callback.
    //
    // Stub: do nothing useful — BB key "PassTarget" remains stale.
    UE_LOG(LogTemp, Log,
           TEXT("AMSoccerAIController::RequestFindPassTarget — STUB (MaxDist=%.0f cm) on %s."),
           MaxPassDistance, *GetNameSafe(this));
}

// ---------------------------------------------------------------------------
// GetPerceivedBallLocation
// ---------------------------------------------------------------------------

FVector AMSoccerAIController::GetPerceivedBallLocation() const
{
    if (!PerceptionComp)
    {
        return FVector::ZeroVector;
    }

    // TODO Milestone-3: filter actors by AMSoccerBall tag
    TArray<AActor*> PerceivedActors;
    PerceptionComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
    for (AActor* Actor : PerceivedActors)
    {
        if (Actor && Actor->ActorHasTag(TEXT("SoccerBall")))
        {
            return Actor->GetActorLocation();
        }
    }
    return FVector::ZeroVector;
}

// ---------------------------------------------------------------------------
// GetNearestPerceivedOpponent
// ---------------------------------------------------------------------------

APawn* AMSoccerAIController::GetNearestPerceivedOpponent() const
{
    if (!PerceptionComp || !GetPawn())
    {
        return nullptr;
    }

    TArray<AActor*> PerceivedActors;
    PerceptionComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

    APawn* NearestOpponent = nullptr;
    float  NearestDistSq   = FLT_MAX;
    const FVector SelfLoc  = GetPawn()->GetActorLocation();

    for (AActor* Actor : PerceivedActors)
    {
        APawn* Pawn = Cast<APawn>(Actor);
        if (!Pawn || Pawn == GetPawn())
        {
            continue;
        }
        // TODO Milestone-3: replace tag check with team affiliation from
        //                   MSoccerGameState::GetTeamForPawn()
        if (Pawn->ActorHasTag(TEXT("TeamB")))
        {
            const float DistSq = FVector::DistSquared(SelfLoc, Pawn->GetActorLocation());
            if (DistSq < NearestDistSq)
            {
                NearestDistSq   = DistSq;
                NearestOpponent = Pawn;
            }
        }
    }
    return NearestOpponent;
}

// ---------------------------------------------------------------------------
// Private: EQS callbacks
// ---------------------------------------------------------------------------

void AMSoccerAIController::OnFindOpenPositionComplete(TSharedPtr<FEnvQueryResult> Result)
{
    // TODO Milestone-3: implement result reading + Blackboard write
    const bool bSuccess = Result.IsValid() && Result->IsSuccessful();
    const FVector BestLoc = bSuccess ? Result->GetItemAsLocation(0) : FVector::ZeroVector;
    OnEQSQueryComplete.Broadcast(bSuccess, BestLoc);

    if (bSuccess)
    {
        if (UBlackboardComponent* BB = GetBlackboardComponent())
        {
            static const FName Key(TEXT("DesiredMoveLocation"));
            BB->SetValueAsVector(Key, BestLoc);
        }
    }
}

void AMSoccerAIController::OnFindPassTargetComplete(TSharedPtr<FEnvQueryResult> Result)
{
    // TODO Milestone-3: cast best item to AMSoccerPlayerCharacter, write to BB
    const bool bSuccess = Result.IsValid() && Result->IsSuccessful();
    OnEQSQueryComplete.Broadcast(bSuccess, FVector::ZeroVector);
}

// ---------------------------------------------------------------------------
// Private: NormalizedPitchToWorld
// ---------------------------------------------------------------------------

FVector AMSoccerAIController::NormalizedPitchToWorld(
    float NormalizedX, float NormalizedY) const
{
    // Pitch centred at world origin.  X axis = pitch length, Y axis = pitch width.
    // Attacking direction = +X (team A default).  TODO Milestone-3: handle mirrored teams.
    const float WorldX = FMath::Lerp(-PitchHalfLength_cm, +PitchHalfLength_cm, NormalizedX);
    const float WorldY = FMath::Lerp(-PitchHalfWidth_cm,  +PitchHalfWidth_cm,  NormalizedY);
    return FVector(WorldX, WorldY, 0.0f);
}
