// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// UEnhancementComponent.h
//
// ActorComponent that manages all card enhancement operations:
//   - TryEnhanceCard()     : consume EP + Gold, roll success, apply boost
//   - ApplyStatBoost()     : stat increments differ for GK vs outfield
//   - Evolution check      : marks card evolution-ready at +10
//   - Breakthrough logic   : guarantees success when a Breakthrough Card is used
//   - Downgrade protection : on failure, level stays (no regression)
//   - Slot unlocks         : trait at +5, skill at +7
//
// GDD Reference  : Section 3.3 (Enhancement System)
// TECHSPEC Reference: Section 2   (Enhancement & Evolution Full Spec)
//
// Attach to: BP_CardManager or any Actor that owns the player's card collection.
// TODO Milestone-3: Wire to backend save system after local implementation passes QA.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MPlayerCardTypes.h"
#include "UEnhancementComponent.generated.h"

// ---- Forward Declarations -------------------------------------------------
class UCardDatabase;

// ---------------------------------------------------------------------------
// FEnhancementAttemptResult — rich result object returned by TryEnhanceCard
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FEnhancementAttemptResult
{
    GENERATED_BODY()

    /** Outcome enum for Blueprint switch nodes */
    UPROPERTY(BlueprintReadOnly, Category = "Enhancement")
    EEnhancementResult Result = EEnhancementResult::Error_InvalidCard;

    /** Enhancement level of the card AFTER the attempt */
    UPROPERTY(BlueprintReadOnly, Category = "Enhancement")
    int32 NewEnhancementLevel = 0;

    /** EP actually deducted (0 if attempt was rejected pre-deduction) */
    UPROPERTY(BlueprintReadOnly, Category = "Enhancement")
    int32 EPSpent = 0;

    /** Gold actually deducted */
    UPROPERTY(BlueprintReadOnly, Category = "Enhancement")
    int64 GoldSpent = 0;

    /** True if a new slot (trait or skill) was unlocked this attempt */
    UPROPERTY(BlueprintReadOnly, Category = "Enhancement")
    bool bNewSlotUnlocked = false;

    /** True if the card is now evolution-ready (+10) */
    UPROPERTY(BlueprintReadOnly, Category = "Enhancement")
    bool bEvolutionUnlocked = false;

    /** Random roll that determined success [0.0, 1.0] — exposed for debug UI */
    UPROPERTY(BlueprintReadOnly, Category = "Enhancement|Debug")
    float DebugRollValue = 0.0f;
};

// ---------------------------------------------------------------------------
// UEnhancementComponent
// ---------------------------------------------------------------------------
UCLASS(ClassGroup = (PlayerCard), meta = (BlueprintSpawnableComponent),
    BlueprintType, Blueprintable)
class SOCCERMOBILEPRO_API UEnhancementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnhancementComponent();

    // -------------------------------------------------------------------------
    // Dependencies — set in Blueprint defaults or via code before first use
    // -------------------------------------------------------------------------

    /**
     * Reference to the game's master card database.
     * Set this in the owning Blueprint's construction or via GameInstance.
     * TODO Milestone-2: Replace with AssetManager async-load.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhancement|Config")
    TObjectPtr<UCardDatabase> CardDatabase;

    // -------------------------------------------------------------------------
    // Currency accessors — implement these in Blueprint or subclass.
    // The component calls them before deducting currency to validate funds.
    // -------------------------------------------------------------------------

    /**
     * Override in Blueprint/subclass to return the player's current EP balance.
     * Default stub returns 0 — production must override.
     * TODO Milestone-2: Wire to UPlayerInventorySubsystem::GetEP()
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Enhancement|Currency")
    int32 GetCurrentEP() const;
    virtual int32 GetCurrentEP_Implementation() const { return 0; }

    /**
     * Override in Blueprint/subclass to return the player's current Gold balance.
     * Default stub returns 0 — production must override.
     * TODO Milestone-2: Wire to UPlayerInventorySubsystem::GetGold()
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Enhancement|Currency")
    int64 GetCurrentGold() const;
    virtual int64 GetCurrentGold_Implementation() const { return 0; }

    /**
     * Override to deduct EP from the player's account.
     * Must be authoritative (server-side in multiplayer, local in single-player).
     * TODO Milestone-2: Wire to UPlayerInventorySubsystem::SpendEP()
     */
    UFUNCTION(BlueprintNativeEvent, Category = "Enhancement|Currency")
    void DeductEP(int32 Amount);
    virtual void DeductEP_Implementation(int32 Amount) {};

    /**
     * Override to deduct Gold from the player's account.
     * TODO Milestone-2: Wire to UPlayerInventorySubsystem::SpendGold()
     */
    UFUNCTION(BlueprintNativeEvent, Category = "Enhancement|Currency")
    void DeductGold(int64 Amount);
    virtual void DeductGold_Implementation(int64 Amount) {};

    // -------------------------------------------------------------------------
    // Core API
    // -------------------------------------------------------------------------

    /**
     * Attempt to enhance a card from its current level to CurrentLevel+1.
     *
     * Flow (TECHSPEC Section 2.2):
     *  1. Validate card and CardDatabase references.
     *  2. Reject if already at +10.
     *  3. Fetch enhancement row for (CurrentLevel+1) from EnhancementTable.
     *  4. Validate EP and Gold balances.
     *  5. Deduct EP and Gold (consumed whether success or failure).
     *  6. Roll success:
     *       - bUseBreakthroughCard == true  → guaranteed success (100%)
     *       - otherwise                     → FMath::FRand() < SuccessRate
     *  7a. On success: call ApplyStatBoost(), update level, check slot unlocks,
     *       check evolution-ready at +10.
     *  7b. On failure: downgrade-protection — level stays unchanged.
     *  8. Return FEnhancementAttemptResult with full details.
     *
     * @param CardInstance          The mutable card to enhance (modified in place).
     * @param bUseBreakthroughCard  If true, one Breakthrough Card is consumed for
     *                              guaranteed success (TECHSPEC 2.2.2).
     *                              The caller is responsible for removing the
     *                              Breakthrough Card from inventory BEFORE calling.
     * @return FEnhancementAttemptResult — query .Result for EEnhancementResult.
     */
    UFUNCTION(BlueprintCallable, Category = "Enhancement")
    FEnhancementAttemptResult TryEnhanceCard(
        UPARAM(ref) FCardInstance& CardInstance,
        bool bUseBreakthroughCard = false);

    /**
     * Apply a stat boost to a card's CurrentAttributes.
     *
     * Rules (TECHSPEC Section 2.3):
     *   Outfield (+1 each per level):
     *     Pace, Shooting, Passing, Dribbling, Defending, Physical
     *   Goalkeeper (+2 each per level):
     *     GKDiving, GKHandling, GKKicking, GKPositioning, GKReflexes
     *     (Overall recalculated as average of 5 GK attrs)
     *   All cards: Overall recalculated after boost.
     *   Stats are clamped to [0, 99].
     *
     * @param CardInstance  The card instance whose CurrentAttributes will be mutated.
     */
    UFUNCTION(BlueprintCallable, Category = "Enhancement")
    void ApplyStatBoost(UPARAM(ref) FCardInstance& CardInstance);

    /**
     * Check and apply slot unlocks for the given card's current enhancement level.
     *
     * Unlock thresholds (TECHSPEC Section 2.2.3):
     *   +5  → bTraitSlotUnlocked = true
     *   +7  → bSkillSlotUnlocked = true
     *   +10 → bEvolutionReady    = true
     *
     * Idempotent — safe to call multiple times on the same card.
     *
     * @param CardInstance  Modified in place.
     * @return True if at least one new unlock occurred this call.
     */
    UFUNCTION(BlueprintCallable, Category = "Enhancement")
    bool CheckAndApplySlotUnlocks(UPARAM(ref) FCardInstance& CardInstance);

    /**
     * Returns the EP cost for enhancing to TargetLevel.
     * Returns -1 if TargetLevel is out of range or CardDatabase is null.
     */
    UFUNCTION(BlueprintPure, Category = "Enhancement")
    int32 GetEPCostForLevel(int32 TargetLevel) const;

    /**
     * Returns the Gold cost for enhancing to TargetLevel.
     * Returns -1 if TargetLevel is out of range or CardDatabase is null.
     */
    UFUNCTION(BlueprintPure, Category = "Enhancement")
    int64 GetGoldCostForLevel(int32 TargetLevel) const;

    /**
     * Returns the success probability [0.0–1.0] for enhancing to TargetLevel.
     * Returns 0.0f if TargetLevel is out of range or CardDatabase is null.
     */
    UFUNCTION(BlueprintPure, Category = "Enhancement")
    float GetSuccessRateForLevel(int32 TargetLevel) const;

    // -------------------------------------------------------------------------
    // Delegates — subscribe in Blueprint for UI reactions
    // -------------------------------------------------------------------------

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnEnhancementAttempted,
        const FCardInstance&, UpdatedCard,
        const FEnhancementAttemptResult&, AttemptResult);

    /** Broadcast after every enhancement attempt (success or failure). */
    UPROPERTY(BlueprintAssignable, Category = "Enhancement|Events")
    FOnEnhancementAttempted OnEnhancementAttempted;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnSlotUnlocked,
        const FCardInstance&, UpdatedCard,
        int32, NewEnhancementLevel);

    /** Broadcast when a new trait or skill slot is unlocked. */
    UPROPERTY(BlueprintAssignable, Category = "Enhancement|Events")
    FOnSlotUnlocked OnSlotUnlocked;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnEvolutionReady,
        const FCardInstance&, UpdatedCard);

    /** Broadcast when a card reaches +10 and bEvolutionReady becomes true. */
    UPROPERTY(BlueprintAssignable, Category = "Enhancement|Events")
    FOnEvolutionReady OnEvolutionReady;

protected:
    // -------------------------------------------------------------------------
    // Internal helpers — not exposed to Blueprint
    // -------------------------------------------------------------------------

    /**
     * Applies outfield stat boosts: +1 to Pace, Shooting, Passing,
     * Dribbling, Defending, Physical. Overall recalculated as a weighted
     * average using position-specific weights.
     * Stats clamped to [0, 99].
     */
    void ApplyOutfieldBoost(FCardInstance& CardInstance);

    /**
     * Applies GK stat boosts: +2 to GKDiving, GKHandling, GKKicking,
     * GKPositioning, GKReflexes. Overall recalculated as simple average
     * of the 5 GK attrs. Stats clamped to [0, 99].
     */
    void ApplyGoalkeeperBoost(FCardInstance& CardInstance);

    /**
     * Recalculates FCardAttributes::Overall for an outfield card.
     * Weights are position-specific (ST prioritises Shooting > Pace > Dribbling;
     * CB prioritises Defending > Physical > Pace, etc.).
     * TODO Milestone-2: Source weights from DT_PositionWeights DataTable.
     */
    int32 RecalculateOutfieldOverall(const FCardAttributes& Attrs, EPlayerPosition Position) const;

    /**
     * Recalculates Overall for a GK card as the unweighted average of
     * GKDiving + GKHandling + GKKicking + GKPositioning + GKReflexes.
     */
    int32 RecalculateGKOverall(const FCardAttributes& Attrs) const;

    /**
     * Returns true if CardDatabase is valid and contains the given CardID.
     * Logs an error on failure.
     */
    bool ValidateCardAndDatabase(const FCardInstance& CardInstance) const;
};
