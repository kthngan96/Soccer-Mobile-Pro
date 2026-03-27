// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerBall.h
//
// Physics-based football actor.
// Powered by UE5 Chaos Physics for spin, drag, curve, and surface interaction.
//
// GDD Reference : Section 13.2 (BP_Ball), Section 2.1.1 (Match Engine)
// GDD Reference : Section 10.1 (Ball Physics — spin, backspin, curve, wet drag)
// TECHSPEC Ref  : Section 3.4 (Ball Physics), Section 3.5 (Possession Tracking)
//
// Blueprint subclass: BP_Ball (Content/Gameplay/)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MSoccerGameMode.h"   // ETeamId
#include "MSoccerBall.generated.h"

// ---- Forward Declarations -------------------------------------------------
class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class AMSoccerPlayerCharacter;

// ---------------------------------------------------------------------------
// ESurfaceType — affects drag and bounce (GDD 10.1)
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EBallSurfaceType : uint8
{
    DryGrass    UMETA(DisplayName = "Dry Grass"),
    WetGrass    UMETA(DisplayName = "Wet Grass"),
    DryTurf     UMETA(DisplayName = "Dry Turf"),
    Indoor      UMETA(DisplayName = "Indoor")
};

// ---------------------------------------------------------------------------
// FBallContactEvent — broadcast when ball is touched by a player
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FBallContactEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AMSoccerPlayerCharacter> Player;

    UPROPERTY(BlueprintReadOnly)
    FVector ContactPoint;

    UPROPERTY(BlueprintReadOnly)
    FVector ImpactNormal;

    UPROPERTY(BlueprintReadOnly)
    float ImpactForce;
};

// ---------------------------------------------------------------------------
// AMSoccerBall  (GDD 13.3: MSoccerBall — extends AActor)
// ---------------------------------------------------------------------------
UCLASS(Blueprintable, BlueprintType)
class SOCCERMOBILEPRO_API AMSoccerBall : public AActor
{
    GENERATED_BODY()

public:
    AMSoccerBall();

    // ---- UE Overrides ------------------------------------------------------
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // =========================================================================
    // COMPONENTS
    // =========================================================================

    /** Collision sphere (radius 11 cm — standard football) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> CollisionSphere;

    /** Visual mesh — swappable ball skin */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> BallMesh;

    // =========================================================================
    // PHYSICS PROPERTIES (GDD 10.1 + TECHSPEC 3.4)
    // =========================================================================

    /** Ball mass in kg (standard: 0.43 kg) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta=(ClampMin="0.3",ClampMax="0.6"))
    float BallMassKg;

    /** Air drag coefficient (affects aerial trajectory) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta=(ClampMin="0.0",ClampMax="1.0"))
    float DragCoefficient;

    /** Spin factor — multiplier for Magnus force on curling shots */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    float SpinFactor;

    /** Angular velocity applied to ball for backspin / topspin effects */
    UPROPERTY(BlueprintReadWrite, Category = "Physics")
    FVector AngularVelocity;

    /** Current surface type — affects bounce restitution and rolling drag */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    EBallSurfaceType CurrentSurface;

    // =========================================================================
    // POSSESSION STATE (TECHSPEC 3.5)
    // =========================================================================

    /** Player currently in possession of this ball (null = loose ball) */
    UPROPERTY(BlueprintReadOnly, Category = "Possession")
    TObjectPtr<AMSoccerPlayerCharacter> PossessingPlayer;

    /** Team that last touched the ball */
    UPROPERTY(BlueprintReadOnly, Category = "Possession")
    ETeamId LastTouchTeam;

    /** Player that last touched the ball (for assist tracking) */
    UPROPERTY(BlueprintReadOnly, Category = "Possession")
    TObjectPtr<AMSoccerPlayerCharacter> LastTouchPlayer;

    // =========================================================================
    // SHOT / PASS INTERFACE
    // =========================================================================

    /**
     * Apply an impulse to kick the ball toward a target direction.
     * @param Direction   Normalized world direction
     * @param Power       0.0 – 1.0 normalised power (maps to impulse magnitude)
     * @param LaunchAngle Degrees above horizontal (0 = ground pass, 30+ = lob)
     * @param Spin        Lateral spin value for curve (-1 left, +1 right)
     */
    UFUNCTION(BlueprintCallable, Category = "Ball|Actions")
    void KickBall(FVector Direction, float Power, float LaunchAngle, float Spin);

    /** Instantly attach ball to a player's foot socket (for possession animation) */
    UFUNCTION(BlueprintCallable, Category = "Ball|Possession")
    void AttachToPlayer(AMSoccerPlayerCharacter* Player);

    /** Release ball from player control back to physics simulation */
    UFUNCTION(BlueprintCallable, Category = "Ball|Possession")
    void DetachFromPlayer();

    /** Returns true if ball is currently attached to a player */
    UFUNCTION(BlueprintPure, Category = "Ball|Possession")
    bool IsAttachedToPlayer() const;

    /** Returns true if ball is inside either goal volume */
    UFUNCTION(BlueprintPure, Category = "Ball|State")
    bool IsInGoal() const;

    /** Teleport ball to world position and zero velocity (for kick-off reset) */
    UFUNCTION(BlueprintCallable, Category = "Ball|Actions")
    void ResetToPosition(FVector WorldPosition);

    // =========================================================================
    // DELEGATES
    // =========================================================================

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallContact, const FBallContactEvent&, ContactEvent);
    /** Fired every time a player touches the ball */
    UPROPERTY(BlueprintAssignable, Category = "Ball|Events")
    FOnBallContact OnBallContact;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGoalDetected, ETeamId, ScoringTeam, AMSoccerPlayerCharacter*, Scorer);
    /** Fired when ball fully crosses goal line */
    UPROPERTY(BlueprintAssignable, Category = "Ball|Events")
    FOnGoalDetected OnGoalDetected;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallOutOfBounds, FVector, ExitPoint);
    /** Fired when ball exits the pitch boundary */
    UPROPERTY(BlueprintAssignable, Category = "Ball|Events")
    FOnBallOutOfBounds OnBallOutOfBounds;

protected:
    // ---- Overlap / Hit callbacks -------------------------------------------
    UFUNCTION()
    void OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
                   UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                   const FHitResult& Hit);

    UFUNCTION()
    void OnBallOverlapBegin(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                            bool bFromSweep, const FHitResult& SweepResult);

    /** Apply Magnus force (spin curve) each physics tick */
    void ApplyMagnusForce(float DeltaTime);

    /** Apply surface-dependent rolling drag */
    void ApplyRollingDrag(float DeltaTime);

private:
    bool bIsInGoal;
    bool bIsOutOfBounds;
};
