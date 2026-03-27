# Soccer Mobile Pro — Mobile Touch Input Setup Guide

> Engine: UE5.6.1 | System: Enhanced Input | Reference: GDD Section 2.1.2

This guide explains how to configure the **Input Data Assets** in the Unreal Editor
and wire them to `BP_FootballPlayer` (the Blueprint subclass of `AMSoccerPlayerCharacter`).

---

## Overview: How It All Fits Together

```
Virtual Joystick Widget (UMG)
        │
        ▼
  IA_Move (InputAction, Axis2D)
        │
        ▼
  IMC_Match (InputMappingContext)
        │
        ▼
  AMSoccerPlayerCharacter::HandleMove()
        │
        ▼
  OnMoveInput_Implementation()  ← override in BP_FootballPlayer
```

---

## Step 1 — Create Input Actions

Create the following **Input Action** Data Assets under `Content/Input/Actions/`:

| Asset Name | Value Type | Triggers | Purpose |
|---|---|---|---|
| `IA_Move` | Axis2D (Vector2D) | — (driven by UMG joystick) | Joystick movement |
| `IA_JoystickDoubleTap` | Boolean | **Tap** × 2 within 0.25 s | Sprint toggle |
| `IA_Pass` | Boolean | Started / Ongoing / Completed | Ground pass / Lob |
| `IA_Shoot` | Boolean | Started / Ongoing / Completed | Shot type detection |
| `IA_ShootSwipeUp` | Axis1D (float) | Triggered | Chip shot swipe-up |
| `IA_ThroughBall` | Boolean | Tap | Through ball |
| `IA_Cross` | Boolean | Tap | Cross |
| `IA_Tackle` | Boolean | Tap | Tackle / Pressure |

### IA_JoystickDoubleTap — Trigger Setup
1. Open `IA_JoystickDoubleTap`
2. Add Trigger: **Tap**
3. Set `Tap Release Time Threshold` = **0.25 s**
4. Duplicate the Tap trigger and set it on the bound key as well
5. Alternatively, use the **Pulse** trigger with period 0.25 s for the second tap window

### IA_Pass — Trigger Setup
| Trigger Type | Settings | Purpose |
|---|---|---|
| `Started` | — | Begin hold timer |
| `Hold` (Ongoing) | Hold Time = 0.0 | Fires every frame while held |
| `Released` / `Completed` | — | Finalize pass type |

### IA_Shoot — Trigger Setup
Same pattern as IA_Pass.

### IA_ShootSwipeUp — Trigger Setup
1. Value Type: **Axis1D**
2. Map to **Touch Y Delta** on the shoot button touch area
3. Add Modifier: **Negate** (UE touch Y is inverted — swipe up = negative raw)
4. Add Modifier: **Scale** by `60.0` to normalize to ~0–1 velocity range
5. Add Trigger: **Threshold** with `ActuationThreshold = 0.1` so noise is ignored

---

## Step 2 — Create Input Mapping Context (IMC_Match)

1. `Content/Input/` → Right-click → **Input** → **Input Mapping Context**
2. Name it `IMC_Match`
3. Add mappings:

| Action | Key / Source | Modifiers | Notes |
|---|---|---|---|
| `IA_Move` | *(driven by UMG, set via `SetValue` in widget)* | — | See Step 3 |
| `IA_JoystickDoubleTap` | `Touch 1` on joystick zone | — | Use Input Mapping Zone widget |
| `IA_Pass` | `Touch 1` on Pass button area | — | |
| `IA_Shoot` | `Touch 1` on Shoot button area | — | |
| `IA_ShootSwipeUp` | `Touch Y Delta` on Shoot button area | Negate, Scale × 60 | |
| `IA_ThroughBall` | `Touch 1` on Through button | — | |
| `IA_Cross` | `Touch 1` on Cross button | — | |
| `IA_Tackle` | `Touch 1` on Tackle button | — | |
| (Gamepad) `IA_Move` | `Gamepad_Left2DAxis` | — | MFi / Xbox fallback |
| (Gamepad) `IA_Pass` | `Gamepad_FaceButton_Left` | — | |
| (Gamepad) `IA_Shoot` | `Gamepad_FaceButton_Bottom` | — | |

---

## Step 3 — Virtual Joystick UMG Widget (BP_VirtualJoystick)

Create `Content/UI/Input/BP_VirtualJoystick` as a **User Widget** Blueprint.

### Widget Hierarchy
```
Canvas Panel
  └─ [Joystick Base]  (Image, 200×200, anchored bottom-left)
        └─ [Joystick Knob]  (Image, 80×80, centered)
```

### Event Graph Logic

**On Touch Pressed (NativeOnTouchDown):**
```
[NativeOnTouchDown]
  → Store TouchID
  → Set JoystickCenter = touch position
  → Show JoystickBase at touch position
```

**On Touch Moved (NativeOnTouchMoved):**
```
[NativeOnTouchMoved]
  → Delta = CurrentTouch - JoystickCenter
  → ClampedDelta = Clamp(Delta, MaxRadius=80px)
  → Move Knob to ClampedDelta
  → Normalized = ClampedDelta / MaxRadius  (range -1..1)
  → Call EnhancedInput InjectInputForAction(
        Action = IA_Move,
        RawValue = FInputActionValue(Normalized),
        Modifiers = [],
        Triggers = []
    )
  → If |ClampedDelta| > DeadZone(0.15) AND bIsFirstTouch:
        Record tap for double-tap sprint detection
        InjectInputForAction(IA_JoystickDoubleTap, true)
        bIsFirstTouch = false
```

**On Touch Released (NativeOnTouchUp):**
```
[NativeOnTouchUp]
  → InjectInputForAction(IA_Move, FVector2D(0,0))
  → Hide JoystickBase
  → Reset bIsFirstTouch = true
```

> 💡 **Use `UEnhancedInputLocalPlayerSubsystem::InjectInputForAction()`** to feed
> virtual joystick values into the Enhanced Input pipeline. This keeps all input
> routing consistent and supports remapping.

---

## Step 4 — Action Button Widgets (BP_ActionButton_Pass, etc.)

Create per button: `BP_ActionButton_Pass`, `BP_ActionButton_Shoot`, etc.

### Blueprint for BP_ActionButton_Pass

**Variables:**
- `PlayerCharRef` (AMSoccerPlayerCharacter ref, set from HUD)
- `EnhancedInputSubsystem` (UEnhancedInputLocalPlayerSubsystem ref)

**NativeOnTouchDown:**
```
→ InjectInputForAction(IA_Pass, true, [], [Started])
→ Start hold timer widget animation (charge-up bar)
```

**NativeOnTouchUp:**
```
→ InjectInputForAction(IA_Pass, false, [], [Completed])
→ Stop charge-up animation
```

### Blueprint for BP_ActionButton_Shoot

**NativeOnTouchDown:**
```
→ InjectInputForAction(IA_Shoot, true, [], [Started])
→ Store initial touch Y position
→ Start power-shot charge animation
```

**NativeOnTouchMoved:**
```
→ SwipeDelta = InitialTouchY - CurrentTouchY  (positive = up)
→ SwipeVelocity = SwipeDelta / DeltaTime
→ If SwipeVelocity > 0:
      InjectInputForAction(IA_ShootSwipeUp, SwipeVelocity, [Scale×1], [Triggered])
```

**NativeOnTouchUp:**
```
→ InjectInputForAction(IA_Shoot, false, [], [Completed])
→ Clear charge animation
```

---

## Step 5 — Wire to BP_FootballPlayer

1. Create `Content/Characters/BP_FootballPlayer` as a Blueprint subclass of `AMSoccerPlayerCharacter`
2. In **Class Defaults**, assign:
   - `IMC_Match` → your `IMC_Match` asset
   - `IA_Move` → `IA_Move` asset
   - `IA_Pass`, `IA_Shoot`, `IA_ShootSwipeUp`, etc. → respective assets
   - `TouchConfig` → tune thresholds (LobHoldThreshold, PowerShotHoldThreshold, etc.)
3. Override `OnPassExecuted` event in BP:
   ```
   [OnPassExecuted (PassType)]
     Switch on PassType:
       GroundPass → Play Anim Montage: AM_GroundPass → Call MatchEngine::ExecutePass(GroundPass)
       LobPass    → Play Anim Montage: AM_LobPass    → Call MatchEngine::ExecutePass(LobPass)
   ```
4. Override `OnShootExecuted` event in BP:
   ```
   [OnShootExecuted (ShotType)]
     Switch on ShotType:
       NormalShot → AM_NormalShot → ExecuteShot(Normal)
       PowerShot  → AM_PowerShot  → ExecuteShot(Power) + Haptic Heavy
       ChipShot   → AM_ChipShot   → ExecuteShot(Chip)
   ```
5. Override `OnSprintStateChanged`:
   ```
   [OnSprintStateChanged (bSprinting)]
     If true  → Set Anim bool bIsSprinting = true  → Play sprint footstep SFX
     If false → Set Anim bool bIsSprinting = false → Stop sprint footstep SFX
   ```

---

## Step 6 — HUD Layout (BP_UIMatchHUD)

Place the following widgets on screen (all anchored):

```
┌─────────────────────────────────────────────────┐
│  [Score / Clock]           (top center)          │
│  [Minimap]                 (top left)            │
│                                                  │
│  [Virtual Joystick Zone]   (bottom left, 40%)    │
│                                                  │
│                    [Cross]  [Through]             │
│                    [Pass]   [Shoot]   (bottom right) │
│                             [Tackle]              │
└─────────────────────────────────────────────────┘
```

For each action button, set `Visibility = HitTestInvisible` on the wrapper,
and handle touch events only inside the button image bounds.

---

## Input Timing Reference

| Action | Condition | Result |
|---|---|---|
| Pass — tap (< 0.35 s) | `PassHoldAccumulator < LobHoldThreshold` | Ground Pass |
| Pass — hold (≥ 0.35 s) | `PassHoldAccumulator >= LobHoldThreshold` | Lob Pass |
| Shoot — tap (< 0.40 s) + no swipe | `ShootHoldAccumulator < PowerShotHoldThreshold` | Normal Shot |
| Shoot — hold (≥ 0.40 s) | `ShootHoldAccumulator >= PowerShotHoldThreshold` | Power Shot |
| Shoot — swipe-up (velocity ≥ 0.6) | `PeakSwipeUpVelocity >= ChipSwipeUpThreshold` | Chip Shot |
| Joystick — double-tap (< 0.25 s gap) | Two taps within `DoubleTapSprintWindow` | Sprint Toggle |

> All thresholds are configurable via `FTouchInputConfig` in BP Class Defaults
> without recompiling C++.

---

## Network Integration (TECHSPEC Section 5.1)

Each tick, `BP_FootballPlayer` should package a `FClientInputFrame` struct
and send it to the server via the `AFootballGameMode::OnReceiveClientInput()` RPC:

```cpp
// In BP_FootballPlayer Tick (or a 30Hz timer):
FClientInputFrame Frame;
Frame.FrameNumber         = FrameCounter++;
Frame.MoveAxisX           = CurrentMoveInput.X;
Frame.MoveAxisY           = CurrentMoveInput.Y;
Frame.bButtonPass         = bIsPassHeld;
Frame.PassHoldSeconds     = PassHeldDuration;
Frame.bButtonShoot        = bIsShootHeld;
Frame.ShootHoldSeconds    = ShootHeldDuration;
Frame.ShootSwipeUpVelocity = ShootSwipeUpVelocity;
Frame.bSprintActive        = bIsSprinting;
Frame.TimestampClientMs    = FDateTime::UtcNow().GetTicks() / 10000;
Server_SendInputFrame(Frame);  // Server RPC
```

---

*Document updated: March 2026 — Milestone 2 Input System*
