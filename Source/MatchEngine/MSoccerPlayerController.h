// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerPlayerController.h
//
// Player Controller for Soccer Mobile Pro.
// Manages:
//   - Input context switching (menu vs in-match)
//   - Camera management (GDD Section 2.1.4)
//   - Server RPC routing for authoritative input (TECHSPEC Section 5.2)
//   - HUD lifecycle
//
// GDD Reference : Section 13.3 (Key C++ Classes)
// TECHSPEC Ref  : Section 5.2 (Server-Side Input Processing)
//
// Blueprint subclass: BP_SoccerPlayerController

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MSoccerInputTypes.h"
#include "MSoccerPlayerController.generated.h"

// ---- Forward Declarations -------------------------------------------------
class AMSoccerPlayerCharacter;
class AMSoccerHUD;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;

// ---------------------------------------------------------------------------
// ECameraMode  (GDD Section 2.1.4)
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECameraMode : uint8
{
    Broadcast       UMETA(DisplayName = "Broadcast"),         // Default elevated side view
    CloseBroadcast  UMETA(DisplayName = "Close Broadcast"),   // Slightly closer
    EndToEnd        UMETA(DisplayName = "End to End"),        // Behind the ball
    PlayerLock      UMETA(DisplayName = "Player Lock"),       // Follow selected player
    Dynamic         UMETA(DisplayName = "Dynamic")            // AI-controlled
};

// ---------------------------------------------------------------------------
// AMSoccerPlayerController
// ---------------------------------------------------------------------------
UCLASS(Blueprintable, BlueprintType)
class SOCCERMOBILEPRO_API AMSoccerPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AMSoccerPlayerController();

    // ---- UE Overrides ------------------------------------------------------
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    virtual void Tick(float DeltaSeconds) override;

    // =========================================================================
    // CAMERA (GDD 2.1.4)
    // =========================================================================

    /** Active camera mode. Change via SetCameraMode(). */
    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    ECameraMode CurrentCameraMode;

    /** Switch camera mode; BP-callable */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraMode(ECameraMode NewMode);

    // =========================================================================
    // INPUT CONTEXT SWITCHING
    // =========================================================================

    /** IMC for main menu navigation */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Contexts")
    TObjectPtr<UInputMappingContext> IMC_Menu;

    /** IMC for in-match controls (also referenced by MSoccerPlayerCharacter) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Contexts")
    TObjectPtr<UInputMappingContext> IMC_Match;

    /** IMC for spectator mode */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Contexts")
    TObjectPtr<UInputMappingContext> IMC_Spectator;

    /** Activate match input context, deactivate others */
    UFUNCTION(BlueprintCallable, Category = "Input")
    void ActivateMatchInput();

    /** Activate menu input context */
    UFUNCTION(BlueprintCallable, Category = "Input")
    void ActivateMenuInput();

    /** Activate spectator input context */
    UFUNCTION(BlueprintCallable, Category = "Input")
    void ActivateSpectatorInput();

    // =========================================================================
    // SERVER RPC — Client → Server authoritative input (TECHSPEC 5.2)
    // =========================================================================

    /**
     * Client calls this to send a packed input frame to the server.
     * Server processes movement, actions, and gesture recognition.
     * Reliable = false (drop is acceptable; next frame will follow)
     */
    UFUNCTION(Server, Unreliable, WithValidation)
    void Server_SendInputFrame(const FClientInputFrame& Frame);

    // =========================================================================
    // HUD
    // =========================================================================

    /** Returns the typed HUD cast; may be null outside a match */
    UFUNCTION(BlueprintPure, Category = "HUD")
    AMSoccerHUD* GetSoccerHUD() const;

    /** Show / hide the match HUD */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetMatchHUDVisible(bool bVisible);

    // =========================================================================
    // MATCH INTERFACE
    // =========================================================================

    /** Called by GameMode when the match state changes */
    UFUNCTION(BlueprintNativeEvent, Category = "Match")
    void OnMatchStateChanged(EMatchState NewState);
    virtual void OnMatchStateChanged_Implementation(EMatchState NewState);

    /** Notify controller that a goal was scored */
    UFUNCTION(BlueprintNativeEvent, Category = "Match")
    void OnGoalScored(ETeamId ScoringTeam, int32 NewScore);
    virtual void OnGoalScored_Implementation(ETeamId ScoringTeam, int32 NewScore);

private:
    UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
    void ClearAllInputContexts();
};
