// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerAIController.cpp
// Stub implementations — TODO tags mark Milestone integration points.

#include "AI/MSoccerAIController.h"
#include "MatchEngine/MSoccerPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
AMSoccerAIController::AMSoccerAIController()
    : FormationSlotIndex(0)
{
    PrimaryActorTick.bCanEverTick = true;
    bWantsPlayerState = false;
}

// ---------------------------------------------------------------------------
// UE Overrides
// ---------------------------------------------------------------------------
void AMSoccerAIController::BeginPlay()
{
    Super::BeginPlay();
    // TODO Milestone-2: subscribe to UMatchStateSubsystem::OnMatchPhaseChanged
    //                   to pause BT during HalfTime / FullTime states.
}

void AMSoccerAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    OwnedCharacter = Cast<AMSoccerPlayerCharacter>(InPawn);

    // Start Behavior Tree if asset is assigned
    if (!BehaviorTreeAsset.IsNull())
    {
        UBehaviorTree* LoadedBT = BehaviorTreeAsset.LoadSynchronous();
        if (LoadedBT)
        {
            RunBehaviorTree(LoadedBT);
            BlackboardComp = GetBlackboardComponent();
        }
        else
        {
            UE_LOG(LogTemp, Warning,
                   TEXT("MSoccerAIController: BehaviorTreeAsset failed to load for pawn %s"),
                   *InPawn->GetName());
        }
    }
    else
    {
        // TODO Milestone-2: replace with proper error telemetry.
        UE_LOG(LogTemp, Warning,
               TEXT("MSoccerAIController: No BehaviorTreeAsset assigned on %s"),
               *GetName());
    }
}

void AMSoccerAIController::OnUnPossess()
{
    StopMovement();
    OwnedCharacter.Reset();
    Super::OnUnPossess();
}

void AMSoccerAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    // High-frequency per-frame logic (e.g. look-at ball) goes here.
    // Keep this lightweight — heavy decisions live in the Behavior Tree.
    // TODO Milestone-3: update UPerceptionComponent sight stimulus.
}

// ---------------------------------------------------------------------------
// EQS — QueryFindOpenPosition
// ---------------------------------------------------------------------------
void AMSoccerAIController::QueryFindOpenPosition(FQueryFinishedSignature QueryFinishedDelegate)
{
    if (EQS_FindOpenPosition.IsNull())
    {
        UE_LOG(LogTemp, Warning,
               TEXT("MSoccerAIController::QueryFindOpenPosition — EQS_FindOpenPosition not set on %s"),
               *GetName());
        return;
    }

    RunEQSQuery(EQS_FindOpenPosition, QueryFinishedDelegate);
    // TODO Milestone-3: cancel any previous ActiveOpenPositionQuery before
    //                   issuing a new one to avoid stale results.
}

// ---------------------------------------------------------------------------
// EQS — QueryFindPassTarget
// ---------------------------------------------------------------------------
void AMSoccerAIController::QueryFindPassTarget(FQueryFinishedSignature QueryFinishedDelegate)
{
    if (EQS_FindPassTarget.IsNull())
    {
        UE_LOG(LogTemp, Warning,
               TEXT("MSoccerAIController::QueryFindPassTarget — EQS_FindPassTarget not set on %s"),
               *GetName());
        return;
    }

    RunEQSQuery(EQS_FindPassTarget, QueryFinishedDelegate);
    // TODO Milestone-3: feed best result directly into a UPassDecisionComponent
    //                   that evaluates risk vs reward before committing.
}

// ---------------------------------------------------------------------------
// Formation — UpdateTeamFormation
// ---------------------------------------------------------------------------
void AMSoccerAIController::UpdateTeamFormation_Implementation(
    const FFormationDefinition& NewFormation)
{
    CachedFormation = NewFormation;
    RefreshFormationSlotBBKey();
    OnAIFormationUpdated.Broadcast(NewFormation);

    // TODO Milestone-2: implement smooth positional interpolation.
    //   - read current WorldLocation
    //   - compute SlotNormalizedToWorld(NewSlot.NormalizedPosition)
    //   - start a timeline or UE movement task to lerp over 3-5 sec
    UE_LOG(LogTemp, Log,
           TEXT("MSoccerAIController::UpdateTeamFormation — %s slot %d → formation %s [STUB]"),
           *GetName(),
           FormationSlotIndex,
           *NewFormation.DisplayName.ToString());
}

// ---------------------------------------------------------------------------
// OnPossessionChanged
// ---------------------------------------------------------------------------
void AMSoccerAIController::OnPossessionChanged(bool bNowInPossession)
{
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsBool(MSoccerBBKeys::bHasBall, bNowInPossession);
        BlackboardComp->SetValueAsBool(MSoccerBBKeys::bReturnToFormation, !bNowInPossession);
    }
    // TODO Milestone-2: when losing possession, set a "press distance" threshold
    //                   in the BB so CDM/CM nodes trigger compact shape.
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------
FVector AMSoccerAIController::SlotNormalizedToWorld(const FVector2D& NormalizedPos) const
{
    // TODO Milestone-2: query UPitchSubsystem for actual pitch bounds.
    // Default pitch: 105m long (X) × 68m wide (Y), origin at centre circle.
    static const float PitchHalfLengthCm = 5250.0f;  // 105m / 2
    static const float PitchHalfWidthCm  = 3400.0f;  // 68m  / 2

    const float WorldX = (NormalizedPos.X - 0.5f) * 2.0f * PitchHalfLengthCm;
    const float WorldY = (NormalizedPos.Y - 0.5f) * 2.0f * PitchHalfWidthCm;
    return FVector(WorldX, WorldY, 0.0f);
}

void AMSoccerAIController::RefreshFormationSlotBBKey()
{
    if (!BlackboardComp) { return; }

    const FFormationSlot* Slot = CachedFormation.GetSlotByIndex(FormationSlotIndex);
    if (!Slot)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("MSoccerAIController::RefreshFormationSlotBBKey — SlotIndex %d not found in %s"),
               FormationSlotIndex,
               *CachedFormation.DisplayName.ToString());
        return;
    }

    const FVector SlotWorld = SlotNormalizedToWorld(Slot->NormalizedPosition);
    BlackboardComp->SetValueAsVector(MSoccerBBKeys::FormationSlotPosition, SlotWorld);
}

void AMSoccerAIController::RunEQSQuery(TSoftObjectPtr<UEnvQuery>& QueryAsset,
                                        FQueryFinishedSignature QueryFinishedDelegate)
{
    UEnvQuery* LoadedQuery = QueryAsset.LoadSynchronous();
    if (!LoadedQuery)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("MSoccerAIController::RunEQSQuery — failed to load EQS asset on %s"),
               *GetName());
        return;
    }

    UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(GetWorld());
    if (!EQSManager) { return; }

    FEnvQueryRequest Request(LoadedQuery, GetPawn());
    Request.Execute(EEnvQueryRunMode::SingleResult, QueryFinishedDelegate);
    // TODO Milestone-3: store the returned query ID so we can cancel it
    //                   via EQSManager->AbortQuery(QueryId) on UnPossess.
}
