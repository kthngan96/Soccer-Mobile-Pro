// Copyright (c) Soccer Mobile Pro. All Rights Reserved.
// GDD Section 2.1.3 | TECHSPEC Section 9.1 — AI: Behavior Trees + EQS

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "MTeamFormationData.h"
#include "MSoccerAIController.generated.h"

/**
 * MSoccerAIController
 *
 * AIController responsible for driving AI-controlled soccer players.
 * Uses a Behavior Tree for decision-making and EQS for spatial queries
 * such as finding open positions and pass targets.
 *
 * TECHSPEC §9.1: Each field player runs an instance of this controller.
 * The BT ticks at 60 Hz; EQS queries are throttled per frame budget.
 */
UCLASS()
class SOCCERMOBILEPRO_API AMSoccerAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMSoccerAIController();

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    // ─── Behavior Tree ───────────────────────────────────────────────────────

    /** Main Behavior Tree asset for this AI player (assigned per role in BP). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|BehaviorTree")
    UBehaviorTree* BehaviorTreeAsset;

    UPROPERTY()
    UBehaviorTreeComponent* BehaviorTreeComponent;

    UPROPERTY()
    UBlackboardComponent* BlackboardComponent;

    // ─── EQS Queries ─────────────────────────────────────────────────────────

    /**
     * EQS Query: FindOpenPosition
     * Evaluates positions on the pitch with no nearby opponents;
     * used when the AI decides to make a run or reposition.
     * TECHSPEC §9.1.2
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|EQS")
    UEnvQuery* EQS_FindOpenPosition;

    /**
     * EQS Query: FindPassTarget
     * Evaluates teammates as potential pass recipients;
     * scores by distance, line-of-sight, and formation slot proximity.
     * TECHSPEC §9.1.3
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|EQS")
    UEnvQuery* EQS_FindPassTarget;

    /** Handle for the in-flight FindOpenPosition query (prevent duplicates). */
    FEnvQueryRequest OpenPositionQueryRequest;

    /** Handle for the in-flight FindPassTarget query (prevent duplicates). */
    FEnvQueryRequest PassTargetQueryRequest;

public:
    // ─── EQS Public API ──────────────────────────────────────────────────────

    /**
     * Triggers the FindOpenPosition EQS and stores the result on the Blackboard.
     * Called by BT Task nodes; result written to BB key "BestOpenPosition".
     */
    UFUNCTION(BlueprintCallable, Category = "AI|EQS")
    void RunQuery_FindOpenPosition();

    /**
     * Triggers the FindPassTarget EQS and stores the best target on the Blackboard.
     * Called by BT Task nodes; result written to BB key "BestPassTarget".
     */
    UFUNCTION(BlueprintCallable, Category = "AI|EQS")
    void RunQuery_FindPassTarget();

    // ─── Formation ───────────────────────────────────────────────────────────

    /**
     * UpdateTeamFormation
     *
     * Stub — full implementation in TECHSPEC §9.1.4.
     * Notifies all AI controllers on the same team to recalculate their
     * target formation slot based on the new EFormationType.
     *
     * @param NewFormation  The formation the team should transition to.
     */
    UFUNCTION(BlueprintCallable, Category = "AI|Formation")
    void UpdateTeamFormation(EFormationType NewFormation);

    /** Current formation slot assigned to this player. */
    UPROPERTY(BlueprintReadWrite, Category = "AI|Formation")
    FFormationSlot AssignedSlot;

private:
    // ─── EQS Callbacks ───────────────────────────────────────────────────────

    void OnOpenPositionQueryFinished(TSharedPtr<FEnvQueryResult> Result);
    void OnPassTargetQueryFinished(TSharedPtr<FEnvQueryResult> Result);
};
