// Copyright (c) 2026 Soccer Mobile Pro. All Rights Reserved.
#include "MTeamFormationData.h"
#include "Misc/AssertionMacros.h"

// ---------------------------------------------------------------------------
// Helper macro: quickly build a slot
// ---------------------------------------------------------------------------
#define MAKE_SLOT(InRole, InX, InY, InDepth, InLabel) \
    [&]() { \
        FFormationSlot S; \
        S.Role               = EPlayerRole::InRole; \
        S.NormalizedX        = InX; \
        S.NormalizedY        = InY; \
        S.TacticalDepthOffset = InDepth; \
        S.DisplayLabel       = FText::FromString(TEXT(InLabel)); \
        return S; \
    }()

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

UFormationDefinition::UFormationDefinition()
{
    // Trigger default population only in CDO context to avoid editor churn.
    // Individual DataAsset instances override Slots via the Details panel.
    if (HasAnyFlags(RF_ClassDefaultObject))
    {
        PopulateDefaultSlots();
    }
}

// ---------------------------------------------------------------------------
// UObject::PostLoad
// ---------------------------------------------------------------------------

void UFormationDefinition::PostLoad()
{
    Super::PostLoad();
    ValidateFormation();
}

// ---------------------------------------------------------------------------
// GetSlotForRole
// ---------------------------------------------------------------------------

const FFormationSlot* UFormationDefinition::GetSlotForRole(EPlayerRole Role) const
{
    for (const FFormationSlot& Slot : Slots)
    {
        if (Slot.Role == Role)
        {
            return &Slot;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// ValidateFormation
// ---------------------------------------------------------------------------

bool UFormationDefinition::ValidateFormation() const
{
    if (Slots.Num() != 11)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("UFormationDefinition::ValidateFormation — '%s' has %d slots (expected 11)."),
               *GetName(), Slots.Num());
        return false;
    }

    const bool bHasGK = Slots.ContainsByPredicate(
        [](const FFormationSlot& S){ return S.Role == EPlayerRole::GK; });

    if (!bHasGK)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("UFormationDefinition::ValidateFormation — '%s' has no GK slot."),
               *GetName());
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// PopulateDefaultSlots  (dispatch to per-formation builders)
// ---------------------------------------------------------------------------

void UFormationDefinition::PopulateDefaultSlots()
{
    switch (FormationType)
    {
        case EFormationType::F_4_3_3:   Build_4_3_3();   break;
        case EFormationType::F_4_2_3_1: Build_4_2_3_1(); break;
        case EFormationType::F_3_5_2:   Build_3_5_2();   break;
        case EFormationType::F_5_4_1:   Build_5_4_1();   break;
        case EFormationType::F_4_4_2:   Build_4_4_2();   break;
        default: Build_4_3_3(); break;
    }
}

// ===========================================================================
// Formation coordinate tables
//
// Convention:
//   NormalizedX  0.0=own-goal  1.0=opponent-goal   (pitch length)
//   NormalizedY  0.0=left      1.0=right            (pitch width)
//   TacticalDepthOffset  applied when Attacking(+) / Defending(-)
//
// Reference: GDD §2.1.3 — standard UEFA pitch 105×68 m mapped 0-1.
// ===========================================================================

// ---------------------------------------------------------------------------
// 4-3-3  (TECHSPEC §9.1.7 formation table)
// ---------------------------------------------------------------------------
void UFormationDefinition::Build_4_3_3()
{
    FormationType = EFormationType::F_4_3_3;
    DisplayName   = FText::FromString(TEXT("4-3-3"));
    Slots =
    {
        // ---- Goalkeeper ----
        MAKE_SLOT(GK,  0.05f, 0.50f,  0.00f, "GK"),
        // ---- Defenders ----
        MAKE_SLOT(LB,  0.25f, 0.12f,  0.05f, "LB"),
        MAKE_SLOT(CB,  0.22f, 0.36f,  0.04f, "CB-L"),
        MAKE_SLOT(CB,  0.22f, 0.64f,  0.04f, "CB-R"),
        MAKE_SLOT(RB,  0.25f, 0.88f,  0.05f, "RB"),
        // ---- Midfielders ----
        MAKE_SLOT(CM,  0.45f, 0.25f,  0.08f, "CM-L"),
        MAKE_SLOT(CM,  0.48f, 0.50f,  0.08f, "CM-C"),
        MAKE_SLOT(CM,  0.45f, 0.75f,  0.08f, "CM-R"),
        // ---- Forwards ----
        MAKE_SLOT(LW,  0.72f, 0.12f,  0.10f, "LW"),
        MAKE_SLOT(ST,  0.78f, 0.50f,  0.12f, "ST"),
        MAKE_SLOT(RW,  0.72f, 0.88f,  0.10f, "RW"),
    };
}

// ---------------------------------------------------------------------------
// 4-2-3-1
// ---------------------------------------------------------------------------
void UFormationDefinition::Build_4_2_3_1()
{
    FormationType = EFormationType::F_4_2_3_1;
    DisplayName   = FText::FromString(TEXT("4-2-3-1"));
    Slots =
    {
        MAKE_SLOT(GK,  0.05f, 0.50f,  0.00f, "GK"),
        MAKE_SLOT(LB,  0.25f, 0.12f,  0.05f, "LB"),
        MAKE_SLOT(CB,  0.22f, 0.36f,  0.04f, "CB-L"),
        MAKE_SLOT(CB,  0.22f, 0.64f,  0.04f, "CB-R"),
        MAKE_SLOT(RB,  0.25f, 0.88f,  0.05f, "RB"),
        // Double pivot
        MAKE_SLOT(CDM, 0.38f, 0.38f,  0.06f, "CDM-L"),
        MAKE_SLOT(CDM, 0.38f, 0.62f,  0.06f, "CDM-R"),
        // Attacking trio
        MAKE_SLOT(LW,  0.60f, 0.14f,  0.10f, "LW"),
        MAKE_SLOT(CAM, 0.63f, 0.50f,  0.10f, "CAM"),
        MAKE_SLOT(RW,  0.60f, 0.86f,  0.10f, "RW"),
        MAKE_SLOT(ST,  0.78f, 0.50f,  0.12f, "ST"),
    };
}

// ---------------------------------------------------------------------------
// 3-5-2
// ---------------------------------------------------------------------------
void UFormationDefinition::Build_3_5_2()
{
    FormationType = EFormationType::F_3_5_2;
    DisplayName   = FText::FromString(TEXT("3-5-2"));
    Slots =
    {
        MAKE_SLOT(GK,  0.05f, 0.50f,  0.00f, "GK"),
        // Three CBs
        MAKE_SLOT(CB,  0.22f, 0.25f,  0.04f, "CB-L"),
        MAKE_SLOT(CB,  0.20f, 0.50f,  0.04f, "CB-C"),
        MAKE_SLOT(CB,  0.22f, 0.75f,  0.04f, "CB-R"),
        // Wing backs
        MAKE_SLOT(LB,  0.42f, 0.08f,  0.10f, "LWB"),
        MAKE_SLOT(RB,  0.42f, 0.92f,  0.10f, "RWB"),
        // Midfield three
        MAKE_SLOT(CDM, 0.40f, 0.30f,  0.07f, "CM-L"),
        MAKE_SLOT(CM,  0.43f, 0.50f,  0.08f, "CM-C"),
        MAKE_SLOT(CDM, 0.40f, 0.70f,  0.07f, "CM-R"),
        // Two strikers
        MAKE_SLOT(ST,  0.76f, 0.38f,  0.12f, "ST-L"),
        MAKE_SLOT(ST,  0.76f, 0.62f,  0.12f, "ST-R"),
    };
}

// ---------------------------------------------------------------------------
// 5-4-1
// ---------------------------------------------------------------------------
void UFormationDefinition::Build_5_4_1()
{
    FormationType = EFormationType::F_5_4_1;
    DisplayName   = FText::FromString(TEXT("5-4-1"));
    Slots =
    {
        MAKE_SLOT(GK,  0.05f, 0.50f,  0.00f, "GK"),
        // Five-man defence
        MAKE_SLOT(LB,  0.22f, 0.08f,  0.04f, "LWB"),
        MAKE_SLOT(CB,  0.20f, 0.26f,  0.03f, "CB-L"),
        MAKE_SLOT(CB,  0.18f, 0.50f,  0.03f, "CB-C"),
        MAKE_SLOT(CB,  0.20f, 0.74f,  0.03f, "CB-R"),
        MAKE_SLOT(RB,  0.22f, 0.92f,  0.04f, "RWB"),
        // Four midfielders
        MAKE_SLOT(LM,  0.42f, 0.14f,  0.07f, "LM"),
        MAKE_SLOT(CM,  0.44f, 0.38f,  0.07f, "CM-L"),
        MAKE_SLOT(CM,  0.44f, 0.62f,  0.07f, "CM-R"),
        MAKE_SLOT(RM,  0.42f, 0.86f,  0.07f, "RM"),
        // Single striker
        MAKE_SLOT(ST,  0.78f, 0.50f,  0.12f, "ST"),
    };
}

// ---------------------------------------------------------------------------
// 4-4-2
// ---------------------------------------------------------------------------
void UFormationDefinition::Build_4_4_2()
{
    FormationType = EFormationType::F_4_4_2;
    DisplayName   = FText::FromString(TEXT("4-4-2"));
    Slots =
    {
        MAKE_SLOT(GK,  0.05f, 0.50f,  0.00f, "GK"),
        MAKE_SLOT(LB,  0.25f, 0.10f,  0.05f, "LB"),
        MAKE_SLOT(CB,  0.22f, 0.35f,  0.04f, "CB-L"),
        MAKE_SLOT(CB,  0.22f, 0.65f,  0.04f, "CB-R"),
        MAKE_SLOT(RB,  0.25f, 0.90f,  0.05f, "RB"),
        // Flat four midfield
        MAKE_SLOT(LM,  0.45f, 0.12f,  0.08f, "LM"),
        MAKE_SLOT(CM,  0.46f, 0.37f,  0.08f, "CM-L"),
        MAKE_SLOT(CM,  0.46f, 0.63f,  0.08f, "CM-R"),
        MAKE_SLOT(RM,  0.45f, 0.88f,  0.08f, "RM"),
        // Two strikers
        MAKE_SLOT(ST,  0.76f, 0.38f,  0.12f, "ST-L"),
        MAKE_SLOT(ST,  0.76f, 0.62f,  0.12f, "ST-R"),
    };
}

#undef MAKE_SLOT
