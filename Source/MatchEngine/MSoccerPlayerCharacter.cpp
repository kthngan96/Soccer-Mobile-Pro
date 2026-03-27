// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerPlayerCharacter.cpp
//
// Mobile Touch Input System — Enhanced Input (UE5.6.1)
// GDD Section 2.1.2 — Virtual Joystick, Pass, Shoot, Sprint double-tap

#include "MSoccerPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/Engine.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
AMSoccerPlayerCharacter::AMSoccerPlayerCharacter()
    : bIsSprinting(false)
    , bIsPassHeld(false)
    , bIsShootHeld(false)
    , PassHeldDuration(0.0f)
    , ShootHeldDuration(0.0f)
    , ShootSwipeUpVelocity(0.0f)
    , InputMode(EPlayerInputMode::Attacker)
    , LastJoystickTapTime(-999.0f)
    , bSprintLockedOn(false)
    , PassHoldAccumulator(0.0f)
    , ShootHoldAccumulator(0.0f)
    , PeakSwipeUpVelocity(0.0f)
    , bHasBallPossession(false)
{
    PrimaryActorTick.bCanEverTick = true;

    // Movement component defaults (tuned for mobile football)
    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->MaxWalkSpeed           = TouchConfig.WalkSpeed;
        Move->MaxAcceleration        = 2048.0f;
        Move->BrakingDecelerationWalking = 2048.0f;
        Move->bOrientRotationToMovement  = true;
        Move->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    }

    bUseControllerRotationYaw   = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll  = false;
}

// ---------------------------------------------------------------------------
// BeginPlay — register IMC_Match with Enhanced Input
// ---------------------------------------------------------------------------
void AMSoccerPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetEnhancedInputSubsystem())
    {
        if (IMC_Match)
        {
            Subsystem->AddMappingContext(IMC_Match, 0);
        }
        else
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[MSoccerPlayerCharacter] IMC_Match is NULL — assign it in BP_FootballPlayer"));
        }
    }
}

// ---------------------------------------------------------------------------
// Tick — accumulate hold timers
// ---------------------------------------------------------------------------
void AMSoccerPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsPassHeld)
    {
        PassHoldAccumulator  += DeltaTime;
        PassHeldDuration      = PassHoldAccumulator;
    }

    if (bIsShootHeld)
    {
        ShootHoldAccumulator += DeltaTime;
        ShootHeldDuration     = ShootHoldAccumulator;
    }
}

// ---------------------------------------------------------------------------
// SetupPlayerInputComponent — bind Enhanced Input actions
// ---------------------------------------------------------------------------
void AMSoccerPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EIC)
    {
        UE_LOG(LogTemp, Error,
            TEXT("[MSoccerPlayerCharacter] PlayerInputComponent is NOT UEnhancedInputComponent! "
                 "Check DefaultInputComponentClass in DefaultInput.ini"));
        return;
    }

    // ---- MOVE ---------------------------------------------------------------
    if (IA_Move)
    {
        EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ThisClass::HandleMove);
        EIC->BindAction(IA_Move, ETriggerEvent::Completed,  this, &ThisClass::HandleMoveCompleted);
    }

    // ---- SPRINT (double-tap joystick) ----------------------------------------
    if (IA_JoystickDoubleTap)
    {
        EIC->BindAction(IA_JoystickDoubleTap, ETriggerEvent::Triggered, this, &ThisClass::HandleJoystickDoubleTap);
    }

    // ---- PASS ---------------------------------------------------------------
    if (IA_Pass)
    {
        EIC->BindAction(IA_Pass, ETriggerEvent::Started,   this, &ThisClass::HandlePassStarted);
        EIC->BindAction(IA_Pass, ETriggerEvent::Ongoing,   this, &ThisClass::HandlePassOngoing);
        EIC->BindAction(IA_Pass, ETriggerEvent::Completed, this, &ThisClass::HandlePassCompleted);
        EIC->BindAction(IA_Pass, ETriggerEvent::Canceled,  this, &ThisClass::HandlePassCanceled);
    }

    // ---- SHOOT + SWIPE-UP ---------------------------------------------------
    if (IA_Shoot)
    {
        EIC->BindAction(IA_Shoot, ETriggerEvent::Started,   this, &ThisClass::HandleShootStarted);
        EIC->BindAction(IA_Shoot, ETriggerEvent::Ongoing,   this, &ThisClass::HandleShootOngoing);
        EIC->BindAction(IA_Shoot, ETriggerEvent::Completed, this, &ThisClass::HandleShootCompleted);
        EIC->BindAction(IA_Shoot, ETriggerEvent::Canceled,  this, &ThisClass::HandleShootCanceled);
    }

    if (IA_ShootSwipeUp)
    {
        EIC->BindAction(IA_ShootSwipeUp, ETriggerEvent::Triggered, this, &ThisClass::HandleShootSwipeUp);
    }

    // ---- THROUGH BALL -------------------------------------------------------
    if (IA_ThroughBall)
    {
        EIC->BindAction(IA_ThroughBall, ETriggerEvent::Triggered, this, &ThisClass::HandleThroughBall);
    }

    // ---- CROSS --------------------------------------------------------------
    if (IA_Cross)
    {
        EIC->BindAction(IA_Cross, ETriggerEvent::Triggered, this, &ThisClass::HandleCross);
    }

    // ---- TACKLE -------------------------------------------------------------
    if (IA_Tackle)
    {
        EIC->BindAction(IA_Tackle, ETriggerEvent::Triggered, this, &ThisClass::HandleTackle);
    }
}

// ===========================================================================
// MOVE
// ===========================================================================

void AMSoccerPlayerCharacter::HandleMove(const FInputActionValue& Value)
{
    const FVector2D MoveVec = Value.Get<FVector2D>();

    if (!MoveVec.IsNearlyZero())
    {
        // World-relative movement (broadcast camera-forward is handled in BP)
        AddMovementInput(GetActorForwardVector(), MoveVec.Y);
        AddMovementInput(GetActorRightVector(),   MoveVec.X);

        OnMoveInput(MoveVec);
    }
}

void AMSoccerPlayerCharacter::HandleMoveCompleted(const FInputActionValue& Value)
{
    // Joystick released — cancel sprint lock if active
    if (bSprintLockedOn)
    {
        bSprintLockedOn = false;
        bIsSprinting    = false;
        ApplyMovementSpeed();
        OnSprintStateChanged(false);
    }
}

// ===========================================================================
// SPRINT — double-tap joystick (GDD 2.1.2: "sprint when double-tapped")
// ===========================================================================

void AMSoccerPlayerCharacter::HandleJoystickDoubleTap(const FInputActionValue& Value)
{
    const float Now = GetWorld()->GetTimeSeconds();
    const float Gap = Now - LastJoystickTapTime;

    if (Gap <= TouchConfig.DoubleTapSprintWindow)
    {
        // Double-tap detected — toggle sprint lock
        bSprintLockedOn = !bSprintLockedOn;
        bIsSprinting    = bSprintLockedOn;
        ApplyMovementSpeed();
        OnSprintStateChanged(bIsSprinting);

        LastJoystickTapTime = -999.0f;  // reset to prevent triple-tap
    }
    else
    {
        LastJoystickTapTime = Now;
    }
}

// ===========================================================================
// PASS  (GDD 2.1.2: tap = ground pass, hold = lob pass)
// ===========================================================================

void AMSoccerPlayerCharacter::HandlePassStarted(const FInputActionValue& Value)
{
    bIsPassHeld          = true;
    PassHoldAccumulator  = 0.0f;
    PassHeldDuration     = 0.0f;
}

void AMSoccerPlayerCharacter::HandlePassOngoing(const FInputActionValue& Value)
{
    // Accumulation happens in Tick()
    // Optional: show charge-up UI feedback here via OnPassHoldProgress delegate
}

void AMSoccerPlayerCharacter::HandlePassCompleted(const FInputActionValue& Value)
{
    bIsPassHeld = false;

    const EPassType Type = (PassHoldAccumulator >= TouchConfig.LobHoldThreshold)
        ? EPassType::LobPass
        : EPassType::GroundPass;

    PassHoldAccumulator = 0.0f;
    PassHeldDuration    = 0.0f;

    OnPassExecuted(Type);
}

void AMSoccerPlayerCharacter::HandlePassCanceled(const FInputActionValue& Value)
{
    bIsPassHeld         = false;
    PassHoldAccumulator = 0.0f;
    PassHeldDuration    = 0.0f;
}

// ===========================================================================
// SHOOT  (GDD 2.1.2: tap = normal, hold = power, swipe-up = chip)
// ===========================================================================

void AMSoccerPlayerCharacter::HandleShootStarted(const FInputActionValue& Value)
{
    bIsShootHeld         = true;
    ShootHoldAccumulator = 0.0f;
    ShootHeldDuration    = 0.0f;
    PeakSwipeUpVelocity  = 0.0f;
}

void AMSoccerPlayerCharacter::HandleShootOngoing(const FInputActionValue& Value)
{
    // Accumulation happens in Tick(); swipe velocity updated in HandleShootSwipeUp
}

void AMSoccerPlayerCharacter::HandleShootCompleted(const FInputActionValue& Value)
{
    bIsShootHeld = false;

    EShotType Type;

    // Priority: chip > power > normal
    if (PeakSwipeUpVelocity >= TouchConfig.ChipSwipeUpThreshold)
    {
        Type = EShotType::ChipShot;
    }
    else if (ShootHoldAccumulator >= TouchConfig.PowerShotHoldThreshold)
    {
        Type = EShotType::PowerShot;
    }
    else
    {
        Type = EShotType::NormalShot;
    }

    ShootHoldAccumulator = 0.0f;
    ShootHeldDuration    = 0.0f;
    PeakSwipeUpVelocity  = 0.0f;

    OnShootExecuted(Type);
}

void AMSoccerPlayerCharacter::HandleShootCanceled(const FInputActionValue& Value)
{
    bIsShootHeld         = false;
    ShootHoldAccumulator = 0.0f;
    ShootHeldDuration    = 0.0f;
    PeakSwipeUpVelocity  = 0.0f;
}

void AMSoccerPlayerCharacter::HandleShootSwipeUp(const FInputActionValue& Value)
{
    // IA_ShootSwipeUp is an Axis1D driven by touch delta-Y (positive = up)
    const float SwipeVelocity = Value.Get<float>();

    // Track the peak swipe velocity during this button press
    if (bIsShootHeld && SwipeVelocity > PeakSwipeUpVelocity)
    {
        PeakSwipeUpVelocity  = SwipeVelocity;
        ShootSwipeUpVelocity = SwipeVelocity;
    }
}

// ===========================================================================
// THROUGH BALL / CROSS / TACKLE
// ===========================================================================

void AMSoccerPlayerCharacter::HandleThroughBall(const FInputActionValue& Value)
{
    OnThroughBallExecuted();
}

void AMSoccerPlayerCharacter::HandleCross(const FInputActionValue& Value)
{
    OnCrossExecuted();
}

void AMSoccerPlayerCharacter::HandleTackle(const FInputActionValue& Value)
{
    OnTackleExecuted();
}

// ===========================================================================
// BlueprintNativeEvent DEFAULT IMPLEMENTATIONS
// (override in BP_FootballPlayer to wire to animation + gameplay logic)
// ===========================================================================

void AMSoccerPlayerCharacter::OnMoveInput_Implementation(FVector2D MoveVector)
{
    // Default: already handled in HandleMove via AddMovementInput
    // BP override: apply camera-relative direction, update minimap arrow, etc.
}

void AMSoccerPlayerCharacter::OnSprintStateChanged_Implementation(bool bNewSprinting)
{
    // Default: movement speed applied in ApplyMovementSpeed()
    // BP override: trigger sprint animation blend, show sprint VFX, haptic feedback
}

void AMSoccerPlayerCharacter::OnPassExecuted_Implementation(EPassType PassType)
{
    // TODO Milestone 3: wire to ball kick logic + AI pass target selection
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerCharacter] Pass: %s"),
        PassType == EPassType::LobPass ? TEXT("Lob") : TEXT("Ground"));
}

void AMSoccerPlayerCharacter::OnShootExecuted_Implementation(EShotType ShotType)
{
    // TODO Milestone 3: wire to ball shoot logic + power/trajectory calculation
    static const TMap<EShotType, FString> Names = {
        { EShotType::NormalShot, TEXT("Normal") },
        { EShotType::PowerShot,  TEXT("Power")  },
        { EShotType::ChipShot,   TEXT("Chip")   }
    };
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerCharacter] Shot: %s"),
        *Names[ShotType]);
}

void AMSoccerPlayerCharacter::OnThroughBallExecuted_Implementation()
{
    // TODO Milestone 3
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerCharacter] Through Ball"));
}

void AMSoccerPlayerCharacter::OnCrossExecuted_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerCharacter] Cross"));
}

void AMSoccerPlayerCharacter::OnTackleExecuted_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[MSoccerPlayerCharacter] Tackle / Pressure"));
}

// ===========================================================================
// POSSESSION
// ===========================================================================

void AMSoccerPlayerCharacter::SetBallPossession(bool bNewPossession)
{
    bHasBallPossession = bNewPossession;
}

// ===========================================================================
// HELPERS
// ===========================================================================

void AMSoccerPlayerCharacter::ApplyMovementSpeed()
{
    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->MaxWalkSpeed = bIsSprinting
            ? TouchConfig.SprintSpeed
            : TouchConfig.WalkSpeed;
    }
}

UEnhancedInputLocalPlayerSubsystem* AMSoccerPlayerCharacter::GetEnhancedInputSubsystem() const
{
    const APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return nullptr;

    return ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
}
