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
 */
UENUM(BlueprintType)
enum class EFormationType : uint8
{
    /** Standard 4-3-3: four defenders, three midfielders, three forwards. */
    F_4_3_3     UMETA(DisplayName = "4-3-3"),

    /** 4-2-3-1: four defenders, two defensive mids, one attacking mid line, one striker. */
    F_4_2_3_1   UMETA(DisplayName = "4-2-3-1"),

    /** 3-5-2: three defenders, five midfielders, two strikers. */
    F_3_5_2     UMETA(DisplayName = "3-5-2"),

    /** 5-4-1: five defenders / wing-backs, four midfielders, one striker. */
    F_5_4_1     UMETA(DisplayName = "5-4-1"),

    /** Classic 4-4-2: four defenders, four midfielders, two strikers. */
    F_4_4_2     UMETA(DisplayName = "4-4-2"),
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
    EFormationType FormationType = EFormationType::F_4_4_2;

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
// Inline implementations (header-only helpers used by editor and runtime)
// ─────────────────────────────────────────────────────────────────────────────

INLINE const FFormationData* UMTeamFormationData::GetFormation(EFormationType Type) const
{
    for (const FFormationData& F : Formations)
    {
        if (F.FormationType == Type) return &F;
    }
    return nullptr;
}

INLINE TArray<FFormationData> UMTeamFormationData::GetDefaultFormations()
{
    TArray<FFormationData> All;
    All.Add(Build_4_3_3());
    All.Add(Build_4_2_3_1());
    All.Add(Build_3_5_2());
    All.Add(Build_5_4_1());
    All.Add(Build_4_4_2());
    return All;
}

// ─────────────────────────────────────────────────────────────────────────────
// 4-3-3
// Slots: GK | LB CB-L CB-R RB | LCM CM RCM | LW ST RW
// ─────────────────────────────────────────────────────────────────────────────
INLINE FFormationData UMTeamFormationData::Build_4_3_3()
{
    FFormationData F(EFormationType::F_4_3_3);
    // Slot 0: Goalkeeper
    F.Slots.Add(FFormationSlot(0,  EPlayerRole::Goalkeeper,  0.50f, 0.05f, "GK"));
    // Defensive line
    F.Slots.Add(FFormationSlot(1,  EPlayerRole::Defender,    0.15f, 0.22f, "LB"));
    F.Slots.Add(FFormationSlot(2,  EPlayerRole::Defender,    0.38f, 0.20f, "CB-L"));
    F.Slots.Add(FFormationSlot(3,  EPlayerRole::Defender,    0.62f, 0.20f, "CB-R"));
    F.Slots.Add(FFormationSlot(4,  EPlayerRole::Defender,    0.85f, 0.22f, "RB"));
    // Midfield line
    F.Slots.Add(FFormationSlot(5,  EPlayerRole::Midfielder,  0.25f, 0.45f, "LCM"));
    F.Slots.Add(FFormationSlot(6,  EPlayerRole::Midfielder,  0.50f, 0.47f, "CM"));
    F.Slots.Add(FFormationSlot(7,  EPlayerRole::Midfielder,  0.75f, 0.45f, "RCM"));
    // Attack line
    F.Slots.Add(FFormationSlot(8,  EPlayerRole::Forward,     0.18f, 0.72f, "LW"));
    F.Slots.Add(FFormationSlot(9,  EPlayerRole::Forward,     0.50f, 0.78f, "ST"));
    F.Slots.Add(FFormationSlot(10, EPlayerRole::Forward,     0.82f, 0.72f, "RW"));
    return F;
}

// ─────────────────────────────────────────────────────────────────────────────
// 4-2-3-1
// Slots: GK | LB CB-L CB-R RB | DM-L DM-R | LAM CAM RAM | ST
// ─────────────────────────────────────────────────────────────────────────────
INLINE FFormationData UMTeamFormationData::Build_4_2_3_1()
{
    FFormationData F(EFormationType::F_4_2_3_1);
    F.Slots.Add(FFormationSlot(0,  EPlayerRole::Goalkeeper,  0.50f, 0.05f, "GK"));
    F.Slots.Add(FFormationSlot(1,  EPlayerRole::Defender,    0.15f, 0.22f, "LB"));
    F.Slots.Add(FFormationSlot(2,  EPlayerRole::Defender,    0.38f, 0.20f, "CB-L"));
    F.Slots.Add(FFormationSlot(3,  EPlayerRole::Defender,    0.62f, 0.20f, "CB-R"));
    F.Slots.Add(FFormationSlot(4,  EPlayerRole::Defender,    0.85f, 0.22f, "RB"));
    // Double pivot
    F.Slots.Add(FFormationSlot(5,  EPlayerRole::Midfielder,  0.36f, 0.38f, "DM-L"));
    F.Slots.Add(FFormationSlot(6,  EPlayerRole::Midfielder,  0.64f, 0.38f, "DM-R"));
    // Attacking mid trio
    F.Slots.Add(FFormationSlot(7,  EPlayerRole::Midfielder,  0.18f, 0.58f, "LAM"));
    F.Slots.Add(FFormationSlot(8,  EPlayerRole::Midfielder,  0.50f, 0.60f, "CAM"));
    F.Slots.Add(FFormationSlot(9,  EPlayerRole::Midfielder,  0.82f, 0.58f, "RAM"));
    // Lone striker
    F.Slots.Add(FFormationSlot(10, EPlayerRole::Forward,     0.50f, 0.80f, "ST"));
    return F;
}

// ─────────────────────────────────────────────────────────────────────────────
// 3-5-2
// Slots: GK | CB-L CB-C CB-R | LWB LCM CM RCM RWB | ST-L ST-R
// ─────────────────────────────────────────────────────────────────────────────
INLINE FFormationData UMTeamFormationData::Build_3_5_2()
{
    FFormationData F(EFormationType::F_3_5_2);
    F.Slots.Add(FFormationSlot(0,  EPlayerRole::Goalkeeper,  0.50f, 0.05f, "GK"));
    // Three-man defence
    F.Slots.Add(FFormationSlot(1,  EPlayerRole::Defender,    0.25f, 0.20f, "CB-L"));
    F.Slots.Add(FFormationSlot(2,  EPlayerRole::Defender,    0.50f, 0.18f, "CB-C"));
    F.Slots.Add(FFormationSlot(3,  EPlayerRole::Defender,    0.75f, 0.20f, "CB-R"));
    // Five-man midfield (incl. wing-backs)
    F.Slots.Add(FFormationSlot(4,  EPlayerRole::Midfielder,  0.08f, 0.48f, "LWB"));
    F.Slots.Add(FFormationSlot(5,  EPlayerRole::Midfielder,  0.30f, 0.45f, "LCM"));
    F.Slots.Add(FFormationSlot(6,  EPlayerRole::Midfielder,  0.50f, 0.47f, "CM"));
    F.Slots.Add(FFormationSlot(7,  EPlayerRole::Midfielder,  0.70f, 0.45f, "RCM"));
    F.Slots.Add(FFormationSlot(8,  EPlayerRole::Midfielder,  0.92f, 0.48f, "RWB"));
    // Two strikers
    F.Slots.Add(FFormationSlot(9,  EPlayerRole::Forward,     0.36f, 0.75f, "ST-L"));
    F.Slots.Add(FFormationSlot(10, EPlayerRole::Forward,     0.64f, 0.75f, "ST-R"));
    return F;
}

// ─────────────────────────────────────────────────────────────────────────────
// 5-4-1
// Slots: GK | LWB CB-L CB-C CB-R RWB | LM LCM RCM RM | ST
// ─────────────────────────────────────────────────────────────────────────────
INLINE FFormationData UMTeamFormationData::Build_5_4_1()
{
    FFormationData F(EFormationType::F_5_4_1);
    F.Slots.Add(FFormationSlot(0,  EPlayerRole::Goalkeeper,  0.50f, 0.05f, "GK"));
    // Five defenders
    F.Slots.Add(FFormationSlot(1,  EPlayerRole::Defender,    0.08f, 0.25f, "LWB"));
    F.Slots.Add(FFormationSlot(2,  EPlayerRole::Defender,    0.28f, 0.20f, "CB-L"));
    F.Slots.Add(FFormationSlot(3,  EPlayerRole::Defender,    0.50f, 0.18f, "CB-C"));
    F.Slots.Add(FFormationSlot(4,  EPlayerRole::Defender,    0.72f, 0.20f, "CB-R"));
    F.Slots.Add(FFormationSlot(5,  EPlayerRole::Defender,    0.92f, 0.25f, "RWB"));
    // Four midfielders
    F.Slots.Add(FFormationSlot(6,  EPlayerRole::Midfielder,  0.18f, 0.48f, "LM"));
    F.Slots.Add(FFormationSlot(7,  EPlayerRole::Midfielder,  0.38f, 0.46f, "LCM"));
    F.Slots.Add(FFormationSlot(8,  EPlayerRole::Midfielder,  0.62f, 0.46f, "RCM"));
    F.Slots.Add(FFormationSlot(9,  EPlayerRole::Midfielder,  0.82f, 0.48f, "RM"));
    // Lone striker
    F.Slots.Add(FFormationSlot(10, EPlayerRole::Forward,     0.50f, 0.78f, "ST"));
    return F;
}

// ─────────────────────────────────────────────────────────────────────────────
// 4-4-2
// Slots: GK | LB CB-L CB-R RB | LM LCM RCM RM | ST-L ST-R
// ─────────────────────────────────────────────────────────────────────────────
INLINE FFormationData UMTeamFormationData::Build_4_4_2()
{
    FFormationData F(EFormationType::F_4_4_2);
    F.Slots.Add(FFormationSlot(0,  EPlayerRole::Goalkeeper,  0.50f, 0.05f, "GK"));
    // Four defenders
    F.Slots.Add(FFormationSlot(1,  EPlayerRole::Defender,    0.15f, 0.22f, "LB"));
    F.Slots.Add(FFormationSlot(2,  EPlayerRole::Defender,    0.38f, 0.20f, "CB-L"));
    F.Slots.Add(FFormationSlot(3,  EPlayerRole::Defender,    0.62f, 0.20f, "CB-R"));
    F.Slots.Add(FFormationSlot(4,  EPlayerRole::Defender,    0.85f, 0.22f, "RB"));
    // Four midfielders
    F.Slots.Add(FFormationSlot(5,  EPlayerRole::Midfielder,  0.15f, 0.46f, "LM"));
    F.Slots.Add(FFormationSlot(6,  EPlayerRole::Midfielder,  0.38f, 0.47f, "LCM"));
    F.Slots.Add(FFormationSlot(7,  EPlayerRole::Midfielder,  0.62f, 0.47f, "RCM"));
    F.Slots.Add(FFormationSlot(8,  EPlayerRole::Midfielder,  0.85f, 0.46f, "RM"));
    // Two strikers
    F.Slots.Add(FFormationSlot(9,  EPlayerRole::Forward,     0.35f, 0.76f, "ST-L"));
    F.Slots.Add(FFormationSlot(10, EPlayerRole::Forward,     0.65f, 0.76f, "ST-R"));
    return F;
}
