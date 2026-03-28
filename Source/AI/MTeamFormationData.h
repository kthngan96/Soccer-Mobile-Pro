// Copyright (c) Soccer Mobile Pro. All Rights Reserved.
// GDD Section 2.1.3 | TECHSPEC Section 9.1 — AI: Behavior Trees + EQS

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MTeamFormationData.generated.h"

/**
 * EFormationType
 *
 * Enumerates all supported team formations.
 * GDD §2.1.3: The player can select a formation before or during a match;
 * AI teammates reposition to normalized pitch slots upon change.
 *
 * Naming convention: Formation_<line1>_<line2>_<line3>
 * (prefix avoids collision with identifiers starting with a digit).
 */
UENUM(BlueprintType)
enum class EFormationType : uint8
{
    /** Standard 4-3-3: four defenders, three midfielders, three forwards. */
    Formation_4_3_3     UMETA(DisplayName = "4-3-3"),

    /** 4-2-3-1: four defenders, two defensive mids, one attacking mid line, one striker. */
    Formation_4_2_3_1   UMETA(DisplayName = "4-2-3-1"),

    /** 3-5-2: three defenders, five midfielders, two strikers. */
    Formation_3_5_2     UMETA(DisplayName = "3-5-2"),

    /** 5-4-1: five defenders / wing-backs, four midfielders, one striker. */
    Formation_5_4_1     UMETA(DisplayName = "5-4-1"),

    /** Classic 4-4-2: four defenders, four midfielders, two strikers. */
    Formation_4_4_2     UMETA(DisplayName = "4-4-2"),
};

/**
 * EPlayerRole
 *
 * Broad positional role used to group formation slots for logic branching
 * (e.g., goalkeepers ignore FindPassTarget EQS, attackers weight it highest).
 */
UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
    Goalkeeper  UMETA(DisplayName = "Goalkeeper"),
    Defender    UMETA(DisplayName = "Defender"),
    Midfielder  UMETA(DisplayName = "Midfielder"),
    Forward     UMETA(DisplayName = "Forward"),
};

/**
 * FFormationSlot
 *
 * Describes a single player's nominal position within a formation.
 *
 * Pitch coordinate system (TECHSPEC §9.1 / GDD §2.1.3):
 *   - Origin (0, 0) = bottom-left corner of the pitch (attacking half starts at Y=0.5).
 *   - X axis  [0..1] = left to right (from the perspective of the attacking team).
 *   - Y axis  [0..1] = bottom (own goal) to top (opponent goal).
 *   - Goalkeeper default: Y ≈ 0.05.
 *   - Center circle: (0.5, 0.5).
 *
 * The runtime AI system converts these normalized values to world-space
 * positions by multiplying against the actual pitch dimensions at BeginPlay.
 */
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FFormationSlot
{
    GENERATED_BODY()

    /** Zero-based index of this slot within its formation (0 = GK, 1-10 = outfield). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
    int32 SlotIndex = 0;

    /** Positional role associated with this slot. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
    EPlayerRole Role = EPlayerRole::Midfielder;

    /**
     * Normalized X position on the pitch [0..1].
     * 0 = left touchline, 1 = right touchline.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation",
        meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float NormalizedX = 0.5f;

    /**
     * Normalized Y position on the pitch [0..1].
     * 0 = own goal line, 1 = opponent goal line.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation",
        meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float NormalizedY = 0.5f;

    /** Human-readable label (e.g., "CB-L", "CAM", "ST-R") for debug overlays. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
    FName PositionLabel = NAME_None;

    FFormationSlot() = default;
    FFormationSlot(int32 InIndex, EPlayerRole InRole, float InX, float InY, FName InLabel)
        : SlotIndex(InIndex), Role(InRole), NormalizedX(InX), NormalizedY(InY), PositionLabel(InLabel)
    {}
};

/**
 * FFormationData
 *
 * Full set of 11 slots (1 GK + 10 outfield) for one formation type.
 */
USTRUCT(BlueprintType)
struct SOCCERMOBILEPRO_API FFormationData
{
    GENERATED_BODY()

    /** Identifying enum value for this formation. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
    EFormationType FormationType = EFormationType::Formation_4_4_2;

    /**
     * All 11 slots ordered by SlotIndex.
     * Index 0 = Goalkeeper, Indices 1-10 = outfield players.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formation")
    TArray<FFormationSlot> Slots;

    FFormationData() = default;
    explicit FFormationData(EFormationType InType) : FormationType(InType) {}
};

/**
 * UMTeamFormationData
 *
 * DataAsset that stores all formation presets.
 * Create one instance in Content/AI/Data/ and reference it from
 * the GameState or AIManager (TECHSPEC §9.1.4).
 *
 * Static helper GetDefaultFormations() returns compile-time defaults
 * so the system is functional without a DataAsset configured.
 */
UCLASS(BlueprintType)
class SOCCERMOBILEPRO_API UMTeamFormationData : public UObject
{
    GENERATED_BODY()

public:
    /** All pre-configured formations available in the game. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Formations")
    TArray<FFormationData> Formations;

    /**
     * Look up a formation by type.
     * @return Pointer to the matching FFormationData, or nullptr if not found.
     */
    UFUNCTION(BlueprintCallable, Category = "Formations")
    const FFormationData* GetFormation(EFormationType Type) const;

    /**
     * Returns the five default formation presets with hard-coded normalized positions.
     * Formations: 4-3-3, 4-2-3-1, 3-5-2, 5-4-1, 4-4-2.
     *
     * Slot coordinate conventions:
     *   X: left (0.0) → right (1.0)   (team always attacks towards Y=1)
     *   Y: own goal (0.0) → opp goal (1.0)
     *
     * GDD §2.1.3: Positions are symmetrical; left/right variants are
     * mirrored around X=0.5.
     */
    static TArray<FFormationData> GetDefaultFormations();

private:
    static FFormationData Build_4_3_3();
    static FFormationData Build_4_2_3_1();
    static FFormationData Build_3_5_2();
    static FFormationData Build_5_4_1();
    static FFormationData Build_4_4_2();
};

// ─────────────────────────────────────────────────────────────────────────────
// Lightweight inline helpers (no implementation bodies here; see MTeamFormationData.cpp)
// ─────────────────────────────────────────────────────────────────────────────

inline const FFormationData* UMTeamFormationData::GetFormation(EFormationType Type) const
{
    for (const FFormationData& F : Formations)
    {
        if (F.FormationType == Type) return &F;
    }
    return nullptr;
}
