// Copyright (c) Soccer Mobile Pro. All Rights Reserved.
// GDD Section 2.1.3 | TECHSPEC Section 9.1 — AI: Behavior Trees + EQS

#include "MTeamFormationData.h"

// ─────────────────────────────────────────────────────────────────────────────
// UMTeamFormationData
// ─────────────────────────────────────────────────────────────────────────────

TArray<FFormationData> UMTeamFormationData::GetDefaultFormations()
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

FFormationData UMTeamFormationData::Build_4_3_3()
{
    FFormationData F(EFormationType::Formation_4_3_3);
    // GK
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

FFormationData UMTeamFormationData::Build_4_2_3_1()
{
    FFormationData F(EFormationType::Formation_4_2_3_1);
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

FFormationData UMTeamFormationData::Build_3_5_2()
{
    FFormationData F(EFormationType::Formation_3_5_2);
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

FFormationData UMTeamFormationData::Build_5_4_1()
{
    FFormationData F(EFormationType::Formation_5_4_1);
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

FFormationData UMTeamFormationData::Build_4_4_2()
{
    FFormationData F(EFormationType::Formation_4_4_2);
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
