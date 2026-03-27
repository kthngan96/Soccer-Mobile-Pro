// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerGameState.h
//
// Replicated game state for Soccer Mobile Pro.
// Holds all data that ALL clients need in real-time:
//   - Live score, match clock, match state
//   - Possession stats, shots, passes
//   - Team lineups (for HUD display)
//
// GDD Reference : Section 13.3 (Key C++ Classes — AFootballGameMode)
// TECHSPEC Ref  : Section 5.3 (Server-to-Client State Replication @ 20Hz)
//
// Blueprint subclass: BP_SoccerGameState

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MSoccerGameMode.h"   // EMatchState, ETeamId
#include "MSoccerGameState.generated.h"

// ---- Forward Declarations -------------------------------------------------
class AMSoccerPlayerCharacter;

// ---------------------------------------------------------------------------
// FTeamMatchStats — per-team live statistics replicated to all clients
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FTeamMatchStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 Score = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 ShotsOnTarget = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 ShotsTotal = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 Passes = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 PassesCompleted = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 Tackles = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 Fouls = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 Corners = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 YellowCards = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 RedCards = 0;

    /** Possession percentage 0.0 – 100.0 */
    UPROPERTY(BlueprintReadOnly)
    float PossessionPercent = 50.0f;
};

// ---------------------------------------------------------------------------
// FGoalEvent — stored in goal history for match summary
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FGoalEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    ETeamId ScoringTeam = ETeamId::None;

    /** Display name of the scorer */
    UPROPERTY(BlueprintReadOnly)
    FString ScorerName;

    /** Display name of the assist provider (empty if none) */
    UPROPERTY(BlueprintReadOnly)
    FString AssistName;

    /** In-game minute the goal was scored */
    UPROPERTY(BlueprintReadOnly)
    float ClockMinute = 0.0f;
};

// ---------------------------------------------------------------------------
// AMSoccerGameState
// ---------------------------------------------------------------------------
UCLASS(Blueprintable, BlueprintType)
class SOCCERMOBILEPRO_API AMSoccerGameState : public AGameState
{
    GENERATED_BODY()

public:
    AMSoccerGameState();

    // ---- UE Overrides ------------------------------------------------------
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // =========================================================================
    // REPLICATED MATCH STATE (TECHSPEC 5.3)
    // =========================================================================

    /** Current phase of the match state machine */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MatchState, Category = "Match")
    EMatchState ReplicatedMatchState;

    /** In-game clock in minutes — replicated at 20Hz */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Match")
    float MatchClockMinutes;

    /** Frame number for client interpolation / jitter detection */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Match")
    int32 ServerFrameNumber;

    // =========================================================================
    // SCORES & STATS
    // =========================================================================

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Score, Category = "Match|Score")
    FTeamMatchStats HomeStats;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Score, Category = "Match|Score")
    FTeamMatchStats AwayStats;

    /** Full ordered list of goals scored this match */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Match|History")
    TArray<FGoalEvent> GoalHistory;

    // =========================================================================
    // POSSESSION (TECHSPEC 3.5)
    // =========================================================================

    /** Player currently holding the ball; null if loose */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Match|Possession")
    TObjectPtr<AMSoccerPlayerCharacter> CurrentBallCarrier;

    /** Team currently in possession */
    UPROPERTY(BlueprintReadOnly, Replicated, Category = "Match|Possession")
    ETeamId PossessionTeam;

    // =========================================================================
    // MATCH STATE INTERFACE
    // =========================================================================

    /** Record a goal and update stats (called by GameMode, server-only) */
    UFUNCTION(BlueprintCallable, Category = "Match|Score")
    void RecordGoal(ETeamId ScoringTeam, const FString& ScorerName, const FString& AssistName, float ClockMinute);

    /** Update possession stats (called by PossessionTracker, server-only) */
    UFUNCTION(BlueprintCallable, Category = "Match|Possession")
    void UpdatePossession(ETeamId TeamInPossession, AMSoccerPlayerCharacter* Carrier);

    /** Returns goal history as a formatted summary string (for HUD) */
    UFUNCTION(BlueprintPure, Category = "Match|History")
    FString GetGoalSummaryText() const;

    // =========================================================================
    // REP NOTIFIES
    // =========================================================================

    UFUNCTION()
    void OnRep_MatchState();

    UFUNCTION()
    void OnRep_Score();

    // =========================================================================
    // DELEGATES (client-side, fired from rep notifies)
    // =========================================================================

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateReplicated, EMatchState, NewState);
    /** Broadcast on all clients when match state changes */
    UPROPERTY(BlueprintAssignable, Category = "Match|Events")
    FOnMatchStateReplicated OnMatchStateReplicated;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreUpdated, int32, HomeScore, int32, AwayScore);
    /** Broadcast on all clients when either team scores */
    UPROPERTY(BlueprintAssignable, Category = "Match|Events")
    FOnScoreUpdated OnScoreUpdated;
};
