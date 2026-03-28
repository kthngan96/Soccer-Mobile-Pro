// Copyright (c) 2025 MSoccer Studio. All Rights Reserved.

#include "Economy/UPackOpeningSystem.h"
#include "Logging/LogMacros.h"
#include "Math/UnrealMathUtility.h"

DEFINE_LOG_CATEGORY_STATIC(LogPackOpening, Log, All);

// ---------------------------------------------------------------------------
// Internal constants — Standard Pack drop weights (TECHSPEC 6.3.1)
// Weights are out of 10 000 (= 100.00 %) to avoid float precision issues.
// Bronze 5000 + Silver 2700 + Gold 1400 + Platinum 500
//       + Diamond 250 + Legend 125 + Transcendence 25 = 10 000
// ---------------------------------------------------------------------------
namespace PackConstants
{
    // Weights in tier order (Bronze → Transcendence)
    // Used only if a FPackDefinition has an empty DropRateTable (fallback).
    static constexpr int32 DefaultWeights[7] = { 5000, 2700, 1400, 500, 250, 125, 25 };

    static constexpr int32 WeightTotal = 10000;
}

// ---------------------------------------------------------------------------
// USubsystem interface
// ---------------------------------------------------------------------------

void UPackOpeningSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // TODO Milestone-3: load PityCounterMap from save-game slot.
    UE_LOG(LogPackOpening, Log, TEXT("UPackOpeningSystem initialised."));
}

void UPackOpeningSystem::Deinitialize()
{
    // TODO Milestone-3: persist PityCounterMap to save-game slot.
    Super::Deinitialize();
}

// ---------------------------------------------------------------------------
// OpenPack
// ---------------------------------------------------------------------------

bool UPackOpeningSystem::OpenPack(const FPackDefinition& PackDef,
                                   int32 MultiCount,
                                   TArray<FPackOpenResult>& OutResults)
{
    OutResults.Reset();

    // -----------------------------------------------------------------------
    // Step 1 — Validate
    // -----------------------------------------------------------------------
    if (PackDef.PackID.IsNone())
    {
        UE_LOG(LogPackOpening, Error, TEXT("OpenPack called with unnamed PackDef."));
        return false;
    }

    const int32 SafeMulti = FMath::Clamp(MultiCount, 1, 50);
    const int32 TotalCards = PackDef.CardsPerPack * SafeMulti;

    // -----------------------------------------------------------------------
    // Step 2 — Verify cost  (TODO Milestone-2: query UEconomyManager)
    // -----------------------------------------------------------------------
    const int64 TotalCost = static_cast<int64>(PackDef.CostPerPack) * SafeMulti;
    UE_LOG(LogPackOpening, Log,
           TEXT("OpenPack: %s x%d — total cost %lld %s"),
           *PackDef.PackID.ToString(), SafeMulti, TotalCost,
           *UEnum::GetValueAsString(PackDef.CostCurrency));
    // TODO Milestone-2:
    //   UEconomyManager* Eco = GetGameInstance()->GetSubsystem<UEconomyManager>();
    //   FTransactionRecord Rec;
    //   if (!Eco->DeductCurrency(PackDef.CostCurrency, TotalCost,
    //                            ETransactionSource::PackPurchase,
    //                            PackDef.PackID.ToString(), Rec))
    //       return false;

    // -----------------------------------------------------------------------
    // Step 3 — Retrieve or create pity counter for this pack type
    // -----------------------------------------------------------------------
    FPityCounter& Counter = PityCounterMap.FindOrAdd(PackDef.PackID);

    OutResults.Reserve(TotalCards);

    // -----------------------------------------------------------------------
    // Step 4 — Roll each card
    // -----------------------------------------------------------------------
    for (int32 i = 0; i < TotalCards; ++i)
    {
        Counter.TotalPulls++;

        // Increment dry-run counters before the roll.
        Counter.PullsSinceRare++;
        Counter.PullsSinceEpic++;
        Counter.PullsSinceLegendary++;
        Counter.PullsSinceTranscendence++;

        // Roll tier (pity overrides happen inside RollTier).
        bool bPityTriggered = false;
        const ECardTier AwardedTier = RollTier(PackDef, Counter, bPityTriggered);

        // Reset the appropriate sub-counters.
        UpdatePityCounters(Counter, AwardedTier);

        // Pick specific card from pool.
        const FName CardID = PickCardFromPool(PackDef, AwardedTier);

        // Build result entry.
        FPackOpenResult Result;
        Result.AwardedTier        = AwardedTier;
        Result.AwardedCardID      = CardID;
        Result.bWasPityGuarantee  = bPityTriggered;
        Result.PullIndex          = Counter.TotalPulls;
        OutResults.Add(Result);

        // -----------------------------------------------------------------------
        // Step 5 — Analytics
        // -----------------------------------------------------------------------
        FPullEventPayload Payload;
        Payload.PackID          = PackDef.PackID;
        Payload.CardID          = CardID;
        Payload.Tier            = AwardedTier;
        Payload.bPityTriggered  = bPityTriggered;
        Payload.TotalPullCount  = Counter.TotalPulls;
        Payload.Timestamp       = FDateTime::UtcNow();
        LogPullEvent(Payload);
    }

    // -----------------------------------------------------------------------
    // Step 6 — Broadcast
    // -----------------------------------------------------------------------
    OnPackOpened.Broadcast(PackDef, OutResults);

    UE_LOG(LogPackOpening, Log,
           TEXT("OpenPack: %s complete — %d cards drawn (pity: %d/%d/%d/%d)"),
           *PackDef.PackID.ToString(), OutResults.Num(),
           Counter.PullsSinceRare, Counter.PullsSinceEpic,
           Counter.PullsSinceLegendary, Counter.PullsSinceTranscendence);

    return true;
}

// ---------------------------------------------------------------------------
// GetPityCounter
// ---------------------------------------------------------------------------

FPityCounter UPackOpeningSystem::GetPityCounter(FName PackID) const
{
    if (const FPityCounter* Found = PityCounterMap.Find(PackID))
    {
        return *Found;
    }
    return FPityCounter{};
}

// ---------------------------------------------------------------------------
// ResetPityCounter
// ---------------------------------------------------------------------------

void UPackOpeningSystem::ResetPityCounter(FName PackID)
{
    PityCounterMap.Add(PackID, FPityCounter{});
    UE_LOG(LogPackOpening, Warning,
           TEXT("[Debug] Pity counter reset for pack '%s'."), *PackID.ToString());
}

// ---------------------------------------------------------------------------
// LogPullEvent
// ---------------------------------------------------------------------------

void UPackOpeningSystem::LogPullEvent(const FPullEventPayload& Payload)
{
    // Bounded in-memory buffer.
    if (AnalyticsLog.Num() >= MaxAnalyticsLogSize)
    {
        AnalyticsLog.RemoveAt(0, 1, false);
    }
    AnalyticsLog.Add(Payload);

    UE_LOG(LogPackOpening, Verbose,
           TEXT("[Pull #%d] Pack=%s Card=%s Tier=%s Pity=%s"),
           Payload.TotalPullCount,
           *Payload.PackID.ToString(),
           *Payload.CardID.ToString(),
           *UEnum::GetValueAsString(Payload.Tier),
           Payload.bPityTriggered ? TEXT("YES") : TEXT("no"));

    // TODO Milestone-3: POST Payload to backend analytics endpoint.
}

// ---------------------------------------------------------------------------
// RollTier  (TECHSPEC 6.3.1 + 6.3.2)
// ---------------------------------------------------------------------------

ECardTier UPackOpeningSystem::RollTier(const FPackDefinition& PackDef,
                                        const FPityCounter& Counter,
                                        bool& bOutPityTriggered) const
{
    bOutPityTriggered = false;

    // ------------------------------------------------------------------
    // Pity overrides — evaluate highest-tier pity first.
    // TECHSPEC 6.3.2: thresholds are "at least" guarantees, so we check
    // most prestigious to least and return on the first hit.
    // ------------------------------------------------------------------
    if (Counter.PullsSinceTranscendence >= FPityCounter::TranscendencePityThreshold)
    {
        bOutPityTriggered = true;
        return ECardTier::Transcendence;
    }
    if (Counter.PullsSinceLegendary >= FPityCounter::LegendaryPityThreshold)
    {
        bOutPityTriggered = true;
        return ECardTier::Platinum;
    }
    if (Counter.PullsSinceEpic >= FPityCounter::EpicPityThreshold)
    {
        bOutPityTriggered = true;
        return ECardTier::Gold;
    }
    if (Counter.PullsSinceRare >= FPityCounter::RarePityThreshold)
    {
        bOutPityTriggered = true;
        return ECardTier::Silver;
    }

    // ------------------------------------------------------------------
    // Standard weighted roll — 0 … 9999 inclusive.
    // ------------------------------------------------------------------
    const int32 Roll = FMath::RandRange(0, PackConstants::WeightTotal - 1);

    // Use the pack's own drop-rate table if valid, otherwise fall back to
    // the hard-coded standard weights from PackConstants.
    const bool bUseCustomTable = (PackDef.DropRateTable.Num() == 7);

    int32 Cumulative = 0;
    const ECardTier TierOrder[7] =
    {
        ECardTier::Bronze,
        ECardTier::Silver,
        ECardTier::Gold,
        ECardTier::Platinum,
        ECardTier::Diamond,
        ECardTier::Legend,
        ECardTier::Transcendence,
    };

    for (int32 i = 0; i < 7; ++i)
    {
        const int32 Weight = bUseCustomTable
            ? PackDef.DropRateTable[i].DropWeight
            : PackConstants::DefaultWeights[i];

        Cumulative += Weight;
        if (Roll < Cumulative)
        {
            return TierOrder[i];
        }
    }

    // Safety net — should never reach here if weights sum to 10 000.
    UE_LOG(LogPackOpening, Error, TEXT("RollTier: weights do not sum to 10000 for pack '%s'."),
           *PackDef.PackID.ToString());
    return ECardTier::Bronze;
}

// ---------------------------------------------------------------------------
// UpdatePityCounters
// ---------------------------------------------------------------------------

void UPackOpeningSystem::UpdatePityCounters(FPityCounter& Counter,
                                             ECardTier AwardedTier) const
{
    // Any tier >= Silver satisfies the Rare pity bucket.
    if (AwardedTier >= ECardTier::Silver)
    {
        Counter.PullsSinceRare = 0;
    }
    if (AwardedTier >= ECardTier::Gold)
    {
        Counter.PullsSinceEpic = 0;
    }
    if (AwardedTier >= ECardTier::Platinum)
    {
        Counter.PullsSinceLegendary = 0;
    }
    if (AwardedTier == ECardTier::Transcendence)
    {
        Counter.PullsSinceTranscendence = 0;
    }
}

// ---------------------------------------------------------------------------
// PickCardFromPool
// ---------------------------------------------------------------------------

FName UPackOpeningSystem::PickCardFromPool(const FPackDefinition& PackDef,
                                            ECardTier Tier) const
{
    // TODO Milestone-2: query UCardDatabase:
    //   UCardDatabase* DB = ...
    //   TArray<FName> Pool;
    //   for (const FCardDefinition& Def : DB->AllCards)
    //   {
    //       if (Def.Tier != Tier) continue;
    //       if (PackDef.FeaturedCardIDs.Num() > 0 &&
    //           !PackDef.FeaturedCardIDs.Contains(Def.CardID)) continue;
    //       Pool.Add(Def.CardID);
    //   }
    //   if (Pool.Num() > 0)
    //       return Pool[FMath::RandRange(0, Pool.Num() - 1)];
    //   return NAME_None;

    // Stub: return a synthetic card name indicating tier.
    const FString TierStr = UEnum::GetValueAsString(Tier)
        .Replace(TEXT("ECardTier::"), TEXT(""));
    return FName(*FString::Printf(TEXT("STUB_%s_Card"), *TierStr));
}
