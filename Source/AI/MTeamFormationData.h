// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MTeamFormationData.h
// Formation definitions for all 5 supported tactical systems.
// Implements GDD Section 2.1.3 pitch position layout (normalized 0-1 coordinates).
// ---------------------------------------------------------------------------
// Coordinate convention:
//   X = 0.0 (own goal line)  →  1.0 (opponent goal line)
//   Y = 0.0 (left touchline) →  1.0 (right touchline)
// Team always attacks in the +X direction; Mirror() flips Away team automatically.
// ---------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "MTeamFormationData.generated.h"

// ---------------------------------------------------------------------------
// EFormationType — five tactical systems (GDD 2.1.3)
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EFormationType : uint8
{
    Formation_4_3_3     UMETA(DisplayName = "4-3-3"),
    Formation_4_2_3_1   UMETA(DisplayName = "4-2-3-1"),
    Formation_3_5_2     UMETA(DisplayName = "3-5-2"),
    Formation_5_4_1     UMETA(DisplayName = "5-4-1"),
    Formation_4_4_2     UMETA(DisplayName = "4-4-2")
};

// ---------------------------------------------------------------------------
// EFormationRole — positional role per slot (maps to EPlayerPosition in PlayerCard)
// ---------------------------------------------------------------------------
UENUM(BlueprintType)
enum class EFormationRole : uint8
{
    GK      UMETA(DisplayName = "Goalkeeper"),
    CB      UMETA(DisplayName = "Centre Back"),
    LB      UMETA(DisplayName = "Left Back"),
    RB      UMETA(DisplayName = "Right Back"),
    LWB     UMETA(DisplayName = "Left Wing Back"),
    RWB     UMETA(DisplayName = "Right Wing Back"),
    CDM     UMETA(DisplayName = "Defensive Mid"),
    CM      UMETA(DisplayName = "Central Mid"),
    CAM     UMETA(DisplayName = "Attacking Mid"),
    LM      UMETA(DisplayName = "Left Mid"),
    RM      UMETA(DisplayName = "Right Mid"),
    LW      UMETA(DisplayName = "Left Wing"),
    RW      UMETA(DisplayName = "Right Wing"),
    CF      UMETA(DisplayName = "Centre Forward"),
    ST      UMETA(DisplayName = "Striker")
};

// ---------------------------------------------------------------------------
// FFormationSlot — one player position in a formation
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FFormationSlot
{
    GENERATED_BODY()

    /** Positional role assigned to this slot */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formation")
    EFormationRole Role = EFormationRole::CM;

    /**
     * Normalized pitch coordinate (X=depth 0-1, Y=lateral 0-1).
     * X=0.0 = own goal line; X=1.0 = opponent goal line.
     * Y=0.5 = centre of pitch.
     */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formation")
    FVector2D NormalizedPosition = FVector2D(0.5f, 0.5f);

    /** Slot index [0..10]; 0 is always GK */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formation")
    int32 SlotIndex = 0;

    /** Display label shown in lineup UI */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formation")
    FText DisplayLabel;

    FFormationSlot() = default;
    FFormationSlot(int32 InIndex, EFormationRole InRole, float InX, float InY, FText InLabel)
        : Role(InRole)
        , NormalizedPosition(InX, InY)
        , SlotIndex(InIndex)
        , DisplayLabel(InLabel)
    {}
};

// ---------------------------------------------------------------------------
// FFormationDefinition — 11-slot definition for one formation
// ---------------------------------------------------------------------------
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FFormationDefinition
{
    GENERATED_BODY()

    /** Which formation this represents */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formation")
    EFormationType FormationType = EFormationType::Formation_4_3_3;

    /** Human-readable name for UI, e.g. "4-3-3" */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formation")
    FText DisplayName;

    /** All 11 player slots (index 0=GK, 1-10=outfield) */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Formation")
    TArray<FFormationSlot> Slots;

    /**
     * Returns a mirrored copy of this definition for the Away team
     * (flips X from 0→1 to 1→0 so Away attacks toward X=0).
     */
    FFormationDefinition Mirror() const
    {
        FFormationDefinition Mirrored = *this;
        for (FFormationSlot& S : Mirrored.Slots)
        {
            S.NormalizedPosition.X = 1.0f - S.NormalizedPosition.X;
        }
        return Mirrored;
    }

    /**
     * Returns the slot for a given SlotIndex, or nullptr if not found.
     * Suitable for O(n) lookups on the 11-slot array.
     */
    const FFormationSlot* GetSlotByIndex(int32 InSlotIndex) const
    {
        for (const FFormationSlot& S : Slots)
        {
            if (S.SlotIndex == InSlotIndex) { return &S; }
        }
        return nullptr;
    }
};

// ---------------------------------------------------------------------------
// UFormationLibrary — static helpers to retrieve formation presets
// All five GDD 2.1.3 formations are pre-populated as static constexpr data.
// ---------------------------------------------------------------------------
UCLASS(BlueprintType, Blueprintable)
class SOCCERMOBILEPRO_API UFormationLibrary : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Returns the 11-slot FFormationDefinition for the requested formation type.
     * Call Mirror() on the result for the Away team.
     *
     * @param FormationType  One of the 5 supported EFormationType values.
     * @param OutDefinition  Populated on success.
     * @return               True on success; false if type is unrecognised.
     *
     * TODO Milestone-2: load overrides from a UDataTable asset instead of
     *                   the hard-coded defaults below.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Formation")
    static bool GetFormationDefinition(EFormationType FormationType,
                                        FFormationDefinition& OutDefinition);

    /**
     * Returns all 5 supported formation definitions in display order.
     * Used by the pre-match lineup UI to populate the formation picker.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Formation")
    static TArray<FFormationDefinition> GetAllFormationDefinitions();

private:
    /** ---- Hard-coded formation presets (GDD 2.1.3) ---- */

    /** 4-3-3: classic flat back four, three midfielders, three forwards */
    static FFormationDefinition Build_4_3_3();

    /** 4-2-3-1: double pivot, attacking mid, lone striker */
    static FFormationDefinition Build_4_2_3_1();

    /** 3-5-2: three centre-backs, wingbacks, double striker */
    static FFormationDefinition Build_3_5_2();

    /** 5-4-1: defensive five-back, compact midfield, lone striker */
    static FFormationDefinition Build_5_4_1();

    /** 4-4-2: two banks of four, two strikers */
    static FFormationDefinition Build_4_4_2();
};
