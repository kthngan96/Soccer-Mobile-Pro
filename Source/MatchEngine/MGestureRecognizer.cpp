// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MGestureRecognizer.cpp

#include "MGestureRecognizer.h"
#include "Kismet/KismetMathLibrary.h"

UMGestureRecognizer::UMGestureRecognizer()
    : PendingGesture(EGestureType::None)
    , CircleStartTime(0.0f)
    , TotalArcDegrees(0.0f)
    , LastAngleDeg(0.0f)
    , bCircleActive(false)
    , FullRotationCount(0)
    , FlickStartPos(FVector2D::ZeroVector)
    , FlickStartTime(0.0f)
    , bFlickActive(false)
    , SidewaysHoldStartTime(0.0f)
    , bSidewaysHoldActive(false)
    , SidewaysSign(1.0f)
    , SwipeUpStartTime(0.0f)
    , bSwipeUpActive(false)
    , SwipeUpCount(0)
    , bBackwardFlickFirst(false)
    , BackwardFlickLastTime(-999.0f)
{
}

// ---------------------------------------------------------------------------
void UMGestureRecognizer::AddSample(const FGestureSample& Sample)
{
    if (Sample.JoystickPosition.Size() < Config.DeadZone) return;

    PruneOldSamples(Sample.Timestamp);
    SampleBuffer.Add(Sample);
    AnalyzeSamples(Sample);
}

void UMGestureRecognizer::InjectDiscreteGesture(EGestureType Gesture)
{
    EmitGesture(Gesture);
}

void UMGestureRecognizer::Reset()
{
    SampleBuffer.Empty();
    PendingGesture      = EGestureType::None;
    bCircleActive       = false;
    TotalArcDegrees     = 0.0f;
    FullRotationCount   = 0;
    bFlickActive        = false;
    bSidewaysHoldActive = false;
    bSwipeUpActive      = false;
    SwipeUpCount        = 0;
    bBackwardFlickFirst = false;
}

EGestureType UMGestureRecognizer::PollGesture()
{
    const EGestureType G = PendingGesture;
    PendingGesture = EGestureType::None;
    return G;
}

// ---------------------------------------------------------------------------
void UMGestureRecognizer::PruneOldSamples(float CurrentTime)
{
    SampleBuffer.RemoveAll([&](const FGestureSample& S)
    {
        return (CurrentTime - S.Timestamp) > Config.SampleWindowSeconds;
    });
}

void UMGestureRecognizer::AnalyzeSamples(const FGestureSample& NewSample)
{
    TryRecognizeCircle(NewSample);
    TryRecognizeFlick(NewSample);
    TryRecognizeHoldSideways(NewSample);
    TryRecognizeSwipeUp(NewSample);
    TryRecognizeBackwardFlick(NewSample);
}

// ===========================================================================
// CIRCLE / SPIN-360 (GDD: StepOver = circle; Roulette = full 360)
// ===========================================================================
void UMGestureRecognizer::TryRecognizeCircle(const FGestureSample& Sample)
{
    const FVector2D Pos = Sample.JoystickPosition;

    if (!bCircleActive)
    {
        // Start circle tracking when joystick is in outer ring
        if (Pos.Size() > 0.65f)
        {
            bCircleActive    = true;
            CircleStartTime  = Sample.Timestamp;
            TotalArcDegrees  = 0.0f;
            FullRotationCount = 0;
            LastAngleDeg     = FMath::RadiansToDegrees(FMath::Atan2(Pos.Y, Pos.X));
        }
        return;
    }

    // Drop out if joystick returns to center
    if (Pos.Size() < 0.4f)
    {
        bCircleActive = false;
        TotalArcDegrees = 0.0f;
        return;
    }

    const float CurrentAngle = FMath::RadiansToDegrees(FMath::Atan2(Pos.Y, Pos.X));
    float DeltaAngle = CurrentAngle - LastAngleDeg;

    // Normalize delta to [-180, 180]
    if (DeltaAngle >  180.0f) DeltaAngle -= 360.0f;
    if (DeltaAngle < -180.0f) DeltaAngle += 360.0f;

    TotalArcDegrees += FMath::Abs(DeltaAngle);
    LastAngleDeg     = CurrentAngle;

    const float Elapsed = Sample.Timestamp - CircleStartTime;

    // Check for full 360 (Roulette / Spin360)
    if (TotalArcDegrees >= 360.0f && Elapsed <= Config.CircleMaxDuration)
    {
        EmitGesture(EGestureType::Spin360);
        bCircleActive   = false;
        TotalArcDegrees = 0.0f;
        return;
    }

    // Check for partial circle >= CircleMinArcDegrees (StepOver)
    if (TotalArcDegrees >= Config.CircleMinArcDegrees && Elapsed <= Config.CircleMaxDuration)
    {
        EmitGesture(EGestureType::Circle);
        bCircleActive   = false;
        TotalArcDegrees = 0.0f;
        return;
    }

    // Timeout
    if (Elapsed > Config.CircleMaxDuration)
    {
        bCircleActive   = false;
        TotalArcDegrees = 0.0f;
    }
}

// ===========================================================================
// FLICK (GDD: Nutmeg = flick at defender; Elastico = right+left sequence)
// ===========================================================================
void UMGestureRecognizer::TryRecognizeFlick(const FGestureSample& Sample)
{
    const float Speed = Sample.Delta.Size() / FMath::Max(0.001f, 1.0f / 30.0f); // approx vel

    if (!bFlickActive)
    {
        if (Speed >= Config.FlickMinSpeed)
        {
            bFlickActive  = true;
            FlickStartPos = Sample.JoystickPosition;
            FlickStartTime = Sample.Timestamp;
        }
        return;
    }

    const float Elapsed = Sample.Timestamp - FlickStartTime;

    if (Elapsed > Config.FlickMaxDuration)
    {
        // Classify flick direction
        const FVector2D Dir = (Sample.JoystickPosition - FlickStartPos).GetSafeNormal();

        if (Dir.X > 0.6f)       EmitGesture(EGestureType::FlickRight);
        else if (Dir.X < -0.6f) EmitGesture(EGestureType::FlickLeft);
        else if (Dir.Y < -0.6f) EmitGesture(EGestureType::FlickAtDefender);
        else if (Dir.Y > 0.6f)  EmitGesture(EGestureType::BackwardFlick);
        else                     EmitGesture(EGestureType::HorizontalSwipe);

        bFlickActive = false;
    }
}

// ===========================================================================
// HOLD SIDEWAYS (GDD: BallRoll = hold joystick sideways)
// ===========================================================================
void UMGestureRecognizer::TryRecognizeHoldSideways(const FGestureSample& Sample)
{
    const FVector2D Pos = Sample.JoystickPosition;
    const bool bLateral = FMath::Abs(Pos.X) > Config.SidewaysThreshold &&
                          FMath::Abs(Pos.Y) < 0.3f;

    if (bLateral)
    {
        if (!bSidewaysHoldActive)
        {
            bSidewaysHoldActive    = true;
            SidewaysHoldStartTime  = Sample.Timestamp;
            SidewaysSign           = FMath::Sign(Pos.X);
        }
        else
        {
            const float Held = Sample.Timestamp - SidewaysHoldStartTime;
            if (Held >= Config.SidewaysHoldMinDuration)
            {
                EmitGesture(EGestureType::HoldSideways);
                bSidewaysHoldActive = false; // Reset to avoid spamming
            }
        }
    }
    else
    {
        bSidewaysHoldActive = false;
    }
}

// ===========================================================================
// SWIPE UP (GDD: SombreroFlick = swipe up twice; ChipShot = swipe up once)
// ===========================================================================
void UMGestureRecognizer::TryRecognizeSwipeUp(const FGestureSample& Sample)
{
    const float DY = Sample.Delta.Y;

    if (!bSwipeUpActive && DY < -0.4f)  // Y negative = upward on screen
    {
        bSwipeUpActive  = true;
        SwipeUpStartTime = Sample.Timestamp;
    }
    else if (bSwipeUpActive)
    {
        const float Elapsed = Sample.Timestamp - SwipeUpStartTime;
        if (Elapsed > 0.25f)
        {
            SwipeUpCount++;
            if (SwipeUpCount >= 2)
            {
                EmitGesture(EGestureType::SwipeUpDouble);
                SwipeUpCount = 0;
            }
            else
            {
                EmitGesture(EGestureType::SwipeUp);
            }
            bSwipeUpActive = false;
        }
    }
}

// ===========================================================================
// BACKWARD FLICK x2 (GDD: HeelToHeel = double backward tap)
// ===========================================================================
void UMGestureRecognizer::TryRecognizeBackwardFlick(const FGestureSample& Sample)
{
    const float DY = Sample.Delta.Y;

    if (DY > 0.4f)  // Y positive = backward (toward player's back)
    {
        const float Now = Sample.Timestamp;
        if (!bBackwardFlickFirst)
        {
            bBackwardFlickFirst   = true;
            BackwardFlickLastTime = Now;
        }
        else
        {
            const float Gap = Now - BackwardFlickLastTime;
            if (Gap < 0.35f)
            {
                EmitGesture(EGestureType::BackwardFlick);
                bBackwardFlickFirst = false;
            }
            else
            {
                // Too slow — restart
                BackwardFlickLastTime = Now;
            }
        }
    }
}

// ---------------------------------------------------------------------------
void UMGestureRecognizer::EmitGesture(EGestureType Gesture)
{
    PendingGesture = Gesture;
    OnGestureRecognized.Broadcast(Gesture);

    if (bDrawDebug)
    {
        UE_LOG(LogTemp, Log, TEXT("[MGestureRecognizer] Gesture: %s"),
            *UEnum::GetValueAsString(Gesture));
    }
}

float UMGestureRecognizer::AngleBetween(FVector2D A, FVector2D B) const
{
    return FMath::RadiansToDegrees(FMath::Acos(
        FVector2D::DotProduct(A.GetSafeNormal(), B.GetSafeNormal())));
}
