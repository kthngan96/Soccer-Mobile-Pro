// Copyright (c) 2025 MSoccer Studio. All Rights Reserved.
// TECHSPEC Section 6 — Pack Opening & Pity System
// GDD Section 7 — Monetisation & Packs

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayerCard/MPlayerCardTypes.h"   // ECardTier, FCardDefinition
#include "Economy/UEconomyManager.h"       // ECurrencyType
#include "UPackOpeningSystem.generated.h"

// ---------------------------------------------------------------------------
// Drop-rate row  (TECHSPEC 6.3.1 — 7-Tier Distribution)
// ---------------------------------------------------------------------------
/**
 * @brief One row in a pack's drop-rate table.
 *
 * All DropWeight values within a single FPackDefinition must sum to 10 000
 * (representing 100.00%) so integer arithmetic avoids float rounding.
 */
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FTierDropRow
{
    GENERATED_BODY()

    /** Card tier this row corresponds to. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pack|DropRate")
    ECardTier Tier = ECardTier::Bronze;

    /**
     * Weight out of 10 000 (e.g. 5000 = 50.00 %).  
     * Engine rolls 0–9999 inclusive; row wins if roll < cumulative weight.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pack|DropRate",
              meta = (ClampMin = "0", ClampMax = "10000"))
    int32 DropWeight = 0;
};

// ---------------------------------------------------------------------------
// FPityCounter  (TECHSPEC 6.3.2 — Pity Rules)
// ---------------------------------------------------------------------------
/**
 * @brief Per-player, per-pack-type pity state.
 *
 * Persisted between sessions so pity is never lost across re-launches.
 * Pity thresholds (TECHSPEC 6.3.2):
 *  - Rare guarantee    : every 10 pulls  → at least Silver.
 *  - Epic guarantee    : every 20 pulls  → at least Gold.
 *  - Legendary pity    : every 50 pulls  → at least Platinum.
 *  - Transcendence pity: every 200 pulls → exactly Transcendence.
 */
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FPityCounter
{
    GENERATED_BODY()

    /** Total pulls performed on this pack type across all sessions. */
    UPROPERTY(BlueprintReadWrite, Category = "Pack|Pity")
    int32 TotalPulls = 0;

    /** Pulls since the last Silver-or-better result. */
    UPROPERTY(BlueprintReadWrite, Category = "Pack|Pity")
    int32 PullsSinceRare = 0;

    /** Pulls since the last Gold-or-better result. */
    UPROPERTY(BlueprintReadWrite, Category = "Pack|Pity")
    int32 PullsSinceEpic = 0;

    /** Pulls since the last Platinum-or-better result. */
    UPROPERTY(BlueprintReadWrite, Category = "Pack|Pity")
    int32 PullsSinceLegendary = 0;

    /** Pulls since the last Transcendence result. */
    UPROPERTY(BlueprintReadWrite, Category = "Pack|Pity")
    int32 PullsSinceTranscendence = 0;

    // Pity thresholds — match TECHSPEC 6.3.2 exactly.
    static constexpr int32 RarePityThreshold           = 10;
    static constexpr int32 EpicPityThreshold           = 20;
    static constexpr int32 LegendaryPityThreshold      = 50;
    static constexpr int32 TranscendencePityThreshold  = 200;
};

// ---------------------------------------------------------------------------
// FPackDefinition
// ---------------------------------------------------------------------------
/**
 * @brief Immutable definition of a single pack product.
 *
 * Authored in a UDataAsset (UPackDatabase) and read-only at runtime.  
 * GDD 7.1 defines the four launch pack SKUs; this struct covers any future
 * variants without code changes.
 */
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FPackDefinition
{
    GENERATED_BODY()

    /** Unique identifier used as the pity map key and store SKU. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pack")
    FName PackID;

    /** Human-readable display name shown in the Store UI. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pack")
    FText DisplayName;

    /** Currency spent to open one pack. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pack|Cost")
    ECurrencyType CostCurrency = ECurrencyType::Gems;

    /** Amount of CostCurrency deducted per single open. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pack|Cost",
              meta = (ClampMin = "1"))
    int32 CostPerPack = 300;

    /** Number of cards granted per single open (usually 1 or 5). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pack",
              meta = (ClampMin = "1", ClampMax = "11"))
    int32 CardsPerPack = 1;

    /**
     * Drop-rate table — must contain exactly 7 rows (one per ECardTier)  
     * and weights must sum to 10 000.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pack|DropRate")
    TArray<FTierDropRow> DropRateTable;

    /**
     * Optional pool restriction: if non-empty, only cards whose CardID is in
     * this list can be dropped from this pack (e.g. "Featured" banner packs).
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pack")
    TArray<FName> FeaturedCardIDs;
};

// ---------------------------------------------------------------------------
// FPackOpenResult
// ---------------------------------------------------------------------------
/**
 * @brief Return value of OpenPack() — one result struct per card drawn.
 *
 * The UI layer reads this to run the reveal animation sequence.
 */
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FPackOpenResult
{
    GENERATED_BODY()

    /** The tier that was awarded (post pity-adjustment). */
    UPROPERTY(BlueprintReadOnly, Category = "Pack|Result")
    ECardTier AwardedTier = ECardTier::Bronze;

    /**
     * Specific card instance created for the player.
     * Invalid if no matching card was found in UCardDatabase  
     * (should not happen in final content; see TODO below).
     */
    UPROPERTY(BlueprintReadOnly, Category = "Pack|Result")
    FName AwardedCardID;

    /** True if pity forced the tier upgrade (useful for analytics). */
    UPROPERTY(BlueprintReadOnly, Category = "Pack|Result")
    bool bWasPityGuarantee = false;

    /** Pull number within this session (1-indexed). */
    UPROPERTY(BlueprintReadOnly, Category = "Pack|Result")
    int32 PullIndex = 0;
};

// ---------------------------------------------------------------------------
// FPullEventPayload  (analytics)
// ---------------------------------------------------------------------------
/**
 * @brief Lightweight payload passed to LogPullEvent for analytics.
 *
 * TODO Milestone-3: serialise and POST to backend analytics endpoint.
 */
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FPullEventPayload
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Analytics")
    FName PackID;

    UPROPERTY(BlueprintReadOnly, Category = "Analytics")
    FName CardID;

    UPROPERTY(BlueprintReadOnly, Category = "Analytics")
    ECardTier Tier = ECardTier::Bronze;

    UPROPERTY(BlueprintReadOnly, Category = "Analytics")
    bool bPityTriggered = false;

    UPROPERTY(BlueprintReadOnly, Category = "Analytics")
    int32 TotalPullCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Analytics")
    FDateTime Timestamp;
};

// ---------------------------------------------------------------------------
// Delegate
// ---------------------------------------------------------------------------

/** Broadcast once per card revealed after OpenPack completes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnPackOpened,
    const FPackDefinition&, Pack,
    const TArray<FPackOpenResult>&, Results
);

// ---------------------------------------------------------------------------
// UPackOpeningSystem
// ---------------------------------------------------------------------------
/**
 * @brief Game-instance subsystem handling pack opening and pity tracking.
 *
 * Access via:
 * @code
 *   UPackOpeningSystem* POS = GetGameInstance()->GetSubsystem<UPackOpeningSystem>();
 * @endcode
 *
 * ### Pity Rules (TECHSPEC 6.3.2)
 * | Threshold | Guaranteed Minimum Tier |
 * |-----------|-------------------------|
 * | 10 pulls  | Silver (Rare)           |
 * | 20 pulls  | Gold (Epic)             |
 * | 50 pulls  | Platinum (Legendary)    |
 * | 200 pulls | Transcendence           |
 *
 * ### 7-Tier Drop Rates — Standard Pack (TECHSPEC 6.3.1)
 * | Tier            | Base Rate |
 * |-----------------|-----------|
 * | Bronze          | 50.00 %   |
 * | Silver          | 27.00 %   |
 * | Gold            | 14.00 %   |
 * | Platinum        |  5.00 %   |
 * | Diamond         |  2.50 %   |
 * | Legend          |  1.25 %   |
 * | Transcendence   |  0.25 %   |
 *
 * TODO Milestone-2: deduct pack cost via UEconomyManager before rolling.  
 * TODO Milestone-2: query UCardDatabase for card pool per tier.  
 * TODO Milestone-3: persist pity counters to save-game slot.
 */
UCLASS()
class SOCCERMOBILEPRO_API UPackOpeningSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    // -----------------------------------------------------------------------
    // USubsystem interface
    // -----------------------------------------------------------------------

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // -----------------------------------------------------------------------
    // Pack opening API
    // -----------------------------------------------------------------------

    /**
     * @brief Opens one or more packs and returns per-card results.
     *
     * Flow (TECHSPEC 6.3):
     *  1. Validate PackDef — reject unknown PackID.
     *  2. Verify the player can afford CostCurrency × CardsPerPack.
     *  3. Deduct cost via UEconomyManager.
     *  4. For each card slot:
     *      a. Increment pity counters.
     *      b. Apply pity overrides (highest tier forced by threshold wins).
     *      c. Roll weighted random on DropRateTable (0–9999).
     *      d. Pick a specific card from the eligible pool in UCardDatabase.
     *      e. Grant the card to the player's inventory.
     *      f. Reset relevant pity counters on success.
     *  5. Log each pull via LogPullEvent().
     *  6. Broadcast OnPackOpened.
     *
     * @param PackDef      Definition of the pack being opened.
     * @param MultiCount   How many packs to open in a single call (default 1).
     * @param[out] OutResults  One entry per card drawn (CardsPerPack × MultiCount).
     * @return             true if opening succeeded (cost paid, cards granted).
     */
    UFUNCTION(BlueprintCallable, Category = "Pack",
              meta = (DisplayName = "Open Pack"))
    bool OpenPack(const FPackDefinition& PackDef,
                  int32 MultiCount,
                  TArray<FPackOpenResult>& OutResults);

    /**
     * @brief Returns the current pity state for a given pack type.
     *
     * @param PackID  Key into the PityCounterMap.
     * @return        Read-only copy of the counter struct.  Returns a
     *                default (zeroed) counter if PackID is unknown.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pack",
              meta = (DisplayName = "Get Pity Counter"))
    FPityCounter GetPityCounter(FName PackID) const;

    /**
     * @brief Resets all pity counters for a pack (admin/QA use only).
     *
     * Should be gated behind a developer cheat flag in shipping builds.
     */
    UFUNCTION(BlueprintCallable, Category = "Pack|Debug",
              meta = (DisplayName = "Reset Pity Counter (Debug)"))
    void ResetPityCounter(FName PackID);

    // -----------------------------------------------------------------------
    // Analytics
    // -----------------------------------------------------------------------

    /**
     * @brief Records a pull event for analytics and debug logging.
     *
     * Called internally by OpenPack after each card is resolved.  
     * TODO Milestone-3: POST payload to backend analytics endpoint.
     *
     * @param Payload  Populated pull-event descriptor.
     */
    UFUNCTION(BlueprintCallable, Category = "Pack|Analytics",
              meta = (DisplayName = "Log Pull Event"))
    void LogPullEvent(const FPullEventPayload& Payload);

    // -----------------------------------------------------------------------
    // Delegates
    // -----------------------------------------------------------------------

    /** Fired after all cards in an OpenPack() call are resolved. */
    UPROPERTY(BlueprintAssignable, Category = "Pack|Events")
    FOnPackOpened OnPackOpened;

private:

    // -----------------------------------------------------------------------
    // Internal helpers
    // -----------------------------------------------------------------------

    /**
     * @brief Rolls one card tier against the drop-rate table.
     *
     * @param PackDef    Pack whose DropRateTable is used.
     * @param Counter    Pity state (read-only for pity override logic).
     * @param[out] bOutPityTriggered  Set to true if pity forced the tier up.
     * @return           The resolved ECardTier.
     */
    ECardTier RollTier(const FPackDefinition& PackDef,
                       const FPityCounter& Counter,
                       bool& bOutPityTriggered) const;

    /**
     * @brief Advances and updates all relevant pity sub-counters.
     *
     * Called *before* the roll so the new pull count is used for pity checks.
     *
     * @param Counter    In-out pity counter to mutate.
     * @param AwardedTier  Tier that was actually granted this pull.
     */
    void UpdatePityCounters(FPityCounter& Counter, ECardTier AwardedTier) const;

    /**
     * @brief Picks a specific card definition for the awarded tier.
     *
     * Falls back to any card of that tier if FeaturedCardIDs is empty.  
     * TODO Milestone-2: query UCardDatabase via GetGameInstance().
     *
     * @param PackDef     Pack (for FeaturedCardIDs filter).
     * @param Tier        Awarded tier.
     * @return            CardID of the selected card, or NAME_None on error.
     */
    FName PickCardFromPool(const FPackDefinition& PackDef, ECardTier Tier) const;

    // -----------------------------------------------------------------------
    // Pity storage
    // -----------------------------------------------------------------------

    /**
     * Keyed by PackID.  One entry per pack type the player has ever opened.  
     * TODO Milestone-3: load/save from persistent save-game slot.
     */
    UPROPERTY()
    TMap<FName, FPityCounter> PityCounterMap;

    // -----------------------------------------------------------------------
    // Analytics event log  (in-memory, bounded)
    // -----------------------------------------------------------------------

    static constexpr int32 MaxAnalyticsLogSize = 1000;

    TArray<FPullEventPayload> AnalyticsLog;
};
