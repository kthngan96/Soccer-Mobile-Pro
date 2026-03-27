# Soccer Mobile Pro — Required UE5.6.1 Plugins

> Engine: Unreal Engine 5.6.1 | Platform: iOS + Android (Mobile First)

## 1. Core Mobile Plugins

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| Mobile Support | `MobileSupport` | ✅ Yes | Core mobile platform layer |
| Android Support | `AndroidPermission` | ✅ Yes | Android runtime permissions |
| iOS Support | `IOSAudio` | ✅ Yes | iOS audio session handling |
| Mobile Patch Utility | `MobilePatchingUtils` | ✅ Yes | OTA asset patching / streaming download |
| Mobile File Server | `MobileFileServer` | 🔧 Dev | Cook-on-the-fly during development |

## 2. Online & Networking Plugins

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| Online Subsystem | `OnlineSubsystem` | ✅ Yes | Base OSS framework |
| Online Subsystem EOS | `OnlineSubsystemEOS` | ✅ Yes | Epic Online Services — auth, matchmaking, friends |
| Online Subsystem Utils | `OnlineSubsystemUtils` | ✅ Yes | Helper functions for OSS |
| EOS SDK | `EOSSDK` | ✅ Yes | Native EOS C SDK wrapper |
| Web Sockets | `WebSockets` | ✅ Yes | Real-time server communication |
| HTTP | `HTTP` | ✅ Yes | REST API calls to backend |
| JSON | `Json` | ✅ Yes | Serialization for API payloads |
| JSON Utilities | `JsonUtilities` | ✅ Yes | Struct-to-JSON helpers |

## 3. Physics Plugins

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| Chaos Physics | `ChaosVehicles` / `Chaos` | ✅ Yes | Ball physics, Chaos solver (UE5 default) |
| PhysX (Legacy) | `PhysXVehicles` | ⚠️ No (deprecated) | DO NOT enable; Chaos replaces PhysX in UE5 |
| Geometry Script | `GeometryScript` | 🔧 Optional | Runtime mesh generation for procedural pitch |

## 4. Animation & Character Plugins

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| Control Rig | `ControlRig` | ✅ Yes | Full-body IK, foot placement, procedural rig |
| IK Rig | `IKRig` | ✅ Yes | RetargetingIK, FABRIK for player body |
| Animation Warping | `AnimationWarping` | ✅ Yes | Stride warping, orientation warping on mobile |
| Motion Matching | `MotionTrajectory` | 🔧 Optional | Better locomotion blending |
| Live Link | `LiveLink` | 🔧 Dev | Facial motion capture pipeline dev only |
| MetaHuman | `MetaHuman` | ✅ Yes | Photorealistic 3D face rendering pipeline |

## 5. AI Plugins

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| AI Module | `AIModule` | ✅ Yes | Behavior Trees, Blackboard, EQS |
| Navigation System | `NavigationSystem` | ✅ Yes | NavMesh for player AI pathfinding on pitch |
| Environment Query System | (built into AIModule) | ✅ Yes | EQS for player positioning decisions |
| StateTree | `StateTree` | ✅ Yes | Lightweight state machine for player/ball AI |

## 6. UI Plugins

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| UMG (Unreal Motion Graphics) | `UMG` | ✅ Yes | All UI widgets and HUD |
| Slate | `Slate` / `SlateCore` | ✅ Yes | Low-level UI framework |
| Common UI | `CommonUI` | ✅ Yes | Mobile-optimized navigation, action bar |
| Common Game | `CommonGame` | ✅ Yes | Input routing, UI policy for console/mobile |
| Enhanced Input | `EnhancedInput` | ✅ Yes | Input mappings, touch / gamepad abstraction |

## 7. Audio Plugins

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| MetaSounds | `Metasound` | ✅ Yes | Dynamic match audio, crowd reactions |
| Audio Mixer | `AudioMixer` | ✅ Yes | Cross-platform audio mixing |
| Synthesis | `Synthesis` | 🔧 Optional | Procedural SFX generation |

## 8. Rendering Plugins

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| Mobile Rendering | (engine default) | ✅ Yes | Mobile Deferred Renderer |
| Nanite | `Nanite` | ❌ No | NOT supported on mobile in UE5.6 |
| Lumen | `Lumen` | ❌ No | NOT supported on mobile; use baked lighting |
| Virtual Shadow Maps | `VSM` | ❌ No | Mobile: use CSM (Cascaded Shadow Maps) instead |
| Texture Compression | `TextureFormat*` | ✅ Yes | ASTC for Android, PVRTC/ASTC for iOS |

## 9. Monetization / Platform SDKs

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| Android IAP (Google Play Billing v5) | `InAppPurchase` | ✅ Yes | Android store purchases |
| iOS IAP (StoreKit 2) | `InAppPurchase` | ✅ Yes | iOS App Store purchases |
| Advertisement | `Advertisement` | 🔧 Optional | Banner/rewarded ads (future) |

## 10. Anti-Cheat & Security

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| Easy Anti-Cheat | `EasyAntiCheatSDK` | ✅ Yes | Official UE5 EAC integration |

## 11. Analytics

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| Analytics | `Analytics` | ✅ Yes | Base analytics framework |
| Analytics ET | `AnalyticsET` | ✅ Yes | Epic Telemetry backend |
| Firebase Analytics | (3rd party plugin) | ✅ Yes | Add via marketplace or custom integration |

## 12. Localization

| Plugin | Module Name | Required? | Notes |
|--------|-------------|-----------|-------|
| Localization Dashboard | `Localization` | ✅ Yes | 9 languages as per GDD Section 1.2 |

---

## How to Enable in .uproject

Add to `SoccerMobilePro.uproject`:

```json
"Plugins": [
  { "Name": "OnlineSubsystem", "Enabled": true },
  { "Name": "OnlineSubsystemEOS", "Enabled": true },
  { "Name": "EOSSDK", "Enabled": true },
  { "Name": "ControlRig", "Enabled": true },
  { "Name": "IKRig", "Enabled": true },
  { "Name": "AnimationWarping", "Enabled": true },
  { "Name": "MetaHuman", "Enabled": true },
  { "Name": "AIModule", "Enabled": true },
  { "Name": "NavigationSystem", "Enabled": true },
  { "Name": "StateTree", "Enabled": true },
  { "Name": "CommonUI", "Enabled": true },
  { "Name": "CommonGame", "Enabled": true },
  { "Name": "EnhancedInput", "Enabled": true },
  { "Name": "Metasound", "Enabled": true },
  { "Name": "EasyAntiCheatSDK", "Enabled": true },
  { "Name": "MobilePatchingUtils", "Enabled": true }
]
```
