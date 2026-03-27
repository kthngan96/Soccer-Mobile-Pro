// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerAnimInstance.cpp

#include "MSoccerAnimInstance.h"
#include "MSoccerPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

// IK trace config
static constexpr float IK_TRACE_LENGTH       = 50.0f;   // cm below foot socket
static constexpr float IK_INTERP_SPEED       = 15.0f;   // smoothing speed
static constexpr float PELVIS_INTERP_SPEED   = 12.0f;
static constexpr float DRIBBLE_BLEND_SPEED   = 5.0f;    // blend-in speed for ball control

// Speed thresholds matching FTouchInputConfig defaults
static constexpr float SPEED_WALK_MIN        = 10.0f;
static constexpr float SPEED_RUN_MIN         = 420.0f;
static constexpr float SPEED_SPRINT_MIN      = 650.0f;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
UMSoccerAnimInstance::UMSoccerAnimInstance()
    : Speed(0.0f)
    , Direction(0.0f)
    , bIsMoving(false)
    , bIsSprinting(false)
    , bIsInAir(false)
    , LocomotionState(ELocomotionState::Idle)
    , bHasBallPossession(false)
    , DribbleBlendWeight(0.0f)
    , ActionState(EPlayerActionState::Locomotion)
    , QueuedShotVariant(EShotAnimVariant::NormalShot)
    , StaminaRatio(1.0f)
    , PelvisOffset(0.0f)
    , FootIKAlpha(0.0f)
    , SmoothedLeftFootLoc(FVector::ZeroVector)
    , SmoothedRightFootLoc(FVector::ZeroVector)
    , SmoothedPelvisOffset(0.0f)
{
}

// ---------------------------------------------------------------------------
// NativeInitializeAnimation
// ---------------------------------------------------------------------------
void UMSoccerAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwnerCharacter = Cast<AMSoccerPlayerCharacter>(GetOwningActor());
    if (OwnerCharacter)
    {
        OwnerMovement = OwnerCharacter->GetCharacterMovement();
    }

    // Pre-size shot montage array for 5 variants
    if (AM_Shots.Num() == 0)
    {
        AM_Shots.SetNum(5);
    }
}

// ---------------------------------------------------------------------------
// NativeUpdateAnimation  (game thread)
// ---------------------------------------------------------------------------
void UMSoccerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!OwnerCharacter || !OwnerMovement) return;

    UpdateLocomotionState();
    UpdateBallControlLayer();
    UpdateActionState();
    UpdateStamina();
    UpdateFootIK(DeltaSeconds);
}

// ---------------------------------------------------------------------------
// NativeThreadSafeUpdateAnimation  (worker thread — read-only, no UObject calls)
// ---------------------------------------------------------------------------
void UMSoccerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
    // Heavy math that doesn't touch UObjects can go here for perf on mobile
}

// ===========================================================================
// LOCOMOTION
// ===========================================================================

void UMSoccerAnimInstance::UpdateLocomotionState()
{
    const FVector Velocity   = OwnerMovement->Velocity;
    Speed                    = Velocity.Size2D();
    bIsMoving                = Speed > SPEED_WALK_MIN;
    bIsInAir                 = OwnerMovement->IsFalling();
    bIsSprinting             = OwnerCharacter->bIsSprinting;

    // Compute direction angle relative to actor facing
    if (bIsMoving)
    {
        const FRotator ActorRot   = OwnerCharacter->GetActorRotation();
        const FRotator VelocityRot = Velocity.Rotation();
        Direction = UKismetMathLibrary::NormalizedDeltaRotator(VelocityRot, ActorRot).Yaw;
    }
    else
    {
        Direction = 0.0f;
    }

    // Snap to locomotion state enum
    if (!bIsMoving)
    {
        LocomotionState = ELocomotionState::Idle;
    }
    else if (Speed < SPEED_RUN_MIN)
    {
        LocomotionState = ELocomotionState::Walk;
    }
    else if (Speed < SPEED_SPRINT_MIN)
    {
        LocomotionState = ELocomotionState::Run;
    }
    else
    {
        LocomotionState = ELocomotionState::Sprint;
    }
}

// ===========================================================================
// BALL CONTROL LAYER
// ===========================================================================

void UMSoccerAnimInstance::UpdateBallControlLayer()
{
    bHasBallPossession = OwnerCharacter->HasBallPossession();

    const float TargetWeight  = bHasBallPossession ? 1.0f : 0.0f;
    const float DeltaSeconds  = GetDeltaSeconds();

    DribbleBlendWeight = FMath::FInterpTo(
        DribbleBlendWeight, TargetWeight, DeltaSeconds, DRIBBLE_BLEND_SPEED);
}

// ===========================================================================
// ACTION STATE
// ===========================================================================

void UMSoccerAnimInstance::UpdateActionState()
{
    // Determine if any full-body montage is playing
    const bool bAnyMontagePlaying = IsAnyMontagePlaying();

    if (!bAnyMontagePlaying)
    {
        ActionState = bHasBallPossession
            ? EPlayerActionState::Dribbling
            : EPlayerActionState::Locomotion;
    }
    // Montage-driven states (Shooting, Tackling, etc.) are set externally
    // via PlayShootMontage / PlayTackleMontage before montage starts.
}

// ===========================================================================
// STAMINA
// ===========================================================================

void UMSoccerAnimInstance::UpdateStamina()
{
    // TODO Milestone 3: read stamina from UAttributeComponent
    // StaminaRatio = OwnerCharacter->GetStaminaComponent()->GetRatio();
    // For now: keep at 1.0 (no fatigue)
}

// ===========================================================================
// FOOT IK
// ===========================================================================

void UMSoccerAnimInstance::UpdateFootIK(float DeltaSeconds)
{
    // IK only when grounded and locomotion active
    const float TargetIKAlpha = (bIsInAir || ActionState == EPlayerActionState::BicycleKick) ? 0.0f : 1.0f;
    FootIKAlpha = FMath::FInterpTo(FootIKAlpha, TargetIKAlpha, DeltaSeconds, IK_INTERP_SPEED);

    if (FootIKAlpha < 0.01f) return;

    // Trace both feet
    LeftFootIK  = TraceFootIK(TEXT("foot_l"), IK_TRACE_LENGTH);
    RightFootIK = TraceFootIK(TEXT("foot_r"), IK_TRACE_LENGTH);

    // Smooth foot effector locations
    SmoothedLeftFootLoc = FMath::VInterpTo(
        SmoothedLeftFootLoc, LeftFootIK.EffectorLocation, DeltaSeconds, IK_INTERP_SPEED);
    SmoothedRightFootLoc = FMath::VInterpTo(
        SmoothedRightFootLoc, RightFootIK.EffectorLocation, DeltaSeconds, IK_INTERP_SPEED);

    LeftFootIK.EffectorLocation  = SmoothedLeftFootLoc;
    RightFootIK.EffectorLocation = SmoothedRightFootLoc;

    // Pelvis offset: lower pelvis by the larger foot drop to avoid leg stretch
    const float LeftDrop  = OwnerCharacter->GetActorLocation().Z - LeftFootIK.EffectorLocation.Z;
    const float RightDrop = OwnerCharacter->GetActorLocation().Z - RightFootIK.EffectorLocation.Z;
    const float TargetPelvisOffset = FMath::Min(LeftDrop, RightDrop);  // negative = lower

    SmoothedPelvisOffset = FMath::FInterpTo(
        SmoothedPelvisOffset, TargetPelvisOffset, DeltaSeconds, PELVIS_INTERP_SPEED);
    PelvisOffset = SmoothedPelvisOffset;
}

FFootIKData UMSoccerAnimInstance::TraceFootIK(FName FootSocketName, float TraceLength) const
{
    FFootIKData Result;
    if (!OwnerCharacter) return Result;

    const FVector FootWorldLoc = OwnerCharacter->GetMesh()->GetSocketLocation(FootSocketName);
    const FVector TraceStart   = FootWorldLoc + FVector(0.0f, 0.0f, IK_TRACE_LENGTH * 0.5f);
    const FVector TraceEnd     = FootWorldLoc - FVector(0.0f, 0.0f, TraceLength);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(FootIK), true, OwnerCharacter);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

    if (bHit)
    {
        Result.bFootGrounded    = true;
        Result.EffectorLocation = Hit.ImpactPoint;
        Result.IKAlpha          = 1.0f;

        // Rotate foot to align with surface normal
        const FVector Normal    = Hit.ImpactNormal;
        const FRotator BaseRot  = OwnerCharacter->GetActorRotation();
        Result.EffectorRotation = FRotator(
            FMath::RadiansToDegrees(FMath::Atan2(Normal.X, Normal.Z)),   // pitch
            BaseRot.Yaw,
            FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.Z)))   // roll
        ;
    }
    else
    {
        Result.bFootGrounded    = false;
        Result.EffectorLocation = FootWorldLoc;
        Result.IKAlpha          = 0.0f;
    }

    return Result;
}

// ===========================================================================
// MONTAGE PLAY
// ===========================================================================

void UMSoccerAnimInstance::PlayShootMontage(EShotAnimVariant Variant)
{
    const int32 Idx = static_cast<int32>(Variant);
    if (!AM_Shots.IsValidIndex(Idx) || !AM_Shots[Idx]) return;

    QueuedShotVariant = Variant;
    ActionState       = EPlayerActionState::Shooting;
    Montage_Play(AM_Shots[Idx]);
}

void UMSoccerAnimInstance::PlayPassMontage(bool bIsLob)
{
    UAnimMontage* Montage = bIsLob ? AM_LobPass : AM_GroundPass;
    if (!Montage) return;

    ActionState = EPlayerActionState::Passing;
    Montage_Play(Montage);
}

void UMSoccerAnimInstance::PlayTackleMontage(bool bIsSliding)
{
    UAnimMontage* Montage = bIsSliding ? AM_SlidingTackle : AM_StandingTackle;
    if (!Montage) return;

    ActionState = EPlayerActionState::Tackling;
    Montage_Play(Montage);
}

void UMSoccerAnimInstance::PlayCelebrationMontage()
{
    if (AM_Celebrations.Num() == 0) return;

    const int32 Idx     = FMath::RandRange(0, AM_Celebrations.Num() - 1);
    UAnimMontage* Montage = AM_Celebrations[Idx];
    if (!Montage) return;

    ActionState = EPlayerActionState::Celebrating;
    Montage_Play(Montage);
}
