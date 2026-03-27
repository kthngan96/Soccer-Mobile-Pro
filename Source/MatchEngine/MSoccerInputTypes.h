// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerInputTypes.h
//
// Shared input-related types used across MatchEngine, UI, and Analytics.
// Include this header instead of MSoccerPlayerCharacter.h where only type
// definitions are needed (reduces compile dependencies).

#pragma once

#include "CoreMinimal.h"
#include "MSoccerInputTypes.generated.h"

/**
 * Snapshot of all player input for one network tick (33ms / 30Hz).
 * Matches the Client Input Packet schema in TECHSPEC Section 5.1.
 * Sent from client to server; server is authoritative.
 */
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FClientInputFrame
{
    GENERATED_BODY()

    /** Monotonically increasing frame counter for jitter detection */
    UPROPERTY(BlueprintReadWrite, Category = "Input")
    int32 FrameNumber = 0;

    /** Joystick X axis [-1, 1] */
    UPROPERTY(BlueprintReadWrite, Category = "Input")
    float MoveAxisX = 0.0f;

    /** Joystick Y axis [-1, 1] */
    UPROPERTY(BlueprintReadWrite, Category = "Input")
    float MoveAxisY = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Input")
    bool bButtonPass = false;

    UPROPERTY(BlueprintReadWrite, Category = "Input")
    bool bButtonShoot = false;

    UPROPERTY(BlueprintReadWrite, Category = "Input")
    bool bButtonThroughBall = false;

    UPROPERTY(BlueprintReadWrite, Category = "Input")
    bool bButtonCross = false;

    UPROPERTY(BlueprintReadWrite, Category = "Input")
    bool bButtonTackle = false;

    UPROPERTY(BlueprintReadWrite, Category = "Input")
    bool bSprintActive = false;

    /** How long pass has been held this frame (used server-side to determine lob) */
    UPROPERTY(BlueprintReadWrite, Category = "Input")
    float PassHoldSeconds = 0.0f;

    /** How long shoot has been held this frame (used server-side for power shot) */
    UPROPERTY(BlueprintReadWrite, Category = "Input")
    float ShootHoldSeconds = 0.0f;

    /** Peak swipe-up velocity during shoot press (chip shot detection) */
    UPROPERTY(BlueprintReadWrite, Category = "Input")
    float ShootSwipeUpVelocity = 0.0f;

    /** Raw gesture samples for skill detection (TECHSPEC Section 5.1) */
    UPROPERTY(BlueprintReadWrite, Category = "Input")
    TArray<FVector2D> GestureSamples;

    /** Client-side timestamp for latency calculation */
    UPROPERTY(BlueprintReadWrite, Category = "Input")
    int64 TimestampClientMs = 0;
};
