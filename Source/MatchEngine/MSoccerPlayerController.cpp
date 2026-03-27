// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerPlayerController.cpp — Stub implementation

#include "MSoccerPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "MSoccerGameMode.h"
#include "Engine/Engine.h"

AMSoccerPlayerController::AMSoccerPlayerController()
    : CurrentCameraMode(ECameraMode::Broadcast)
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMSoccerPlayerController::BeginPlay()
{
    Super::BeginPlay();
    // TODO Milestone 2: activate appropriate IMC based on game phase
}

void AMSoccerPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    // TODO Milestone 2: bind camera arm to possessed character
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerController] Possessed: %s"), *GetNameSafe(InPawn));
}

void AMSoccerPlayerController::OnUnPossess()
{
    Super::OnUnPossess();
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerController] UnPossessed"));
}

void AMSoccerPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    // TODO Milestone 2: camera interpolation for Dynamic mode
}

// ---------------------------------------------------------------------------
// Camera
// ---------------------------------------------------------------------------
void AMSoccerPlayerController::SetCameraMode(ECameraMode NewMode)
{
    CurrentCameraMode = NewMode;
    // TODO Milestone 2: blend camera spring arm params per mode
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerController] CameraMode -> %s"),
        *UEnum::GetValueAsString(NewMode));
}

// ---------------------------------------------------------------------------
// Input Context Switching
// ---------------------------------------------------------------------------
void AMSoccerPlayerController::ActivateMatchInput()
{
    ClearAllInputContexts();
    if (UEnhancedInputLocalPlayerSubsystem* S = GetEnhancedInputSubsystem())
    {
        if (IMC_Match) S->AddMappingContext(IMC_Match, 0);
    }
}

void AMSoccerPlayerController::ActivateMenuInput()
{
    ClearAllInputContexts();
    if (UEnhancedInputLocalPlayerSubsystem* S = GetEnhancedInputSubsystem())
    {
        if (IMC_Menu) S->AddMappingContext(IMC_Menu, 0);
    }
}

void AMSoccerPlayerController::ActivateSpectatorInput()
{
    ClearAllInputContexts();
    if (UEnhancedInputLocalPlayerSubsystem* S = GetEnhancedInputSubsystem())
    {
        if (IMC_Spectator) S->AddMappingContext(IMC_Spectator, 0);
    }
}

void AMSoccerPlayerController::ClearAllInputContexts()
{
    if (UEnhancedInputLocalPlayerSubsystem* S = GetEnhancedInputSubsystem())
    {
        if (IMC_Match)     S->RemoveMappingContext(IMC_Match);
        if (IMC_Menu)      S->RemoveMappingContext(IMC_Menu);
        if (IMC_Spectator) S->RemoveMappingContext(IMC_Spectator);
    }
}

// ---------------------------------------------------------------------------
// Server RPC
// ---------------------------------------------------------------------------
bool AMSoccerPlayerController::Server_SendInputFrame_Validate(const FClientInputFrame& Frame)
{
    // TODO Milestone 2: anti-cheat validation (axis range, timestamp delta)
    return true;
}

void AMSoccerPlayerController::Server_SendInputFrame_Implementation(const FClientInputFrame& Frame)
{
    // TODO Milestone 2: forward to AFootballGameMode::OnReceiveClientInput()
}

// ---------------------------------------------------------------------------
// HUD
// ---------------------------------------------------------------------------
AMSoccerHUD* AMSoccerPlayerController::GetSoccerHUD() const
{
    return Cast<AMSoccerHUD>(GetHUD());
}

void AMSoccerPlayerController::SetMatchHUDVisible(bool bVisible)
{
    // TODO Milestone 2: call BP_UIMatchHUD Show/Hide
}

// ---------------------------------------------------------------------------
// Match Events
// ---------------------------------------------------------------------------
void AMSoccerPlayerController::OnMatchStateChanged_Implementation(EMatchState NewState)
{
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerController] MatchState -> %s"),
        *UEnum::GetValueAsString(NewState));
    // TODO Milestone 2: switch HUD panels per state (half-time stats, full-time screen)
}

void AMSoccerPlayerController::OnGoalScored_Implementation(ETeamId ScoringTeam, int32 NewScore)
{
    // TODO Milestone 2: trigger goal celebration HUD overlay + haptic burst
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerController] Goal! Team %s Score %d"),
        *UEnum::GetValueAsString(ScoringTeam), NewScore);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
UEnhancedInputLocalPlayerSubsystem* AMSoccerPlayerController::GetEnhancedInputSubsystem() const
{
    return ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
}
