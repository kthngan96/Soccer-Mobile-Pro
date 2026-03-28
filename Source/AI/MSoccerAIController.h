// Copyright (c) 2026 Soccer Mobile Pro. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "MTeamFormationData.h"
#include "MSoccerAIController.generated.h"

// Forward declarations
class AMSoccerPlayerCharacter;
class AMSoccerBall;
class UFormationDefinition;

// ---------------------------------------------------------------------------
// Delegates
// ---------------------------------------------------------------------------

/** Broadcast when the AI controller transitions tactical state. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnTacticalStateChanged,
    AAIController*, Controller,
    FName, NewStateName
);

/** Broadcast when an EQS result resolves (FindOpenPosition / FindPassTarget). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnEQSQueryComplete,
    bool, bSuccess,
    FVector, ResultLocation
);

// ---------------------------------------------------------------------------
// Tactical state enum  (TECHSPEC §9.1.4)
// ---------------------------------------------------------------------------

/**
 * @brief High-level tactical mode the AI is currently executing.
 *        Drives Behavior Tree sub-tree selection via the Blackboard key
 *        "TacticalState" (FName).
 */
UENUM(BlueprintType)
enum class EAITacticalState : uint8
{
    Attacking       UMETA(DisplayName = "Attacking"),
    Defending       UMETA(DisplayName = "Defending"),
    Transition      UMETA(DisplayName = "Transition"),
    SetPiece        UMETA(DisplayName = "Set Piece"),
};

// ---------------------------------------------------------------------------
// EQS context tags  (TECHSPEC §9.1.5)
// ---------------------------------------------------------------------------

/** Identifies which EQS query a result belongs to. */
UENUM(BlueprintType)
enum class EAIQueryType : uint8
{
    FindOpenPosition    UMETA(DisplayName = "Find Open Position"),
    FindPassTarget      UMETA(DisplayName = "Find Pass Target"),
};

// ---------------------------------------------------------------------------
// Main controller class
// ---------------------------------------------------------------------------

/**
 * @brief AIController for every AI-controlled player on the pitch.
 *
 * Architecture (GDD §2.1.3 / TECHSPEC §9.1):
 *  - Runs a Behavior Tree (BehaviorTreeAsset) for reactive decision making.
 *  - Uses two persistent EQS queries:
 *      FindOpenPosition  — finds the best positional option given spacing rules.
 *      FindPassTarget    — scores potential pass receivers by lane + proximity.
 *  - Consults UFormationDefinition to anchor positional logic.
 *  - Emits TacticalState changes so the Behavior Tree can select sub-trees.
 *
 * All heavy logic is deferred to stubs tagged TODO Milestone-X.
 */
UCLASS()
class SOCCERMOBILEPRO_API AMSoccerAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMSoccerAIController();

    // =========================================================================
    // Assets & Configuration
    // =========================================================================

    /**
     * The Behavior Tree asset that drives this controller.
     * Assign in Blueprint subclass or via the AIController component.
     * TODO Milestone-3: create BT_SoccerPlayer_Base asset.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|BehaviorTree")
    TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

    /**
     * EQS query asset for scouting open pitch positions.
     * Scores candidates by distance-to-nearest-opponent and formation anchor.
     * TODO Milestone-3: create EQS_FindOpenPosition asset.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|EQS")
    TObjectPtr<UEnvQuery> EQS_FindOpenPosition;

    /**
     * EQS query asset for evaluating pass targets.
     * Scores teammates by lane freedom, distance, and facing angle.
     * TODO Milestone-3: create EQS_FindPassTarget asset.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|EQS")
    TObjectPtr<UEnvQuery> EQS_FindPassTarget;

    /**
     * Formation data asset currently in use for this player's team.
     * Set by MSoccerGameMode when the match begins or on substitution.
     */
    UPROPERTY(BlueprintReadWrite, Category = "AI|Formation")
    TObjectPtr<UFormationDefinition> CurrentFormation;

    /**
     * Role this specific player occupies within the formation.
     * Used to look up the FFormationSlot anchor position.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Formation")
    EPlayerRole AssignedRole = EPlayerRole::CM;

    // =========================================================================
    // Runtime State
    // =========================================================================

    /** Current high-level tactical mode. Changes are broadcast via OnTacticalStateChanged. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI|State")
    EAITacticalState TacticalState = EAITacticalState::Defending;

    // =========================================================================
    // Delegates
    // =========================================================================

    /** Fired whenever TacticalState transitions to a new value. */
    UPROPERTY(BlueprintAssignable, Category = "AI|Events")
    FOnTacticalStateChanged OnTacticalStateChanged;

    /** Fired when the latest EQS query completes (either query type). */
    UPROPERTY(BlueprintAssignable, Category = "AI|Events")
    FOnEQSQueryComplete OnEQSQueryComplete;

    // =========================================================================
    // Public API
    // =========================================================================

    /**
     * Transitions this controller to the requested tactical state and writes
     * the new state name to the Blackboard key "TacticalState".
     * Safe to call every tick from the Behavior Tree decorator.
     *
     * @param NewState   The state to transition to.
     */
    UFUNCTION(BlueprintCallable, Category = "AI|Tactics")
    void SetTacticalState(EAITacticalState NewState);

    /**
     * Applies the team's current formation to this controller's anchor position.
     * Recalculates the Blackboard key "FormationAnchorLocation" in world space.
     *
     * Should be called:
     *   - At match start after CurrentFormation is set.
     *   - After a substitution changes AssignedRole.
     *   - When the game mode signals a set-piece reset.
     *
     * TODO Milestone-3: resolve APitchBounds world transform; for now anchors
     *                   are approximated from a flat 105×68 m pitch centred at origin.
     *
     * @param InFormation   New formation to apply (nullptr = keep current).
     */
    UFUNCTION(BlueprintCallable, Category = "AI|Formation")
    void UpdateTeamFormation(UFormationDefinition* InFormation = nullptr);

    /**
     * Fires the EQS_FindOpenPosition query asynchronously.
     * Result is delivered via OnEQSQueryComplete delegate.
     * The Blackboard key "DesiredMoveLocation" is updated on success.
     *
     * TODO Milestone-3: wire custom generator nodes and context params.
     */
    UFUNCTION(BlueprintCallable, Category = "AI|EQS")
    void RequestFindOpenPosition();

    /**
     * Fires the EQS_FindPassTarget query asynchronously.
     * Result (best receiver actor) is written to Blackboard key "PassTarget".
     *
     * @param MaxPassDistance   Maximum distance (cm) to consider a receiver.
     *
     * TODO Milestone-3: add lane-freedom scoring via custom EQS test.
     */
    UFUNCTION(BlueprintCallable, Category = "AI|EQS")
    void RequestFindPassTarget(float MaxPassDistance = 2500.0f);

    /**
     * Returns the world-space anchor position for this player given the
     * current formation and tactical state depth offset.
     *
     * @return FVector world location, or FVector::ZeroVector if no formation is set.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Formation")
    FVector GetFormationAnchorWorldLocation() const;

    // =========================================================================
    // Perception helpers (TECHSPEC §9.1.6)
    // =========================================================================

    /**
     * Returns the last sensed location of the ball.
     * Updated automatically by the sight perception configuration.
     *
     * @return FVector world location; FVector::ZeroVector if ball not in sight.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Perception")
    FVector GetPerceivedBallLocation() const;

    /**
     * Returns the nearest sensed opponent in the controller's sight cone.
     *
     * @return Pointer to the nearest opponent pawn, or nullptr.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Perception")
    APawn* GetNearestPerceivedOpponent() const;

    // =========================================================================
    // AAIController interface overrides
    // =========================================================================

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

protected:
    virtual void BeginPlay() override;

private:
    // ------------------------------------------------------------------
    // Components
    // ------------------------------------------------------------------

    /** Sight + hearing perception component. */
    UPROPERTY()
    TObjectPtr<UAIPerceptionComponent> PerceptionComp;

    UPROPERTY()
    TObjectPtr<UAISenseConfig_Sight> SightConfig;

    UPROPERTY()
    TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

    // ------------------------------------------------------------------
    // EQS request tracking
    // ------------------------------------------------------------------

    /** Handle for the in-flight FindOpenPosition query request. */
    TSharedPtr<FEnvQueryResult> PendingOpenPositionResult;

    /** Handle for the in-flight FindPassTarget query request. */
    TSharedPtr<FEnvQueryResult> PendingPassTargetResult;

    // ------------------------------------------------------------------
    // Internal callbacks
    // ------------------------------------------------------------------

    /**
     * EQS callback for FindOpenPosition.
     * Updates Blackboard "DesiredMoveLocation" and fires OnEQSQueryComplete.
     */
    void OnFindOpenPositionComplete(TSharedPtr<FEnvQueryResult> Result);

    /**
     * EQS callback for FindPassTarget.
     * Updates Blackboard "PassTarget" and fires OnEQSQueryComplete.
     */
    void OnFindPassTargetComplete(TSharedPtr<FEnvQueryResult> Result);

    /**
     * Converts a normalised formation slot (0-1) to a UE5 world-space FVector.
     * TODO Milestone-3: query APitchBounds for the authoritative transform.
     *
     * @param NormalizedX   0.0 (own goal) → 1.0 (opponent goal).
     * @param NormalizedY   0.0 (left touch) → 1.0 (right touch).
     * @return              World-space FVector at pitch-level Z.
     */
    FVector NormalizedPitchToWorld(float NormalizedX, float NormalizedY) const;

    // ------------------------------------------------------------------
    // Constants (TECHSPEC §9.1.2 — default pitch 105×68 m)
    // ------------------------------------------------------------------
    static constexpr float PitchHalfLength_cm = 5250.0f;  // 52.5 m half-length
    static constexpr float PitchHalfWidth_cm  = 3400.0f;  // 34.0 m half-width
};
