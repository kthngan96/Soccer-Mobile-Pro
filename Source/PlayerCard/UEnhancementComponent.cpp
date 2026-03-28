// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// UEnhancementComponent.cpp

#include "UEnhancementComponent.h"
#include "UCardDatabase.h"
#include "Logging/LogMacros.h"
#include "Math/UnrealMathUtility.h"

DEFINE_LOG_CATEGORY_STATIC(LogEnhancement, Log, All);

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
UEnhancementComponent::UEnhancementComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// ---------------------------------------------------------------------------
// TryEnhanceCard — core enhancement loop (TECHSPEC Section 2.2)
// ---------------------------------------------------------------------------
FEnhancementAttemptResult UEnhancementComponent::TryEnhanceCard(
    FCardInstance& CardInstance, bool bUseBreakthroughCard)
{
    FEnhancementAttemptResult AttemptResult;
    AttemptResult.NewEnhancementLevel = CardInstance.EnhancementLevel;

    // ---- 1. Validate card and database ------------------------------------
    if (!ValidateCardAndDatabase(CardInstance))
    {
        AttemptResult.Result = EEnhancementResult::Error_InvalidCard;
        return AttemptResult;
    }

    // ---- 2. Already at max level? ----------------------------------------
    if (CardInstance.IsMaxLevel())
    {
        UE_LOG(LogEnhancement, Warning,
            TEXT("TryEnhanceCard: Card '%s' is already at +10."),
            *CardInstance.CardID.ToString());
        AttemptResult.Result = EEnhancementResult::Error_AlreadyMaxLevel;
        return AttemptResult;
    }

    const int32 TargetLevel = CardInstance.EnhancementLevel + 1;

    // ---- 3. Fetch enhancement row ----------------------------------------
    FEnhancementRow EnhRow;
    if (!CardDatabase->GetEnhancementRow(TargetLevel, EnhRow))
    {
        UE_LOG(LogEnhancement, Error,
            TEXT("TryEnhanceCard: No enhancement row for TargetLevel=%d."),
            TargetLevel);
        AttemptResult.Result = EEnhancementResult::Error_InvalidCard;
        return AttemptResult;
    }

    // ---- 4. Validate balances --------------------------------------------
    const int32 CurrentEP   = GetCurrentEP();
    const int64 CurrentGold = GetCurrentGold();

    if (CurrentEP < EnhRow.EPCost)
    {
        UE_LOG(LogEnhancement, Log,
            TEXT("TryEnhanceCard: Insufficient EP. Have=%d Need=%d."),
            CurrentEP, EnhRow.EPCost);
        AttemptResult.Result = EEnhancementResult::Error_InsufficientEP;
        return AttemptResult;
    }
    if (CurrentGold < EnhRow.GoldCost)
    {
        UE_LOG(LogEnhancement, Log,
            TEXT("TryEnhanceCard: Insufficient Gold. Have=%lld Need=%lld."),
            CurrentGold, EnhRow.GoldCost);
        AttemptResult.Result = EEnhancementResult::Error_InsufficientGold;
        return AttemptResult;
    }

    // ---- 5. Deduct resources (consumed regardless of outcome) ------------
    DeductEP(EnhRow.EPCost);
    DeductGold(EnhRow.GoldCost);
    CardInstance.TotalEPInvested   += EnhRow.EPCost;
    CardInstance.TotalGoldInvested += EnhRow.GoldCost;
    AttemptResult.EPSpent   = EnhRow.EPCost;
    AttemptResult.GoldSpent = EnhRow.GoldCost;

    // ---- 6. Roll success -------------------------------------------------
    // Breakthrough Card (TECHSPEC 2.2.2) guarantees 100% success.
    // Downgrade protection (TECHSPEC 2.2.4): on failure, level stays.
    bool bSuccess = false;
    if (bUseBreakthroughCard)
    {
        bSuccess = true;
        AttemptResult.DebugRollValue = 0.0f; // roll irrelevant
        UE_LOG(LogEnhancement, Log,
            TEXT("TryEnhanceCard: Breakthrough Card used — guaranteed success for '%s' +%d."),
            *CardInstance.CardID.ToString(), TargetLevel);
    }
    else
    {
        const float Roll = FMath::FRand(); // [0.0, 1.0)
        bSuccess = (Roll < EnhRow.SuccessRate);
        AttemptResult.DebugRollValue = Roll;
        UE_LOG(LogEnhancement, Log,
            TEXT("TryEnhanceCard: '%s' +%d — Roll=%.4f SuccessRate=%.2f → %s"),
            *CardInstance.CardID.ToString(), TargetLevel,
            Roll, EnhRow.SuccessRate, bSuccess ? TEXT("SUCCESS") : TEXT("FAILURE"));
    }

    // ---- 7a. SUCCESS -----------------------------------------------------
    if (bSuccess)
    {
        ApplyStatBoost(CardInstance);
        CardInstance.EnhancementLevel = TargetLevel;

        const bool bNewUnlock = CheckAndApplySlotUnlocks(CardInstance);
        AttemptResult.bNewSlotUnlocked  = bNewUnlock;
        AttemptResult.bEvolutionUnlocked = CardInstance.bEvolutionReady;
        AttemptResult.Result = EEnhancementResult::Success;
        AttemptResult.NewEnhancementLevel = TargetLevel;

        if (bNewUnlock)
        {
            OnSlotUnlocked.Broadcast(CardInstance, TargetLevel);
        }
        if (CardInstance.bEvolutionReady)
        {
            OnEvolutionReady.Broadcast(CardInstance);
        }
    }
    // ---- 7b. FAILURE — downgrade protection (level stays) ----------------
    else
    {
        // Enhancement level is NOT decremented (TECHSPEC 2.2.4: downgrade protection).
        AttemptResult.Result = EEnhancementResult::Failure_Protected;
        AttemptResult.NewEnhancementLevel = CardInstance.EnhancementLevel;
        UE_LOG(LogEnhancement, Log,
            TEXT("TryEnhanceCard: Failure — card '%s' stays at +%d (downgrade protection active)."),
            *CardInstance.CardID.ToString(), CardInstance.EnhancementLevel);
    }

    // ---- 8. Broadcast result --------------------------------------------
    OnEnhancementAttempted.Broadcast(CardInstance, AttemptResult);
    return AttemptResult;
}

// ---------------------------------------------------------------------------
// ApplyStatBoost — dispatcher (TECHSPEC Section 2.3)
// ---------------------------------------------------------------------------
void UEnhancementComponent::ApplyStatBoost(FCardInstance& CardInstance)
{
    // Determine whether the card is a goalkeeper by checking the GK attributes.
    // The definition lookup provides the authoritative position, but the
    // IsGoalkeeper() helper on FCardAttributes is fast and sufficient here.
    if (CardInstance.CurrentAttributes.IsGoalkeeper())
    {
        ApplyGoalkeeperBoost(CardInstance);
    }
    else
    {
        ApplyOutfieldBoost(CardInstance);
    }
}

// ---------------------------------------------------------------------------
// ApplyOutfieldBoost (TECHSPEC 2.3 — outfield rules)
// ---------------------------------------------------------------------------
void UEnhancementComponent::ApplyOutfieldBoost(FCardInstance& CardInstance)
{
    FCardAttributes& A = CardInstance.CurrentAttributes;

    // +1 to each outfield attribute per enhancement level, clamped to 99.
    A.Pace       = FMath::Clamp(A.Pace       + 1, 0, 99);
    A.Shooting   = FMath::Clamp(A.Shooting   + 1, 0, 99);
    A.Passing    = FMath::Clamp(A.Passing    + 1, 0, 99);
    A.Dribbling  = FMath::Clamp(A.Dribbling  + 1, 0, 99);
    A.Defending  = FMath::Clamp(A.Defending  + 1, 0, 99);
    A.Physical   = FMath::Clamp(A.Physical   + 1, 0, 99);

    // Recalculate Overall — requires the card's position for weighting.
    // TODO Milestone-2: Fetch position from CardDatabase instead of
    //   relying on IsGoalkeeper() heuristic; load position weights from
    //   DT_PositionWeights DataTable (ASSET_MANIFEST.md).
    //
    // Temporary placeholder: use a simple average of the 6 outfield attrs.
    A.Overall = FMath::Clamp(
        (A.Pace + A.Shooting + A.Passing + A.Dribbling + A.Defending + A.Physical) / 6,
        0, 99);

    UE_LOG(LogEnhancement, Verbose,
        TEXT("ApplyOutfieldBoost: Overall after boost = %d."), A.Overall);
}

// ---------------------------------------------------------------------------
// ApplyGoalkeeperBoost (TECHSPEC 2.3 — GK rules)
// ---------------------------------------------------------------------------
void UEnhancementComponent::ApplyGoalkeeperBoost(FCardInstance& CardInstance)
{
    FCardAttributes& A = CardInstance.CurrentAttributes;

    // +2 to each GK attribute per enhancement level, clamped to 99.
    A.GKDiving       = FMath::Clamp(A.GKDiving       + 2, 0, 99);
    A.GKHandling     = FMath::Clamp(A.GKHandling     + 2, 0, 99);
    A.GKKicking      = FMath::Clamp(A.GKKicking      + 2, 0, 99);
    A.GKPositioning  = FMath::Clamp(A.GKPositioning  + 2, 0, 99);
    A.GKReflexes     = FMath::Clamp(A.GKReflexes     + 2, 0, 99);

    // Recalculate Overall as unweighted average of the 5 GK attributes.
    A.Overall = RecalculateGKOverall(A);

    UE_LOG(LogEnhancement, Verbose,
        TEXT("ApplyGoalkeeperBoost: GK Overall after boost = %d."), A.Overall);
}

// ---------------------------------------------------------------------------
// CheckAndApplySlotUnlocks (TECHSPEC 2.2.3)
// ---------------------------------------------------------------------------
bool UEnhancementComponent::CheckAndApplySlotUnlocks(FCardInstance& CardInstance)
{
    bool bAnyNewUnlock = false;
    const int32 Level = CardInstance.EnhancementLevel;

    // +5 → Trait slot unlock
    if (Level >= 5 && !CardInstance.bTraitSlotUnlocked)
    {
        CardInstance.bTraitSlotUnlocked = true;
        bAnyNewUnlock = true;
        UE_LOG(LogEnhancement, Log,
            TEXT("CheckAndApplySlotUnlocks: Trait slot unlocked for '%s' at +%d."),
            *CardInstance.CardID.ToString(), Level);
    }

    // +7 → Skill slot unlock
    if (Level >= 7 && !CardInstance.bSkillSlotUnlocked)
    {
        CardInstance.bSkillSlotUnlocked = true;
        bAnyNewUnlock = true;
        UE_LOG(LogEnhancement, Log,
            TEXT("CheckAndApplySlotUnlocks: Skill slot unlocked for '%s' at +%d."),
            *CardInstance.CardID.ToString(), Level);
    }

    // +10 → Evolution ready
    if (Level >= 10 && !CardInstance.bEvolutionReady)
    {
        CardInstance.bEvolutionReady = true;
        bAnyNewUnlock = true;
        UE_LOG(LogEnhancement, Log,
            TEXT("CheckAndApplySlotUnlocks: Evolution-ready flag set for '%s' at +10."),
            *CardInstance.CardID.ToString());
    }

    return bAnyNewUnlock;
}

// ---------------------------------------------------------------------------
// GetEPCostForLevel
// ---------------------------------------------------------------------------
int32 UEnhancementComponent::GetEPCostForLevel(int32 TargetLevel) const
{
    if (!CardDatabase) return -1;
    FEnhancementRow Row;
    return CardDatabase->GetEnhancementRow(TargetLevel, Row) ? Row.EPCost : -1;
}

// ---------------------------------------------------------------------------
// GetGoldCostForLevel
// ---------------------------------------------------------------------------
int64 UEnhancementComponent::GetGoldCostForLevel(int32 TargetLevel) const
{
    if (!CardDatabase) return -1;
    FEnhancementRow Row;
    return CardDatabase->GetEnhancementRow(TargetLevel, Row) ? Row.GoldCost : -1;
}

// ---------------------------------------------------------------------------
// GetSuccessRateForLevel
// ---------------------------------------------------------------------------
float UEnhancementComponent::GetSuccessRateForLevel(int32 TargetLevel) const
{
    if (!CardDatabase) return 0.0f;
    FEnhancementRow Row;
    return CardDatabase->GetEnhancementRow(TargetLevel, Row) ? Row.SuccessRate : 0.0f;
}

// ---------------------------------------------------------------------------
// RecalculateGKOverall
// ---------------------------------------------------------------------------
int32 UEnhancementComponent::RecalculateGKOverall(const FCardAttributes& Attrs) const
{
    return FMath::Clamp(
        (Attrs.GKDiving + Attrs.GKHandling + Attrs.GKKicking +
         Attrs.GKPositioning + Attrs.GKReflexes) / 5,
        0, 99);
}

// ---------------------------------------------------------------------------
// RecalculateOutfieldOverall
// TODO Milestone-2: Replace with DataTable-driven position weights.
// ---------------------------------------------------------------------------
int32 UEnhancementComponent::RecalculateOutfieldOverall(
    const FCardAttributes& Attrs, EPlayerPosition Position) const
{
    // Placeholder: unweighted average of 6 outfield stats.
    // A proper position-weighted formula will be driven by DT_PositionWeights.
    return FMath::Clamp(
        (Attrs.Pace + Attrs.Shooting + Attrs.Passing +
         Attrs.Dribbling + Attrs.Defending + Attrs.Physical) / 6,
        0, 99);
}

// ---------------------------------------------------------------------------
// ValidateCardAndDatabase
// ---------------------------------------------------------------------------
bool UEnhancementComponent::ValidateCardAndDatabase(const FCardInstance& CardInstance) const
{
    if (!CardDatabase)
    {
        UE_LOG(LogEnhancement, Error,
            TEXT("UEnhancementComponent: CardDatabase is null. Assign it before calling TryEnhanceCard."));
        return false;
    }
    if (CardInstance.CardID.IsNone())
    {
        UE_LOG(LogEnhancement, Error,
            TEXT("UEnhancementComponent: CardInstance has empty CardID."));
        return false;
    }
    FCardDefinition Def;
    if (!CardDatabase->GetCardDefinition(CardInstance.CardID, Def))
    {
        UE_LOG(LogEnhancement, Error,
            TEXT("UEnhancementComponent: CardID '%s' not found in CardDatabase."),
            *CardInstance.CardID.ToString());
        return false;
    }
    return true;
}
