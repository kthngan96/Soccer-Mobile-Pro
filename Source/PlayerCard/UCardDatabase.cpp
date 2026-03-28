// Copyright 2026 Soccer Mobile Pro. All Rights Reserved.
// UCardDatabase.cpp

#include "UCardDatabase.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogCardDatabase, Log, All);

// ---------------------------------------------------------------------------
// Constructor — pre-populate the standard 10-row enhancement table so the
// asset works out-of-the-box even before a designer fills in the DataAsset.
// Values match TECHSPEC Section 2.2.1 Table 3.5.1 exactly.
// ---------------------------------------------------------------------------
UCardDatabase::UCardDatabase()
{
    // +1  : EP 100,    Gold 5,000,    Success 100%
    EnhancementTable.Add({ 1,    100,     5000,   1.00f });
    // +2  : EP 200,    Gold 10,000,   Success 100%
    EnhancementTable.Add({ 2,    200,    10000,   1.00f });
    // +3  : EP 500,    Gold 25,000,   Success 100%
    EnhancementTable.Add({ 3,    500,    25000,   1.00f });
    // +4  : EP 1,000,  Gold 50,000,   Success 80%
    EnhancementTable.Add({ 4,   1000,    50000,   0.80f });
    // +5  : EP 2,000,  Gold 100,000,  Success 70%  << Trait slot unlock
    EnhancementTable.Add({ 5,   2000,   100000,   0.70f });
    // +6  : EP 3,500,  Gold 200,000,  Success 60%
    EnhancementTable.Add({ 6,   3500,   200000,   0.60f });
    // +7  : EP 5,000,  Gold 350,000,  Success 50%  << Skill slot unlock
    EnhancementTable.Add({ 7,   5000,   350000,   0.50f });
    // +8  : EP 8,000,  Gold 500,000,  Success 40%
    EnhancementTable.Add({ 8,   8000,   500000,   0.40f });
    // +9  : EP 12,000, Gold 750,000,  Success 30%
    EnhancementTable.Add({ 9,  12000,   750000,   0.30f });
    // +10 : EP 20,000, Gold 1,000,000,Success 25%  << Evolution ready
    EnhancementTable.Add({ 10, 20000,  1000000,   0.25f });
}

// ---------------------------------------------------------------------------
// UObject overrides
// ---------------------------------------------------------------------------
void UCardDatabase::PostLoad()
{
    Super::PostLoad();
    RebuildCardIndexCache();
    ValidateDatabase();
}

#if WITH_EDITOR
void UCardDatabase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    RebuildCardIndexCache();
}
#endif

// ---------------------------------------------------------------------------
// RebuildCardIndexCache
// ---------------------------------------------------------------------------
void UCardDatabase::RebuildCardIndexCache()
{
    CardIndexCache.Empty(CardDefinitions.Num());
    for (int32 i = 0; i < CardDefinitions.Num(); ++i)
    {
        const FName ID = CardDefinitions[i].CardID;
        if (ID.IsNone())
        {
            UE_LOG(LogCardDatabase, Warning,
                TEXT("Card at index %d has empty CardID — skipped in cache."), i);
            continue;
        }
        if (CardIndexCache.Contains(ID))
        {
            UE_LOG(LogCardDatabase, Error,
                TEXT("Duplicate CardID '%s' at index %d — first occurrence wins."),
                *ID.ToString(), i);
            continue;
        }
        CardIndexCache.Add(ID, i);
    }
    UE_LOG(LogCardDatabase, Verbose,
        TEXT("CardIndexCache rebuilt: %d unique cards."), CardIndexCache.Num());
}

// ---------------------------------------------------------------------------
// GetCardDefinition
// ---------------------------------------------------------------------------
bool UCardDatabase::GetCardDefinition(FName CardID, FCardDefinition& OutDef) const
{
    const int32* Idx = CardIndexCache.Find(CardID);
    if (!Idx || !CardDefinitions.IsValidIndex(*Idx))
    {
        UE_LOG(LogCardDatabase, Warning,
            TEXT("GetCardDefinition: CardID '%s' not found."), *CardID.ToString());
        return false;
    }
    OutDef = CardDefinitions[*Idx];
    return true;
}

// ---------------------------------------------------------------------------
// GetEnhancementRow
// ---------------------------------------------------------------------------
bool UCardDatabase::GetEnhancementRow(int32 TargetLevel, FEnhancementRow& OutRow) const
{
    for (const FEnhancementRow& Row : EnhancementTable)
    {
        if (Row.TargetLevel == TargetLevel)
        {
            OutRow = Row;
            return true;
        }
    }
    UE_LOG(LogCardDatabase, Warning,
        TEXT("GetEnhancementRow: No row found for TargetLevel=%d."), TargetLevel);
    return false;
}

// ---------------------------------------------------------------------------
// GetCardsByTier
// ---------------------------------------------------------------------------
TArray<FCardDefinition> UCardDatabase::GetCardsByTier(ECardTier Tier) const
{
    TArray<FCardDefinition> Result;
    for (const FCardDefinition& Def : CardDefinitions)
    {
        if (Def.Tier == Tier)
        {
            Result.Add(Def);
        }
    }
    return Result;
}

// ---------------------------------------------------------------------------
// GetCardsByPosition
// ---------------------------------------------------------------------------
TArray<FCardDefinition> UCardDatabase::GetCardsByPosition(EPlayerPosition Position) const
{
    TArray<FCardDefinition> Result;
    for (const FCardDefinition& Def : CardDefinitions)
    {
        if (Def.Position == Position)
        {
            Result.Add(Def);
        }
    }
    return Result;
}

// ---------------------------------------------------------------------------
// ValidateDatabase
// ---------------------------------------------------------------------------
void UCardDatabase::ValidateDatabase() const
{
    // --- Enhancement table must have exactly 10 rows ----------
    if (EnhancementTable.Num() != 10)
    {
        UE_LOG(LogCardDatabase, Error,
            TEXT("EnhancementTable has %d rows — expected exactly 10 (TECHSPEC 2.2.1)."),
            EnhancementTable.Num());
    }
    else
    {
        for (int32 i = 0; i < EnhancementTable.Num(); ++i)
        {
            const int32 ExpectedLevel = i + 1;
            if (EnhancementTable[i].TargetLevel != ExpectedLevel)
            {
                UE_LOG(LogCardDatabase, Warning,
                    TEXT("EnhancementTable[%d].TargetLevel = %d, expected %d."),
                    i, EnhancementTable[i].TargetLevel, ExpectedLevel);
            }
        }
    }

    // --- All CardIDs must be unique (cache already enforces this) ---
    if (CardIndexCache.Num() != CardDefinitions.Num())
    {
        UE_LOG(LogCardDatabase, Error,
            TEXT("CardDatabase has %d definitions but only %d unique CardIDs — check for duplicates."),
            CardDefinitions.Num(), CardIndexCache.Num());
    }

    UE_LOG(LogCardDatabase, Log,
        TEXT("UCardDatabase validation complete: %d cards, %d enhancement rows."),
        CardDefinitions.Num(), EnhancementTable.Num());
}
