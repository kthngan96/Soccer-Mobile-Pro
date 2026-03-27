// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// USkillSystem.h
//
// ActorComponent managing the full Skill System for one player.
// Attaches to AMSoccerPlayerCharacter.
//
// Responsibilities:
//   1. Consume gesture events from UMGestureRecognizer
//   2. Star-gate check against owning player's card star rating
//   3. Cooldown management per skill
//   4. Success-chance roll (modified by defender proximity)
//   5. Trigger anim montage on success
//   6. Broadcast result delegates
//
// GDD Reference : Section 4.1–4.5 (Skill & Technique System)
// TECHSPEC Ref  : Section 4.3 (Skill Execution Pipeline)
//
// Blueprint: USkillSystem is added as a component to BP_FootballPlayer.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MSkillTypes.h"
#include "USkillSystem.generated.h"

// ---- Forward Declarations -------------------------------------------------
class AMSoccerPlayerCharacter;
class UMGestureRecognizer;
class UAnimMontage;
class UDataTable;

// ---------------------------------------------------------------------------
// FSkillRuntimeState — per-skill live state tracked during a match
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FSkillRuntimeState
{
    GENERATED_BODY()

    /** Time the skill was last successfully executed */
    UPROPERTY(BlueprintReadOnly)
    float LastExecutedTime = -999.0f;

    /** True while the skill animation is playing */
    UPROPERTY(BlueprintReadOnly)
    bool bIsExecuting = false;

    /** Cumulative success count this match (for analytics / Skill XP) */
    UPROPERTY(BlueprintReadOnly)
    int32 SuccessCount = 0;
};

// ---------------------------------------------------------------------------
// USkillSystem (ActorComponent)
// ---------------------------------------------------------------------------
UCLASS(ClassGroup = "Soccer", BlueprintType, Blueprintable,
       meta = (BlueprintSpawnableComponent))
class SOCCERMOBILEPRO_API USkillSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    USkillSystem();

    // ---- UE Overrides ------------------------------------------------------
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    // =========================================================================
    // CONFIGURATION
    // =========================================================================

    /**
     * DataTable (row type FSkillDefinition) with all skill configs.
     * Assign: DT_SkillDefinitions (Content/Gameplay/Skills/)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Config")
    TObjectPtr<UDataTable> SkillDataTable;

    /**
     * Montage map: ESkillID -> AnimMontage.
     * Assign in BP_FootballPlayer Class Defaults.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Config")
    TMap<ESkillID, TObjectPtr<UAnimMontage>> SkillMontages;

    /**
     * This player's card Skill Stars (1–5).
     * Set from UCardDatabase when the player is instantiated.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Config",
              meta=(ClampMin="1", ClampMax="5"))
    int32 PlayerSkillStars = 3;

    /**
     * Defender proximity radius that scales down success chance.
     * If nearest defender is within this radius, success chance is halved.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Config",
              meta=(ClampMin="50.0"))
    float DefenderInfluenceRadius = 150.0f;  // cm

    // =========================================================================
    // RUNTIME STATE
    // =========================================================================

    /** True while any skill is executing (blocks pass/shoot/new skills) */
    UPROPERTY(BlueprintReadOnly, Category = "Skill|State")
    bool bAnySkillExecuting;

    /** Currently executing skill (None if idle) */
    UPROPERTY(BlueprintReadOnly, Category = "Skill|State")
    ESkillID ActiveSkillID;

    // =========================================================================
    // GESTURE RECOGNIZER REFERENCE
    // =========================================================================

    /**
     * Gesture recognizer instance.
     * Created automatically in BeginPlay; can be replaced from BP.
     */
    UPROPERTY(BlueprintReadWrite, Category = "Skill|Gesture")
    TObjectPtr<UMGestureRecognizer> GestureRecognizer;

    // =========================================================================
    // PRIMARY INTERFACE
    // =========================================================================

    /**
     * Feed a joystick sample into the gesture recognizer.
     * Call from BP_VirtualJoystick at 30Hz.
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    void FeedJoystickSample(const FGestureSample& Sample);

    /**
     * Inject ShootCancel gesture directly (fired from BP_ActionButton_Shoot
     * when shoot is pressed then released in < 0.15s with direction change).
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    void InjectShootCancelGesture();

    /**
     * Attempt to execute a specific skill by ID (bypass gesture, e.g. for tutorial).
     * @return Execution result enum
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    ESkillExecutionResult TryExecuteSkill(ESkillID SkillID);

    /**
     * Check if a skill is currently on cooldown.
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    bool IsSkillOnCooldown(ESkillID SkillID) const;

    /**
     * Get remaining cooldown seconds for a skill (0 if not on cooldown).
     */
    UFUNCTION(BlueprintPure, Category = "Skill")
    float GetCooldownRemaining(ESkillID SkillID) const;

    /**
     * Returns the FSkillDefinition for a given SkillID.
     * @param bFound  Set to true if definition was found in DataTable.
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    FSkillDefinition GetSkillDefinition(ESkillID SkillID, bool& bFound) const;

    /**
     * Set the player's star rating (call from CardDatabase on lineup change).
     */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    void SetPlayerSkillStars(int32 Stars);

    // =========================================================================
    // DELEGATES
    // =========================================================================

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillExecuted, const FSkillExecutionEvent&, Event);
    /**
     * Broadcast whenever a skill attempt is resolved (success or failure).
     * Bind in HUD to show skill feedback UI.
     */
    UPROPERTY(BlueprintAssignable, Category = "Skill|Events")
    FOnSkillExecuted OnSkillExecuted;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillCooldownUpdated, ESkillID, SkillID, float, RemainingSeconds);
    /** Broadcast each tick while a skill is on cooldown (for cooldown HUD ring) */
    UPROPERTY(BlueprintAssignable, Category = "Skill|Events")
    FOnSkillCooldownUpdated OnSkillCooldownUpdated;

    // =========================================================================
    // BLUEPRINT NATIVE EVENTS
    // =========================================================================

    /** Called when a skill succeeds — override in BP to apply gameplay effect */
    UFUNCTION(BlueprintNativeEvent, Category = "Skill")
    void OnSkillSucceeded(ESkillID SkillID);
    virtual void OnSkillSucceeded_Implementation(ESkillID SkillID);

    /** Called when a skill fails (bad luck roll or blocked) */
    UFUNCTION(BlueprintNativeEvent, Category = "Skill")
    void OnSkillFailed(ESkillID SkillID);
    virtual void OnSkillFailed_Implementation(ESkillID SkillID);

    /** Called when a skill montage finishes (reset execution state) */
    UFUNCTION(BlueprintCallable, Category = "Skill")
    void OnSkillMontageEnded(ESkillID FinishedSkill);

private:
    // ---- Cached references -------------------------------------------------
    TObjectPtr<AMSoccerPlayerCharacter> OwnerCharacter;

    // ---- Per-skill runtime state map (keyed by ESkillID as uint8) ----------
    TMap<uint8, FSkillRuntimeState> SkillStates;

    // ---- Internal helpers --------------------------------------------------
    ESkillID GestureToSkill(EGestureType Gesture) const;
    bool CheckStarGate(const FSkillDefinition& Def) const;
    bool CheckCooldown(ESkillID SkillID, const FSkillDefinition& Def) const;
    float ComputeSuccessChance(const FSkillDefinition& Def) const;
    float FindNearestDefenderDistance() const;
    void PlaySkillMontage(ESkillID SkillID, const FSkillDefinition& Def);
    void BroadcastCooldowns(float CurrentTime);
    FSkillRuntimeState& GetOrCreateState(ESkillID SkillID);

    void InitDefaultSkillData();
    TMap<ESkillID, FSkillDefinition> FallbackSkillDefs; // used if DataTable not set
};
