// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// MTeamFormationData.cpp
// Formation preset data — GDD Section 2.1.3 normalized pitch coordinates.

#include "AI/MTeamFormationData.h"

// ---------------------------------------------------------------------------
// Helper macro — reduces verbosity when building slot arrays
// Args: Index, Role, X (depth), Y (lateral), Label string
// ---------------------------------------------------------------------------
#define SLOT(Idx, RoleEnum, Xf, Yf, Lbl) \
    FFormationSlot(Idx, EFormationRole::RoleEnum, Xf, Yf, \
                   FText::FromString(TEXT(Lbl)))

// ===========================================================================
// 4-3-3
// ---------------------------------------------------------------------------
// Backline    : LB-CB-CB-RB at X≈0.20
// Midfield    : CM-CM-CM at X≈0.50  (wide CMs at Y=0.25/0.75)
// Forward line: LW-ST-RW  at X≈0.80
// ===========================================================================
FFormationDefinition UFormationLibrary::Build_4_3_3()
{
    FFormationDefinition D;
    D.FormationType = EFormationType::Formation_4_3_3;
    D.DisplayName   = FText::FromString(TEXT("4-3-3"));
    D.Slots =
    {
        SLOT( 0, GK,  0.05f, 0.50f, "GK"),
        // Defenders
        SLOT( 1, LB,  0.20f, 0.15f, "LB"),
        SLOT( 2, CB,  0.20f, 0.37f, "CB"),
        SLOT( 3, CB,  0.20f, 0.63f, "CB"),
        SLOT( 4, RB,  0.20f, 0.85f, "RB"),
        // Midfielders
        SLOT( 5, CM,  0.50f, 0.25f, "CM"),
        SLOT( 6, CM,  0.50f, 0.50f, "CM"),
        SLOT( 7, CM,  0.50f, 0.75f, "CM"),
        // Forwards
        SLOT( 8, LW,  0.80f, 0.18f, "LW"),
        SLOT( 9, ST,  0.82f, 0.50f, "ST"),
        SLOT(10, RW,  0.80f, 0.82f, "RW")
    };
    return D;
}

// ===========================================================================
// 4-2-3-1
// ---------------------------------------------------------------------------
// Backline    : LB-CB-CB-RB at X≈0.20
// Double pivot: CDM-CDM     at X≈0.40
// Attacking 3 : LW-CAM-RW   at X≈0.63
// Striker     : ST           at X≈0.82
// ===========================================================================
FFormationDefinition UFormationLibrary::Build_4_2_3_1()
{
    FFormationDefinition D;
    D.FormationType = EFormationType::Formation_4_2_3_1;
    D.DisplayName   = FText::FromString(TEXT("4-2-3-1"));
    D.Slots =
    {
        SLOT( 0, GK,  0.05f, 0.50f, "GK"),
        // Defenders
        SLOT( 1, LB,  0.20f, 0.15f, "LB"),
        SLOT( 2, CB,  0.20f, 0.37f, "CB"),
        SLOT( 3, CB,  0.20f, 0.63f, "CB"),
        SLOT( 4, RB,  0.20f, 0.85f, "RB"),
        // Double pivot
        SLOT( 5, CDM, 0.40f, 0.37f, "CDM"),
        SLOT( 6, CDM, 0.40f, 0.63f, "CDM"),
        // Attacking three
        SLOT( 7, LW,  0.63f, 0.18f, "LW"),
        SLOT( 8, CAM, 0.63f, 0.50f, "CAM"),
        SLOT( 9, RW,  0.63f, 0.82f, "RW"),
        // Striker
        SLOT(10, ST,  0.82f, 0.50f, "ST")
    };
    return D;
}

// ===========================================================================
// 3-5-2
// ---------------------------------------------------------------------------
// Three CBs    at X≈0.20
// Wingbacks    : LWB-RWB at X≈0.45  (Y=0.10 / 0.90)
// Three CMs    at X≈0.50  centre channel
// Two strikers at X≈0.80
// ===========================================================================
FFormationDefinition UFormationLibrary::Build_3_5_2()
{
    FFormationDefinition D;
    D.FormationType = EFormationType::Formation_3_5_2;
    D.DisplayName   = FText::FromString(TEXT("3-5-2"));
    D.Slots =
    {
        SLOT( 0, GK,  0.05f, 0.50f, "GK"),
        // Three centre-backs
        SLOT( 1, CB,  0.20f, 0.27f, "CB"),
        SLOT( 2, CB,  0.20f, 0.50f, "CB"),
        SLOT( 3, CB,  0.20f, 0.73f, "CB"),
        // Wing-backs (high and wide)
        SLOT( 4, LWB, 0.45f, 0.10f, "LWB"),
        SLOT( 5, RWB, 0.45f, 0.90f, "RWB"),
        // Three midfielders
        SLOT( 6, CM,  0.50f, 0.30f, "CM"),
        SLOT( 7, CM,  0.50f, 0.50f, "CM"),
        SLOT( 8, CM,  0.50f, 0.70f, "CM"),
        // Two strikers
        SLOT( 9, ST,  0.80f, 0.38f, "ST"),
        SLOT(10, ST,  0.80f, 0.62f, "ST")
    };
    return D;
}

// ===========================================================================
// 5-4-1
// ---------------------------------------------------------------------------
// Five-back    : LWB-CB-CB-CB-RWB at X≈0.18
// Four midfield: LM-CM-CM-RM       at X≈0.48
// Lone striker  at X≈0.80
// ===========================================================================
FFormationDefinition UFormationLibrary::Build_5_4_1()
{
    FFormationDefinition D;
    D.FormationType = EFormationType::Formation_5_4_1;
    D.DisplayName   = FText::FromString(TEXT("5-4-1"));
    D.Slots =
    {
        SLOT( 0, GK,  0.05f, 0.50f, "GK"),
        // Five defenders
        SLOT( 1, LWB, 0.18f, 0.08f, "LWB"),
        SLOT( 2, CB,  0.18f, 0.28f, "CB"),
        SLOT( 3, CB,  0.18f, 0.50f, "CB"),
        SLOT( 4, CB,  0.18f, 0.72f, "CB"),
        SLOT( 5, RWB, 0.18f, 0.92f, "RWB"),
        // Four midfielders
        SLOT( 6, LM,  0.48f, 0.15f, "LM"),
        SLOT( 7, CM,  0.48f, 0.37f, "CM"),
        SLOT( 8, CM,  0.48f, 0.63f, "CM"),
        SLOT( 9, RM,  0.48f, 0.85f, "RM"),
        // Lone striker
        SLOT(10, ST,  0.80f, 0.50f, "ST")
    };
    return D;
}

// ===========================================================================
// 4-4-2
// ---------------------------------------------------------------------------
// Backline   : LB-CB-CB-RB at X≈0.20
// Midfield 4 : LM-CM-CM-RM  at X≈0.50
// Two strikers at X≈0.80
// ===========================================================================
FFormationDefinition UFormationLibrary::Build_4_4_2()
{
    FFormationDefinition D;
    D.FormationType = EFormationType::Formation_4_4_2;
    D.DisplayName   = FText::FromString(TEXT("4-4-2"));
    D.Slots =
    {
        SLOT( 0, GK,  0.05f, 0.50f, "GK"),
        // Defenders
        SLOT( 1, LB,  0.20f, 0.15f, "LB"),
        SLOT( 2, CB,  0.20f, 0.37f, "CB"),
        SLOT( 3, CB,  0.20f, 0.63f, "CB"),
        SLOT( 4, RB,  0.20f, 0.85f, "RB"),
        // Midfielders
        SLOT( 5, LM,  0.50f, 0.15f, "LM"),
        SLOT( 6, CM,  0.50f, 0.37f, "CM"),
        SLOT( 7, CM,  0.50f, 0.63f, "CM"),
        SLOT( 8, RM,  0.50f, 0.85f, "RM"),
        // Two strikers
        SLOT( 9, ST,  0.80f, 0.38f, "ST"),
        SLOT(10, ST,  0.80f, 0.62f, "ST")
    };
    return D;
}

#undef SLOT

// ---------------------------------------------------------------------------
// UFormationLibrary — static dispatch
// ---------------------------------------------------------------------------
bool UFormationLibrary::GetFormationDefinition(EFormationType FormationType,
                                                FFormationDefinition& OutDefinition)
{
    switch (FormationType)
    {
    case EFormationType::Formation_4_3_3:   OutDefinition = Build_4_3_3();   return true;
    case EFormationType::Formation_4_2_3_1: OutDefinition = Build_4_2_3_1(); return true;
    case EFormationType::Formation_3_5_2:   OutDefinition = Build_3_5_2();   return true;
    case EFormationType::Formation_5_4_1:   OutDefinition = Build_5_4_1();   return true;
    case EFormationType::Formation_4_4_2:   OutDefinition = Build_4_4_2();   return true;
    default:
        UE_LOG(LogTemp, Warning,
               TEXT("UFormationLibrary::GetFormationDefinition — unknown EFormationType %d"),
               static_cast<int32>(FormationType));
        return false;
    }
}

TArray<FFormationDefinition> UFormationLibrary::GetAllFormationDefinitions()
{
    return
    {
        Build_4_3_3(),
        Build_4_2_3_1(),
        Build_3_5_2(),
        Build_5_4_1(),
        Build_4_4_2()
    };
}
