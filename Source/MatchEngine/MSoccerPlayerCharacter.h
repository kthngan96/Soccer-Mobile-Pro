// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerPlayerCharacter.h
//
// Base character for all football players on the pitch.
// Handles:
//   - Virtual joystick movement + sprint (double-tap)
//   - Pass button  : tap = ground pass | hold = lob pass
//   - Shoot button : tap = normal shot  | hold = power shot | swipe-up = chip
//   - Enhanced Input System (UE5.6.1)
//
// GDD Reference: Section 2.1.2 (Controls)
// TECHSPEC Reference: Section 4.3 (Skill Execution Pipeline)
//
// Blueprint subclass: BP_FootballPlayer (Content/Characters/)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MSoccerPlayerCharacter.generated.h"

// ---- Forward Declarations ------------------------------------------------
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UEnhancedInputLocalPlayerSubsystem;

// ---------------------------------------------------------------------------
// EPlayerInputMode — distinguishes attacker vs defender input routing
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EPlayerInputMode : uint8
{
    None        UMETA(DisplayName = "None"),
    Attacker    UMETA(DisplayName = "Attacker"),   // Movement, Pass, Shoot, Skill
    Defender    UMETA(DisplayName = "Defender"),   // Movement, Tackle, Pressure
    Goalkeeper  UMETA(DisplayName = "Goalkeeper")  // Dive override only
};

// ---------------------------------------------------------------------------
// EPassType
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EPassType : uint8
{
    GroundPass  UMETA(DisplayName = "Ground Pass"),   // tap
    LobPass     UMETA(DisplayName = "Lob Pass")        // hold
};

// ---------------------------------------------------------------------------
// EShotType
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EShotType : uint8
{
    NormalShot  UMETA(DisplayName = "Normal Shot"),   // tap
    PowerShot   UMETA(DisplayName = "Power Shot"),    // hold
    ChipShot    UMETA(DisplayName = "Chip Shot")      // swipe-up while pressing shoot
};

// ---------------------------------------------------------------------------
// FTouchInputConfig — tuneable parameters exposed to Blueprint/Designer
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FTouchInputConfig
{
    GENERATED_BODY()

    /** Seconds the Pass button must be held to trigger a Lob (default 0.35s) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Pass", meta = (ClampMin = "0.1", ClampMax = "1.0"))
    float LobHoldThreshold = 0.35f;

    /** Seconds the Shoot button must be held to trigger a Power Shot (default 0.4s) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Shoot", meta = (ClampMin = "0.1", ClampMax = "1.0"))
    float PowerShotHoldThreshold = 0.40f;

    /** Upward swipe speed (normalized units/s) required to trigger a Chip Shot */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Shoot", meta = (ClampMin = "0.1"))
    float ChipSwipeUpThreshold = 0.6f;

    /** Maximum seconds between two joystick taps to count as a double-tap sprint */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Sprint", meta = (ClampMin = "0.05", ClampMax = "0.5"))
    float DoubleTapSprintWindow = 0.25f;

    /** Walk speed (cm/s) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Movement")
    float WalkSpeed = 400.0f;

    /** Sprint speed (cm/s) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Movement")
    float SprintSpeed = 700.0f;
};

// ---------------------------------------------------------------------------
// AMSoccerPlayerCharacter
// ---------------------------------------------------------------------------
UCLASS(Blueprintable, BlueprintType, Config = Game)
class SOCCERMOBILEPRO_API AMSoccerPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMSoccerPlayerCharacter();

    // ---- UE Overrides ------------------------------------------------------
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // =========================================================================
    // ENHANCED INPUT — Data Assets (assign in BP subclass or via GameMode)
    // =========================================================================

    /**
     * Input Mapping Context for in-match controls.
     * Assign: IMC_Match (see Content/Input/IMC_Match)
     * Priority: 0
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Contexts")
    TObjectPtr<UInputMappingContext> IMC_Match;

    // ---- Move ---------------------------------------------------------------
    /** IA_Move — Axis2D from virtual joystick (X=Right, Y=Forward) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_Move;

    /** IA_JoystickDoubleTap — fired when joystick is double-tapped for sprint toggle */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_JoystickDoubleTap;

    // ---- Pass ---------------------------------------------------------------
    /** IA_Pass — bool action; Triggered = tap, Ongoing = hold */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_Pass;

    // ---- Shoot --------------------------------------------------------------
    /** IA_Shoot — bool action; also listens for swipe gesture modifier */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_Shoot;

    /**
     * IA_ShootSwipeUp — Axis1D from touch delta Y while Shoot is pressed.
     * Used to detect chip shot gesture.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_ShootSwipeUp;

    // ---- Through Ball / Cross (GDD 2.1.2) -----------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_ThroughBall;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_Cross;

    // ---- Tackle / Pressure --------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
    TObjectPtr<UInputAction> IA_Tackle;

    // =========================================================================
    // CONFIG
    // =========================================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Config")
    FTouchInputConfig TouchConfig;

    UPROPERTY(BlueprintReadWrite, Category = "Input|Mode")
    EPlayerInputMode InputMode;

    // =========================================================================
    // RUNTIME STATE (readable from Blueprint/HUD)
    // =========================================================================

    UPROPERTY(BlueprintReadOnly, Category = "Input|State")
    bool bIsSprinting;

    UPROPERTY(BlueprintReadOnly, Category = "Input|State")
    bool bIsPassHeld;

    UPROPERTY(BlueprintReadOnly, Category = "Input|State")
    bool bIsShootHeld;

    /** How long Pass button has been held this frame (seconds) */
    UPROPERTY(BlueprintReadOnly, Category = "Input|State")
    float PassHeldDuration;

    /** How long Shoot button has been held this frame (seconds) */
    UPROPERTY(BlueprintReadOnly, Category = "Input|State")
    float ShootHeldDuration;

    /** Latest swipe-up velocity while shoot is pressed */
    UPROPERTY(BlueprintReadOnly, Category = "Input|State")
    float ShootSwipeUpVelocity;

    // =========================================================================
    // ACTION INTERFACE — called from C++ input handlers, overridable in BP
    // =========================================================================

    /** Called every tick while Move input is active */
    UFUNCTION(BlueprintNativeEvent, Category = "Input|Actions")
    void OnMoveInput(FVector2D MoveVector);
    virtual void OnMoveInput_Implementation(FVector2D MoveVector);

    /** Called when sprint is activated / deactivated */
    UFUNCTION(BlueprintNativeEvent, Category = "Input|Actions")
    void OnSprintStateChanged(bool bNewSprinting);
    virtual void OnSprintStateChanged_Implementation(bool bNewSprinting);

    /** Called when Pass action is completed — type determined by hold duration */
    UFUNCTION(BlueprintNativeEvent, Category = "Input|Actions")
    void OnPassExecuted(EPassType PassType);
    virtual void OnPassExecuted_Implementation(EPassType PassType);

    /** Called when Shoot action is completed — type determined by hold + swipe */
    UFUNCTION(BlueprintNativeEvent, Category = "Input|Actions")
    void OnShootExecuted(EShotType ShotType);
    virtual void OnShootExecuted_Implementation(EShotType ShotType);

    /** Called when Through Ball button is tapped */
    UFUNCTION(BlueprintNativeEvent, Category = "Input|Actions")
    void OnThroughBallExecuted();
    virtual void OnThroughBallExecuted_Implementation();

    /** Called when Cross button is tapped */
    UFUNCTION(BlueprintNativeEvent, Category = "Input|Actions")
    void OnCrossExecuted();
    virtual void OnCrossExecuted_Implementation();

    /** Called when Tackle / Pressure button is tapped */
    UFUNCTION(BlueprintNativeEvent, Category = "Input|Actions")
    void OnTackleExecuted();
    virtual void OnTackleExecuted_Implementation();

    // =========================================================================
    // UTILITY
    // =========================================================================

    /** Returns true if the player currently has ball possession */
    UFUNCTION(BlueprintPure, Category = "Soccer")
    bool HasBallPossession() const { return bHasBallPossession; }

    /** Called by PossessionTracker to update possession state */
    UFUNCTION(BlueprintCallable, Category = "Soccer")
    void SetBallPossession(bool bNewPossession);

protected:
    // =========================================================================
    // ENHANCED INPUT HANDLERS (bound in SetupPlayerInputComponent)
    // =========================================================================

    void HandleMove(const FInputActionValue& Value);
    void HandleMoveCompleted(const FInputActionValue& Value);

    void HandleJoystickDoubleTap(const FInputActionValue& Value);

    void HandlePassStarted(const FInputActionValue& Value);
    void HandlePassOngoing(const FInputActionValue& Value);
    void HandlePassCompleted(const FInputActionValue& Value);
    void HandlePassCanceled(const FInputActionValue& Value);

    void HandleShootStarted(const FInputActionValue& Value);
    void HandleShootOngoing(const FInputActionValue& Value);
    void HandleShootCompleted(const FInputActionValue& Value);
    void HandleShootCanceled(const FInputActionValue& Value);

    void HandleShootSwipeUp(const FInputActionValue& Value);

    void HandleThroughBall(const FInputActionValue& Value);
    void HandleCross(const FInputActionValue& Value);
    void HandleTackle(const FInputActionValue& Value);

private:
    // ---- Sprint double-tap state -------------------------------------------
    float LastJoystickTapTime;
    bool  bSprintLockedOn;     // true = sprint stays on until joystick released

    // ---- Pass hold accumulator ---------------------------------------------
    float PassHoldAccumulator;

    // ---- Shoot hold + swipe state ------------------------------------------
    float ShootHoldAccumulator;
    float PeakSwipeUpVelocity;  // max swipe velocity seen during this press

    // ---- Possession --------------------------------------------------------
    bool bHasBallPossession;

    // ---- Internal helpers --------------------------------------------------
    void ApplyMovementSpeed();
    UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
};
