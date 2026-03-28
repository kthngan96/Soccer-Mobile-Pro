// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerAIController.h
// AIController that drives AI-controlled soccer players using
// Behavior Trees and Environment Query System (EQS).
// Implements TECHSPEC Section 9.1 (AI: Behavior Trees + EQS).
// References GDD Section 2.1.3 for formation-aware positioning.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "AI/MTeamFormationData.h"
#include "MSoccerAIController.generated.h"

class UEnvQuery;
class AMSoccerPlayerCharacter;

// ---------------------------------------------------------------------------
// Blackboard key name constants — defined once here, referenced everywhere
// ---------------------------------------------------------------------------
namespace MSoccerBBKeys
{
    /** FVector: world position this player should move toward */
    static const FName TargetMoveLocation    = TEXT("TargetMoveLocation");
    /** AActor*: pawn the AI is currently tracking (e.g. ball or opponent) */
    static const FName TargetActor           = TEXT("TargetActor");
    /** AActor*: preferred pass recipient found by EQS_FindPassTarget */
    static const FName PassTarget            = TEXT("PassTarget");
    /** ETeamId (uint8): which team this pawn belongs to */
    static const FName TeamId               = TEXT("TeamId");
    /** bool: is this player in possession of the ball? */
    static const FName bHasBall             = TEXT("bHasBall");
    /** bool: should this pawn return to its formation slot? */
    static const FName bReturnToFormation   = TEXT("bReturnToFormation");
    /** FVector: cached formation slot world position for this pawn */
    static const FName FormationSlotPosition = TEXT("FormationSlotPosition");
}

// ---------------------------------------------------------------------------
// AMSoccerAIController
// ---------------------------------------------------------------------------
UCLASS(Blueprintable, BlueprintType)
class SOCCERMOBILEPRO_API AMSoccerAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMSoccerAIController();

    // -------------------------------------------------------------------------
    // UE Overrides
    // -------------------------------------------------------------------------
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    virtual void Tick(float DeltaSeconds) override;

    // -------------------------------------------------------------------------
    // Behavior Tree — TECHSPEC 9.1.1
    // -------------------------------------------------------------------------

    /**
     * Soft-pointer to the shared Behavior Tree asset.
     * Assign BT_SoccerPlayer in the AIController Blueprint subclass
     * or via per-pawn AI config data.
     *
     * TODO Milestone-3: load from a UDataAsset-based AI profile so difficulty
     *                   tiers (Easy / Normal / Hard) can swap BT assets.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|BehaviorTree")
    TSoftObjectPtr<UBehaviorTree> BehaviorTreeAsset;

    /** Live Blackboard component — populated once the BT runs */
    UPROPERTY(BlueprintReadOnly, Category = "AI|BehaviorTree")
    TObjectPtr<UBlackboardComponent> BlackboardComp;

    // -------------------------------------------------------------------------
    // EQS Query Assets — TECHSPEC 9.1.2
    // -------------------------------------------------------------------------

    /**
     * EQS query that scores open positions on the pitch.
     * Factors: distance to formation slot, spacing from teammates,
     * distance from nearest opponent.
     *
     * Assign EQS_FindOpenPosition in Blueprint.
     *
     * TODO Milestone-3: tune scoring weights via UAIPerceptionConfig.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|EQS")
    TSoftObjectPtr<UEnvQuery> EQS_FindOpenPosition;

    /**
     * EQS query that evaluates nearby teammates as pass recipients.
     * Factors: line-of-sight, distance, receiver speed, opponent pressure.
     *
     * Assign EQS_FindPassTarget in Blueprint.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|EQS")
    TSoftObjectPtr<UEnvQuery> EQS_FindPassTarget;

    // -------------------------------------------------------------------------
    // Formation
    // -------------------------------------------------------------------------

    /** Slot index [0..10] this pawn occupies in the current formation */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Formation")
    int32 FormationSlotIndex;

    /**
     * Caches the current formation definition on this controller.
     * Called by UpdateTeamFormation() so EQS contexts and BT tasks
     * can read formation data without a subsystem lookup each frame.
     */
    UPROPERTY(BlueprintReadOnly, Category = "AI|Formation")
    FFormationDefinition CachedFormation;

    // -------------------------------------------------------------------------
    // EQS — public query launchers
    // -------------------------------------------------------------------------

    /**
     * Runs EQS_FindOpenPosition asynchronously.
     * On completion the best result is written to BB key TargetMoveLocation.
     *
     * @param QueryFinishedDelegate  Optional delegate called after the query
     *                                resolves (useful for BT Task nodes).
     *
     * TODO Milestone-3: accept a UEnvQueryContext override for pressing/zonal-
     *                   defence scenarios.
     */
    UFUNCTION(BlueprintCallable, Category = "AI|EQS")
    void QueryFindOpenPosition(FQueryFinishedSignature QueryFinishedDelegate);

    /**
     * Runs EQS_FindPassTarget asynchronously.
     * On completion the best scoring actor is written to BB key PassTarget.
     *
     * @param QueryFinishedDelegate  Optional delegate called after the query
     *                                resolves.
     */
    UFUNCTION(BlueprintCallable, Category = "AI|EQS")
    void QueryFindPassTarget(FQueryFinishedSignature QueryFinishedDelegate);

    // -------------------------------------------------------------------------
    // Formation update
    // -------------------------------------------------------------------------

    /**
     * Called by the match GameMode whenever the team switches tactical formation.
     * Recalculates the world-space slot position for this pawn's SlotIndex
     * and updates the BlackBoard key FormationSlotPosition.
     *
     * @param NewFormation  The incoming formation definition (home-team space;
     *                      Mirror() has already been applied for Away pawns).
     *
     * TODO Milestone-2: implement positional interpolation — smoothly slide
     *                   pawn from old slot to new slot over 3–5 seconds.
     * TODO Milestone-3: trigger PositionShift animation montage on the Character.
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AI|Formation")
    void UpdateTeamFormation(const FFormationDefinition& NewFormation);
    virtual void UpdateTeamFormation_Implementation(const FFormationDefinition& NewFormation);

    // -------------------------------------------------------------------------
    // Team possession signal — called by ball/GameMode
    // -------------------------------------------------------------------------

    /**
     * Notifies this controller that possession state has changed.
     * Updates BB key bHasBall and triggers reactive behaviour
     * (press when losing ball, hold shape when winning).
     *
     * @param bNowInPossession  True = this pawn's team now has the ball.
     *
     * TODO Milestone-2: implement pressure / compactness radius logic.
     */
    UFUNCTION(BlueprintCallable, Category = "AI|Team")
    void OnPossessionChanged(bool bNowInPossession);

    // -------------------------------------------------------------------------
    // Delegates
    // -------------------------------------------------------------------------

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnAIFormationUpdated, const FFormationDefinition&, NewFormation);

    /** Broadcast after UpdateTeamFormation resolves (after slot position updated) */
    UPROPERTY(BlueprintAssignable, Category = "AI|Events")
    FOnAIFormationUpdated OnAIFormationUpdated;

protected:
    // -------------------------------------------------------------------------
    // Internal helpers
    // -------------------------------------------------------------------------

    /**
     * Converts a normalized formation slot position (0-1 space) to world space.
     * Requires that the level contain a valid pitch Actor with a known transform.
     *
     * TODO Milestone-2: obtain pitch bounds from a UPitchSubsystem instead
     *                   of the hard-coded defaults.
     */
    FVector SlotNormalizedToWorld(const FVector2D& NormalizedPos) const;

    /** Writes FormationSlotPosition BB key using the current CachedFormation */
    void RefreshFormationSlotBBKey();

    /** Safely resolves and runs a loaded UEnvQuery, logs warning on null asset */
    void RunEQSQuery(TSoftObjectPtr<UEnvQuery>& QueryAsset,
                     FQueryFinishedSignature QueryFinishedDelegate);

private:
    /** Handle for the currently pending EQS_FindOpenPosition request */
    FEnvQueryRequest ActiveOpenPositionQuery;

    /** Handle for the currently pending EQS_FindPassTarget request */
    FEnvQueryRequest ActivePassTargetQuery;

    /** Weak pointer to the possessed soccer character (set in OnPossess) */
    TWeakObjectPtr<AMSoccerPlayerCharacter> OwnedCharacter;
};
