// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// UCardDatabase.h
//
// UDataAsset that holds the master list of all FCardDefinition templates.
// Referenced by UEnhancementComponent and the UI card-viewer screens.
//
// GDD Reference  : Section 3.2 (Card Database)
// TECHSPEC Reference: Section 2.0 (Player Card System Architecture)
//
// Attach this asset to the GameInstance or load it via AssetManager.
// Do NOT store mutable runtime data here — use FCardInstance for that.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MPlayerCardTypes.h"
#include "UCardDatabase.generated.h"

// ---------------------------------------------------------------------------
// UCardDatabase
// ---------------------------------------------------------------------------
UCLASS(BlueprintType, Blueprintable)
class SOCCERMOBILEPRO_API UCardDatabase : public UDataAsset
{
    GENERATED_BODY()

public:
    UCardDatabase();

    // -------------------------------------------------------------------------
    // Data — filled in the Editor via DataAsset inspector
    // -------------------------------------------------------------------------

    /**
     * Complete list of player card definitions.
     * Each entry maps to one row in DT_PlayerCards (ASSET_MANIFEST.md #140).
     */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Card Database")
    TArray<FCardDefinition> CardDefinitions;

    /**
     * Enhancement cost table — exactly 10 rows (+1 through +10).
     * Populated from TECHSPEC Section 2.2.1 Table 3.5.1:
     *   +1  : EP 100,    Gold 5K,    Success 100%
     *   +2  : EP 200,    Gold 10K,   Success 100%
     *   +3  : EP 500,    Gold 25K,   Success 100%
     *   +4  : EP 1000,   Gold 50K,   Success 80%
     *   +5  : EP 2000,   Gold 100K,  Success 70%  << Trait slot unlock
     *   +6  : EP 3500,   Gold 200K,  Success 60%
     *   +7  : EP 5000,   Gold 350K,  Success 50%  << Skill slot unlock
     *   +8  : EP 8000,   Gold 500K,  Success 40%
     *   +9  : EP 12000,  Gold 750K,  Success 30%
     *   +10 : EP 20000,  Gold 1M,    Success 25%  << Evolution ready
     */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Card Database|Enhancement")
    TArray<FEnhancementRow> EnhancementTable;

    // -------------------------------------------------------------------------
    // Lookup API
    // -------------------------------------------------------------------------

    /**
     * Find a card definition by its CardID.
     * @param CardID    Row name that uniquely identifies the card template.
     * @param OutDef    Populated on success.
     * @return          True if found, false if CardID is unknown.
     */
    UFUNCTION(BlueprintPure, Category = "Card Database")
    bool GetCardDefinition(FName CardID, FCardDefinition& OutDef) const;

    /**
     * Return the enhancement row for the requested target level.
     * @param TargetLevel   Desired level after enhancement [1, 10].
     * @param OutRow        Populated on success.
     * @return              True if a matching row was found.
     */
    UFUNCTION(BlueprintPure, Category = "Card Database|Enhancement")
    bool GetEnhancementRow(int32 TargetLevel, FEnhancementRow& OutRow) const;

    /**
     * Return all card definitions of the given tier.
     * Useful for pack-opening drop-table construction.
     */
    UFUNCTION(BlueprintPure, Category = "Card Database")
    TArray<FCardDefinition> GetCardsByTier(ECardTier Tier) const;

    /**
     * Return all card definitions for a given field position.
     */
    UFUNCTION(BlueprintPure, Category = "Card Database")
    TArray<FCardDefinition> GetCardsByPosition(EPlayerPosition Position) const;

    /**
     * Returns the total number of card definitions loaded.
     */
    UFUNCTION(BlueprintPure, Category = "Card Database")
    int32 GetCardCount() const { return CardDefinitions.Num(); }

    // -------------------------------------------------------------------------
    // Validation (Editor-only)
    // -------------------------------------------------------------------------

    /**
     * Validates that EnhancementTable has exactly 10 entries and all CardIDs
     * are unique. Logs errors via UE_LOG. Call from editor utilities or tests.
     */
    UFUNCTION(CallInEditor, Category = "Card Database|Validation")
    void ValidateDatabase() const;

protected:
    // -------------------------------------------------------------------------
    // Internal cache — rebuilt on PostLoad
    // -------------------------------------------------------------------------

    /** Fast CardID → index lookup built from CardDefinitions on PostLoad */
    UPROPERTY(Transient)
    TMap<FName, int32> CardIndexCache;

    /** Populates CardIndexCache from CardDefinitions array */
    void RebuildCardIndexCache();

public:
    // -------------------------------------------------------------------------
    // UObject overrides
    // -------------------------------------------------------------------------
    virtual void PostLoad() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
