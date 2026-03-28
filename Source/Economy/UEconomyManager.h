// Copyright (c) 2025 MSoccer Studio. All Rights Reserved.
// TECHSPEC Section 6 — Economy & Currency
// GDD Section 7 — Monetisation & Packs

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UEconomyManager.generated.h"

// ---------------------------------------------------------------------------
// ECurrencyType
// ---------------------------------------------------------------------------
/**
 * @brief Identifies which in-game currency is being referenced.
 *
 * Matches the four wallet columns defined in TECHSPEC 6.1.
 */
UENUM(BlueprintType)
enum class ECurrencyType : uint8
{
    /** Soft currency earned through matches and daily quests. */
    GoldCoins   UMETA(DisplayName = "Gold Coins"),

    /** Premium currency purchased or earned from ranked rewards. */
    Gems        UMETA(DisplayName = "Gems"),

    /** Enhancement Points consumed by UEnhancementComponent. */
    EP          UMETA(DisplayName = "Enhancement Points"),

    /** Experience fed into the Skill Tree (TECHSPEC 5.x). */
    SkillXP     UMETA(DisplayName = "Skill XP"),
};

// ---------------------------------------------------------------------------
// ETransactionSource
// ---------------------------------------------------------------------------
/**
 * @brief High-level category that produced or consumed a currency amount.
 *
 * Used for analytics grouping and audit filtering.
 */
UENUM(BlueprintType)
enum class ETransactionSource : uint8
{
    MatchReward     UMETA(DisplayName = "Match Reward"),
    DailyQuest      UMETA(DisplayName = "Daily Quest"),
    PackPurchase    UMETA(DisplayName = "Pack Purchase"),
    PackOpening     UMETA(DisplayName = "Pack Opening Drop"),
    Enhancement     UMETA(DisplayName = "Enhancement Cost"),
    AdminGrant      UMETA(DisplayName = "Admin / Promotion Grant"),
    Other           UMETA(DisplayName = "Other"),
};

// ---------------------------------------------------------------------------
// FTransactionRecord
// ---------------------------------------------------------------------------
/**
 * @brief Immutable record of a single currency flow event.
 *
 * Appended to the in-memory transaction log held by UEconomyManager.  
 * Serialisation to backend is handled in Milestone-3.
 */
USTRUCT(BlueprintType)
struct MSOCCER_API FTransactionRecord
{
    GENERATED_BODY()

    /** Unique sequential ID within this game session. */
    UPROPERTY(BlueprintReadOnly, Category = "Economy|Transaction")
    int64 TransactionID = 0;

    /** Wall-clock timestamp of the transaction (UTC). */
    UPROPERTY(BlueprintReadOnly, Category = "Economy|Transaction")
    FDateTime Timestamp;

    /** Which wallet was affected. */
    UPROPERTY(BlueprintReadOnly, Category = "Economy|Transaction")
    ECurrencyType Currency = ECurrencyType::GoldCoins;

    /**
     * Signed delta applied to the wallet.
     * Positive  = credit (earn).  Negative = debit (spend).
     */
    UPROPERTY(BlueprintReadOnly, Category = "Economy|Transaction")
    int64 Delta = 0;

    /** Wallet balance *after* this transaction was applied. */
    UPROPERTY(BlueprintReadOnly, Category = "Economy|Transaction")
    int64 BalanceAfter = 0;

    /** What triggered this transaction. */
    UPROPERTY(BlueprintReadOnly, Category = "Economy|Transaction")
    ETransactionSource Source = ETransactionSource::Other;

    /** Optional free-text note for debugging or CS tooling. */
    UPROPERTY(BlueprintReadOnly, Category = "Economy|Transaction")
    FString Note;
};

// ---------------------------------------------------------------------------
// Delegate declarations
// ---------------------------------------------------------------------------

/** Fired after any successful currency change. Parameters: type, new balance, record. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnBalanceChanged,
    ECurrencyType, Currency,
    int64, NewBalance,
    const FTransactionRecord&, Record
);

/** Fired when a deduction is rejected due to insufficient funds. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnInsufficientFunds,
    ECurrencyType, Currency,
    int64, RequiredAmount
);

// ---------------------------------------------------------------------------
// UEconomyManager
// ---------------------------------------------------------------------------
/**
 * @brief Game-instance subsystem managing all player currency wallets.
 *
 * Lifetime: exists for the entire game session.  Access via:  
 * @code
 *   UEconomyManager* Eco = GetGameInstance()->GetSubsystem<UEconomyManager>();
 * @endcode
 *
 * ### Currency Overview (TECHSPEC 6.1)
 * | Currency   | Primary Source            | Primary Sink              |
 * |------------|---------------------------|---------------------------|
 * | GoldCoins  | Match rewards, quests     | Enhancement gold cost     |
 * | Gems       | Purchase / ranked rewards | Pack purchases            |
 * | EP         | Match / daily quest drops | Enhancement EP cost       |
 * | SkillXP    | Match performance         | Skill tree upgrades       |
 *
 * TODO Milestone-2: load persistent balances from save-game slot.  
 * TODO Milestone-3: sync balance deltas to backend ledger API.
 */
UCLASS()
class MSOCCER_API UEconomyManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    // -----------------------------------------------------------------------
    // USubsystem interface
    // -----------------------------------------------------------------------

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // -----------------------------------------------------------------------
    // Core wallet API
    // -----------------------------------------------------------------------

    /**
     * @brief Credits an amount to the specified wallet.
     *
     * Always succeeds (no ceiling enforced at this layer — LiveOps can raise
     * caps via remote config in Milestone-3).  Broadcasts OnBalanceChanged.
     *
     * @param Currency   Target wallet.
     * @param Amount     Positive value to add.  Clamped to 1 if ≤ 0.
     * @param Source     Audit category.
     * @param Note       Optional free-text context.
     * @return           Updated balance after the credit.
     */
    UFUNCTION(BlueprintCallable, Category = "Economy",
              meta = (DisplayName = "Add Currency"))
    int64 AddCurrency(ECurrencyType Currency, int64 Amount,
                      ETransactionSource Source = ETransactionSource::Other,
                      const FString& Note = TEXT(""));

    /**
     * @brief Attempts to deduct an amount from the specified wallet.
     *
     * Fails atomically (no balance mutation) if funds are insufficient.  
     * Broadcasts OnInsufficientFunds on failure; OnBalanceChanged on success.
     *
     * @param Currency   Target wallet.
     * @param Amount     Positive value to remove.
     * @param Source     Audit category.
     * @param Note       Optional free-text context.
     * @param[out] OutRecord  Filled on success with the transaction record.
     * @return           true if deduction succeeded, false if insufficient.
     */
    UFUNCTION(BlueprintCallable, Category = "Economy",
              meta = (DisplayName = "Deduct Currency"))
    bool DeductCurrency(ECurrencyType Currency, int64 Amount,
                        ETransactionSource Source = ETransactionSource::Other,
                        const FString& Note = TEXT(""),
                        FTransactionRecord& OutRecord = *static_cast<FTransactionRecord*>(nullptr));

    /**
     * @brief Returns the current balance of a wallet.  Thread-safe read.
     *
     * @param Currency   Target wallet.
     * @return           Current balance (≥ 0).
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Economy",
              meta = (DisplayName = "Get Balance"))
    int64 GetBalance(ECurrencyType Currency) const;

    /**
     * @brief Returns true if the wallet holds at least the requested amount.
     *
     * Convenience wrapper used by UI to grey-out buttons.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Economy",
              meta = (DisplayName = "Can Afford"))
    bool CanAfford(ECurrencyType Currency, int64 Amount) const;

    // -----------------------------------------------------------------------
    // Transaction history
    // -----------------------------------------------------------------------

    /**
     * @brief Returns a copy of the in-memory transaction log.
     *
     * The log is bounded to MaxTransactionLogSize entries (oldest pruned).  
     * TODO Milestone-3: flush to persistent storage before pruning.
     */
    UFUNCTION(BlueprintCallable, Category = "Economy|Debug",
              meta = (DisplayName = "Get Transaction Log"))
    TArray<FTransactionRecord> GetTransactionLog() const;

    /**
     * @brief Returns the most recent N transactions for a specific wallet.
     *
     * @param Currency  Wallet filter.
     * @param Count     Maximum entries to return (newest-first).
     */
    UFUNCTION(BlueprintCallable, Category = "Economy|Debug",
              meta = (DisplayName = "Get Recent Transactions"))
    TArray<FTransactionRecord> GetRecentTransactions(ECurrencyType Currency, int32 Count = 10) const;

    // -----------------------------------------------------------------------
    // Delegates
    // -----------------------------------------------------------------------

    /** Broadcast after any successful credit or debit. */
    UPROPERTY(BlueprintAssignable, Category = "Economy|Events")
    FOnBalanceChanged OnBalanceChanged;

    /** Broadcast when DeductCurrency fails due to insufficient funds. */
    UPROPERTY(BlueprintAssignable, Category = "Economy|Events")
    FOnInsufficientFunds OnInsufficientFunds;

private:

    // -----------------------------------------------------------------------
    // Internal helpers
    // -----------------------------------------------------------------------

    /**
     * @brief Writes a new FTransactionRecord to the log and broadcasts.
     *
     * @param Currency     Affected wallet.
     * @param Delta        Signed amount applied.
     * @param BalanceAfter Resulting balance.
     * @param Source       Audit category.
     * @param Note         Free-text annotation.
     * @return             The recorded struct (also appended to TransactionLog).
     */
    FTransactionRecord RecordTransaction(ECurrencyType Currency, int64 Delta,
                                         int64 BalanceAfter,
                                         ETransactionSource Source,
                                         const FString& Note);

    /** Returns a mutable reference to the wallet backing the enum value. */
    int64& WalletRef(ECurrencyType Currency);

    /** Returns a const reference to the wallet backing the enum value. */
    const int64& WalletRefConst(ECurrencyType Currency) const;

    // -----------------------------------------------------------------------
    // Wallet storage
    // -----------------------------------------------------------------------

    UPROPERTY()
    int64 Balance_GoldCoins = 0;

    UPROPERTY()
    int64 Balance_Gems = 0;

    UPROPERTY()
    int64 Balance_EP = 0;

    UPROPERTY()
    int64 Balance_SkillXP = 0;

    // -----------------------------------------------------------------------
    // Transaction log
    // -----------------------------------------------------------------------

    /** Maximum entries kept in TransactionLog before oldest are pruned. */
    static constexpr int32 MaxTransactionLogSize = 500;

    /** Monotonically increasing ID counter for this session. */
    int64 NextTransactionID = 1;

    /** Circular in-memory log of all committed transactions. */
    TArray<FTransactionRecord> TransactionLog;
};
