# Soccer Mobile Pro — AnimGraph Setup Guide
## ABP_FootballPlayer (Animation Blueprint)

> Engine: UE5.6.1 | Class Parent: UMSoccerAnimInstance  
> Asset Path: `Content/Characters/Animations/ABP_FootballPlayer`  
> GDD Reference: Section 2.1 | TECHSPEC: Section 9.1

---

## Overview: Full AnimGraph Architecture

```
                    ┌─────────────────────────────────────────┐
                    │           OUTPUT POSE                   │
                    └──────────────────┬──────────────────────┘
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │      Control Rig (Foot IK)              │  ← Layer 4
                    │  [CR_FootballPlayer_FootIK]             │
                    └──────────────────┬──────────────────────┘
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │    Layered Blend Per Bone               │  ← Layer 3
                    │  Base: Locomotion  |  Layer: Dribble    │
                    └──────────────────┬──────────────────────┘
                                       │
               ┌───────────────────────┼────────────────────────┐
               │                       │                        │
  ┌────────────▼──────────┐ ┌──────────▼──────────┐ ┌──────────▼──────────┐
  │  Main State Machine   │ │  Dribble Upper Body │ │   Montage Slots     │
  │  (Locomotion SM)      │ │  BlendSpace 1D      │ │  (Full Body / Upper)│
  └───────────────────────┘ └─────────────────────┘ └─────────────────────┘
```

---

## STEP 1 — Create the Animation Blueprint

1. `Content/Characters/Animations/` → Right-click → **Animation** → **Animation Blueprint**
2. Parent Class: `MSoccerAnimInstance` (your C++ class)
3. Skeleton: `SK_FootballPlayer` (your rigged skeleton asset)
4. Name: `ABP_FootballPlayer`

---

## STEP 2 — Create BlendSpaces

### BS_Locomotion (2D BlendSpace)

Path: `Content/Characters/Animations/BlendSpaces/BS_Locomotion`

**Axis Setup:**

| Axis | Variable | Min | Max | Grid Points |
|---|---|---|---|---|
| Horizontal (X) | Speed | 0 | 700 | 8 |
| Vertical (Y) | Direction | -180 | 180 | 5 |

**Sample Points:**

| Speed | Direction | Animation | Notes |
|---|---|---|---|
| 0 | 0 | `A_Idle` | Standing still |
| 200 | 0 | `A_Walk_Fwd` | Walking forward |
| 200 | 90 | `A_Walk_Right` | Strafe right |
| 200 | -90 | `A_Walk_Left` | Strafe left |
| 200 | 180 | `A_Walk_Back` | Walk backward |
| 500 | 0 | `A_Run_Fwd` | Running |
| 500 | 90 | `A_Run_Right` | |
| 500 | -90 | `A_Run_Left` | |
| 700 | 0 | `A_Sprint_Fwd` | Full sprint |
| 700 | 90 | `A_Sprint_Right` | |

**Settings:**
- Interpolation Time: `0.2s`
- Loop: ✅ Yes
- Axis to Scale Animation: Speed

---

### BS_Dribble_Upper (1D BlendSpace)

Path: `Content/Characters/Animations/BlendSpaces/BS_Dribble_Upper`

| Value | Animation |
|---|---|
| 0.0 | `A_Idle` (no ball — additive zero pose) |
| 0.5 | `A_Dribble_Walk` (arms low, controlling ball) |
| 1.0 | `A_Dribble_Sprint` (arms pumping, ball touch sprint) |

- Axis Variable: **Speed** (0–700, remapped to 0–1 via `MapRangeClamped` in EventGraph)
- Loop: ✅

---

## STEP 3 — Main State Machine (Locomotion SM)

In the AnimGraph, add a **State Machine** node named `SM_Locomotion`.

### States:

```
 [Idle] ──────────────────► [Walk/Run]
    ▲                            │
    └────────────────────────────┘
         Speed < 10

 [Walk/Run] ────────────► [Sprint]
    ▲                         │
    └─────────────────────────┘
         Speed < 650

 [Sprint] ──► [Walk/Run]   (Speed drops)
 [Any]    ──► [InAir]      (bIsInAir == true)
 [InAir]  ──► [Land]       (bIsInAir == false)
 [Land]   ──► [Idle]       (after 0.2s)
```

### State Contents:

**Idle State:**
```
[BS_Locomotion] → speed=0, dir=0
```

**Walk/Run State:**
```
[BS_Locomotion] → X=Speed, Y=Direction
```

**Sprint State:**
```
[BS_Locomotion] → X=Speed (clamped 650-700), Y=Direction
+ [Additive: A_Sprint_Lean] @ weight 0.4   ← forward lean
```

**InAir State:**
```
[A_Jump_Loop]  (looping fall animation)
```

**Land State:**
```
[A_Land] (one-shot, then → Idle transition)
```

### Transition Rules (all use automatic rule based on time / variable):

| From | To | Rule |
|---|---|---|
| Idle | Walk/Run | `Speed > 10` |
| Walk/Run | Idle | `Speed < 10 AND !bIsMoving` |
| Walk/Run | Sprint | `bIsSprinting AND Speed > 650` |
| Sprint | Walk/Run | `!bIsSprinting OR Speed < 600` |
| Any | InAir | `bIsInAir == true` |
| InAir | Land | `bIsInAir == false` |
| Land | Idle | Automatic after `A_Land` finishes |

---

## STEP 4 — Layered Blend Per Bone (Dribbling Override)

After `SM_Locomotion`, add a **Layered blend per bone** node:

```
[SM_Locomotion] ──► Base Pose
                         │
[BS_Dribble_Upper] ──► Layer 0 ───► [Layered Blend Per Bone] ──► next
                         │
                    Blend Weight: DribbleBlendWeight
```

**Settings:**

| Setting | Value |
|---|---|
| Layer Bone | `spine_01` (upper body split point) |
| Blend Depth | `-1` (all children of spine_01 affected) |
| Blend Weights | `DribbleBlendWeight` (driven by C++ 0→1 interp) |
| Mesh Space Rotation Blend | ✅ Yes |

> This keeps lower body (locomotion) pure while overlaying dribbling arms + torso.

---

## STEP 5 — Montage Slots

Add two **Slot** nodes after the Layered Blend:

### Slot 1: `FullBody` slot
```
[Layered Blend] ──► [Slot: FullBody] ──► next
```
Used by: Tackle montages, Bicycle Kick, Celebration  
Montages that override the **entire body**.

### Slot 2: `UpperBody` slot  
```
[Slot: FullBody result] ──► [Slot: UpperBody] ──► next
```
Used by: Shoot montages, Pass montages  
Montages that only override **upper body** — lower body locomotion continues.

### Montage Asset Setup:

For each montage in `Content/Characters/Animations/Montages/`:

| Asset | Slot | Loop | Blend In | Blend Out |
|---|---|---|---|---|
| `AM_GroundPass` | UpperBody | ❌ | 0.1s | 0.1s |
| `AM_LobPass` | UpperBody | ❌ | 0.1s | 0.15s |
| `AM_NormalShot` | UpperBody | ❌ | 0.05s | 0.1s |
| `AM_PowerShot` | UpperBody | ❌ | 0.05s | 0.15s |
| `AM_ChipShot` | UpperBody | ❌ | 0.05s | 0.1s |
| `AM_VolleyShot` | FullBody | ❌ | 0.1s | 0.2s |
| `AM_BicycleKick` | FullBody | ❌ | 0.15s | 0.3s |
| `AM_SlidingTackle` | FullBody | ❌ | 0.1s | 0.3s |
| `AM_StandingTackle` | UpperBody | ❌ | 0.05s | 0.1s |
| `AM_Celebration_*` | FullBody | ❌ | 0.2s | 0.5s |

### Adding Anim Notify to Montages:

In each shoot/pass montage, add **AnimNotify** at the "contact frame":

```
AM_NormalShot timeline:
  [0.0s ─────── 0.25s ──●── 0.6s]
                         │
                    Notify: AN_BallKickContact
                    → fires MSoccerBall::KickBall() from BP
```

---

## STEP 6 — Control Rig (Full-Body Foot IK)

### Create the Control Rig

1. `Content/Characters/Animations/` → Right-click → **Animation** → **Control Rig**
2. Name: `CR_FootballPlayer_FootIK`
3. Forward Solve graph setup:

### Control Rig Nodes (Rig Graph):

```
[Begin Execute]
    │
    ▼
[Get Transform: root]  ← read pelvis world position
    │
    ▼
[Offset Transform: root]  ← apply PelvisOffset (Z only)
    │           ↑ value from AnimInstance.PelvisOffset
    ▼
[FABRIK: left leg chain]
    ├── Root:    pelvis
    ├── Effector: foot_l
    ├── Target:   LeftFootIK.EffectorLocation  ← from AnimInstance
    ├── Precision: 0.001
    └── MaxIterations: 10
    │
    ▼
[FABRIK: right leg chain]
    ├── Root:    pelvis
    ├── Effector: foot_r
    ├── Target:   RightFootIK.EffectorLocation
    ├── Precision: 0.001
    └── MaxIterations: 10
    │
    ▼
[Set Rotation: foot_l]  ← LeftFootIK.EffectorRotation
[Set Rotation: foot_r]  ← RightFootIK.EffectorRotation
    │
    ▼
[End Execute]
```

### Wire Control Rig into AnimGraph:

```
[Slot: UpperBody result]
    │
    ▼
[Control Rig]
    ├── Rig: CR_FootballPlayer_FootIK
    ├── Alpha: FootIKAlpha          ← from AnimInstance (0 when airborne)
    ├── Input: PelvisOffset         ← AnimInstance.PelvisOffset
    ├── Input: LeftFootTarget       ← AnimInstance.LeftFootIK.EffectorLocation
    ├── Input: RightFootTarget      ← AnimInstance.RightFootIK.EffectorLocation
    ├── Input: LeftFootRotation     ← AnimInstance.LeftFootIK.EffectorRotation
    └── Input: RightFootRotation    ← AnimInstance.RightFootIK.EffectorRotation
    │
    ▼
[Output Pose]
```

---

## STEP 7 — Event Graph Wiring

In `ABP_FootballPlayer` **Event Graph**:

### On Blueprint Initialize Animation:
```
[Event Blueprint Initialize Animation]
    → Cast to AMSoccerPlayerCharacter → Store as "OwnerCharRef"
    → Get Character Movement → Store as "MovementComp"
```

### On Blueprint Update Animation (runs every frame):
```
[Event Blueprint Update Animation (DeltaTimeX)]
    → IsValid(OwnerCharRef)?
         YES → all variables auto-updated by C++ NativeUpdateAnimation
               (Speed, Direction, bIsSprinting, etc. are already set)
               → Nothing extra needed unless adding BP-only logic
```

### Wire Pass/Shoot to Montages (called from BP_FootballPlayer):
```
[Custom Event: BP_OnPassExecuted (PassType)]
    → Switch on PassType:
        GroundPass → Call PlayPassMontage(false)
        LobPass    → Call PlayPassMontage(true)

[Custom Event: BP_OnShootExecuted (ShotType)]
    → Switch on ShotType:
        NormalShot  → PlayShootMontage(NormalShot)
        PowerShot   → PlayShootMontage(PowerShot)
        ChipShot    → PlayShootMontage(ChipShot)
    → (Volley / Bicycle triggered by AnimNotify from match engine)

[Custom Event: BP_OnTackle]
    → PlayTackleMontage(bIsSliding)

[Custom Event: BP_OnGoalScored]  ← called from MSoccerGameState.OnGoalScored
    → PlayCelebrationMontage()
```

---

## STEP 8 — AnimGraph Final Node Order

Complete left-to-right AnimGraph node chain:

```
[SM_Locomotion]
    │
    ▼
[Layered Blend Per Bone]  ← Layer 0: BS_Dribble_Upper @ DribbleBlendWeight
    │
    ▼
[Slot: FullBody]           ← Tackle / Bicycle / Celebration montages
    │
    ▼
[Slot: UpperBody]          ← Shoot / Pass montages
    │
    ▼
[Control Rig]              ← CR_FootballPlayer_FootIK @ FootIKAlpha
    │
    ▼
[Output Pose]
```

---

## STEP 9 — Mobile Performance Optimizations

| Setting | Location | Recommended Value |
|---|---|---|
| Update Rate | Mesh Component | `URO` enabled: 30Hz for off-screen players |
| IK Trace | `UpdateFootIK()` | Skip if `Speed < 10` (standing still is exact) |
| FABRIK iterations | Control Rig | `10` for hero player, `4` for AI players |
| Blend Space grid | BS_Locomotion | 8×5 = 40 samples (no more on mobile) |
| Montage LOD | Each montage | Disable UpperBody slot at LOD3+ |
| Thread Safe Update | AnimInstance | Move read-only math to `NativeThreadSafeUpdateAnimation` |
| Skeleton LOD | SK_FootballPlayer | LOD0=full, LOD2=no fingers, LOD3=no facial blend shapes |

---

## Variable Summary (set by C++, read in AnimGraph)

| Variable | Type | Source | Used By |
|---|---|---|---|
| `Speed` | float | `OwnerMovement->Velocity.Size2D()` | BS_Locomotion X |
| `Direction` | float | NormalizedDeltaRotator.Yaw | BS_Locomotion Y |
| `bIsSprinting` | bool | `OwnerCharacter->bIsSprinting` | SM transition |
| `bIsMoving` | bool | Speed > 10 | SM transition |
| `bIsInAir` | bool | `IsFalling()` | SM transition |
| `DribbleBlendWeight` | float | Interpolated 0→1 on possession | Layered Blend |
| `FootIKAlpha` | float | 0 when airborne, 1 grounded | Control Rig alpha |
| `LeftFootIK` | FFootIKData | Line trace result | Control Rig input |
| `RightFootIK` | FFootIKData | Line trace result | Control Rig input |
| `PelvisOffset` | float | Min foot drop, smoothed | Control Rig input |
| `ActionState` | enum | Set before montage play | Debugging / HUD |
| `QueuedShotVariant` | enum | Set before shoot montage | Montage selection |

---

*Document updated: March 2026 — Milestone 2 Animation System*
