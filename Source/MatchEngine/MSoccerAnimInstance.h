// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerAnimInstance.h
//
// Animation Blueprint C++ base class for BP_FootballPlayer.
// Drives the full AnimGraph:
//   - Locomotion BlendSpace (Idle / Walk / Run / Sprint)
//   - Ball Control layer (dribbling override)
//   - Full-body IK foot placement (Control Rig)
//   - Montage slots: Tackle, Shoot (5 variations), Pass, Celebration
//
// GDD Reference  : Section 2.1 (Match Engine), Section 5.3 (Celebrations)
// TECHSPEC Ref   : Section 9.1 (Control Rig + AnimGraph)
//
// Blueprint subclass: ABP_FootballPlayer (Content/Characters/Animations/)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MSoccerAnimInstance.generated.h"

// ---- Forward Declarations -------------------------------------------------
class AMSoccerPlayerCharacter;
class UCharacterMovementComponent;

// ---------------------------------------------------------------------------
// ELocomotionState — drives BlendSpace selection
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Walk        UMETA(DisplayName = "Walk"),
    Run         UMETA(DisplayName = "Run"),
    Sprint      UMETA(DisplayName = "Sprint")
};

// ---------------------------------------------------------------------------
// EPlayerActionState — top-level state machine state
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EPlayerActionState : uint8
{
    Locomotion      UMETA(DisplayName = "Locomotion"),
    Dribbling       UMETA(DisplayName = "Dribbling"),       // Ball control layer active
    Shooting        UMETA(DisplayName = "Shooting"),        // Shoot montage playing
    Passing         UMETA(DisplayName = "Passing"),         // Pass montage playing
    Tackling        UMETA(DisplayName = "Tackling"),        // Tackle montage playing
    Celebrating     UMETA(DisplayName = "Celebrating"),     // Celebration montage
    KnockedDown     UMETA(DisplayName = "Knocked Down"),    // Foul / collision reaction
    GKDive          UMETA(DisplayName = "GK Dive")          // Goalkeeper only
};

// ---------------------------------------------------------------------------
// EShotAnimVariant — 5 shooting animation variations (GDD 4.4)
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EShotAnimVariant : uint8
{
    NormalShot      UMETA(DisplayName = "Normal Shot"),
    PowerShot       UMETA(DisplayName = "Power Shot"),
    ChipShot        UMETA(DisplayName = "Chip Shot"),
    VolleyShot      UMETA(DisplayName = "Volley Shot"),
    BicycleKick     UMETA(DisplayName = "Bicycle Kick")
};

// ---------------------------------------------------------------------------
// FFootIKData — per-foot IK result computed each tick
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FFootIKData
{
    GENERATED_BODY()

    /** World-space target location for this foot's IK effector */
    UPROPERTY(BlueprintReadOnly)
    FVector EffectorLocation = FVector::ZeroVector;

    /** Foot rotation to align with surface normal */
    UPROPERTY(BlueprintReadOnly)
    FRotator EffectorRotation = FRotator::ZeroRotator;

    /** Alpha 0-1: how much IK is blended in (0 = off, 1 = fully on) */
    UPROPERTY(BlueprintReadOnly)
    float IKAlpha = 0.0f;

    /** True if the foot trace hit the ground this tick */
    UPROPERTY(BlueprintReadOnly)
    bool bFootGrounded = false;
};

// ---------------------------------------------------------------------------
// UMSoccerAnimInstance
// ---------------------------------------------------------------------------
UCLASS(Blueprintable, BlueprintType)
class SOCCERMOBILEPRO_API UMSoccerAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UMSoccerAnimInstance();

    // ---- UE Overrides ------------------------------------------------------
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

    // =========================================================================
    // LOCOMOTION VARIABLES  (read by BlendSpace in AnimGraph)
    // =========================================================================

    /**
     * Movement speed in cm/s.
     * BlendSpace X axis: 0 = Idle, 400 = Walk, 600 = Run, 700+ = Sprint
     */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    float Speed;

    /**
     * Lateral movement direction in degrees (-180..180).
     * BlendSpace Y axis: drives strafe animations.
     */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    float Direction;

    /** True when character velocity > 10 cm/s */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsMoving;

    /** True when sprint is active (from AMSoccerPlayerCharacter) */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsSprinting;

    /** True when character is airborne (jump / bicycle kick wind-up) */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsInAir;

    /** Smoothed locomotion state for transition logic */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    ELocomotionState LocomotionState;

    // =========================================================================
    // BALL CONTROL  (dribbling layer)
    // =========================================================================

    /** True when this player has ball possession */
    UPROPERTY(BlueprintReadOnly, Category = "BallControl")
    bool bHasBallPossession;

    /**
     * Dribble blend weight 0-1.
     * Drives the Layered Blend Per Bone that overlays dribbling arms/torso
     * on top of the locomotion lower body.
     */
    UPROPERTY(BlueprintReadOnly, Category = "BallControl")
    float DribbleBlendWeight;

    // =========================================================================
    // ACTION STATE
    // =========================================================================

    /** Current high-level action state (drives State Machine in AnimGraph) */
    UPROPERTY(BlueprintReadOnly, Category = "ActionState")
    EPlayerActionState ActionState;

    /** Which shot variant is queued (set before playing shoot montage) */
    UPROPERTY(BlueprintReadWrite, Category = "ActionState")
    EShotAnimVariant QueuedShotVariant;

    /** Stamina 0-1, drives fatigue blend (slower animations after 75 min) */
    UPROPERTY(BlueprintReadOnly, Category = "ActionState")
    float StaminaRatio;

    // =========================================================================
    // FOOT IK  (Full-body IK via Control Rig)
    // =========================================================================

    /** IK data for the left foot */
    UPROPERTY(BlueprintReadOnly, Category = "FootIK")
    FFootIKData LeftFootIK;

    /** IK data for the right foot */
    UPROPERTY(BlueprintReadOnly, Category = "FootIK")
    FFootIKData RightFootIK;

    /**
     * Pelvis vertical offset to prevent foot-IK stretching the legs.
     * Applied as a negative Z offset on the root bone.
     */
    UPROPERTY(BlueprintReadOnly, Category = "FootIK")
    float PelvisOffset;

    /** Master IK blend alpha: 0 when airborne, 1 when grounded */
    UPROPERTY(BlueprintReadOnly, Category = "FootIK")
    float FootIKAlpha;

    // =========================================================================
    // MONTAGE REFERENCES  (assign in ABP_FootballPlayer Class Defaults)
    // =========================================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages|Pass")
    TObjectPtr<UAnimMontage> AM_GroundPass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages|Pass")
    TObjectPtr<UAnimMontage> AM_LobPass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages|Tackle")
    TObjectPtr<UAnimMontage> AM_SlidingTackle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages|Tackle")
    TObjectPtr<UAnimMontage> AM_StandingTackle;

    /** Array index = EShotAnimVariant (0=Normal,1=Power,2=Chip,3=Volley,4=BicycleKick) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages|Shoot")
    TArray<TObjectPtr<UAnimMontage>> AM_Shots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages|Celebrate")
    TArray<TObjectPtr<UAnimMontage>> AM_Celebrations;

    // =========================================================================
    // MONTAGE PLAY INTERFACE  (called from BP_FootballPlayer event graph)
    // =========================================================================

    /** Play the correct shoot montage based on QueuedShotVariant */
    UFUNCTION(BlueprintCallable, Category = "Montages")
    void PlayShootMontage(EShotAnimVariant Variant);

    /** Play pass montage (ground or lob) */
    UFUNCTION(BlueprintCallable, Category = "Montages")
    void PlayPassMontage(bool bIsLob);

    /** Play tackle montage (sliding or standing) */
    UFUNCTION(BlueprintCallable, Category = "Montages")
    void PlayTackleMontage(bool bIsSliding);

    /** Play a random celebration from AM_Celebrations array */
    UFUNCTION(BlueprintCallable, Category = "Montages")
    void PlayCelebrationMontage();

    // =========================================================================
    // IK INTERFACE  (called from NativeUpdateAnimation)
    // =========================================================================

    /** Perform line traces and compute FFootIKData for both feet */
    UFUNCTION(BlueprintCallable, Category = "FootIK")
    void UpdateFootIK(float DeltaSeconds);

private:
    // ---- Cached references -------------------------------------------------
    TObjectPtr<AMSoccerPlayerCharacter> OwnerCharacter;
    TObjectPtr<UCharacterMovementComponent> OwnerMovement;

    // ---- IK smoothing internals --------------------------------------------
    FVector  SmoothedLeftFootLoc;
    FVector  SmoothedRightFootLoc;
    float    SmoothedPelvisOffset;

    // ---- Internal helpers --------------------------------------------------
    void UpdateLocomotionState();
    void UpdateBallControlLayer();
    void UpdateActionState();
    void UpdateStamina();

    FFootIKData TraceFootIK(FName FootSocketName, float TraceLength) const;
};
