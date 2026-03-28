// Copyright (c) Soccer Mobile Pro. All Rights Reserved.
// GDD Section 2.1.3 | TECHSPEC Section 9.1 — AI: Behavior Trees + EQS

#include "MSoccerAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Kismet/GameplayStatics.h"

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

AMSoccerAIController::AMSoccerAIController()
{
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
    BlackboardComponent   = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

    // TECHSPEC §9.1.1: Use the shared Blackboard asset; individual controllers
    // write to per-instance keys so they don't clobber teammates.
}

// ─────────────────────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────────────────────

void AMSoccerAIController::BeginPlay()
{
    Super::BeginPlay();
}

void AMSoccerAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset)
    {
        // Initialize Blackboard before running the tree.
        if (UBlackboardData* BBAsset = BehaviorTreeAsset->BlackboardAsset)
        {
            UseBlackboard(BBAsset, BlackboardComponent);
        }

        RunBehaviorTree(BehaviorTreeAsset);

        UE_LOG(LogTemp, Log,
            TEXT("MSoccerAIController: BehaviorTree '%s' started for pawn '%s'."),
            *BehaviorTreeAsset->GetName(),
            *InPawn->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("MSoccerAIController: No BehaviorTree assigned to '%s'."),
            *GetName());
    }
}

void AMSoccerAIController::OnUnPossess()
{
    Super::OnUnPossess();

    if (BehaviorTreeComponent)
    {
        BehaviorTreeComponent->StopTree();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// EQS — FindOpenPosition
// TECHSPEC §9.1.2
// ─────────────────────────────────────────────────────────────────────────────

void AMSoccerAIController::RunQuery_FindOpenPosition()
{
    if (!EQS_FindOpenPosition)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("MSoccerAIController: EQS_FindOpenPosition asset is not assigned."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(World);
    if (!EQSManager) return;

    // Run query and bind result callback.
    OpenPositionQueryRequest = FEnvQueryRequest(EQS_FindOpenPosition, GetPawn());
    OpenPositionQueryRequest.Execute(
        EEnvQueryRunMode::SingleResult,
        this,
        &AMSoccerAIController::OnOpenPositionQueryFinished
    );
}

void AMSoccerAIController::OnOpenPositionQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
    if (!Result.IsValid() || Result->IsAborted())
    {
        return;
    }

    if (Result->Items.Num() > 0)
    {
        const FVector BestLocation = Result->GetItemAsLocation(0);

        if (BlackboardComponent)
        {
            BlackboardComponent->SetValueAsVector(
                FName("BestOpenPosition"), BestLocation
            );
        }

        UE_LOG(LogTemp, Verbose,
            TEXT("MSoccerAIController [%s]: FindOpenPosition → (%.1f, %.1f, %.1f)"),
            *GetName(),
            BestLocation.X, BestLocation.Y, BestLocation.Z);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// EQS — FindPassTarget
// TECHSPEC §9.1.3
// ─────────────────────────────────────────────────────────────────────────────

void AMSoccerAIController::RunQuery_FindPassTarget()
{
    if (!EQS_FindPassTarget)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("MSoccerAIController: EQS_FindPassTarget asset is not assigned."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(World);
    if (!EQSManager) return;

    PassTargetQueryRequest = FEnvQueryRequest(EQS_FindPassTarget, GetPawn());
    PassTargetQueryRequest.Execute(
        EEnvQueryRunMode::SingleResult,
        this,
        &AMSoccerAIController::OnPassTargetQueryFinished
    );
}

void AMSoccerAIController::OnPassTargetQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
    if (!Result.IsValid() || Result->IsAborted())
    {
        return;
    }

    if (Result->Items.Num() > 0)
    {
        // The EQS query generates ActorItem results; retrieve the winning actor.
        UObject* BestActor = Result->GetItemAsActor(0);

        if (BlackboardComponent && BestActor)
        {
            BlackboardComponent->SetValueAsObject(
                FName("BestPassTarget"), BestActor
            );
        }

        UE_LOG(LogTemp, Verbose,
            TEXT("MSoccerAIController [%s]: FindPassTarget → '%s'"),
            *GetName(),
            BestActor ? *BestActor->GetName() : TEXT("None"));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Formation
// TECHSPEC §9.1.4 — stub, full logic TBD
// ─────────────────────────────────────────────────────────────────────────────

void AMSoccerAIController::UpdateTeamFormation(EFormationType NewFormation)
{
    // TODO (TECHSPEC §9.1.4): Broadcast formation change to all team members.
    // Steps when implemented:
    //   1. Retrieve the team manager via subsystem / game state.
    //   2. Look up FFormationData for NewFormation in MTeamFormationData.
    //   3. Assign each AI controller its new FFormationSlot by player index.
    //   4. Write the new target position to BB key "FormationTargetPosition".
    //   5. Trigger BT Decorator "Formation Changed" to force re-evaluation.

    UE_LOG(LogTemp, Log,
        TEXT("MSoccerAIController [%s]: UpdateTeamFormation called (stub). Formation=%d"),
        *GetName(),
        static_cast<int32>(NewFormation));
}
