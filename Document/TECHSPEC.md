# SOCCER MOBILE PRO - TECHNICAL SPECIFICATION APPENDIX
## For AI Code Generation (ChatGPT Codex / Claude)

**Version:** 1.1  
**Date:** March 2026  
**Engine:** Unreal Engine 4.27+  
**Language:** C++ & Blueprint  
**Audience:** AI Developers & Development Teams

---

## TABLE OF CONTENTS

1. [Core Data Models](#1-core-data-models)
2. [Enhancement System (FIFA Online 3-Inspired)](#2-enhancement-system-fifa-online-3-inspired)
3. [Match Engine State Machine](#3-match-engine-state-machine)
4. [Input & Skill System (FIFA Mobile-Inspired)](#4-input--skill-system-fifa-mobile-inspired)
5. [Networking & Authoritative Model](#5-networking--authoritative-model)
6. [Economy & Pack Opening](#6-economy--pack-opening)
7. [Chemistry & Squad System](#7-chemistry--squad-system)
8. [Database Schema & Queries](#8-database-schema--queries)
9. [AI Development Prompts for Codex](#9-ai-development-prompts-for-codex)

---

## 1. CORE DATA MODELS

### 1.1 PlayerCard Data Schema

**Complete JSON structure for card instances:**

```json
{
  "CardId": "string",
  "PlayerId": "string",
  "DisplayName": "string",
  "ClubId": "string",
  "LeagueId": "string",
  "NationId": "string",
  "Position": "string",
  "Tier": "int",
  "RarityName": "string",
  "BaseOverall": "int",
  "EnhancementLevel": "int",
  "SkillStars": "int",
  "WeakFoot": "int",
  "IsTradable": "bool",
  "CardType": "string",
  "Attributes": {
    "Pace": {
      "SprintSpeed": "int",
      "Acceleration": "int"
    },
    "Shooting": {
      "Finishing": "int",
      "LongShots": "int",
      "ShotPower": "int",
      "Volleys": "int",
      "Penalties": "int"
    },
    "Passing": {
      "ShortPassing": "int",
      "LongPassing": "int",
      "Vision": "int",
      "Crossing": "int",
      "FKAccuracy": "int"
    },
    "Dribbling": {
      "BallControl": "int",
      "Agility": "int",
      "Balance": "int",
      "Reactions": "int",
      "Composure": "int"
    },
    "Defending": {
      "Interceptions": "int",
      "Marking": "int",
      "SlidingTackle": "int",
      "StandingTackle": "int",
      "Heading": "int"
    },
    "Physicality": {
      "Strength": "int",
      "Stamina": "int",
      "Aggression": "int",
      "Jumping": "int"
    },
    "Goalkeeping": {
      "Diving": "int",
      "Handling": "int",
      "Kicking": "int",
      "Positioning": "int",
      "Reflexes": "int",
      "Speed": "int"
    }
  },
  "Traits": [ "string" ],
  "SkillUnlocks": [ "string" ],
  "FaceModelId": "string",
  "CreatedAt": "string",
  "LastUpdatedAt": "string"
}
```

### 1.2 EffectiveOverall Calculation

**Formula:**

```
EffectiveOverall = BaseOverall + EnhancementBonus + ChemistryBonus + MasteryBonus

Where:
  - EnhancementBonus = EnhancementLevel (0..10)
  - ChemistryBonus = floor(BaseOverall × ChemistryPercentage)
    • Perfect Chemistry (Same Club + Nation): +10%
    • Club Chemistry: +6%
    • League Chemistry: +4%
    • Nation Chemistry: +3%
    • No Chemistry: 0%
  - MasteryBonus = 5 if Mastery Badge applied, else 0
```

**C++ Implementation Example:**

```cpp
int32 FCardStatistics::CalculateEffectiveOverall(
    int32 BaseOVR,
    int32 EnhancementLvl,
    float ChemistryPercent,
    bool bHasMasteryBadge
)
{
    int32 EnhancementBonus = EnhancementLvl;
    int32 ChemistryBonus = FMath::Floor(BaseOVR * ChemistryPercent);
    int32 MasteryBonus = bHasMasteryBadge ? 5 : 0;
    
    return BaseOVR + EnhancementBonus + ChemistryBonus + MasteryBonus;
}
```

### 1.3 UserSquad Schema

```json
{
  "UserId": "string",
  "SquadId": "string",
  "Name": "string",
  "Formation": "string",
  "StartingXI": [ "CardInstanceId" ],
  "Substitutes": [ "CardInstanceId" ],
  "Tactics": {
    "PressingIntensity": "string",
    "DefensiveLine": "string",
    "Width": "string",
    "BuildUpPlay": "string",
    "AttackingStyle": "string"
  },
  "ChemistryScore": "int",
  "CreatedAt": "string",
  "UpdatedAt": "string"
}
```

**Valid Formation Values:**
- 4-3-3, 4-2-3-1, 3-5-2, 5-4-1, 4-4-2, 5-3-2, 4-1-4-1, 3-4-3, Custom

### 1.4 CardInstance vs CardDefinition

**CardDefinition** (immutable, stored in DataAsset):
- Template for all instances of a player card
- Contains base stats, artwork, animations

**CardInstance** (mutable, per-user):
- Individual copy owned by a player
- Tracks EnhancementLevel, traits unlocked, etc.
- Stored in database, cached in memory

```cpp
UCLASS()
class ACardDefinition : public UDataAsset
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere)
    FString CardId;
    
    UPROPERTY(EditAnywhere)
    FString PlayerId;
    
    UPROPERTY(EditAnywhere)
    int32 BaseOverall;
    
    UPROPERTY(EditAnywhere)
    int32 DefaultSkillStars;
    
    UPROPERTY(EditAnywhere)
    class UFaceRenderComponent* FaceTemplate;
};
```

---

## 2. ENHANCEMENT SYSTEM (FIFA ONLINE 3-INSPIRED)

### 2.1 Enhancement Levels Table

**Hardcoded or Config-driven:**

| Level | Stat Boost | EP Cost | Gold Cost | Success % | Unlocks |
|-------|------------|---------|-----------|-----------|---------|
| +1    | +1 all    | 100     | 5,000     | 100%      | —       |
| +2    | +2 OVR    | 200     | 10,000    | 100%      | —       |
| +3    | +3 OVR    | 400     | 20,000    | 100%      | —       |
| +4    | +4 OVR    | 800     | 40,000    | 85%       | —       |
| +5    | +5 OVR    | 1,500   | 80,000    | 75%       | Trait Slot |
| +6    | +6 OVR    | 2,500   | 150,000   | 65%       | —       |
| +7    | +7 OVR    | 4,000   | 250,000   | 55%       | Skill Slot |
| +8    | +8 OVR    | 6,000   | 400,000   | 45%       | —       |
| +9    | +9 OVR    | 10,000  | 600,000   | 35%       | —       |
| +10   | +10 OVR   | 20,000  | 1,000,000 | 25%       | Evolution Ready |

### 2.2 Enhancement Algorithm (Server-Authoritative)

**Pseudo-code:**

```
FUNCTION TryEnhanceCard(userId, cardInstanceId, useBreakthroughProtection, useDowngradeProtection):
    card = LoadCardInstance(userId, cardInstanceId)
    
    IF card.EnhancementLevel >= 10:
        RETURN Error("Card already at max level")
    
    level = card.EnhancementLevel
    params = ENHANCEMENT_TABLE[level + 1]
    
    IF NOT UserHasEnoughCurrency(userId, EP=params.EP, Gold=params.Gold):
        RETURN Error("Insufficient currency")
    
    DeductCurrency(userId, EP=params.EP, Gold=params.Gold)
    
    successRate = params.SuccessRate
    
    IF useBreakthroughProtection:
        IF NOT UserHasItem(userId, "BreakthroughCard"):
            RETURN Error("No Breakthrough Card")
        ConsumeItem(userId, "BreakthroughCard")
        successRate = 1.0
    
    roll = RANDOM_FLOAT(0, 1)
    
    IF roll <= successRate:
        card.EnhancementLevel += 1
        ApplyStatBoost(card, params.StatBoostPerAttribute)
        
        IF card.EnhancementLevel == 5:
            UnlockTraitSlot(card)
        IF card.EnhancementLevel == 7:
            UnlockSkillSlot(card)
        IF card.EnhancementLevel == 10:
            MarkEligibleForEvolution(card)
        
        SaveCardInstance(card)
        LogEvent("CARD_ENHANCED", userId, cardInstanceId, level + 1)
        RETURN { "Result": "Success", "NewLevel": card.EnhancementLevel }
    ELSE:
        // Failure: card level stays the same (Downgrade Protection)
        SaveCardInstance(card)
        LogEvent("CARD_ENHANCE_FAILED", userId, cardInstanceId, level)
        RETURN { "Result": "Fail", "NewLevel": card.EnhancementLevel }
END FUNCTION
```

### 2.3 ApplyStatBoost Function

```cpp
void FEnhancementSystem::ApplyStatBoost(FCardInstance& Card, int32 BoostPerAttribute)
{
    if (Card.Position == "GK")
    {
        Card.Attributes.Goalkeeping.Diving += BoostPerAttribute;
        Card.Attributes.Goalkeeping.Handling += BoostPerAttribute;
        Card.Attributes.Goalkeeping.Kicking += BoostPerAttribute;
        Card.Attributes.Goalkeeping.Positioning += BoostPerAttribute;
        Card.Attributes.Goalkeeping.Reflexes += BoostPerAttribute;
        Card.Attributes.Goalkeeping.Speed += BoostPerAttribute;
    }
    else
    {
        Card.Attributes.Pace.SprintSpeed += BoostPerAttribute;
        Card.Attributes.Pace.Acceleration += BoostPerAttribute;
        Card.Attributes.Shooting.Finishing += BoostPerAttribute;
        Card.Attributes.Shooting.LongShots += BoostPerAttribute;
        Card.Attributes.Shooting.ShotPower += BoostPerAttribute;
        Card.Attributes.Passing.ShortPassing += BoostPerAttribute;
        Card.Attributes.Passing.LongPassing += BoostPerAttribute;
        Card.Attributes.Dribbling.BallControl += BoostPerAttribute;
        Card.Attributes.Defending.Interceptions += BoostPerAttribute;
        Card.Attributes.Physicality.Strength += BoostPerAttribute;
        Card.Attributes.Physicality.Stamina += BoostPerAttribute;
    }
}
```

### 2.4 Evolution System

**Prerequisites:**
- Base card at +10
- 3× same player at any enhancement level (fodder)
- Required Evolution Shards (scaling by tier)
- Evolution cost in coins (scaling by tier)

**Evolution Shards Table:**

| Tier | Required Shards | Evolution Cost (Gold) |
|------|-----------------|----------------------|
| 1→2  | 10              | 50,000               |
| 2→3  | 15              | 100,000              |
| 3→4  | 20              | 200,000              |
| 4→5  | 30              | 500,000              |
| 5→6  | 50              | 1,000,000            |
| 6→7  | 100             | 2,000,000            |

**Algorithm:**

```
FUNCTION TryEvolveCard(userId, baseCardInstanceId, fodderInstanceIds[], shardCount):
    baseCard = LoadCardInstance(userId, baseCardInstanceId)
    
    IF baseCard.EnhancementLevel < 10:
        RETURN Error("Base card must be at +10")
    
    IF LENGTH(fodderInstanceIds) < 3:
        RETURN Error("Need minimum 3 fodder cards")
    
    fodderCards = LoadCardInstances(userId, fodderInstanceIds)
    FOR EACH fodderCard IN fodderCards:
        IF fodderCard.PlayerId != baseCard.PlayerId:
            RETURN Error("All fodder must be same player")
    
    requiredShards = EVOLUTION_SHARDS_TABLE[baseCard.Tier]
    requiredGold = EVOLUTION_GOLD_TABLE[baseCard.Tier]
    
    IF shardCount < requiredShards:
        RETURN Error("Insufficient Evolution Shards")
    
    IF NOT UserHasEnoughCurrency(userId, Gold=requiredGold):
        RETURN Error("Insufficient Gold")
    
    // Commit transaction
    DeductCurrency(userId, Gold=requiredGold)
    DeductEvolutionShards(userId, requiredShards)
    ConsumeCards(userId, fodderInstanceIds)
    
    // Evolve base card
    baseCard.Tier += 1
    baseCard.RarityName = MapTierToRarity(baseCard.Tier)
    baseCard.BaseOverall += EVOLUTION_OVR_BONUS[baseCard.Tier]
    baseCard.EnhancementLevel = 0  // Reset
    baseCard.SkillStars = MIN(baseCard.SkillStars + 1, 5)  // +1 skill star
    
    SaveCardInstance(baseCard)
    LogEvent("CARD_EVOLVED", userId, baseCardInstanceId, baseCard.Tier)
    
    RETURN { "Result": "Success", "NewTier": baseCard.Tier }
END FUNCTION
```

---

## 3. MATCH ENGINE STATE MACHINE

### 3.1 Match States

**C++ Enum:**

```cpp
UENUM(BlueprintType)
enum class EMatchState : uint8
{
    PreMatch = 0,
    KickOffFirstHalf = 1,
    InPlayFirstHalf = 2,
    HalfTime = 3,
    KickOffSecondHalf = 4,
    InPlaySecondHalf = 5,
    ExtraTimeFirstHalf = 6,
    ExtraTimeBreak = 7,
    ExtraTimeSecondHalf = 8,
    Penalties = 9,
    FullTime = 10
};
```

### 3.2 Match Clock

**Tick-based progression:**

```cpp
UCLASS()
class AFootballGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category="Match")
    EMatchState CurrentMatchState;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    float MatchClockMinutes;  // 0 to 90+ (or 120 with extra time)
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    int32 HomeTeamScore;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    int32 AwayTeamScore;
    
    UPROPERTY(BlueprintReadOnly, Category="Match")
    float TimeScaleMultiplier;  // Usually 1.0 (real-time); can be faster for replay
    
    virtual void Tick(float DeltaSeconds) override;
    
private:
    void UpdateMatchState(float DeltaSeconds);
    void TransitionState(EMatchState NewState);
};
```

### 3.3 State Transition Flow

**Simplified Tick logic:**

```cpp
void AFootballGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    MatchClockMinutes += (DeltaSeconds / 60.0f) * TimeScaleMultiplier;
    
    switch (CurrentMatchState)
    {
        case EMatchState::PreMatch:
            if (AllPlayersConnected() && SquadsLoaded())
            {
                TransitionState(EMatchState::KickOffFirstHalf);
            }
            break;
            
        case EMatchState::KickOffFirstHalf:
            SetupKickOff(ETeamId::Home);
            TransitionState(EMatchState::InPlayFirstHalf);
            break;
            
        case EMatchState::InPlayFirstHalf:
            SimulatePhysicsFrame(DeltaSeconds);
            ProcessPlayerInputs(DeltaSeconds);
            UpdateAIControllers(DeltaSeconds);
            CheckCollisionsAndEvents();
            
            if (MatchClockMinutes >= 45.0f)
            {
                Whistle();
                TransitionState(EMatchState::HalfTime);
            }
            break;
            
        case EMatchState::HalfTime:
            // Player rest, show stats
            if (HalfTimeTimerExpired())
            {
                TransitionState(EMatchState::KickOffSecondHalf);
            }
            break;
            
        case EMatchState::InPlaySecondHalf:
            SimulatePhysicsFrame(DeltaSeconds);
            ProcessPlayerInputs(DeltaSeconds);
            UpdateAIControllers(DeltaSeconds);
            CheckCollisionsAndEvents();
            
            // Fatigue kicks in after 60 real minutes played
            if (MatchClockMinutes > 60.0f)
            {
                ApplyPlayerFatigue();
            }
            
            if (MatchClockMinutes >= 90.0f)
            {
                if (IsKnockoutCompetition() && HomeTeamScore == AwayTeamScore)
                {
                    TransitionState(EMatchState::ExtraTimeFirstHalf);
                }
                else
                {
                    TransitionState(EMatchState::FullTime);
                }
            }
            break;
            
        case EMatchState::ExtraTimeFirstHalf:
        case EMatchState::ExtraTimeSecondHalf:
            // 15 min each, similar to regular play
            if (MatchClockMinutes >= 120.0f)
            {
                if (HomeTeamScore != AwayTeamScore)
                {
                    TransitionState(EMatchState::FullTime);
                }
                else
                {
                    TransitionState(EMatchState::Penalties);
                }
            }
            break;
            
        case EMatchState::Penalties:
            RunPenaltyShootout();
            TransitionState(EMatchState::FullTime);
            break;
            
        case EMatchState::FullTime:
            FinalizeMatch();
            SaveMatchResult();
            break;
            
        default:
            break;
    }
}
```

### 3.4 Ball Physics

**AStrikerBall class:**

```cpp
UCLASS()
class AStrikerBall : public APawn
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category="Ball")
    UPrimitiveComponent* BallMesh;
    
    UPROPERTY(BlueprintReadWrite, Category="Ball")
    float BallMass;
    
    UPROPERTY(BlueprintReadWrite, Category="Ball")
    float DragCoefficient;
    
    UPROPERTY(BlueprintReadWrite, Category="Ball")
    float SpinFactor;
    
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBallContact, AStrikerPlayerCharacter*, EContactType);
    FOnBallContact BallContactDelegate;
    
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGoalScored, ETeamId, AStrikerPlayerCharacter*);
    FOnGoalScored GoalScoredDelegate;
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    UFUNCTION()
    void OnBallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
                   UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
                   const FHitResult& Hit);
};
```

### 3.5 Possession Tracking

```cpp
UCLASS()
class UPossessionTracker : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    ETeamId CurrentPossessionTeam;
    
    UPROPERTY()
    AStrikerPlayerCharacter* CurrentBallCarrier;
    
    UPROPERTY()
    float PossessionTimeHome;  // seconds
    
    UPROPERTY()
    float PossessionTimeAway;  // seconds
    
    void UpdatePossession(AStrikerPlayerCharacter* NewCarrier);
    void ResetPossession();
};
```

---

## 4. INPUT & SKILL SYSTEM (FIFA MOBILE-INSPIRED)

### 4.1 Skill Definitions

**Base skill structure:**

```json
{
  "SkillId": "string",
  "DisplayName": "string",
  "MinSkillStars": "int",
  "RequiredInputPattern": "string",
  "CooldownSeconds": "float",
  "ExecutionDurationSeconds": "float",
  "StaminaCost": "int",
  "SuccessRateModifier": "float",
  "AnimationRef": "string",
  "Description": "string"
}
```

**Examples:**

```json
[
  {
    "SkillId": "RainbowFlick",
    "DisplayName": "Rainbow Flick",
    "MinSkillStars": 5,
    "RequiredInputPattern": "JoystickBackwardThenForwardWithin0.4s",
    "CooldownSeconds": 2.0,
    "ExecutionDurationSeconds": 0.7,
    "StaminaCost": 5,
    "SuccessRateModifier": 0.9
  },
  {
    "SkillId": "StepOver",
    "DisplayName": "Step Over",
    "MinSkillStars": 3,
    "RequiredInputPattern": "CircleGestureOnJoystick",
    "CooldownSeconds": 1.5,
    "ExecutionDurationSeconds": 0.5,
    "StaminaCost": 2,
    "SuccessRateModifier": 0.95
  },
  {
    "SkillId": "Elastico",
    "DisplayName": "Elastico",
    "MinSkillStars": 5,
    "RequiredInputPattern": "FlickRightThenLeftOnJoystickWithin0.3s",
    "CooldownSeconds": 2.5,
    "ExecutionDurationSeconds": 0.8,
    "StaminaCost": 6,
    "SuccessRateModifier": 0.85
  }
]
```

### 4.2 Gesture Input Recognizer

```cpp
UCLASS()
class UGestureRecognizer : public UObject
{
    GENERATED_BODY()

public:
    struct FInputSample
    {
        FVector2D Delta;      // dx, dy
        float Timestamp;
    };
    
    UPROPERTY()
    TArray<FInputSample> InputBuffer;
    
    UPROPERTY(EditAnywhere)
    float InputBufferDurationSeconds;
    
    void RegisterInputSample(FVector2D JoystickDelta, float DeltaTime);
    
    bool TryDetectPattern(const FString& PatternId, FString& OutDetectedPattern);
    
    // Pattern detection methods
    bool IsCircleGesture();
    bool IsBackwardThenForwardFlick();
    bool IsRightThenLeftFlick();
    
private:
    void CleanOldSamples();
    float CalculateTotalDistance();
    FVector2D CalculateNetDirection();
};
```

### 4.3 Skill Execution Pipeline

```cpp
UCLASS()
class USkillSystem : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    class UGestureRecognizer* GestureRecognizer;
    
    UPROPERTY()
    TMap<AStrikerPlayerCharacter*, float> SkillCooldowns;
    
    UFUNCTION(BlueprintCallable)
    bool TryExecuteSkill(AStrikerPlayerCharacter* Player, const FString& SkillId);
    
    UFUNCTION(BlueprintCallable)
    bool CanExecuteSkill(AStrikerPlayerCharacter* Player, const FString& SkillId);
    
private:
    void StartSkillAnimation(AStrikerPlayerCharacter* Player, const FString& SkillId);
    void ApplySkillMovement(AStrikerPlayerCharacter* Player, const FString& SkillId);
    void ApplySkillEffect(AStrikerPlayerCharacter* Player, const FString& SkillId);
    void SetSkillCooldown(AStrikerPlayerCharacter* Player, const FString& SkillId, float Duration);
};
```

**Execution pseudo-code:**

```
FUNCTION TryExecuteSkill(player, skillId):
    skillDef = GetSkillDefinition(skillId)
    
    IF player.SkillStars < skillDef.MinSkillStars:
        RETURN false
    
    IF player.Stamina < skillDef.StaminaCost:
        RETURN false
    
    IF IsOnCooldown(player, skillId):
        RETURN false
    
    // Play animation
    StartSkillAnimation(player, skillId)
    
    // Deduct stamina
    player.Stamina -= skillDef.StaminaCost
    
    // Set cooldown
    SetSkillCooldown(player, skillId, skillDef.CooldownSeconds)
    
    // Apply movement/effect
    ApplySkillMovement(player, skillId)
    
    RETURN true
END FUNCTION
```

---

## 5. NETWORKING & AUTHORITATIVE MODEL

### 5.1 Client Input Packet

**Sent every 33ms (30 Hz tick):**

```json
{
  "UserId": "string",
  "MatchId": "string",
  "FrameNumber": "int",
  "TimestampClient": "int64",
  "Input": {
    "MoveX": "float",
    "MoveY": "float",
    "ButtonPass": "bool",
    "ButtonShoot": "bool",
    "ButtonThrough": "bool",
    "ButtonCross": "bool",
    "ButtonSkill": "bool",
    "SprintModifier": "bool",
    "GestureData": {
      "Samples": [
        {
          "DeltaX": "float",
          "DeltaY": "float",
          "TimestampDelta": "float"
        }
      ]
    }
  }
}
```

**C++ Structure:**

```cpp
USTRUCT(BlueprintType)
struct FClientInputFrame
{
    GENERATED_BODY()
    
    UPROPERTY()
    int32 FrameNumber;
    
    UPROPERTY()
    float MoveAxisX;
    
    UPROPERTY()
    float MoveAxisY;
    
    UPROPERTY()
    bool bButtonPass;
    
    UPROPERTY()
    bool bButtonShoot;
    
    UPROPERTY()
    bool bButtonThrough;
    
    UPROPERTY()
    bool bButtonCross;
    
    UPROPERTY()
    bool bButtonSkill;
    
    UPROPERTY()
    bool bSprintModifier;
    
    UPROPERTY()
    TArray<FVector2D> GestureSamples;
};
```

### 5.2 Server-Side Input Processing

```cpp
void AFootballGameMode::OnReceiveClientInput(const FString& UserId, const FClientInputFrame& InputFrame)
{
    AStrikerPlayerCharacter* PlayerPawn = GetPlayerPawnForUser(UserId);
    if (!PlayerPawn) return;
    
    // Movement input
    PlayerPawn->AddMovementInput(
        FVector(InputFrame.MoveAxisX, InputFrame.MoveAxisY, 0.0f),
        1.0f
    );
    
    // Sprint
    if (InputFrame.bSprintModifier)
    {
        PlayerPawn->StartSprinting();
    }
    else
    {
        PlayerPawn->StopSprinting();
    }
    
    // Button presses (sent to action system)
    if (InputFrame.bButtonPass)
    {
        PlayerPawn->RequestPass();
    }
    
    if (InputFrame.bButtonShoot)
    {
        PlayerPawn->RequestShot();
    }
    
    if (InputFrame.bButtonSkill)
    {
        GestureRecognizer->RegisterInputSamples(InputFrame.GestureSamples);
        if (GestureRecognizer->TryDetectSkill(PlayerPawn, OutSkillId))
        {
            SkillSystem->TryExecuteSkill(PlayerPawn, OutSkillId);
        }
    }
}
```

### 5.3 Server-to-Client State Replication

**Every 50ms (20 Hz):**

```json
{
  "FrameNumber": "int",
  "MatchState": "string",
  "BallPosition": { "X": "float", "Y": "float", "Z": "float" },
  "BallVelocity": { "X": "float", "Y": "float", "Z": "float" },
  "PlayerStates": [
    {
      "PlayerId": "int",
      "Position": { "X": "float", "Y": "float", "Z": "float" },
      "Velocity": { "X": "float", "Y": "float", "Z": "float" },
      "AnimationState": "string",
      "HasPossession": "bool",
      "Stamina": "int"
    }
  ],
  "Score": { "Home": "int", "Away": "int" },
  "MatchClock": "float"
}
```

---

## 6. ECONOMY & PACK OPENING

### 6.1 Pack Templates

**Example Gold Pack:**

```json
{
  "PackId": "GOLD_PACK",
  "PackName": "Gold Pack",
  "PriceGoldCoins": 20000,
  "PriceGems": 50,
  "CardsPerPack": 5,
  "Guarantees": [
    {
      "MinTier": 3,
      "MinCount": 1
    }
  ],
  "DropRates": [
    { "Tier": 1, "Probability": 0.60 },
    { "Tier": 2, "Probability": 0.25 },
    { "Tier": 3, "Probability": 0.10 },
    { "Tier": 4, "Probability": 0.035 },
    { "Tier": 5, "Probability": 0.012 },
    { "Tier": 6, "Probability": 0.0025 },
    { "Tier": 7, "Probability": 0.0005 }
  ]
}
```

**All Pack Types:**

| Pack | Cost | Cards | Guarantee | Notes |
|------|------|-------|-----------|-------|
| Starter | Free | 5 | 1× Silver | One-time only |
| Basic | 5K GC | 5 | 3× Bronze+ | Farmable |
| Gold | 20K GC / 50 💎 | 5 | 1× Gold+ | Most popular |
| Premium Gold | 100 💎 | 5 | 2× Gold | Higher chance Elite |
| Elite | 300 💎 | 5 | 1× Elite+ | – |
| Legend | 700 💎 | 5 | 1× Legend+ | – |
| Icon | 1,500 💎 | 3 | 1× Icon+ | Limited |
| Transcendence | 3,000 💎 | 1 | 1× Transcendence | Rarest |

### 6.2 Pity System

**Server-side tracking:**

```cpp
USTRUCT(BlueprintType)
struct FPityCounter
{
    GENERATED_BODY()
    
    UPROPERTY()
    int32 GoldOrBetterOpened;  // Gold+
    
    UPROPERTY()
    int32 EliteOrBetterOpened; // Elite+
    
    UPROPERTY()
    int32 LegendOrBetterOpened; // Legend+
    
    UPROPERTY()
    int32 IconOrBetterOpened;  // Icon+
    
    UPROPERTY()
    int32 TranscendenceOpened; // Transcendence
};
```

**Pity Rules (from GDD):**

```
Every 10 packs from Gold Pack+:        1 guaranteed Gold+ (Tier 3+)
Every 20 packs from Elite Pack+:       1 guaranteed Elite+ (Tier 4+)
Every 50 packs from any Tier 3+ pack:  1 guaranteed Legend+ (Tier 5+)
Every 200 packs from Icon Pack:        1 guaranteed Icon+ (Tier 6+)

Pity counter resets after each guaranteed pull.
Carries over across sessions.
```

### 6.3 Pack Opening Algorithm

```cpp
void UEconomyManager::OpenPack(FString UserId, FString PackId)
{
    FPackDefinition PackDef = GetPackDefinition(PackId);
    
    // Deduct cost
    if (!DeductCurrency(UserId, PackDef.PriceGoldCoins, PackDef.PriceGems))
    {
        return; // Insufficient funds
    }
    
    FPityCounter Pity = LoadPityCounter(UserId);
    TArray<FCardInstance> CardsOpened;
    
    for (int32 i = 0; i < PackDef.CardsPerPack; ++i)
    {
        int32 CardTier = RollTierFromDropRates(PackDef.DropRates);
        
        // Apply pity logic
        if (Pity.GoldOrBetterOpened >= 9 && CardTier < 3)
        {
            CardTier = 3;
            Pity.GoldOrBetterOpened = 0;
        }
        
        if (Pity.EliteOrBetterOpened >= 19 && CardTier < 4)
        {
            CardTier = 4;
            Pity.EliteOrBetterOpened = 0;
        }
        
        if (Pity.LegendOrBetterOpened >= 49 && CardTier < 5)
        {
            CardTier = 5;
            Pity.LegendOrBetterOpened = 0;
        }
        
        if (Pity.IconOrBetterOpened >= 199 && CardTier < 6)
        {
            CardTier = 6;
            Pity.IconOrBetterOpened = 0;
        }
        
        // Create card instance
        FCardDefinition* CardDef = GetRandomCardByTier(CardTier);
        FCardInstance NewCard = CreateCardInstance(UserId, *CardDef);
        CardsOpened.Add(NewCard);
        
        // Update pity counters
        if (CardTier >= 3) Pity.GoldOrBetterOpened++;
        if (CardTier >= 4) Pity.EliteOrBetterOpened++;
        if (CardTier >= 5) Pity.LegendOrBetterOpened++;
        if (CardTier >= 6) Pity.IconOrBetterOpened++;
    }
    
    // Save results
    SavePityCounter(UserId, Pity);
    AddCardsToInventory(UserId, CardsOpened);
    LogPullEvent(UserId, PackId, CardsOpened);
}
```

---

## 7. CHEMISTRY & SQUAD SYSTEM

### 7.1 Chemistry Calculation

**Algorithm:**

```cpp
float FChemistryCalculator::CalculateChemistry(
    const FUserSquad& Squad,
    const TMap<FString, FCardInstance>& CardMap
)
{
    float TotalChemistry = 0.0f;
    int32 PairCount = 0;
    
    for (int32 i = 0; i < Squad.StartingXI.Num(); ++i)
    {
        for (int32 j = i + 1; j < Squad.StartingXI.Num(); ++j)
        {
            FCardInstance* Card1 = CardMap.Find(Squad.StartingXI[i]);
            FCardInstance* Card2 = CardMap.Find(Squad.StartingXI[j]);
            
            if (Card1 && Card2)
            {
                float PairChemistry = CalculatePairChemistry(*Card1, *Card2);
                TotalChemistry += PairChemistry;
                PairCount++;
            }
        }
    }
    
    return (PairCount > 0) ? (TotalChemistry / PairCount) : 0.0f;
}

float FChemistryCalculator::CalculatePairChemistry(
    const FCardInstance& Card1,
    const FCardInstance& Card2
)
{
    if (Card1.ClubId == Card2.ClubId && Card1.NationId == Card2.NationId)
    {
        return 10.0f; // Perfect Chemistry
    }
    if (Card1.ClubId == Card2.ClubId)
    {
        return 6.0f; // Club Chemistry
    }
    if (Card1.LeagueId == Card2.LeagueId)
    {
        return 4.0f; // League Chemistry
    }
    if (Card1.NationId == Card2.NationId)
    {
        return 3.0f; // Nation Chemistry
    }
    
    return 0.0f; // No Chemistry
}
```

### 7.2 Squad Formation Validation

```cpp
bool FSquadValidator::ValidateSquad(const FUserSquad& Squad)
{
    // Check starting XI count
    if (Squad.StartingXI.Num() != 11)
    {
        return false;
    }
    
    // Check formation compatibility
    if (!IsValidFormation(Squad.Formation))
    {
        return false;
    }
    
    // Check positions match formation
    TMap<FString, int32> FormationPositions = GetFormationPositions(Squad.Formation);
    TMap<FString, int32> SquadPositions;
    
    for (const FString& CardId : Squad.StartingXI)
    {
        FString Position = GetCardPosition(CardId);
        SquadPositions[Position]++;
    }
    
    // Validate each position count
    for (const auto& Pair : FormationPositions)
    {
        if (SquadPositions[Pair.Key] != Pair.Value)
        {
            return false;
        }
    }
    
    return true;
}
```

---

## 8. DATABASE SCHEMA & QUERIES

### 8.1 PostgreSQL Schema (Example)

```sql
-- Users Table
CREATE TABLE users (
    user_id UUID PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP,
    account_level INT DEFAULT 1,
    total_gems INT DEFAULT 0,
    total_gold_coins INT DEFAULT 0,
    total_ep INT DEFAULT 0
);

-- CardInstances Table
CREATE TABLE card_instances (
    instance_id UUID PRIMARY KEY,
    user_id UUID NOT NULL REFERENCES users(user_id),
    card_id VARCHAR(100) NOT NULL,
    player_id VARCHAR(100) NOT NULL,
    tier INT NOT NULL,
    enhancement_level INT DEFAULT 0,
    skill_stars INT DEFAULT 1,
    weak_foot INT DEFAULT 1,
    is_tradable BOOLEAN DEFAULT true,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_user_id (user_id),
    INDEX idx_player_id (player_id)
);

-- UserSquads Table
CREATE TABLE user_squads (
    squad_id UUID PRIMARY KEY,
    user_id UUID NOT NULL REFERENCES users(user_id),
    squad_name VARCHAR(100) NOT NULL,
    formation VARCHAR(20) NOT NULL,
    starting_xi UUID[] NOT NULL,
    substitutes UUID[] NOT NULL,
    chemistry_score INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_user_id (user_id)
);

-- MatchResults Table
CREATE TABLE match_results (
    match_id UUID PRIMARY KEY,
    user_id_home UUID NOT NULL REFERENCES users(user_id),
    user_id_away UUID NOT NULL REFERENCES users(user_id),
    squad_id_home UUID NOT NULL REFERENCES user_squads(squad_id),
    squad_id_away UUID NOT NULL REFERENCES user_squads(squad_id),
    score_home INT NOT NULL,
    score_away INT NOT NULL,
    match_duration_seconds INT NOT NULL,
    match_type VARCHAR(50) NOT NULL,
    completed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_user_id_home (user_id_home),
    INDEX idx_user_id_away (user_id_away)
);

-- EnhancementLogs Table
CREATE TABLE enhancement_logs (
    log_id BIGSERIAL PRIMARY KEY,
    user_id UUID NOT NULL REFERENCES users(user_id),
    card_instance_id UUID NOT NULL REFERENCES card_instances(instance_id),
    previous_level INT NOT NULL,
    new_level INT NOT NULL,
    success BOOLEAN NOT NULL,
    ep_spent INT NOT NULL,
    gold_spent INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_user_id (user_id),
    INDEX idx_card_instance_id (card_instance_id)
);

-- PityCounters Table
CREATE TABLE pity_counters (
    user_id UUID PRIMARY KEY REFERENCES users(user_id),
    gold_or_better_opened INT DEFAULT 0,
    elite_or_better_opened INT DEFAULT 0,
    legend_or_better_opened INT DEFAULT 0,
    icon_or_better_opened INT DEFAULT 0,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### 8.2 Key Queries

**Get user's cards with enhancement level:**

```sql
SELECT 
    ci.instance_id,
    ci.card_id,
    ci.enhancement_level,
    ci.tier,
    ci.skill_stars
FROM card_instances ci
WHERE ci.user_id = $1
ORDER BY ci.tier DESC, ci.card_id ASC;
```

**Get squad with all card details:**

```sql
SELECT 
    us.squad_id,
    us.squad_name,
    us.formation,
    us.chemistry_score,
    ci.instance_id,
    ci.player_id,
    ci.tier,
    ci.enhancement_level
FROM user_squads us
LEFT JOIN UNNEST(us.starting_xi) AS card_id ON true
LEFT JOIN card_instances ci ON ci.instance_id = card_id
WHERE us.squad_id = $1;
```

**Get recent enhancement attempts:**

```sql
SELECT 
    el.card_instance_id,
    el.previous_level,
    el.new_level,
    el.success,
    el.created_at
FROM enhancement_logs el
WHERE el.user_id = $1
ORDER BY el.created_at DESC
LIMIT 20;
```

---

## 9. AI DEVELOPMENT PROMPTS FOR CODEX

### 9.1 Prompt Template: Card Enhancement System

```
You are a UE4 C++ game developer. Implement the Card Enhancement System 
for Soccer Mobile Pro based on this specification:

Requirements:
1. Create a C++ class `UEnhancementComponent` that handles card upgrades
2. Follow FIFA Online 3 enhancement rules with +0 to +10 levels
3. Implement success rate logic (25% at +10, 100% at +1)
4. Track enhancement history server-side
5. Support Breakthrough Cards for guaranteed success
6. Support Downgrade Protection (card doesn't drop level on failure)
7. Unlock trait/skill slots at +5 and +7 respectively
8. Mark card eligible for evolution at +10

Class Interface:
- TryEnhanceCard(CardInstanceId, UseBreakthrough, UseDowngradeProtection) -> Result
- ApplyStatBoost(CardInstance, BoostAmount) -> void
- GetEnhancementCost(Level) -> EPCost, GoldCost
- IsEligibleForEvolution(CardInstance) -> bool

Use the attached Enhancement Levels Table as config (EP costs 100..20000, Gold 5K..1M, Success 25..100%)

Data structures to use:
- FCardInstance (stores enhancement level, unlocked traits, etc.)
- FEnhancementParameters (EP cost, Gold cost, success rate per level)

Output: Complete C++ header (.h) and implementation (.cpp) with proper error handling, 
logging, and server-authoritative validation.
```

### 9.2 Prompt Template: Match Engine State Machine

```
Implement the Match Engine State Machine for a football game in UE4 C++.

Requirements:
1. Create AFootballGameMode class with 11 match states (PreMatch, InPlay, HalfTime, FullTime, etc.)
2. Implement state transitions based on match clock (0..90+ minutes)
3. Handle ball physics via AStrikerBall (uses Chaos Physics)
4. Process client inputs server-side (movement, pass, shoot, skill)
5. Replicate match state to clients every 50ms (20 Hz)
6. Apply player fatigue after 60 real-time minutes
7. Support extra time for cup competitions
8. Support penalty shootout resolution

State Flow:
PreMatch -> KickOff1H -> InPlay1H -> HalfTime -> KickOff2H -> InPlay2H -> [ExtraTime/Penalties] -> FullTime

Key Functions:
- Tick(DeltaSeconds) -> state machine loop
- ProcessPlayerInput(UserId, InputFrame) -> handle movement, buttons, skills
- CheckGoalScored() -> detect ball in goal volume
- UpdateMatchClock() -> increment in-game minutes
- BroadcastMatchState() -> send state to all clients

Use provided Match Clock calculation and State Enum.

Output: AFootballGameMode.h/cpp with complete state handling, physics integration, 
and authoritative simulation.
```

### 9.3 Prompt Template: Skill Gesture Recognition

```
Implement the Skill Gesture Recognition System for mobile football game.

Requirements:
1. Create UGestureRecognizer class to detect finger swipe patterns
2. Support gesture patterns: CircleGesture, BackwardThenForward, RightThenLeft, DirectionalFlick
3. Register input samples (dx, dy, dt) from joystick each frame
4. Detect patterns with 0.3-0.4 second time window
5. Return detected skill ID if match found
6. Only allow skills if player has sufficient skill stars (1..5)
7. Deduct stamina on successful skill execution
8. Apply skill cooldowns per player

Gesture Patterns:
- CircleGesture: 360° rotation on joystick (Step Over)
- BackwardThenForward: Backward flick, pause, forward flick (Rainbow Flick)
- RightThenLeft: Right flick, pause, left flick (Elastico)
- Hold+Swipe: Hold shoot button + swipe (Rabona)

Input Data Structure:
FInputSample { FVector2D Delta, float Timestamp }

Output: UGestureRecognizer.h/cpp with pattern detection algorithms, 
FSkillDefinition struct, and TryExecuteSkill() function.
```

### 9.4 Prompt Template: Pack Opening with Pity System

```
Implement the Pack Opening and Pity Counter system for a gacha card game.

Requirements:
1. Create UPackOpeningSystem class
2. Support 10 pack types (Starter, Basic, Gold, Premium Gold, Elite, Legend, Icon, Transcendence, etc.)
3. Implement pity system: every 10 packs guarantee Gold+, every 20 guarantee Elite+, etc.
4. Track 5 separate pity counters per user
5. Deduct currency (Gold Coins or Gems) from user account
6. Generate 3-5 random cards from pack definition
7. Apply drop rate probabilities (Tier 1..7)
8. Enforce pity rules if counter threshold reached
9. Persist pity counters across sessions
10. Log all pack openings for analytics

Pack Definition JSON:
{
  "PackId": string,
  "Cost": { "GoldCoins": int, "Gems": int },
  "CardsPerPack": int,
  "Guarantees": [ { "MinTier": int, "Count": int } ],
  "DropRates": [ { "Tier": int, "Probability": float } ]
}

Pity Rules:
- Every 10 Gold+ packs: 1 guaranteed Tier 3+
- Every 20 Elite+ packs: 1 guaranteed Tier 4+
- Every 50 packs: 1 guaranteed Tier 5+
- Every 200 Icon packs: 1 guaranteed Tier 6+

Output: UPackOpeningSystem.h/cpp with FPackDefinition, FPityCounter, 
OpenPack() function, and server-side authoritative logic.
```

### 9.5 Prompt Template: Card Upgrade UI

```
Implement the Card Upgrade UI using UE4 UMG (Unreal Motion Graphics).

Requirements:
1. Create BP_UICardUpgrade widget (Blueprintable)
2. Display current card with 3D model
3. Show enhancement level progress bar (+0 to +10)
4. Display costs: EP and Gold coins required for next level
5. Show success probability percentage (25%..100%)
6. Add buttons: "Upgrade", "Use Breakthrough Protection", "Cancel"
7. Implement item inventory check for Breakthrough Cards
8. On upgrade success: animate card glow, increase level, play SFX
9. On upgrade failure: shake card, play failure SFX
10. Show tooltip with stat improvements

UI Layout:
- Top: Card 3D Model Viewer (rotatable)
- Middle: Enhancement Level Progress (current +X / +10)
- Bottom: Cost Display + Button Section
- Right: Success Rate % + Item Inventory

Animation:
- Success: 0.5s glow effect, confetti particles
- Failure: 0.3s shake effect, red flash

Output: BP_UICardUpgrade.uasset and C++ companion class UCardUpgradeWidget.h/cpp 
for backend logic integration.
```

---

## 10. TRANSFER MARKET SYSTEM (AUCTION HOUSE)

### 10.1 Data Models

```json
{
  "ListingId": "string",
  "SellerUserId": "string",
  "CardInstanceId": "string",
  "StartPrice": "int",
  "BuyNowPrice": "int",
  "CurrentBid": "int",
  "CurrentBidUserId": "string",
  "Currency": "string",       // "GoldCoins"
  "Status": "string",         // "Active","Sold","Expired","Cancelled"
  "CreatedAt": "string",
  "ExpiresAt": "string",
  "MinIncrement": "int"
}
```

### 10.2 Constraints

```
- Only cards with IsTradable = true can be listed.
- Limit the number of active listings per user (e.g., 50).
- Marketplace includes price floors/ceilings based on Tier (e.g., Tier 3: 5,000–200,000 Gold).
```

### 10.3 Server API (Pseudo)

```
POST /market/createListing(userId, cardInstanceId, startPrice, buyNowPrice, durationHours)
GET  /market/search(filters)  // Tier, Position, League, PriceRange...
POST /market/placeBid(userId, listingId, bidAmount)
POST /market/buyNow(userId, listingId)
POST /market/cancelListing(userId, listingId)
```

### 10.4 Auction Logic

```
FUNCTION PlaceBid(userId, listingId, bidAmount):
    listing = LoadListing(listingId)
    ASSERT listing.Status == "Active"
    ASSERT NOW < listing.ExpiresAt

    minBid = MAX(listing.StartPrice,
                 listing.CurrentBid + listing.MinIncrement)

    ASSERT bidAmount >= minBid
    ASSERT UserHasEnoughGold(userId, bidAmount)

    // Refund previous bidder
    IF listing.CurrentBidUserId != null:
        RefundGold(listing.CurrentBidUserId, listing.CurrentBid)

    DeductGold(userId, bidAmount)
    listing.CurrentBid = bidAmount
    listing.CurrentBidUserId = userId
    SaveListing(listing)
```

### 10.5 Expiry & Settlement

```
- Scheduled job runs every X minutes:

  - For listings with NOW >= ExpiresAt and Status = Active:
    - If CurrentBidUserId exists:
      - Set Status = Sold
      - Transfer card to buyer
      - Pay seller (minus marketplace fee, e.g. 5–10%)
    - Else:
      - Set Status = Expired
      - Return card to seller inventory.
```

---

## 11. CLUB SYSTEM (GUILDS)

### 11.1 Club Schema

```json
{
  "ClubId": "string",
  "Name": "string",
  "Tag": "string",
  "OwnerUserId": "string",
  "Members": [
    {
      "UserId": "string",
      "Role": "string",        
      "JoinDate": "string",
      "ContributionPoints": "int"
    }
  ],
  "MaxMembers": "int",         
  "Region": "string",
  "Description": "string",
  "TrophyPoints": "int",
  "CreatedAt": "string"
}
```

### 11.2 Core Features

```
- Create / search / join / leave Clubs.
- Weekly Club Objectives that give Club Tokens.
- Club League with seasonal ranking and rewards.
- Club Shop that uses Club Tokens for packs, cosmetics, boosts.
```

### 11.3 Weekly Club Objectives

```json
{
  "ObjectiveId": "string",
  "ClubId": "string",
  "Description": "string",
  "TargetValue": "int",
  "CurrentValue": "int",
  "RewardClubTokens": "int",
  "StartAt": "string",
  "EndAt": "string"
}
```

*** Rules: ***
- Each member match (PvP or PvE) contributes to CurrentValue (e.g. goals, wins).
- When CurrentValue >= TargetValue, all members receive rewards once.

### 11.4 Club League Ranking

```json
{
  "SeasonId": "string",
  "ClubId": "string",
  "Points": "int",
  "Wins": "int",
  "Losses": "int",
  "Rank": "int"
}
```
- Points are accumulated from ranked wins by club members.
- At season end, rewards are given based on Rank (Club Tokens, banners, frames).

---

## 12. ESPORTS & COMPETITIVE SYSTEM

### 12.1 Competitive Tiers

```json
{
  "TierId": "string",
  "MinRating": "int",
  "MaxRating": "int",
  "SeasonRewards": {
    "GoldCoins": "int",
    "Gems": "int",
    "Packs": [ "string" ],
    "ExclusiveItems": [ "string" ]
  }
}
```
- Rating uses ELO-like system updated after each ranked PvP match.

### 12.2 Esports Tournament Model

```json
{
  "TournamentId": "string",
  "Name": "string",
  "Type": "string",           
  "Format": "string",         
  "Stage": "string",          
  "MaxPlayers": "int",
  "CheckInWindowMinutes": "int",
  "StartAt": "string",
  "Status": "string",         
  "Region": "string"
}
```

### 12.3 Tournament Matchmaking

```
FUNCTION EnqueueEsportsPlayer(userId, tournamentId):
    ASSERT IsRegistered(userId, tournamentId)
    AddToQueue(tournamentId, userId)

    WHILE QueueHasPair(tournamentId):
        (u1, u2) = PopBestMatchPair(tournamentId)
        matchId = CreateTournamentMatch(tournamentId, u1, u2)
        NotifyPlayers(u1, u2, matchId)
```
- `PopBestMatchPair` selects closest rating + lowest latency pair.

### 12.4 Spectator & Broadcast

```json
{
  "MatchId": "string",
  "SpectatorToken": "string",
  "StreamEndpoints": {
    "Primary": "wss://primary-endpoint",
    "Backup": "wss://backup-endpoint"
  }
}
```
- Spectator clients:
  - Receive read-only match state snapshots.
  - Cannot send any gameplay input.

---

## REFERENCE TABLES

### Currency Conversion Chart

| Activity | GoldCoins | Gems | EP | Notes |
|----------|-----------|------|-----|-------|
| Win Quick Match | 500 | — | 50 | vs AI |
| Win League Match | 1,000 | — | 100 | Ranked |
| Win Weekend League | 2,000 | 10 | 200 | per match |
| Daily Login (Day 7) | — | 50 | — | Streak reward |
| Quick Sell Card | Varies | — | Varies | Per rarity |
| Squad Battles Win | 800 | 5 | 80 | vs CPU squad |

### Attribute Range by Tier

| Tier | Name | OVR Range | Drop % |
|------|------|-----------|--------|
| 1 | Bronze | 40–59 | 60.0% |
| 2 | Silver | 60–74 | 25.0% |
| 3 | Gold | 75–84 | 10.0% |
| 4 | Elite | 85–89 | 3.5% |
| 5 | Legend | 90–94 | 1.2% |
| 6 | Icon | 95–99 | 0.25% |
| 7 | Transcendence | 100 | 0.05% |

### Skill Star Requirements

| Stars | Dribbling Skills | Shooting Skills | Passing Skills |
|-------|------------------|-----------------|----------------|
| 1★ | Basic controls | Low power shot | Ground pass |
| 2★ | Fake shot | Standard shot | Lob pass |
| 3★ | Step Over, Ball Roll | Chip shot | Through Ball |
| 4★ | Roulette, La Croqueta | Power shot | No-Look Pass |
| 5★ | Rainbow Flick, Elastico | Bicycle Kick | Threaded Needle |

---

## APPENDIX: EXAMPLE JSON RESPONSES

### Card Instance (Full)

```json
{
  "CardId": "PL_MESSI_2024_TOTY",
  "PlayerId": "PL_MESSI",
  "DisplayName": "Lionel Messi TOTY",
  "ClubId": "CLB_INTER_MIAMI",
  "LeagueId": "LG_MLS",
  "NationId": "NAT_ARGENTINA",
  "Position": "LW",
  "Tier": 5,
  "RarityName": "Legend",
  "BaseOverall": 93,
  "EnhancementLevel": 7,
  "SkillStars": 5,
  "WeakFoot": 4,
  "IsTradable": false,
  "CardType": "TOTY",
  "Attributes": {
    "Pace": {
      "SprintSpeed": 86,
      "Acceleration": 92
    },
    "Shooting": {
      "Finishing": 95,
      "LongShots": 91,
      "ShotPower": 85,
      "Volleys": 87,
      "Penalties": 92
    },
    "Passing": {
      "ShortPassing": 91,
      "LongPassing": 87,
      "Vision": 95,
      "Crossing": 84,
      "FKAccuracy": 93
    },
    "Dribbling": {
      "BallControl": 96,
      "Agility": 94,
      "Balance": 95,
      "Reactions": 93,
      "Composure": 96
    },
    "Defending": {
      "Interceptions": 38,
      "Marking": 35,
      "SlidingTackle": 27,
      "StandingTackle": 28,
      "Heading": 71
    },
    "Physicality": {
      "Strength": 65,
      "Stamina": 85,
      "Aggression": 48,
      "Jumping": 69
    },
    "Goalkeeping": {}
  },
  "Traits": [ "Weak Foot Specialization", "Left Foot Accuracy" ],
  "SkillUnlocks": [ "RainbowFlick", "Elastico", "StepOver", "LaCroqueta", "Nutmeg" ],
  "FaceModelId": "FACE_MESSI_2024",
  "CreatedAt": "2024-01-10T15:30:00Z",
  "LastUpdatedAt": "2024-03-25T08:00:00Z"
}
```

### Enhancement Success Response

```json
{
  "Result": "Success",
  "CardInstanceId": "CARD_INST_12345",
  "PreviousLevel": 6,
  "NewLevel": 7,
  "NewOverall": 100,
  "UnlockedSkillSlot": true,
  "NewSkillSlots": [ "PassiveSlot1", "PassiveSlot2", "PassiveSlot3" ],
  "Timestamp": "2024-03-25T20:38:00Z"
}
```

### Match Result Payload

```json
{
  "MatchId": "MATCH_UUID_XYZ",
  "MatchType": "LeagueMatch",
  "HomeTeam": {
    "UserId": "USER_123",
    "SquadName": "Dream Squad",
    "FinalScore": 3,
    "ShotsOnTarget": 7,
    "Possession": 58.5
  },
  "AwayTeam": {
    "UserId": "USER_456",
    "SquadName": "Elite XI",
    "FinalScore": 2,
    "ShotsOnTarget": 5,
    "Possession": 41.5
  },
  "Duration": 540,
  "Winner": "USER_123",
  "Rewards": {
    "WinnerReward": {
      "GoldCoins": 1000,
      "Gems": 10,
      "EP": 100,
      "DivisionPoints": 50
    },
    "LoserReward": {
      "GoldCoins": 500,
      "Gems": 5,
      "EP": 50,
      "DivisionPoints": 20
    }
  },
  "CompletedAt": "2024-03-25T20:30:00Z"
}
```

---

**End of Technical Specification Document**
