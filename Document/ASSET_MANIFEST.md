# Soccer Mobile Pro — Asset Manifest
## Danh Sách Tài Nguyên Cần Import vào Unreal Engine 5.6.1

> **Trạng thái ký hiệu:**
> - ⬜ `TODO`     — chưa tạo / chưa import
> - 🔄 `WIP`      — đang làm
> - ✅ `DONE`     — đã import vào UE5 và commit `.uasset`
> - 🔗 `EXTERNAL` — lấy từ Fab / Quixel / MetaHuman (link ghi chú)
>
> GDD Reference: Section 5 (Face/Character), Section 9 (Tech), Section 10 (Art)

---

## 1. CHARACTERS — `Content/Characters/`

### 1.1 Player Skeleton & Base Mesh

| # | File UE5 (`.uasset`) | Source File | Loại | Priority | Trạng thái |
|---|---|---|---|---|---|
| 1 | `SK_FootballPlayer` | `SK_FootballPlayer.fbx` | Skeletal Mesh | P0 | ⬜ TODO |
| 2 | `SKEL_FootballPlayer` | `SKEL_FootballPlayer.fbx` | Skeleton Asset | P0 | ⬜ TODO |
| 3 | `PHY_FootballPlayer` | *(tạo trong UE5 Physics Asset Editor)* | Physics Asset | P0 | ⬜ TODO |
| 4 | `SK_Goalkeeper` | `SK_Goalkeeper.fbx` | Skeletal Mesh | P1 | ⬜ TODO |
| 5 | `SK_PlayerLOD1` | `SK_FootballPlayer_LOD1.fbx` | LOD 1 (50% poly) | P1 | ⬜ TODO |
| 6 | `SK_PlayerLOD2` | `SK_FootballPlayer_LOD2.fbx` | LOD 2 (25% poly) | P1 | ⬜ TODO |
| 7 | `SK_PlayerLOD3` | `SK_FootballPlayer_LOD3.fbx` | LOD 3 (10% poly, AI) | P2 | ⬜ TODO |

> 📦 **Nguồn gợi ý**: [Fab.com — "Football Player Character"](https://www.fab.com/search?q=football+player)
> hoặc **MetaHuman Creator** (miễn phí, tích hợp UE5 native)

### 1.2 Player Face Textures (GDD Section 5.1)

| # | File UE5 | Source | Độ phân giải | Trạng thái |
|---|---|---|---|---|
| 8 | `T_Face_Generic_Diffuse` | `face_diffuse.png` | 4K (4096×4096) | ⬜ TODO |
| 9 | `T_Face_Generic_Normal` | `face_normal.png` | 4K | ⬜ TODO |
| 10 | `T_Face_Generic_Specular` | `face_specular.png` | 4K | ⬜ TODO |
| 11 | `T_Face_Generic_SSS` | `face_subsurface.png` | 2K (Subsurface Scatter) | ⬜ TODO |
| 12 | `T_Face_Ronaldo_Diffuse` | `face_ronaldo_diffuse.png` | 4K | ⬜ TODO |
| 13 | `T_Face_Messi_Diffuse` | `face_messi_diffuse.png` | 4K | ⬜ TODO |
| 14 | `T_Face_Mbappe_Diffuse` | `face_mbappe_diffuse.png` | 4K | ⬜ TODO |

> 📸 **Pipeline**: Photo → Photogrammetry → Texture Bake → Import PNG → UE5 Texture
> (GDD Section 5.1.1 — Face Data Pipeline)

### 1.3 Player Kit Textures

| # | File UE5 | Source | Club | Trạng thái |
|---|---|---|---|---|
| 15 | `T_Kit_Home_Generic` | `kit_home.png` | Generic placeholder | ⬜ TODO |
| 16 | `T_Kit_Away_Generic` | `kit_away.png` | Generic placeholder | ⬜ TODO |
| 17 | `T_Kit_Home_ClubA` | `kit_cluba_home.png` | Club A (replace với tên thật) | ⬜ TODO |
| 18 | `T_Kit_Away_ClubA` | `kit_cluba_away.png` | Club A | ⬜ TODO |
| 19 | `T_Boots_Default` | `boots_default.png` | — | ⬜ TODO |
| 20 | `T_Boots_Nike_Mercurial` | `boots_mercurial.png` | — | ⬜ TODO |

### 1.4 Materials

| # | File UE5 | Dùng cho | Trạng thái |
|---|---|---|---|
| 21 | `M_PlayerSkin` | Skin PBR (Diffuse+Normal+SSS) | ⬜ TODO |
| 22 | `M_PlayerKit` | Kit mesh | ⬜ TODO |
| 23 | `M_PlayerBoots` | Boots mesh | ⬜ TODO |
| 24 | `M_PlayerHair` | Hair cards + Strand shader | ⬜ TODO |
| 25 | `MI_Kit_Home` (Material Instance) | Kit home color variant | ⬜ TODO |
| 26 | `MI_Kit_Away` (Material Instance) | Kit away color variant | ⬜ TODO |

---

## 2. ANIMATIONS — `Content/Characters/Animations/`

### 2.1 Locomotion (BS_Locomotion BlendSpace)

| # | File UE5 | Source FBX | Mô tả | Trạng thái |
|---|---|---|---|---|
| 27 | `AN_Idle` | `anim_idle.fbx` | Đứng yên, idle breathing | ⬜ TODO |
| 28 | `AN_Walk_Fwd` | `anim_walk_fwd.fbx` | Đi thẳng | ⬜ TODO |
| 29 | `AN_Walk_Left` | `anim_walk_left.fbx` | Đi trái | ⬜ TODO |
| 30 | `AN_Walk_Right` | `anim_walk_right.fbx` | Đi phải | ⬜ TODO |
| 31 | `AN_Walk_Back` | `anim_walk_back.fbx` | Đi lùi | ⬜ TODO |
| 32 | `AN_Run_Fwd` | `anim_run_fwd.fbx` | Chạy thường | ⬜ TODO |
| 33 | `AN_Run_Left` | `anim_run_left.fbx` | Chạy trái | ⬜ TODO |
| 34 | `AN_Run_Right` | `anim_run_right.fbx` | Chạy phải | ⬜ TODO |
| 35 | `AN_Sprint` | `anim_sprint.fbx` | Sprint tốc độ cao | ⬜ TODO |
| 36 | `AN_Sprint_Left` | `anim_sprint_left.fbx` | Sprint nghiêng trái | ⬜ TODO |
| 37 | `AN_Sprint_Right` | `anim_sprint_right.fbx` | Sprint nghiêng phải | ⬜ TODO |

> 📦 **Nguồn gợi ý**: [Mixamo.com](https://www.mixamo.com) (miễn phí, retarget về SKEL_FootballPlayer)

### 2.2 BlendSpaces

| # | File UE5 | Axes | Trạng thái |
|---|---|---|---|
| 38 | `BS_Locomotion` | X=Speed (0→600), Y=Direction (-180→180) | ⬜ TODO |
| 39 | `BS_Dribble_Upper` | X=DribbleIntensity (0→1) | ⬜ TODO |

### 2.3 Montages — Tackle & Shooting (GDD 2.1)

| # | File UE5 | Source FBX | Slot | Trạng thái |
|---|---|---|---|---|
| 40 | `AM_Tackle_Sliding` | `anim_tackle_slide.fbx` | FullBody | ⬜ TODO |
| 41 | `AM_Tackle_Standing` | `anim_tackle_stand.fbx` | FullBody | ⬜ TODO |
| 42 | `AM_Shot_Power` | `anim_shot_power.fbx` | UpperBody | ⬜ TODO |
| 43 | `AM_Shot_Finesse` | `anim_shot_finesse.fbx` | UpperBody | ⬜ TODO |
| 44 | `AM_Shot_Chip` | `anim_shot_chip.fbx` | UpperBody | ⬜ TODO |
| 45 | `AM_Shot_Volley` | `anim_shot_volley.fbx` | FullBody | ⬜ TODO |
| 46 | `AM_Shot_BicycleKick` | `anim_bicycle_kick.fbx` | FullBody | ⬜ TODO |

### 2.4 Montages — Skill Moves (GDD Section 4)

| # | File UE5 | Source FBX | Stars | Trạng thái |
|---|---|---|---|---|
| 47 | `AM_Skill_StepOver` | `anim_skill_stepover.fbx` | 3★ | ⬜ TODO |
| 48 | `AM_Skill_BallRoll` | `anim_skill_ballroll.fbx` | 3★ | ⬜ TODO |
| 49 | `AM_Skill_FakeShot` | `anim_skill_fakeshot.fbx` | 2★ | ⬜ TODO |
| 50 | `AM_Skill_Nutmeg` | `anim_skill_nutmeg.fbx` | 3★ | ⬜ TODO |
| 51 | `AM_Skill_Roulette` | `anim_skill_roulette.fbx` | 4★ | ⬜ TODO |
| 52 | `AM_Skill_Elastico` | `anim_skill_elastico.fbx` | 5★ | ⬜ TODO |
| 53 | `AM_Skill_LaCroqueta` | `anim_skill_lacroqueta.fbx` | 4★ | ⬜ TODO |
| 54 | `AM_Skill_RainbowFlick` | `anim_skill_rainbow.fbx` | 5★ | ⬜ TODO |
| 55 | `AM_Skill_Sombrero` | `anim_skill_sombrero.fbx` | 4★ | ⬜ TODO |
| 56 | `AM_Skill_HeelToHeel` | `anim_skill_heeltoheel.fbx` | 4★ | ⬜ TODO |

### 2.5 Montages — Goalkeeper

| # | File UE5 | Source FBX | Trạng thái |
|---|---|---|---|
| 57 | `AM_GK_DiveLeft` | `anim_gk_dive_left.fbx` | ⬜ TODO |
| 58 | `AM_GK_DiveRight` | `anim_gk_dive_right.fbx` | ⬜ TODO |
| 59 | `AM_GK_Catch` | `anim_gk_catch.fbx` | ⬜ TODO |
| 60 | `AM_GK_Punch` | `anim_gk_punch.fbx` | ⬜ TODO |
| 61 | `AM_GK_Fingertip` | `anim_gk_fingertip.fbx` | ⬜ TODO |

### 2.6 Celebrations (GDD Section 5.3)

| # | File UE5 | Source FBX | Player | Trạng thái |
|---|---|---|---|---|
| 62 | `AM_Celebrate_Siuuu` | `anim_celebrate_siuuu.fbx` | Ronaldo | ⬜ TODO |
| 63 | `AM_Celebrate_ArmsWide` | `anim_celebrate_arms.fbx` | Messi | ⬜ TODO |
| 64 | `AM_Celebrate_Pharaoh` | `anim_celebrate_pharaoh.fbx` | Mbappe | ⬜ TODO |
| 65 | `AM_Celebrate_Backflip` | `anim_celebrate_backflip.fbx` | Generic | ⬜ TODO |
| 66 | `AM_Celebrate_Slide` | `anim_celebrate_slide.fbx` | Generic | ⬜ TODO |

---

## 3. STADIUMS — `Content/Stadiums/`

### 3.1 Stadium Meshes (GDD Section 10.1 — 15 stadiums at launch)

| # | File UE5 | Source | LOD | Trạng thái |
|---|---|---|---|---|
| 67 | `SM_Stadium_Generic` | `stadium_generic.fbx` | 3 LODs | ⬜ TODO |
| 68 | `SM_Stadium_WembleyStyle` | `stadium_wembley.fbx` | 3 LODs | ⬜ TODO |
| 69 | `SM_Stadium_SantiagoStyle` | `stadium_santiago.fbx` | 3 LODs | ⬜ TODO |
| 70 | `SM_Stadium_CampNouStyle` | `stadium_campnou.fbx` | 3 LODs | ⬜ TODO |
| 71 | `SM_Stadium_AnfieldStyle` | `stadium_anfield.fbx` | 3 LODs | ⬜ TODO |
| 72 | `SM_Stadium_OldTraffordStyle` | `stadium_oldtrafford.fbx` | 3 LODs | ⬜ TODO |
| 73 | `SM_Stadium_AsianStyle` | `stadium_asian.fbx` | 3 LODs | ⬜ TODO |

> 📦 **Nguồn gợi ý**: [Fab.com — "Soccer Stadium"](https://www.fab.com/search?q=soccer+stadium)

### 3.2 Pitch (Sân cỏ)

| # | File UE5 | Source | Mô tả | Trạng thái |
|---|---|---|---|---|
| 74 | `SM_Pitch_Grass` | `pitch_plane.fbx` | Flat mesh 105×68m | ⬜ TODO |
| 75 | `T_Grass_Diffuse` | `grass_diffuse.png` | 4K tiling grass | ⬜ TODO |
| 76 | `T_Grass_Normal` | `grass_normal.png` | 4K normal map | ⬜ TODO |
| 77 | `T_Grass_Wet` | `grass_wet.png` | 2K wet variant | ⬜ TODO |
| 78 | `T_PitchLines` | `pitch_lines.png` | 4K line markings overlay | ⬜ TODO |
| 79 | `M_Pitch_Grass` | *(tạo trong Material Editor)* | Master material + wetness param | ⬜ TODO |
| 80 | `MI_Pitch_Dry` | *(Material Instance)* | Dry weather variant | ⬜ TODO |
| 81 | `MI_Pitch_Wet` | *(Material Instance)* | Rainy weather variant | ⬜ TODO |
| 82 | `MI_Pitch_Night` | *(Material Instance)* | Night lighting variant | ⬜ TODO |

### 3.3 Stadium Props

| # | File UE5 | Mô tả | Trạng thái |
|---|---|---|---|
| 83 | `SM_Goal_Post` | Khung cầu môn | ⬜ TODO |
| 84 | `SM_Corner_Flag` | Cờ góc sân | ⬜ TODO |
| 85 | `SM_Dugout_Bench` | Băng ghế dự bị | ⬜ TODO |
| 86 | `SM_Advertising_Board` | Bảng quảng cáo sân | ⬜ TODO |
| 87 | `SM_Floodlight` | Đèn sân (4 cột) | ⬜ TODO |
| 88 | `SM_Crowd_Procedural` | Procedural crowd mesh | ⬜ TODO |
| 89 | `SM_Net_Goal` | Lưới cầu môn (cloth sim) | ⬜ TODO |

---

## 4. BALL — `Content/Gameplay/Ball/`

| # | File UE5 | Source | Mô tả | Trạng thái |
|---|---|---|---|---|
| 90 | `SM_Ball` | `ball.fbx` | Sphere mesh 22cm | ⬜ TODO |
| 91 | `T_Ball_Diffuse` | `ball_diffuse.png` | 2K texture | ⬜ TODO |
| 92 | `T_Ball_Normal` | `ball_normal.png` | 2K normal map | ⬜ TODO |
| 93 | `M_Ball` | *(Material Editor)* | PBR + wet param | ⬜ TODO |
| 94 | `BP_Ball` | *(Blueprint)* | Physics + spin logic | ⬜ TODO |
| 95 | `PS_BallTrail` | *(Niagara System)* | Power shot trail VFX | ⬜ TODO |
| 96 | `PS_BallSplash` | *(Niagara System)* | Water splash trên sân ướt | ⬜ TODO |

---

## 5. VFX — `Content/Gameplay/VFX/`

| # | File UE5 | Công cụ | Mô tả | Trạng thái |
|---|---|---|---|---|
| 97 | `PS_GoalExplosion` | Niagara | Hiệu ứng bàn thắng | ⬜ TODO |
| 98 | `PS_SkillAura` | Niagara | Glow khi thực hiện skill | ⬜ TODO |
| 99 | `PS_SprintDust` | Niagara | Bụi khi sprint trên sân khô | ⬜ TODO |
| 100 | `PS_WetGrass_Footstep` | Niagara | Giọt nước bắn khi chạy sân ướt | ⬜ TODO |
| 101 | `PS_TackleImpact` | Niagara | Va chạm tackle | ⬜ TODO |
| 102 | `PS_NutmegFlash` | Niagara | Flash khi nutmeg thành công | ⬜ TODO |
| 103 | `PS_CardPack_Open` | Niagara | Hiệu ứng mở gói thẻ | ⬜ TODO |
| 104 | `PS_LegendCard_Glow` | Niagara | Holographic glow thẻ Legend+ | ⬜ TODO |

---

## 6. AUDIO — `Content/Audio/`

### 6.1 Match SFX

| # | File UE5 | Source WAV | Mô tả | Trạng thái |
|---|---|---|---|---|
| 105 | `SC_BallKick_Power` | `sfx_kick_power.wav` | Tiếng đá bóng mạnh | ⬜ TODO |
| 106 | `SC_BallKick_Pass` | `sfx_kick_pass.wav` | Tiếng chuyền bóng | ⬜ TODO |
| 107 | `SC_Tackle_Hard` | `sfx_tackle_hard.wav` | Tiếng vào bóng mạnh | ⬜ TODO |
| 108 | `SC_Tackle_Soft` | `sfx_tackle_soft.wav` | Tiếng vào bóng nhẹ | ⬜ TODO |
| 109 | `SC_Whistle_Start` | `sfx_whistle_start.wav` | Còi bắt đầu trận | ⬜ TODO |
| 110 | `SC_Whistle_Foul` | `sfx_whistle_foul.wav` | Còi phạt | ⬜ TODO |
| 111 | `SC_GoalNet_Hit` | `sfx_goalnet.wav` | Tiếng bóng vào lưới | ⬜ TODO |
| 112 | `SC_Grass_Footstep_Dry` | `sfx_footstep_dry.wav` | Bước chân sân khô | ⬜ TODO |
| 113 | `SC_Grass_Footstep_Wet` | `sfx_footstep_wet.wav` | Bước chân sân ướt | ⬜ TODO |

### 6.2 Crowd Audio (MetaSounds)

| # | File UE5 | Mô tả | Trạng thái |
|---|---|---|---|
| 114 | `MS_Crowd_Ambient` | MetaSound — ambient crowd hum | ⬜ TODO |
| 115 | `MS_Crowd_Goal` | MetaSound — crowd roar on goal | ⬜ TODO |
| 116 | `MS_Crowd_NearMiss` | MetaSound — crowd "Ohhhh" | ⬜ TODO |
| 117 | `MS_Crowd_Tackle` | MetaSound — crowd react to tackle | ⬜ TODO |

---

## 7. UI TEXTURES — `Content/UI/`

| # | File UE5 | Source PNG | Mô tả | Trạng thái |
|---|---|---|---|---|
| 118 | `T_UI_CardFrame_Bronze` | `card_frame_bronze.png` | Khung thẻ Bronze | ⬜ TODO |
| 119 | `T_UI_CardFrame_Silver` | `card_frame_silver.png` | Khung thẻ Silver | ⬜ TODO |
| 120 | `T_UI_CardFrame_Gold` | `card_frame_gold.png` | Khung thẻ Gold | ⬜ TODO |
| 121 | `T_UI_CardFrame_Elite` | `card_frame_elite.png` | Khung thẻ Elite (Purple) | ⬜ TODO |
| 122 | `T_UI_CardFrame_Legend` | `card_frame_legend.png` | Khung thẻ Legend (Black+Gold) | ⬜ TODO |
| 123 | `T_UI_CardFrame_Icon` | `card_frame_icon.png` | Khung thẻ Icon (Rainbow Foil) | ⬜ TODO |
| 124 | `T_UI_CardFrame_Transcendence` | `card_frame_trans.png` | Khung Animated Holographic | ⬜ TODO |
| 125 | `T_UI_Logo_Main` | `logo_main.png` | Logo game chính | ⬜ TODO |
| 126 | `T_UI_HUD_Background` | `hud_bg.png` | Nền HUD trong trận | ⬜ TODO |
| 127 | `T_UI_SkillButton_Icons` | `skill_icons_atlas.png` | Sprite atlas 5 skill buttons | ⬜ TODO |
| 128 | `T_UI_StarRating` | `star_rating.png` | Star icon 1★–5★ sprites | ⬜ TODO |
| 129 | `T_UI_Position_Icons` | `position_icons.png` | ST/CAM/CM/CB/GK icons | ⬜ TODO |

---

## 8. BLUEPRINTS — `Content/Gameplay/` & phân nhóm

> Các Blueprint này không cần file FBX/PNG — tạo trực tiếp trong UE5 Editor.

| # | Blueprint | Kế thừa | Priority | Trạng thái |
|---|---|---|---|---|
| 130 | `BP_FootballPlayer` | `AMSoccerPlayerCharacter` | P0 | ⬜ TODO |
| 131 | `BP_Ball` | `AMSoccerBall` | P0 | ⬜ TODO |
| 132 | `BP_MatchController` | `AFootballGameMode` | P0 | ⬜ TODO |
| 133 | `BP_TeamAI` | `AAIController` | P1 | ⬜ TODO |
| 134 | `BP_CardManager` | `AActor` | P1 | ⬜ TODO |
| 135 | `BP_PackOpener` | `AActor` | P1 | ⬜ TODO |
| 136 | `BP_TransferMarket` | `AActor` | P2 | ⬜ TODO |
| 137 | `BP_Goalkeeper` | `BP_FootballPlayer` | P1 | ⬜ TODO |
| 138 | `ABP_FootballPlayer` | AnimBlueprint | P0 | ⬜ TODO |

---

## 9. DATA TABLES — `Content/Gameplay/Data/`

| # | File UE5 | Row Struct | Mô tả | Trạng thái |
|---|---|---|---|---|
| 139 | `DT_SkillDefinitions` | `FSkillDefinition` | 5 starter skills config | ⬜ TODO |
| 140 | `DT_PlayerCards` | `FPlayerCardData` | Base player card database | ⬜ TODO |
| 141 | `DT_EnhancementCosts` | `FEnhancementRow` | GDD Table 3.5.1 EP/GC costs | ⬜ TODO |
| 142 | `DT_PackDefinitions` | `FPackDefinition` | GDD Table 7.2 pack types | ⬜ TODO |
| 143 | `DT_FormationPresets` | `FFormationData` | 4-3-3, 4-2-3-1, 3-5-2... | ⬜ TODO |
| 144 | `DT_ChemistryBonuses` | `FChemistryRow` | GDD Table 2.2.3 | ⬜ TODO |

---

## 10. LEVELS — `Content/Levels/`

| # | Level | Mô tả | Trạng thái |
|---|---|---|---|
| 145 | `L_MainMenu` | Main menu / lobby | ⬜ TODO |
| 146 | `L_Match_Generic` | Generic match level | ⬜ TODO |
| 147 | `L_Match_Stadium_A` | Stadium A match level | ⬜ TODO |
| 148 | `L_Training` | Training dojo level | ⬜ TODO |
| 149 | `L_CardUpgrade` | Card upgrade UI level | ⬜ TODO |

---

## Tóm Tắt Tiến Độ

| Category | Tổng số | ✅ DONE | ⬜ TODO |
|---|---|---|---|
| Skeleton & Meshes | 7 | 0 | 7 |
| Face Textures | 7 | 0 | 7 |
| Kit Textures | 6 | 0 | 6 |
| Materials | 6 | 0 | 6 |
| Animations (FBX) | 40 | 0 | 40 |
| Stadium Assets | 23 | 0 | 23 |
| Ball & VFX | 18 | 0 | 18 |
| Audio | 14 | 0 | 14 |
| UI Textures | 12 | 0 | 12 |
| Blueprints | 9 | 0 | 9 |
| Data Tables | 6 | 0 | 6 |
| Levels | 5 | 0 | 5 |
| **TOTAL** | **153** | **0** | **153** |

---

## Thứ Tự Ưu Tiên Import

```
Phase 1 — P0 (cần để chạy được 1 trận thử):
  SK_FootballPlayer → SKEL → PHY → AN_Idle → AN_Run → BS_Locomotion
  SM_Pitch_Grass → M_Pitch_Grass → SM_Goal_Post
  SM_Ball → M_Ball → BP_Ball
  ABP_FootballPlayer → BP_FootballPlayer → BP_MatchController

Phase 2 — P1 (hoàn chỉnh gameplay loop):
  Tất cả AN_* animations → Tất cả AM_* montages
  Face Textures + Kit Materials
  DT_SkillDefinitions → DT_PlayerCards
  BP_TeamAI → BP_CardManager

Phase 3 — P2 (polish & content):
  Tất cả Stadium meshes
  VFX Niagara Systems
  Audio + MetaSounds
  UI Card frames
  Data Tables (Economy, Pack, Chemistry)
```

---

## Nguồn Asset Miễn Phí Gợi Ý

| Asset Type | Nguồn | Link |
|---|---|---|
| Player Character | Fab.com | https://www.fab.com/search?q=football+player |
| Player Character | Mixamo | https://www.mixamo.com |
| Player Face | MetaHuman Creator | https://metahuman.unrealengine.com |
| Animations | Mixamo | https://www.mixamo.com/#/?page=1&type=Motion%2CMotionPack |
| Stadium | Fab.com | https://www.fab.com/search?q=soccer+stadium |
| Grass Textures | Quixel Bridge | *(built into UE5 — Megascans)* |
| Ball | Fab.com | https://www.fab.com/search?q=soccer+ball |
| UI Frames | Freepik / Canva | *(create custom PNG) * |
| SFX | Freesound.org | https://freesound.org/search/?q=football |
| SFX | Zapsplat | https://www.zapsplat.com |

---

*Asset Manifest v1.0 — Soccer Mobile Pro — March 2026*
*Cập nhật file này mỗi khi import thêm asset: thay ⬜ TODO → ✅ DONE*
