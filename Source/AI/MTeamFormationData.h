// Copyright (c) 2026 Soccer Mobile Pro. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MTeamFormationData.generated.h"

/**
 * @brief Canonical role labels used for slot lookup.
 *        Matches GDD §2.1.3 position taxonomy.
 */
UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
    GK    UMETA(DisplayName = "Goalkeeper"),
    CB    UMETA(DisplayName = "Centre Back"),
    LB    UMETA(DisplayName = "Left Back"),
    RB    UMETA(DisplayName = "Right Back"),
    CDM   UMETA(DisplayName = "Defensive Mid"),
    CM    UMETA(DisplayName = "Central Mid"),
    CAM   UMETA(DisplayName = "Attacking Mid"),
    LM    UMETA(DisplayName = "Left Mid"),
    RM    UMETA(DisplayName = "Right Mid"),
    LW    UMETA(DisplayName = "Left Wing"),
    RW    UMETA(DisplayName = "Right Wing"),
    CF    UMETA(DisplayName = "Centre Forward"),
    ST    UMETA(DisplayName = "Striker"),
};

/**
 * @brief All formations supported by the game (GDD §2.1.3, TECHSPEC §9.1).
 *        Used to key into FFormationDefinition lookup tables.
 */
UENUM(BlueprintType)
enum class EFormationType : uint8
{
    F_4_3_3      UMETA(DisplayName = "4-3-3"),
    F_4_2_3_1    UMETA(DisplayName = "4-2-3-1"),
    F_3_5_2      UMETA(DisplayName = "3-5-2"),
    F_5_4_1      UMETA(DisplayName = "5-4-1"),
    F_4_4_2      UMETA(DisplayName = "4-4-2"),
};

/**
 * @brief One player's "anchor" position within a formation.
 *
 * Pitch coordinate system (TECHSPEC §9.1.2):
 *   - Origin (0,0) = own goal line, left touchline corner (attacking left→right)
 *   - NormalizedX : 0.0 (own goal) → 1.0 (opponent goal)   [along pitch length]
 *   - NormalizedY : 0.0 (left touch) → 1.0 (right touch)  [across pitch width]
 *
 * At runtime MSoccerAIController converts these to world-space via
 * APitchBounds::NormalizedToWorld() (TODO Milestone-2).
 */
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FFormationSlot
{
    GENERATED_BODY()

    /** Role this slot belongs to (used for lookup & substitution). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation")
    EPlayerRole Role = EPlayerRole::CM;

    /**
     * Normalised X position along pitch length.
     * 0.0 = own goal line, 1.0 = opponent goal line.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation",
              meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float NormalizedX = 0.5f;

    /**
     * Normalised Y position across pitch width.
     * 0.0 = left touchline, 1.0 = right touchline.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation",
              meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float NormalizedY = 0.5f;

    /**
     * Tactical depth offset applied when team is defending (-) or attacking (+).
     * Expressed in normalised pitch units; clamped by MSoccerAIController at runtime.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation",
              meta = (ClampMin = "-0.3", ClampMax = "0.3"))
    float TacticalDepthOffset = 0.0f;

    /** Human-readable label shown in the formation editor widget. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation")
    FText DisplayLabel;
};

/**
 * @brief Full definition of one formation — 11 FFormationSlots plus metadata.
 *
 * Stored as a UPrimaryDataAsset so the formation library can be hot-reloaded
 * without recompiling (TECHSPEC §9.1.1).
 *
 * Usage:
 * @code
 *   const FFormationSlot* Slot = FormationData->GetSlotForRole(EPlayerRole::ST);
 * @endcode
 */
UCLASS(BlueprintType)
class SOCCERMOBILEPRO_API UFormationDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UFormationDefinition();

    // -------------------------------------------------------------------------
    // Identity
    // -------------------------------------------------------------------------

    /** Enum key identifying this formation. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation|Identity")
    EFormationType FormationType = EFormationType::F_4_3_3;

    /** Short display name shown in UI (e.g. "4-3-3"). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation|Identity")
    FText DisplayName;

    // -------------------------------------------------------------------------
    // Slot data  (always exactly 11 entries when valid)
    // -------------------------------------------------------------------------

    /**
     * Ordered list of all 11 player slots.
     * Index 0 is always GK.  Remaining indices follow the convention:
     * defenders → midfielders → attackers (left→right within each line).
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation|Slots")
    TArray<FFormationSlot> Slots;

    // -------------------------------------------------------------------------
    // Tactical parameters  (TECHSPEC §9.1.3)
    // -------------------------------------------------------------------------

    /**
     * Defensive line depth (0 = deep block, 1 = high press).
     * Applied as a multiplier to TacticalDepthOffset when team state == Defending.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation|Tactics",
              meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DefensiveLineHeight = 0.5f;

    /**
     * Pressing intensity (0 = passive, 1 = gegenpressing).
     * Drives the EQS PressingRadius context parameter.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation|Tactics",
              meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float PressingIntensity = 0.5f;

    /**
     * Width of attacking shape (0 = narrow, 1 = wide).
     * Scales the NormalizedY spread of attacking slots at runtime.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formation|Tactics",
              meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AttackingWidth = 0.5f;

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    /**
     * Returns a read-only pointer to the first slot matching the requested role,
     * or nullptr if the formation does not contain that role.
     *
     * @param Role  The player role to look up.
     * @return      Pointer to the matching FFormationSlot, or nullptr.
     */
    UFUNCTION(BlueprintCallable, Category = "Formation")
    const FFormationSlot* GetSlotForRole(EPlayerRole Role) const;

    /**
     * Validates that Slots contains exactly 11 entries and includes a GK.
     * Called automatically on PostLoad and from Editor validation utilities.
     *
     * @return True if the formation is structurally valid.
     */
    UFUNCTION(BlueprintCallable, Category = "Formation")
    bool ValidateFormation() const;

    // UObject interface
    virtual void PostLoad() override;

private:
    /**
     * Populates Slots with authoritative default coordinates for all 5 formations.
     * Invoked by the constructor when no saved asset data is present.
     * TODO Milestone-1: expose a standalone editor utility to regenerate presets.
     */
    void PopulateDefaultSlots();

    // Pre-built coordinate tables for each formation ---------------------------
    void Build_4_3_3();
    void Build_4_2_3_1();
    void Build_3_5_2();
    void Build_5_4_1();
    void Build_4_4_2();
};
