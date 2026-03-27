// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerGameState.cpp — Stub implementation

#include "MSoccerGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AMSoccerGameState::AMSoccerGameState()
    : ReplicatedMatchState(EMatchState::PreMatch)
    , MatchClockMinutes(0.0f)
    , ServerFrameNumber(0)
    , PossessionTeam(ETeamId::None)
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMSoccerGameState::BeginPlay()
{
    Super::BeginPlay();
    // TODO Milestone 2: subscribe to GameMode delegates for score/state updates
}

// ---------------------------------------------------------------------------
// Replication
// ---------------------------------------------------------------------------
void AMSoccerGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMSoccerGameState, ReplicatedMatchState);
    DOREPLIFETIME(AMSoccerGameState, MatchClockMinutes);
    DOREPLIFETIME(AMSoccerGameState, ServerFrameNumber);
    DOREPLIFETIME(AMSoccerGameState, HomeStats);
    DOREPLIFETIME(AMSoccerGameState, AwayStats);
    DOREPLIFETIME(AMSoccerGameState, GoalHistory);
    DOREPLIFETIME(AMSoccerGameState, CurrentBallCarrier);
    DOREPLIFETIME(AMSoccerGameState, PossessionTeam);
}

// ---------------------------------------------------------------------------
// Match State
// ---------------------------------------------------------------------------
void AMSoccerGameState::RecordGoal(ETeamId ScoringTeam, const FString& ScorerName,
                                   const FString& AssistName, float ClockMinute)
{
    FGoalEvent Event;
    Event.ScoringTeam  = ScoringTeam;
    Event.ScorerName   = ScorerName;
    Event.AssistName   = AssistName;
    Event.ClockMinute  = ClockMinute;
    GoalHistory.Add(Event);

    if (ScoringTeam == ETeamId::Home) HomeStats.Score++;
    else if (ScoringTeam == ETeamId::Away) AwayStats.Score++;

    // OnRep_Score fires automatically on clients via replication
    OnRep_Score();

    UE_LOG(LogTemp, Log, TEXT("[MSoccerGameState] Goal by %s (%s) at %.0f'"),
        *ScorerName, *UEnum::GetValueAsString(ScoringTeam), ClockMinute);
}

void AMSoccerGameState::UpdatePossession(ETeamId TeamInPossession, AMSoccerPlayerCharacter* Carrier)
{
    PossessionTeam    = TeamInPossession;
    CurrentBallCarrier = Carrier;
    // Possession percent calculation: TODO Milestone 3
}

FString AMSoccerGameState::GetGoalSummaryText() const
{
    FString Out;
    for (const FGoalEvent& G : GoalHistory)
    {
        Out += FString::Printf(TEXT("%s %d' (%s)\n"),
            *G.ScorerName,
            FMath::RoundToInt(G.ClockMinute),
            G.AssistName.IsEmpty() ? TEXT("-") : *G.AssistName);
    }
    return Out;
}

// ---------------------------------------------------------------------------
// Rep Notifies
// ---------------------------------------------------------------------------
void AMSoccerGameState::OnRep_MatchState()
{
    OnMatchStateReplicated.Broadcast(ReplicatedMatchState);
    UE_LOG(LogTemp, Log, TEXT("[MSoccerGameState] Client MatchState -> %s"),
        *UEnum::GetValueAsString(ReplicatedMatchState));
}

void AMSoccerGameState::OnRep_Score()
{
    OnScoreUpdated.Broadcast(HomeStats.Score, AwayStats.Score);
}
