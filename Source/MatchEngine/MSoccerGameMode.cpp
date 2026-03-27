// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerGameMode.cpp
// Implementation — foundation / state machine skeleton only.
// Gameplay logic (physics, AI, input) will be added in subsequent milestones.

#include "MSoccerGameMode.h"
#include "Engine/Engine.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
AMSoccerGameMode::AMSoccerGameMode()
    : CurrentMatchState(EMatchState::PreMatch)
    , MatchClockMinutes(0.0f)
    , TimeScaleMultiplier(1.0f)
    , HomeTeamScore(0)
    , AwayTeamScore(0)
    , bIsKnockoutCompetition(false)
    , HalfTimeBreakDurationSeconds(30.0f)  // 30 real-seconds for mobile UX
    , MatchHalfLengthMinutes(45.0f)
    , HalfTimeElapsedSeconds(0.0f)
    , bMatchFinalized(false)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

// ---------------------------------------------------------------------------
// InitGame
// ---------------------------------------------------------------------------
void AMSoccerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    // Parse options passed via URL (e.g. ?IsKnockout=true)
    bIsKnockoutCompetition = UGameplayStatics::HasOption(Options, TEXT("IsKnockout"));

    UE_LOG(LogTemp, Log, TEXT("[MSoccerGameMode] InitGame — Map: %s | Knockout: %d"),
        *MapName, (int32)bIsKnockoutCompetition);
}

// ---------------------------------------------------------------------------
// BeginPlay
// ---------------------------------------------------------------------------
void AMSoccerGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Ensure we start in PreMatch regardless of level defaults
    CurrentMatchState = EMatchState::PreMatch;
    MatchClockMinutes = 0.0f;
    HomeTeamScore = 0;
    AwayTeamScore = 0;
    bMatchFinalized = false;

    UE_LOG(LogTemp, Log, TEXT("[MSoccerGameMode] BeginPlay — State: PreMatch"));
}

// ---------------------------------------------------------------------------
// Tick — main state machine loop
// ---------------------------------------------------------------------------
void AMSoccerGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    switch (CurrentMatchState)
    {
        case EMatchState::PreMatch:
            TickPreMatch(DeltaSeconds);
            break;

        case EMatchState::KickOffFirstHalf:
        case EMatchState::KickOffSecondHalf:
            TickKickOff(DeltaSeconds);
            break;

        case EMatchState::InPlayFirstHalf:
        case EMatchState::InPlaySecondHalf:
            MatchClockMinutes += (DeltaSeconds / 60.0f) * TimeScaleMultiplier;
            TickInPlay(DeltaSeconds);
            break;

        case EMatchState::HalfTime:
            TickHalfTime(DeltaSeconds);
            break;

        case EMatchState::ExtraTimeFirstHalf:
        case EMatchState::ExtraTimeBreak:
        case EMatchState::ExtraTimeSecondHalf:
            MatchClockMinutes += (DeltaSeconds / 60.0f) * TimeScaleMultiplier;
            TickExtraTime(DeltaSeconds);
            break;

        case EMatchState::Penalties:
            TickPenalties(DeltaSeconds);
            break;

        case EMatchState::FullTime:
            TickFullTime(DeltaSeconds);
            break;

        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// TransitionState
// ---------------------------------------------------------------------------
void AMSoccerGameMode::TransitionState(EMatchState NewState)
{
    if (NewState == CurrentMatchState)
    {
        return;
    }

    const EMatchState OldState = CurrentMatchState;
    CurrentMatchState = NewState;

    UE_LOG(LogTemp, Log, TEXT("[MSoccerGameMode] State: %s → %s | Clock: %.1f min"),
        *GetMatchStateDisplayName(),
        *UEnum::GetValueAsString(NewState),
        MatchClockMinutes);

    OnMatchStateChanged.Broadcast(OldState, NewState);
}

// ---------------------------------------------------------------------------
// GetMatchStateDisplayName
// ---------------------------------------------------------------------------
FString AMSoccerGameMode::GetMatchStateDisplayName() const
{
    return UEnum::GetValueAsString(CurrentMatchState);
}

// ---------------------------------------------------------------------------
// RegisterGoal
// ---------------------------------------------------------------------------
void AMSoccerGameMode::RegisterGoal(ETeamId ScoringTeam)
{
    int32 NewScore = 0;

    if (ScoringTeam == ETeamId::Home)
    {
        HomeTeamScore++;
        NewScore = HomeTeamScore;
    }
    else if (ScoringTeam == ETeamId::Away)
    {
        AwayTeamScore++;
        NewScore = AwayTeamScore;
    }

    UE_LOG(LogTemp, Log, TEXT("[MSoccerGameMode] Goal! Home %d – %d Away"),
        HomeTeamScore, AwayTeamScore);

    OnGoalScored.Broadcast(ScoringTeam, NewScore);
}

// ---------------------------------------------------------------------------
// State Tick Implementations — STUBS (foundation only)
// ---------------------------------------------------------------------------

void AMSoccerGameMode::TickPreMatch(float DeltaSeconds)
{
    // TODO: Milestone 2 — wait for client ready confirmations via EOS
    if (AllPlayersReady())
    {
        SetupKickOff(ETeamId::Home);
        TransitionState(EMatchState::KickOffFirstHalf);
    }
}

void AMSoccerGameMode::TickKickOff(float DeltaSeconds)
{
    // TODO: Milestone 2 — play whistle, enable input, transition to InPlay
    const EMatchState NextState = (CurrentMatchState == EMatchState::KickOffFirstHalf)
        ? EMatchState::InPlayFirstHalf
        : EMatchState::InPlaySecondHalf;

    TransitionState(NextState);
}

void AMSoccerGameMode::TickInPlay(float DeltaSeconds)
{
    // TODO: Milestone 2 — SimulatePhysicsFrame, ProcessPlayerInputs, UpdateAIControllers

    // Apply player fatigue after 60 in-game minutes
    if (MatchClockMinutes > 60.0f)
    {
        ApplyPlayerFatigue();
    }

    // First half end
    if (CurrentMatchState == EMatchState::InPlayFirstHalf
        && MatchClockMinutes >= MatchHalfLengthMinutes)
    {
        Whistle();
        HalfTimeElapsedSeconds = 0.0f;
        TransitionState(EMatchState::HalfTime);
        return;
    }

    // Second half end
    if (CurrentMatchState == EMatchState::InPlaySecondHalf
        && MatchClockMinutes >= MatchHalfLengthMinutes * 2.0f)
    {
        Whistle();

        if (bIsKnockoutCompetition && HomeTeamScore == AwayTeamScore)
        {
            TransitionState(EMatchState::ExtraTimeFirstHalf);
        }
        else
        {
            TransitionState(EMatchState::FullTime);
        }
    }
}

void AMSoccerGameMode::TickHalfTime(float DeltaSeconds)
{
    HalfTimeElapsedSeconds += DeltaSeconds;

    if (HalfTimeTimerExpired())
    {
        SetupKickOff(ETeamId::Away);
        TransitionState(EMatchState::KickOffSecondHalf);
    }
}

void AMSoccerGameMode::TickExtraTime(float DeltaSeconds)
{
    // Extra time: two halves of 15 in-game minutes (90–105, 105–120)
    const float ExtraTimeFirstHalfEnd = MatchHalfLengthMinutes * 2.0f + 15.0f;   // 105
    const float ExtraTimeSecondHalfEnd = MatchHalfLengthMinutes * 2.0f + 30.0f;  // 120

    if (CurrentMatchState == EMatchState::ExtraTimeBreak)
    {
        HalfTimeElapsedSeconds += DeltaSeconds;
        if (HalfTimeElapsedSeconds >= HalfTimeBreakDurationSeconds)
        {
            TransitionState(EMatchState::ExtraTimeSecondHalf);
        }
        return;
    }

    if (CurrentMatchState == EMatchState::ExtraTimeFirstHalf
        && MatchClockMinutes >= ExtraTimeFirstHalfEnd)
    {
        Whistle();
        HalfTimeElapsedSeconds = 0.0f;
        TransitionState(EMatchState::ExtraTimeBreak);
        return;
    }

    if (CurrentMatchState == EMatchState::ExtraTimeSecondHalf
        && MatchClockMinutes >= ExtraTimeSecondHalfEnd)
    {
        Whistle();

        if (HomeTeamScore != AwayTeamScore)
        {
            TransitionState(EMatchState::FullTime);
        }
        else
        {
            TransitionState(EMatchState::Penalties);
        }
    }
}

void AMSoccerGameMode::TickPenalties(float DeltaSeconds)
{
    // TODO: Milestone 5 — implement penalty shootout logic
    // Placeholder: immediately end after entering state
    TransitionState(EMatchState::FullTime);
}

void AMSoccerGameMode::TickFullTime(float DeltaSeconds)
{
    if (!bMatchFinalized)
    {
        FinalizeMatch();
        bMatchFinalized = true;
    }
}

// ---------------------------------------------------------------------------
// Helper Stubs
// ---------------------------------------------------------------------------

bool AMSoccerGameMode::AllPlayersReady() const
{
    // TODO: Milestone 2 — query EOS session for all client ready states
    return true;  // Permissive default for single-player / AI test
}

bool AMSoccerGameMode::HalfTimeTimerExpired() const
{
    return HalfTimeElapsedSeconds >= HalfTimeBreakDurationSeconds;
}

void AMSoccerGameMode::SetupKickOff(ETeamId KickingTeam)
{
    // TODO: Milestone 2 — reset ball position, set possession, notify players
    UE_LOG(LogTemp, Log, TEXT("[MSoccerGameMode] SetupKickOff — Team: %s"),
        KickingTeam == ETeamId::Home ? TEXT("Home") : TEXT("Away"));
}

void AMSoccerGameMode::Whistle()
{
    // TODO: Milestone 2 — trigger MetaSound whistle cue + haptic feedback
    UE_LOG(LogTemp, Log, TEXT("[MSoccerGameMode] Whistle!"));
}

void AMSoccerGameMode::ApplyPlayerFatigue()
{
    // TODO: Milestone 3 — iterate all player pawns, reduce stamina attributes
}

void AMSoccerGameMode::FinalizeMatch()
{
    UE_LOG(LogTemp, Log, TEXT("[MSoccerGameMode] FinalizeMatch — Result: Home %d – %d Away"),
        HomeTeamScore, AwayTeamScore);

    // TODO: Milestone 2 — POST result to backend, award GoldCoins/EP, update Division ranking

    OnMatchFinished.Broadcast();
}
