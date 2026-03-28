// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MPlayerCardTypes.h
//
// Core structs and enums for the Player Card system.
// All types referenced by UCardDatabase and UEnhancementComponent.
//
// GDD Reference  : Section 3   (Player Card System)
// TECHSPEC Reference: Section 2   (Enhancement & Evolution)
//
// Do NOT include gameplay logic here — pure data definitions only.

#pragma once

#include "CoreMinimal.h"
#include "MPlayerCardTypes.generated.h"

// ---------------------------------------------------------------------------
// ECardTier — TECHSPEC Section 2.1 (matches GDD Section 3.1 rarity ladder)
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class ECardTier : uint8
{
    Bronze          UMETA(DisplayName = "Bronze"),
    Silver          UMETA(DisplayName = "Silver"),
    Gold            UMETA(DisplayName = "Gold"),
    Elite           UMETA(DisplayName = "Elite"),
    Legend          UMETA(DisplayName = "Legend"),
    Icon            UMETA(DisplayName = "Icon"),
    Transcendence   UMETA(DisplayName = "Transcendence")
};

// ---------------------------------------------------------------------------
// EPlayerPosition — used to gate GK-specific vs outfield stat boosts
// TECHSPEC Section 2.3
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EPlayerPosition : uint8
{
    GK      UMETA(DisplayName = "Goalkeeper"),
    CB      UMETA(DisplayName = "Centre Back"),
    LB      UMETA(DisplayName = "Left Back"),
    RB      UMETA(DisplayName = "Right Back"),
    CDM     UMETA(DisplayName = "Defensive Mid"),
    CM      UMETA(DisplayName = "Central Mid"),
    CAM     UMETA(DisplayName = "Attacking Mid"),
    LW      UMETA(DisplayName = "Left Wing"),
    RW      UMETA(DisplayName = "Right Wing"),
    CF      UMETA(DisplayName = "Centre Forward"),
    ST      UMETA(DisplayName = "Striker")
};

// ---------------------------------------------------------------------------
// EEnhancementResult — return value for UEnhancementComponent::TryEnhanceCard
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EEnhancementResult : uint8
{
    Success             UMETA(DisplayName = "Success"),
    Failure_Protected   UMETA(DisplayName = "Failure (Level Protected)"),
    Error_InsufficientEP    UMETA(DisplayName = "Error: Insufficient EP"),
    Error_InsufficientGold  UMETA(DisplayName = "Error: Insufficient Gold"),
    Error_AlreadyMaxLevel   UMETA(DisplayName = "Error: Already +10"),
    Error_InvalidCard       UMETA(DisplayName = "Error: Invalid Card")
};

// ---------------------------------------------------------------------------
// FCardAttributes — runtime stat block for one player card.
// TECHSPEC Section 2.3: GK attributes differ from outfield.
// All values are 0-99 unless noted.
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FCardAttributes
{
    GENERATED_BODY()

    // ------- Outfield attributes (all positions except GK) -----------------

    /** Overall rating derived from weighted position formula (0-99) */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes")
    int32 Overall = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|Outfield")
    int32 Pace = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|Outfield")
    int32 Shooting = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|Outfield")
    int32 Passing = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|Outfield")
    int32 Dribbling = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|Outfield")
    int32 Defending = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|Outfield")
    int32 Physical = 0;

    // ------- Goalkeeper-only attributes (TECHSPEC 2.3) ---------------------
    // These are 0 for non-GK cards; only read by ApplyStatBoost when
    // the card position == EPlayerPosition::GK.

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|GK")
    int32 GKDiving = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|GK")
    int32 GKHandling = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|GK")
    int32 GKKicking = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|GK")
    int32 GKPositioning = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Attributes|GK")
    int32 GKReflexes = 0;

    // ------- Derived helper ------------------------------------------------

    /** Returns true if this struct represents a goalkeeper card. */
    bool IsGoalkeeper() const { return GKDiving > 0 || GKReflexes > 0; }
};

// ---------------------------------------------------------------------------
// FEnhancementRow — one entry in the +1 → +10 enhancement cost table.
// TECHSPEC Section 2.2.1 — Table 3.5.1
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FEnhancementRow
{
    GENERATED_BODY()

    /** Target enhancement level reached on success (1 through 10) */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Enhancement")
    int32 TargetLevel = 0;

    /** Enhancement Points consumed on attempt regardless of outcome */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Enhancement")
    int32 EPCost = 0;

    /** Gold Coins consumed on attempt regardless of outcome */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Enhancement")
    int64 GoldCost = 0;

    /** Base success probability [0.0 – 1.0]. Breakthrough Card bypasses this. */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Enhancement",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SuccessRate = 1.0f;
};

// ---------------------------------------------------------------------------
// FCardDefinition — read-only template stored in UCardDatabase.
// Holds the base attributes before any enhancement is applied.
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FCardDefinition
{
    GENERATED_BODY()

    /** Unique identifier (matches DT_PlayerCards RowName) */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Card")
    FName CardID;

    /** Localised display name shown in the UI */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Card")
    FText DisplayName;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Card")
    ECardTier Tier;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Card")
    EPlayerPosition Position;

    /** Base attributes at enhancement level +0 */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Card")
    FCardAttributes BaseAttributes;

    /** Thumbnail texture — set by art pipeline (ASSET_MANIFEST.md \#8-14) */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Card|Visuals")
    TSoftObjectPtr<UTexture2D> FaceTexture;

    /** Kit texture for in-match rendering */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Card|Visuals")
    TSoftObjectPtr<UTexture2D> KitTexture;
};

// ---------------------------------------------------------------------------
// FCardInstance — mutable runtime state for a card owned by a player.
// This is what gets serialised to the save game / backend.
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FCardInstance
{
    GENERATED_BODY()

    /** Universally unique ID for this specific card instance */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card")
    FGuid InstanceID;

    /** Links back to the template in UCardDatabase */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card")
    FName CardID;

    // -------------------------------------------------------------------------
    // Enhancement state — TECHSPEC Section 2
    // -------------------------------------------------------------------------

    /** Current enhancement level [0, 10] */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Enhancement",
        meta = (ClampMin = "0", ClampMax = "10"))
    int32 EnhancementLevel = 0;

    /** Runtime attributes after all enhancement boosts are applied */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Enhancement")
    FCardAttributes CurrentAttributes;

    /** Total EP invested into this card across all enhancement attempts */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Enhancement")
    int64 TotalEPInvested = 0;

    /** Total Gold invested into this card across all enhancement attempts */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Enhancement")
    int64 TotalGoldInvested = 0;

    // -------------------------------------------------------------------------
    // Slot unlocks — TECHSPEC Section 2.2.3
    // Trait slot unlocked at +5, skill slot at +7.
    // -------------------------------------------------------------------------

    /** True when enhancement level has reached +5 (trait slot unlocked) */
    UPROPERTY(BlueprintReadOnly, Category = "Card|Slots")
    bool bTraitSlotUnlocked = false;

    /** True when enhancement level has reached +7 (skill slot unlocked) */
    UPROPERTY(BlueprintReadOnly, Category = "Card|Slots")
    bool bSkillSlotUnlocked = false;

    /** True when enhancement level == 10 (eligible for Evolution path) */
    UPROPERTY(BlueprintReadOnly, Category = "Card|Evolution")
    bool bEvolutionReady = false;

    // -------------------------------------------------------------------------
    // Optional trait / skill equip references
    // -------------------------------------------------------------------------

    /** Row name into DT_TraitDefinitions; NAME_None if no trait equipped */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Slots",
        meta = (EditCondition = "bTraitSlotUnlocked"))
    FName EquippedTraitID;

    /** Row name into DT_SkillDefinitions; NAME_None if no skill equipped */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Card|Slots",
        meta = (EditCondition = "bSkillSlotUnlocked"))
    FName EquippedSkillID;

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    /** Returns true when the card is at maximum enhancement (+10). */
    bool IsMaxLevel()       const { return EnhancementLevel >= 10; }

    /** Returns true when trait slot is unlocked (+5 or higher). */
    bool HasTraitSlot()     const { return bTraitSlotUnlocked; }

    /** Returns true when skill slot is unlocked (+7 or higher). */
    bool HasSkillSlot()     const { return bSkillSlotUnlocked; }
};
