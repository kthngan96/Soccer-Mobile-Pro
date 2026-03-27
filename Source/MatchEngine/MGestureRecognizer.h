// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MGestureRecognizer.h
//
// Stateful gesture recognizer for the joystick touch stream.
// Consumes FGestureSample stream at 30Hz and emits EGestureType events.
//
// Recognized gestures (GDD Section 4.2):
//   Circle       — joystick traces a loop >= 270° arc in <= 0.6s
//   Spin360      — full 360° loop
//   FlickRight   — fast rightward burst (dx > threshold, duration < 0.2s)
//   FlickLeft    — fast leftward burst
//   FlickAtDefender — flick toward nearest defender direction
//   HoldSideways — joystick held > 0.8 laterally for >= 0.3s
//   ShootCancel  — handled externally by input system, injected here
//   SwipeUp / SwipeUpDouble — upward Y burst
//   BackwardFlick / HorizontalSwipe — variants

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MSkillTypes.h"
#include "MGestureRecognizer.generated.h"

// ---------------------------------------------------------------------------
// Gesture recognizer config
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FGestureConfig
{
    GENERATED_BODY()

    /** Max seconds to complete a Circle gesture */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gesture", meta=(ClampMin="0.2"))
    float CircleMaxDuration = 0.6f;

    /** Minimum arc degrees for circle detection */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gesture", meta=(ClampMin="180.0"))
    float CircleMinArcDegrees = 270.0f;

    /** Minimum normalized speed for a flick */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gesture", meta=(ClampMin="0.1"))
    float FlickMinSpeed = 1.8f;

    /** Max seconds for flick duration */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gesture", meta=(ClampMin="0.05"))
    float FlickMaxDuration = 0.20f;

    /** Minimum seconds joystick must be held laterally for HoldSideways */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gesture", meta=(ClampMin="0.1"))
    float SidewaysHoldMinDuration = 0.30f;

    /** Lateral threshold for HoldSideways (|X| > this) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gesture", meta=(ClampMin="0.3"))
    float SidewaysThreshold = 0.80f;

    /** Max sample window kept in ring buffer (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gesture", meta=(ClampMin="0.2"))
    float SampleWindowSeconds = 1.0f;

    /** Dead zone — samples with magnitude below this are ignored */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gesture", meta=(ClampMin="0.0"))
    float DeadZone = 0.12f;
};

// ---------------------------------------------------------------------------
// UMGestureRecognizer
// ---------------------------------------------------------------------------
UCLASS(BlueprintType, Blueprintable)
class SOCCERMOBILEPRO_API UMGestureRecognizer : public UObject
{
    GENERATED_BODY()

public:
    UMGestureRecognizer();

    // =========================================================================
    // CONFIG
    // =========================================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gesture")
    FGestureConfig Config;

    // =========================================================================
    // INPUT
    // =========================================================================

    /**
     * Feed a new joystick sample into the recognizer.
     * Call at 30Hz from BP_FootballPlayer Tick / input widget.
     * @param Sample   Normalized joystick position + delta + timestamp
     */
    UFUNCTION(BlueprintCallable, Category = "Gesture")
    void AddSample(const FGestureSample& Sample);

    /**
     * Inject a discrete gesture event from the input system.
     * Used for gestures that are detected externally (e.g., ShootCancel from
     * the button widget rather than the joystick).
     */
    UFUNCTION(BlueprintCallable, Category = "Gesture")
    void InjectDiscreteGesture(EGestureType Gesture);

    /** Reset the sample buffer and all running gesture state */
    UFUNCTION(BlueprintCallable, Category = "Gesture")
    void Reset();

    // =========================================================================
    // OUTPUT
    // =========================================================================

    /**
     * Poll the latest recognized gesture.
     * Returns EGestureType::None if no gesture completed since last poll.
     * Calling this clears the pending gesture.
     */
    UFUNCTION(BlueprintCallable, Category = "Gesture")
    EGestureType PollGesture();

    /** Non-consuming peek — does not clear pending gesture */
    UFUNCTION(BlueprintPure, Category = "Gesture")
    EGestureType PeekGesture() const { return PendingGesture; }

    // =========================================================================
    // DELEGATE
    // =========================================================================

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGestureRecognized, EGestureType, Gesture);
    /** Broadcast immediately when a gesture completes */
    UPROPERTY(BlueprintAssignable, Category = "Gesture")
    FOnGestureRecognized OnGestureRecognized;

    // =========================================================================
    // DEBUG
    // =========================================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bDrawDebug = false;

private:
    // ---- Sample ring buffer -----------------------------------------------
    TArray<FGestureSample>  SampleBuffer;
    EGestureType            PendingGesture;

    // ---- Per-gesture running state ----------------------------------------
    // Circle / Spin360
    float   CircleStartTime;
    float   TotalArcDegrees;
    float   LastAngleDeg;
    bool    bCircleActive;
    int32   FullRotationCount;

    // Flick
    FVector2D FlickStartPos;
    float     FlickStartTime;
    bool      bFlickActive;

    // HoldSideways
    float     SidewaysHoldStartTime;
    bool      bSidewaysHoldActive;
    float     SidewaysSign; // +1 = right, -1 = left

    // SwipeUp
    float     SwipeUpStartTime;
    bool      bSwipeUpActive;
    int32     SwipeUpCount;

    // BackwardFlick
    bool      bBackwardFlickFirst;
    float     BackwardFlickLastTime;

    // ---- Internal helpers -------------------------------------------------
    void PruneOldSamples(float CurrentTime);
    void AnalyzeSamples(const FGestureSample& NewSample);
    void TryRecognizeCircle(const FGestureSample& Sample);
    void TryRecognizeFlick(const FGestureSample& Sample);
    void TryRecognizeHoldSideways(const FGestureSample& Sample);
    void TryRecognizeSwipeUp(const FGestureSample& Sample);
    void TryRecognizeBackwardFlick(const FGestureSample& Sample);
    void EmitGesture(EGestureType Gesture);

    float AngleBetween(FVector2D A, FVector2D B) const;
};
