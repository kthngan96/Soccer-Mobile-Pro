// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSkillTypes.h
//
// Shared skill system data types used across MatchEngine, UI, and Analytics.
// Keep this header lightweight — no UObject includes.
//
// GDD Reference: Section 4 (Skill & Technique System)

#pragma once

#include "CoreMinimal.h"
#include "MSkillTypes.generated.h"

// ---------------------------------------------------------------------------
// ESkillID — all skills defined in GDD Section 4.2
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ESkillID : uint8
{
    None              UMETA(DisplayName = "None"),

    // ---- 5 Starter Skills (GDD 4.2) ----------------------------------------
    StepOver          UMETA(DisplayName = "Step Over"),          // 3★  circle
    BallRoll          UMETA(DisplayName = "Ball Roll"),          // 3★  hold side + tap
    FakeShot          UMETA(DisplayName = "Fake Shot"),          // 2★  shoot + cancel
    Nutmeg            UMETA(DisplayName = "Nutmeg"),             // 3★  flick at defender
    Roulette          UMETA(DisplayName = "Roulette"),           // 4★  360 spin

    // ---- Extended skills (available for future milestones) -----------------
    DoubleStepOver    UMETA(DisplayName = "Double Step Over"),   // 4★
    Elastico          UMETA(DisplayName = "Elastico"),           // 5★
    ReverseElastico   UMETA(DisplayName = "Reverse Elastico"),   // 5★
    HeelToHeel        UMETA(DisplayName = "Heel to Heel"),       // 4★
    LaCroqueta        UMETA(DisplayName = "La Croqueta"),        // 4★
    RabonaFake        UMETA(DisplayName = "Rabona Fake"),        // 5★
    SombreroFlick     UMETA(DisplayName = "Sombrero Flick"),     // 4★
    RainbowFlick      UMETA(DisplayName = "Rainbow Flick"),      // 5★
    ScoopTurn         UMETA(DisplayName = "Scoop Turn"),         // 3★
    SpinShot          UMETA(DisplayName = "Spin Shot"),          // 4★
};

// ---------------------------------------------------------------------------
// EGestureType — raw gesture shapes recognized by UGestureRecognizer
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EGestureType : uint8
{
    None              UMETA(DisplayName = "None"),
    Circle            UMETA(DisplayName = "Circle"),           // StepOver
    DoubleCircle      UMETA(DisplayName = "Double Circle"),    // DoubleStepOver
    FlickRight        UMETA(DisplayName = "Flick Right"),      // Elastico first
    FlickLeft         UMETA(DisplayName = "Flick Left"),       // Elastico second
    FlickAtDefender   UMETA(DisplayName = "Flick At Defender"),// Nutmeg
    Spin360           UMETA(DisplayName = "360 Spin"),         // Roulette
    HoldSideways      UMETA(DisplayName = "Hold Sideways"),    // BallRoll pre-condition
    ShootCancel       UMETA(DisplayName = "Shoot + Cancel"),   // FakeShot
    SwipeUp           UMETA(DisplayName = "Swipe Up"),         // Sombrero
    SwipeUpDouble     UMETA(DisplayName = "Swipe Up x2"),      // Sombrero x2
    BackwardFlick     UMETA(DisplayName = "Backward Flick"),   // HeelToHeel
    HorizontalSwipe   UMETA(DisplayName = "Horizontal Swipe"), // La Croqueta
    FullRotation      UMETA(DisplayName = "Full Rotation"),    // Roulette variant
};

// ---------------------------------------------------------------------------
// ESkillExecutionResult
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ESkillExecutionResult : uint8
{
    Success           UMETA(DisplayName = "Success"),
    StarGateFailed    UMETA(DisplayName = "Star Gate Failed"),  // card star rating too low
    OnCooldown        UMETA(DisplayName = "On Cooldown"),
    NoBallPossession  UMETA(DisplayName = "No Ball Possession"),
    NotRecognized     UMETA(DisplayName = "Not Recognized"),
    Blocked           UMETA(DisplayName = "Blocked"),           // defender too close
};

// ---------------------------------------------------------------------------
// FSkillDefinition — data row for USkillDataTable
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FSkillDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    ESkillID SkillID = ESkillID::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FString DisplayName;

    /** Minimum player card Skill Stars required (1–5) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta=(ClampMin="1", ClampMax="5"))
    int32 RequiredStars = 1;

    /** Gesture pattern that triggers this skill */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    EGestureType RequiredGesture = EGestureType::None;

    /** Cooldown in seconds after execution */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta=(ClampMin="0.5"))
    float CooldownSeconds = 3.0f;

    /** Base success chance (0–1). Reduced by defender proximity. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta=(ClampMin="0.0", ClampMax="1.0"))
    float BaseSuccessChance = 0.85f;

    /** Montage slot name: "FullBody" or "UpperBody" */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FName MontageSlot = TEXT("UpperBody");

    /** True if the ball stays attached during the skill (e.g., StepOver) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    bool bBallAttachedDuringSkill = true;

    /** True = skill can only trigger while moving */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    bool bRequiresMovement = false;
};

// ---------------------------------------------------------------------------
// FGestureSample — one touch-delta sample from the joystick (30Hz)
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FGestureSample
{
    GENERATED_BODY()

    /** Normalized joystick position at this sample (-1..1 per axis) */
    UPROPERTY(BlueprintReadWrite)
    FVector2D JoystickPosition = FVector2D::ZeroVector;

    /** Delta from previous sample */
    UPROPERTY(BlueprintReadWrite)
    FVector2D Delta = FVector2D::ZeroVector;

    /** Timestamp (GetWorld()->GetTimeSeconds()) */
    UPROPERTY(BlueprintReadWrite)
    float Timestamp = 0.0f;
};

// ---------------------------------------------------------------------------
// FSkillExecutionEvent — broadcast when a skill is attempted
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FSkillExecutionEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    ESkillID SkillID = ESkillID::None;

    UPROPERTY(BlueprintReadOnly)
    ESkillExecutionResult Result = ESkillExecutionResult::NotRecognized;

    /** Actual success after success-chance roll (only meaningful if Result==Success) */
    UPROPERTY(BlueprintReadOnly)
    bool bExecutionSucceeded = false;

    UPROPERTY(BlueprintReadOnly)
    float ExecutionTimestamp = 0.0f;
};
