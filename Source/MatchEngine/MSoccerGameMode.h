// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerGameMode.h
// Base GameMode for Soccer Mobile Pro match sessions.
// Implements the match state machine defined in TECHSPEC Section 3.
// DO NOT add gameplay logic here — foundation only.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MSoccerGameMode.generated.h"

// ---------------------------------------------------------------------------
// EMatchState — mirrors TECHSPEC Section 3.1 exactly
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EMatchState : uint8
{
    PreMatch            UMETA(DisplayName = "Pre Match"),
    KickOffFirstHalf    UMETA(DisplayName = "Kick Off First Half"),
    InPlayFirstHalf     UMETA(DisplayName = "In Play First Half"),
    HalfTime            UMETA(DisplayName = "Half Time"),
    KickOffSecondHalf   UMETA(DisplayName = "Kick Off Second Half"),
    InPlaySecondHalf    UMETA(DisplayName = "In Play Second Half"),
    ExtraTimeFirstHalf  UMETA(DisplayName = "Extra Time First Half"),
    ExtraTimeBreak      UMETA(DisplayName = "Extra Time Break"),
    ExtraTimeSecondHalf UMETA(DisplayName = "Extra Time Second Half"),
    Penalties           UMETA(DisplayName = "Penalties"),
    FullTime            UMETA(DisplayName = "Full Time")
};

// ---------------------------------------------------------------------------
// ETeamId — identifies Home vs Away team
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ETeamId : uint8
{
    Home    UMETA(DisplayName = "Home"),
    Away    UMETA(DisplayName = "Away"),
    None    UMETA(DisplayName = "None")
};

// ---------------------------------------------------------------------------
// AMSoccerGameMode
// ---------------------------------------------------------------------------
UCLASS(Blueprintable, BlueprintType, Config = Game)
class SOCCERMOBILEPRO_API AMSoccerGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AMSoccerGameMode();

    // -------------------------------------------------------------------------
    // UE Overrides
    // -------------------------------------------------------------------------
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

    // -------------------------------------------------------------------------
    // Match State — public read, protected write
    // -------------------------------------------------------------------------

    /** Current state of the match state machine */
    UPROPERTY(BlueprintReadOnly, Category = "Match|State")
    EMatchState CurrentMatchState;

    /** In-game clock in minutes (0 → 90+, or up to 120 with extra time) */
    UPROPERTY(BlueprintReadOnly, Category = "Match|Clock")
    float MatchClockMinutes;

    /** Multiplier applied to clock progression (1.0 = real-time; >1.0 for replay) */
    UPROPERTY(BlueprintReadWrite, Category = "Match|Clock")
    float TimeScaleMultiplier;

    // -------------------------------------------------------------------------
    // Score
    // -------------------------------------------------------------------------

    UPROPERTY(BlueprintReadOnly, Category = "Match|Score")
    int32 HomeTeamScore;

    UPROPERTY(BlueprintReadOnly, Category = "Match|Score")
    int32 AwayTeamScore;

    // -------------------------------------------------------------------------
    // Match Settings (set before BeginPlay via GameOptions or Data)
    // -------------------------------------------------------------------------

    /** Whether this match uses extra time / penalties (e.g. Cup match) */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Match|Settings")
    bool bIsKnockoutCompetition;

    /** Real-time duration of half time break in seconds */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Match|Settings")
    float HalfTimeBreakDurationSeconds;

    /** Standard half length in in-game minutes */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Match|Settings")
    float MatchHalfLengthMinutes;

    // -------------------------------------------------------------------------
    // State Machine Interface
    // -------------------------------------------------------------------------

    /** Force transition to a new state (server-authoritative) */
    UFUNCTION(BlueprintCallable, Category = "Match|State")
    void TransitionState(EMatchState NewState);

    /** Returns a human-readable name for the current match state */
    UFUNCTION(BlueprintPure, Category = "Match|State")
    FString GetMatchStateDisplayName() const;

    // -------------------------------------------------------------------------
    // Score Interface
    // -------------------------------------------------------------------------

    /** Register a goal. Handles score increment and notifies listeners. */
    UFUNCTION(BlueprintCallable, Category = "Match|Score")
    void RegisterGoal(ETeamId ScoringTeam);

    // -------------------------------------------------------------------------
    // Delegates — subscribe in Blueprint or C++ for event-driven reactions
    // -------------------------------------------------------------------------

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnMatchStateChanged, EMatchState, OldState, EMatchState, NewState);

    /** Fired whenever the match transitions to a new state */
    UPROPERTY(BlueprintAssignable, Category = "Match|Events")
    FOnMatchStateChanged OnMatchStateChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnGoalScored, ETeamId, ScoringTeam, int32, NewScore);

    /** Fired whenever a goal is scored */
    UPROPERTY(BlueprintAssignable, Category = "Match|Events")
    FOnGoalScored OnGoalScored;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchFinished);

    /** Fired when match reaches FullTime and result is finalized */
    UPROPERTY(BlueprintAssignable, Category = "Match|Events")
    FOnMatchFinished OnMatchFinished;

protected:
    // -------------------------------------------------------------------------
    // Internal State Machine Steps
    // Called from Tick — each returns true when it has caused a transition.
    // Gameplay logic is NOT implemented here (foundation only).
    // -------------------------------------------------------------------------

    /** Tick logic for PreMatch — wait for all players/squads ready */
    virtual void TickPreMatch(float DeltaSeconds);

    /** Tick logic for KickOff (first or second half) */
    virtual void TickKickOff(float DeltaSeconds);

    /** Tick logic while ball is in active play */
    virtual void TickInPlay(float DeltaSeconds);

    /** Tick logic during HalfTime break */
    virtual void TickHalfTime(float DeltaSeconds);

    /** Tick logic for Extra Time halves */
    virtual void TickExtraTime(float DeltaSeconds);

    /** Tick logic for Penalty Shootout */
    virtual void TickPenalties(float DeltaSeconds);

    /** Tick logic for FullTime — finalize and broadcast result */
    virtual void TickFullTime(float DeltaSeconds);

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    /** Returns true once all connected players and squads are ready */
    virtual bool AllPlayersReady() const;

    /** Returns true once the half-time timer has elapsed */
    virtual bool HalfTimeTimerExpired() const;

    /** Resets ball and player positions for a kick-off */
    virtual void SetupKickOff(ETeamId KickingTeam);

    /** Plays referee whistle (cosmetic — triggers audio + haptics) */
    virtual void Whistle();

    /** Applies fatigue penalty to all outfield players */
    virtual void ApplyPlayerFatigue();

    /** Saves match result to backend and awards rewards */
    virtual void FinalizeMatch();

private:
    /** Accumulator for half-time break elapsed real-time seconds */
    float HalfTimeElapsedSeconds;

    /** True once FullTime finalization has been executed (prevent re-entry) */
    bool bMatchFinalized;
};
