# Soccer Mobile Pro — Skill System Guide
## USkillSystem + UMGestureRecognizer Setup

> Engine: UE5.6.1 | GDD Section 4.1–4.5 | TECHSPEC Section 4.3

---

## Architecture Overview

```
BP_VirtualJoystick (UMG)
  │  FGestureSample @ 30Hz
  ▼
UMGestureRecognizer
  │  OnGestureRecognized (EGestureType)
  ▼
USkillSystem (ActorComponent on BP_FootballPlayer)
  │
  ├── 1. HasBallPossession?
  ├── 2. bAnySkillExecuting?
  ├── 3. GetSkillDefinition (DataTable / fallback)
  ├── 4. CheckStarGate (PlayerSkillStars >= RequiredStars)
  ├── 5. CheckCooldown (GetTimeSeconds - LastExecuted >= CooldownSeconds)
  ├── 6. CheckMovement (if bRequiresMovement)
  ├── 7. ComputeSuccessChance (BaseChance - DefenderProximityPenalty + StarBonus)
  ├── 8. Roll FMath::FRand()
  │
  ├── SUCCESS → PlaySkillMontage → OnSkillSucceeded (BPNativeEvent)
  └── FAILURE → OnSkillFailed (BPNativeEvent) + half cooldown
```

---

## Gesture → Skill Mapping (GDD 4.2)

| Gesture | Skill | Stars | Cooldown | Base Success |
|---|---|---|---|---|
| Circle (≥270° arc, ≤0.6s) | Step Over | 3★ | 3.0s | 85% |
| 360° Spin | Roulette | 4★ | 3.5s | 78% |
| Hold Sideways (≥0.8 lateral, ≥0.3s) | Ball Roll | 3★ | 2.5s | 90% |
| Shoot + Cancel (injected) | Fake Shot | 2★ | 2.0s | 92% |
| Flick At Defender (forward Y flick) | Nutmeg | 3★ | 4.0s | 55% |

### Extended Gesture Map (future milestones)

| Gesture | Skill |
|---|---|
| Double circle | Double Step Over (4★) |
| Backward flick × 2 | Heel-to-Heel (4★) |
| Horizontal swipe | La Croqueta (4★) |
| Swipe Up × 2 | Sombrero Flick (4★) |
| FlickRight → FlickLeft | Elastico (5★) |
| FlickLeft → FlickRight | Reverse Elastico (5★) |

---

## Step 1 — Add USkillSystem to BP_FootballPlayer

1. Open `BP_FootballPlayer`
2. **Add Component** → search `SkillSystem` → add `USkillSystem`
3. In the component's **Details panel**:
   - `Skill Data Table` → `DT_SkillDefinitions` *(create in Step 3)*
   - `Player Skill Stars` → 3 (default; overridden from card data at runtime)
   - `Defender Influence Radius` → 150 cm
4. In **Skill Montages** map, add entries:

| Key (ESkillID) | Value (AnimMontage) |
|---|---|
| StepOver | `AM_Skill_StepOver` |
| BallRoll | `AM_Skill_BallRoll` |
| FakeShot | `AM_Skill_FakeShot` |
| Nutmeg | `AM_Skill_Nutmeg` |
| Roulette | `AM_Skill_Roulette` |

---

## Step 2 — Wire Joystick to Skill System

In `BP_VirtualJoystick` **NativeOnTouchMoved**, add after existing move input:

```
[NativeOnTouchMoved]
  ... existing move input injection ...
  │
  ▼
[Make FGestureSample]
  ├── JoystickPosition = NormalizedDelta  (-1..1)
  ├── Delta            = CurrentNorm - PreviousNorm
  └── Timestamp        = Get Time Seconds
  │
  ▼
[Get Player Character → Cast to AMSoccerPlayerCharacter]
  │
  ▼
[Get SkillSystem Component]
  │
  ▼
[FeedJoystickSample(GestureSample)]
```

---

## Step 3 — Create DT_SkillDefinitions DataTable

1. `Content/Gameplay/Skills/` → Right-click → **Miscellaneous** → **Data Table**
2. Row type: `FSkillDefinition`
3. Name: `DT_SkillDefinitions`
4. Add rows (Row Name = ESkillID string value):

| Row Name | DisplayName | RequiredStars | RequiredGesture | CooldownSeconds | BaseSuccessChance |
|---|---|---|---|---|---|
| `StepOver` | Step Over | 3 | Circle | 3.0 | 0.85 |
| `BallRoll` | Ball Roll | 3 | HoldSideways | 2.5 | 0.90 |
| `FakeShot` | Fake Shot | 2 | ShootCancel | 2.0 | 0.92 |
| `Nutmeg` | Nutmeg | 3 | FlickAtDefender | 4.0 | 0.55 |
| `Roulette` | Roulette | 4 | Spin360 | 3.5 | 0.78 |

> If `DT_SkillDefinitions` is not assigned, `USkillSystem` automatically uses
> the hardcoded `FallbackSkillDefs` built in `InitDefaultSkillData()` — no crash.

---

## Step 4 — Create Skill Montages

For each skill, create a montage in `Content/Characters/Animations/Montages/Skills/`:

### AM_Skill_StepOver
- Slot: `UpperBody`
- Blend In: 0.1s | Blend Out: 0.15s
- Add **AnimNotify** at frame 0.1s: `AN_SkillStart` (lock ball to foot)
- Add **AnimNotify** at frame 0.5s: `AN_SkillEnd`  → call `OnSkillMontageEnded(StepOver)`

### AM_Skill_Roulette
- Slot: `FullBody` (whole body spins)
- Blend In: 0.15s | Blend Out: 0.2s
- Root motion: ✅ Enable (moves character forward during spin)
- `AN_SkillEnd` at final frame

### AM_Skill_FakeShot
- Slot: `UpperBody`
- Reuse first 0.3s of `AM_NormalShot`, then branch to idle
- `AN_SkillEnd` at 0.3s

### AM_Skill_Nutmeg
- Slot: `UpperBody`
- Kick animation aiming low through defender's legs
- `AN_BallRelease` at frame 0.2s → detach ball and set velocity through defender

### AM_Skill_BallRoll
- Slot: `UpperBody`
- Side-step foot roll animation
- `AN_SkillEnd` at 0.4s

### AnimNotify Blueprint Handler (in BP_FootballPlayer):
```
[AnimNotify: AN_SkillEnd (SkillID param)]
  │
  ▼
[Get SkillSystem Component]
  │
  ▼
[OnSkillMontageEnded(SkillID)]  → resets bAnySkillExecuting = false
```

---

## Step 5 — FakeShot Special Case

Fake Shot requires detecting "shoot button pressed then direction changed" — 
this is NOT a joystick gesture, it comes from the **shoot button widget**.

In `BP_ActionButton_Shoot` **NativeOnTouchMoved**:
```
[NativeOnTouchMoved]
  │
  ▼
[If shoot is held AND touch moved laterally by > 30px]
  │
  ▼
[Get SkillSystem Component]
  │
  ▼
[InjectShootCancelGesture()]  → triggers EGestureType::ShootCancel → FakeShot
  │
  ▼
[Cancel IA_Shoot injection]  → prevents actual shot from firing
```

---

## Step 6 — Cooldown HUD Ring (BP_UIMatchHUD)

Bind `OnSkillCooldownUpdated` to update the HUD:

```
[BeginPlay in BP_UIMatchHUD]
  │
  ▼
[Get Player SkillSystem]
  │
  ▼
[Bind to OnSkillCooldownUpdated]
    → (SkillID, RemainingSeconds)
    → Switch on SkillID
       StepOver → Set SkillButton1_Cooldown ring progress = Remaining / 3.0
       Roulette → Set SkillButton_Roulette ring = Remaining / 3.5
       ...
```

---

## Step 7 — Star Rating from Card System

When the player's card is loaded into the lineup, call:

```
[On Match Start / Lineup Set]
  │
  ▼
[For Each Player in Lineup]
  │
  ▼
[Get FPlayerCardData.SkillStars]
  │
  ▼
[Get Character → Get SkillSystem Component]
  │
  ▼
[SetPlayerSkillStars(SkillStars)]
```

This automatically enables/disables skills based on the equipped card — 
a 2★ player cannot trigger Step Over (3★ required).

---

## Success Chance Formula

```
final_chance = BaseSuccessChance
             + (PlayerSkillStars - RequiredStars) × 0.03
             - DefenderProximityPenalty

DefenderProximityPenalty = lerp(0.5, 0.0, defenderDist / DefenderInfluenceRadius)
                           when defenderDist < DefenderInfluenceRadius
```

**Examples (StepOver, BaseChance=85%, 3★ player):**

| Defender Distance | Penalty | Final Chance |
|---|---|---|
| > 150cm (no nearby def) | 0% | **85%** |
| 75cm | 25% | **64%** |
| 30cm | 40% | **51%** |
| < 10cm | 50% | **43%** |

With a 5★ player on a StepOver (requires 3★): +2×3% = **91%** base.

---

## Skill Execution Result Codes

| Result | Cause | HUD Response |
|---|---|---|
| `Success` | Skill started | Show skill name flash |
| `StarGateFailed` | Card stars too low | Show ★ required tooltip |
| `OnCooldown` | Timer not elapsed | Show cooldown ring |
| `NoBallPossession` | No ball | No feedback (silently ignored) |
| `Blocked` | Success roll failed / no movement | Stumble anim |

---

*Document updated: March 2026 — Milestone 2 Skill System*
