// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MSoccerBall.cpp — Stub implementation
// Chaos Physics integration: Milestone 3

#include "MSoccerBall.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MSoccerPlayerCharacter.h"
#include "Engine/Engine.h"

AMSoccerBall::AMSoccerBall()
    : BallMassKg(0.43f)
    , DragCoefficient(0.47f)   // Standard football Cd
    , SpinFactor(1.0f)
    , AngularVelocity(FVector::ZeroVector)
    , CurrentSurface(EBallSurfaceType::DryGrass)
    , LastTouchTeam(ETeamId::None)
    , bIsInGoal(false)
    , bIsOutOfBounds(false)
{
    PrimaryActorTick.bCanEverTick = true;

    // Collision sphere
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->InitSphereRadius(11.0f);   // 11 cm = FIFA standard
    CollisionSphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    CollisionSphere->SetSimulatePhysics(true);
    CollisionSphere->SetEnableGravity(true);
    CollisionSphere->OnComponentHit.AddDynamic(this, &AMSoccerBall::OnBallHit);
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMSoccerBall::OnBallOverlapBegin);
    SetRootComponent(CollisionSphere);

    // Visual mesh
    BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
    BallMesh->SetupAttachment(CollisionSphere);
    BallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BallMesh->SetCastShadow(true);
}

void AMSoccerBall::BeginPlay()
{
    Super::BeginPlay();
    // TODO Milestone 3: set Chaos physics material per surface type
}

void AMSoccerBall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // TODO Milestone 3: implement full physics tick
    ApplyMagnusForce(DeltaTime);
    ApplyRollingDrag(DeltaTime);
}

// ---------------------------------------------------------------------------
// KickBall
// ---------------------------------------------------------------------------
void AMSoccerBall::KickBall(FVector Direction, float Power, float LaunchAngle, float Spin)
{
    // TODO Milestone 3: map Power(0..1) to impulse magnitude, apply Chaos impulse
    // Detach from player first
    if (PossessingPlayer)
    {
        DetachFromPlayer();
    }
    AngularVelocity = FVector(0.0f, 0.0f, Spin * 500.0f); // lateral spin
    UE_LOG(LogTemp, Log, TEXT("[MSoccerBall] KickBall Power=%.2f Angle=%.1f Spin=%.2f"),
        Power, LaunchAngle, Spin);
}

// ---------------------------------------------------------------------------
// Possession
// ---------------------------------------------------------------------------
void AMSoccerBall::AttachToPlayer(AMSoccerPlayerCharacter* Player)
{
    if (!Player) return;
    PossessingPlayer = Player;
    // TODO Milestone 3: attach to "FootSocket" bone, disable physics sim
    CollisionSphere->SetSimulatePhysics(false);
}

void AMSoccerBall::DetachFromPlayer()
{
    PossessingPlayer = nullptr;
    CollisionSphere->SetSimulatePhysics(true);
}

bool AMSoccerBall::IsAttachedToPlayer() const
{
    return PossessingPlayer != nullptr;
}

bool AMSoccerBall::IsInGoal() const
{
    return bIsInGoal;
}

void AMSoccerBall::ResetToPosition(FVector WorldPosition)
{
    DetachFromPlayer();
    CollisionSphere->SetPhysicsLinearVelocity(FVector::ZeroVector);
    CollisionSphere->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    SetActorLocation(WorldPosition, false, nullptr, ETeleportType::TeleportPhysics);
    bIsInGoal = false;
    bIsOutOfBounds = false;
    AngularVelocity = FVector::ZeroVector;
    UE_LOG(LogTemp, Log, TEXT("[MSoccerBall] Reset to %s"), *WorldPosition.ToString());
}

// ---------------------------------------------------------------------------
// Hit / Overlap callbacks
// ---------------------------------------------------------------------------
void AMSoccerBall::OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                             const FHitResult& Hit)
{
    AMSoccerPlayerCharacter* Player = Cast<AMSoccerPlayerCharacter>(OtherActor);
    if (Player)
    {
        LastTouchPlayer = Player;
        FBallContactEvent Event;
        Event.Player       = Player;
        Event.ContactPoint = Hit.ImpactPoint;
        Event.ImpactNormal = Hit.ImpactNormal;
        Event.ImpactForce  = NormalImpulse.Size();
        OnBallContact.Broadcast(Event);
    }
}

void AMSoccerBall::OnBallOverlapBegin(UPrimitiveComponent* OverlapComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                      bool bFromSweep, const FHitResult& SweepResult)
{
    // TODO Milestone 3: detect GoalVolume tag -> broadcast OnGoalDetected
    // TODO Milestone 3: detect OutOfBoundsVolume tag -> broadcast OnBallOutOfBounds
}

// ---------------------------------------------------------------------------
// Physics helpers — STUBS
// ---------------------------------------------------------------------------
void AMSoccerBall::ApplyMagnusForce(float DeltaTime)
{
    // TODO Milestone 3:
    // FVector Velocity = CollisionSphere->GetPhysicsLinearVelocity();
    // FVector MagnusForce = SpinFactor * FVector::CrossProduct(AngularVelocity, Velocity) * BallMassKg;
    // CollisionSphere->AddForce(MagnusForce);
}

void AMSoccerBall::ApplyRollingDrag(float DeltaTime)
{
    // TODO Milestone 3: apply surface-dependent linear damping via Chaos material
}
