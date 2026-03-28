// Copyright (c) 2025 MSoccer Studio. All Rights Reserved.

#include "Economy/UEconomyManager.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogEconomy, Log, All);

// ---------------------------------------------------------------------------
// USubsystem interface
// ---------------------------------------------------------------------------

void UEconomyManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // TODO Milestone-2: load persisted balances from save-game slot.
    UE_LOG(LogEconomy, Log, TEXT("UEconomyManager initialised — all balances start at 0."));
}

void UEconomyManager::Deinitialize()
{
    // TODO Milestone-3: flush TransactionLog to backend ledger before teardown.
    Super::Deinitialize();
}

// ---------------------------------------------------------------------------
// Core wallet API
// ---------------------------------------------------------------------------

int64 UEconomyManager::AddCurrency(ECurrencyType Currency,
                                    int64 Amount,
                                    ETransactionSource Source,
                                    const FString& Note)
{
    // Clamp nonsensical amounts silently; caller should validate before calling.
    const int64 SafeAmount = FMath::Max<int64>(Amount, 1);

    int64& Wallet = WalletRef(Currency);
    Wallet += SafeAmount;

    const FTransactionRecord Record = RecordTransaction(Currency, SafeAmount, Wallet, Source, Note);

    UE_LOG(LogEconomy, Verbose,
           TEXT("[AddCurrency] %s +%lld => balance %lld  (%s)"),
           *UEnum::GetValueAsString(Currency), SafeAmount, Wallet, *Note);

    return Wallet;
}

bool UEconomyManager::DeductCurrency(ECurrencyType Currency,
                                      int64 Amount,
                                      ETransactionSource Source,
                                      const FString& Note,
                                      FTransactionRecord& OutRecord)
{
    const int64 CurrentBalance = GetBalance(Currency);

    if (CurrentBalance < Amount)
    {
        UE_LOG(LogEconomy, Warning,
               TEXT("[DeductCurrency] Insufficient %s: need %lld, have %lld."),
               *UEnum::GetValueAsString(Currency), Amount, CurrentBalance);

        OnInsufficientFunds.Broadcast(Currency, Amount);
        return false;
    }

    int64& Wallet = WalletRef(Currency);
    Wallet -= Amount;

    OutRecord = RecordTransaction(Currency, -Amount, Wallet, Source, Note);

    UE_LOG(LogEconomy, Verbose,
           TEXT("[DeductCurrency] %s -%lld => balance %lld  (%s)"),
           *UEnum::GetValueAsString(Currency), Amount, Wallet, *Note);

    return true;
}

int64 UEconomyManager::GetBalance(ECurrencyType Currency) const
{
    return WalletRefConst(Currency);
}

bool UEconomyManager::CanAfford(ECurrencyType Currency, int64 Amount) const
{
    return GetBalance(Currency) >= Amount;
}

// ---------------------------------------------------------------------------
// Transaction history
// ---------------------------------------------------------------------------

TArray<FTransactionRecord> UEconomyManager::GetTransactionLog() const
{
    return TransactionLog;
}

TArray<FTransactionRecord> UEconomyManager::GetRecentTransactions(
    ECurrencyType Currency, int32 Count) const
{
    TArray<FTransactionRecord> Result;
    Result.Reserve(Count);

    // Walk backwards (newest first).
    for (int32 i = TransactionLog.Num() - 1; i >= 0 && Result.Num() < Count; --i)
    {
        if (TransactionLog[i].Currency == Currency)
        {
            Result.Add(TransactionLog[i]);
        }
    }
    return Result;
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

FTransactionRecord UEconomyManager::RecordTransaction(
    ECurrencyType Currency, int64 Delta, int64 BalanceAfter,
    ETransactionSource Source, const FString& Note)
{
    FTransactionRecord Record;
    Record.TransactionID = NextTransactionID++;
    Record.Timestamp     = FDateTime::UtcNow();
    Record.Currency      = Currency;
    Record.Delta         = Delta;
    Record.BalanceAfter  = BalanceAfter;
    Record.Source        = Source;
    Record.Note          = Note;

    // Prune oldest entry if we've hit the cap.
    if (TransactionLog.Num() >= MaxTransactionLogSize)
    {
        TransactionLog.RemoveAt(0, 1, false);
    }
    TransactionLog.Add(Record);

    // Notify listeners.
    OnBalanceChanged.Broadcast(Currency, BalanceAfter, Record);
    return Record;
}

int64& UEconomyManager::WalletRef(ECurrencyType Currency)
{
    switch (Currency)
    {
        case ECurrencyType::GoldCoins: return Balance_GoldCoins;
        case ECurrencyType::Gems:      return Balance_Gems;
        case ECurrencyType::EP:        return Balance_EP;
        case ECurrencyType::SkillXP:   return Balance_SkillXP;
        default:
            UE_LOG(LogEconomy, Error, TEXT("WalletRef: unknown ECurrencyType %d"), (int32)Currency);
            return Balance_GoldCoins; // safe fallback
    }
}

const int64& UEconomyManager::WalletRefConst(ECurrencyType Currency) const
{
    switch (Currency)
    {
        case ECurrencyType::GoldCoins: return Balance_GoldCoins;
        case ECurrencyType::Gems:      return Balance_Gems;
        case ECurrencyType::EP:        return Balance_EP;
        case ECurrencyType::SkillXP:   return Balance_SkillXP;
        default:
            UE_LOG(LogEconomy, Error, TEXT("WalletRefConst: unknown ECurrencyType %d"), (int32)Currency);
            return Balance_GoldCoins;
    }
}
