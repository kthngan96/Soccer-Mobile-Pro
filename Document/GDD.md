**Soccer Mobile Pro**

**GAME DESIGN DOCUMENT (GDD)**

Version 1.0  |  Unreal Engine 5.6.1  |  Mobile Platform

*Classification: Internal Development Reference*


**Inspired by: eFootball Mobile | FIFA Online 3 | FIFA Mobile**

# **1\. Game Overview**

## **1.1 Game Summary**

Soccer Mobile Pro is a AAA-quality football simulation game built on Unreal Engine 5.6.1 for iOS and Android platforms. It combines the best elements of the world's leading football mobile games: the deep simulation gameplay of eFootball Mobile, the rich player card upgrade system of FIFA Online 3, and the intuitive skill mechanics of FIFA Mobile.

The game features photorealistic 3D player faces generated from real-world reference data, a comprehensive team-building meta-game, live match simulation, and a full suite of competitive and cooperative modes.

 

## **1.2 High Concept**

| Property | Details |
| :---- | :---- |
| Title | Soccer Mobile Pro |
| Genre | Football Simulation / Sports / Card Collecting |
| Platform | iOS 14+ and Android 10+ (Mobile First) |
| Engine | Unreal Engine 5.6.1.27 (UE4) |
| Target Rating | E (Everyone) / PEGI 3 |
| Business Model | Free-to-Play with optional IAP |
| Multiplayer | Real-time PvP, Asynchronous, Co-op |
| Update Cycle | Monthly content updates, weekly events |
| Languages | English, Vietnamese, Spanish, Japanese, Korean, Portuguese, French, German |
| Inspiration | eFootball Mobile \+ FIFA Online 3 \+ FIFA Mobile |

 

## **1.3 Target Audience**

* Core football fans aged 16–35

* Mobile gamers familiar with card-collection games

* Competitive players seeking ranked progression

* Casual fans wanting quick match sessions (5–10 min)

* Existing eFootball / FIFA Mobile player base

 

## **1.4 USPs (Unique Selling Points)**

* Photorealistic 3D player faces — closest to real life on mobile

* Three-system fusion: eFootball gameplay \+ FIFA Online 3 cards \+ FIFA Mobile skills

* Full offline \+ online mode support

* Advanced AI referee and match physics on UE4

* Season-based competitive ranked system

* Deep card evolution with up to 7 upgrade tiers

# **2\. Core Gameplay Systems**

## **2.1 Match Engine (eFootball Mobile-style)**

The match engine is the core experience of Soccer Mobile Pro. It is a full physics-based football simulation powered by UE4's PhysX and Chaos physics systems.

 

### **2.1.1 Match Formats**

| Mode | Duration | Format | Players |
| :---- | :---- | :---- | :---- |
| Quick Match | 6 min real-time | 5v5 or 11v11 | 1v1 Online / vs AI |
| League Match | 8 min real-time | 11v11 | 1v1 Online |
| Cup Match | 10 min real-time | 11v11 \+ Extra Time | 1v1 Online |
| Co-op Match | 10 min real-time | 11v11 | 2v2 Online |
| Training Match | Unlimited | 11v11 vs AI | Solo |
| Friendly | Custom | Custom | 1v1 / Co-op |

 

### **2.1.2 Controls — Touch & Virtual Joystick**

* Virtual Left Joystick: Player movement / sprint when double-tapped

* Pass Button: Ground pass (tap) / Lob pass (hold)

* Shoot Button: Low shot (tap) / Power shot (hold) / Chip (swipe up)

* Through Ball: Tap \+ directional flick

* Cross: Dedicated cross button; angle based on player position

* Tackle / Pressure: Auto-assist tackle button

* Skill Move: Triggered via specific gesture patterns (see Section 4\)

* Goalkeeper: Auto-controlled with manual dive override

* Gyroscope: Optional directional assist

 

### **2.1.3 Player AI Behavior (eFootball-inspired)**

* Teammates make intelligent off-the-ball runs

* AI adapts formation shape dynamically based on possession state

* Defender AI uses anticipation zones, not just reactive chasing

* Goalkeeper uses real save animations (fingertip, full dive, reflex)

* Player fatigue influences sprint speed and decision-making after 75th minute

 

### **2.1.4 Camera & View Options**

| Camera Mode | Description |
| :---- | :---- |
| Broadcast (Default) | Fixed elevated side view, best for match overview |
| Close Broadcast | Slightly closer, good for dribbling focus |
| End-to-End | Behind the ball, cinematic perspective |
| Player Lock | Follow selected player — for skill players |
| Dynamic | AI-controlled camera that follows action intelligently |

 

## **2.2 Team Management**

Players build and manage a squad of up to 30 players. The starting lineup consists of 11 players \+ 7 substitutes. Formation selection, player positioning, and tactical assignments are fully customizable.

 

### **2.2.1 Formations**

* All standard real-world formations supported: 4-3-3, 4-2-3-1, 3-5-2, 5-4-1, 4-4-2, etc.

* Custom formation builder: players can create and save up to 3 custom formations

* In-match formation changes allowed on-the-fly from pause menu

 

### **2.2.2 Tactics & Strategy**

* Pressing Intensity: Low / Medium / High / Ultra High

* Defensive Line: Deep / Mid / High

* Width: Narrow / Normal / Wide

* Build-up Play: Short / Mixed / Long Ball

* Attacking Style: Possession / Counter / Wing Play / Central Overload

* Set Piece Strategy: Pre-set routines for corners, free kicks, penalties

 

### **2.2.3 Player Chemistry**

Chemistry is a system borrowed and expanded from FIFA Online 3\. Players gain chemistry bonuses when teammates share the same League, Club, or Nationality.

| Chemistry Type | Bonus | Requirement |
| :---- | :---- | :---- |
| Perfect Chemistry | \+10% to all attributes | Same Club \+ Nationality |
| Club Chemistry | \+6% to all attributes | Same Club |
| League Chemistry | \+4% to key attributes | Same League |
| Nation Chemistry | \+3% to key attributes | Same Country |
| No Chemistry | 0% bonus | No shared connection |

# **3\. Player Card System (FIFA Online 3-Inspired)**

## **3.1 Card Overview**

Player cards are the core collectible unit in Soccer Mobile Pro. Every player is represented as a digital card with stats, skills, special abilities, and a 3D face model. Cards are collected, traded, and upgraded to form the player's dream squad.

 

## **3.2 Card Tiers & Rarity**

| Tier | Name | Color | Overall Range | Drop Rate (Pack) |
| :---- | :---- | :---- | :---- | :---- |
| Tier 1 | Bronze | Bronze | 40–59 OVR | 60.0% |
| Tier 2 | Silver | Silver | 60–74 OVR | 25.0% |
| Tier 3 | Gold | Gold | 75–84 OVR | 10.0% |
| Tier 4 | Elite | Purple | 85–89 OVR | 3.5% |
| Tier 5 | Legend | Black & Gold | 90–94 OVR | 1.2% |
| Tier 6 | Icon | Rainbow Foil | 95–99 OVR | 0.25% |
| Tier 7 | Transcendence | Animated Holographic | 100 OVR (Fixed) | 0.05% |

 

## **3.3 Card Types**

* Base Card: Standard version of a player at their current real-life performance level

* In-Form Card: Weekly updated card based on real-match performance (Man of Match, POTW)

* TOTY Card: Team of the Year — released annually, among the best cards

* TOTS Card: Team of the Season — end-of-season best performers

* Icon Card: Legendary retired players (Ronaldo R9, Zidane, Pele, Maradona)

* Hero Card: Rising legends — semi-retired, influential players

* Event Card: Limited-time themed cards (World Cup, Euro, Champions League)

* Custom Card: Community-built cards via Card Creator (cosmetic only)

 

## **3.4 Player Attributes**

| Category | Attributes |
| :---- | :---- |
| Pace | Sprint Speed, Acceleration |
| Shooting | Finishing, Long Shots, Shot Power, Volleys, Penalties |
| Passing | Short Passing, Long Passing, Vision, Crossing, FK Accuracy |
| Dribbling | Ball Control, Agility, Balance, Reactions, Composure |
| Defending | Interceptions, Marking, Sliding Tackle, Standing Tackle, Heading |
| Physicality | Strength, Stamina, Aggression, Jumping |
| GK (Goalkeepers) | Diving, Handling, Kicking, Positioning, Reflexes, Speed |

 

## **3.5 Card Upgrade System (FIFA Online 3-Style)**

The upgrade system is directly inspired by FIFA Online 3's Enhancement System. Players spend resources to level up their cards across multiple enhancement slots, unlock special traits, and evolve to higher tiers.

 

### **3.5.1 Enhancement Levels**

Each card has 10 Enhancement Levels (+0 to \+10). Each enhancement provides a stat boost. Upgrading requires Enhancement Points (EP) and Gold Coins.

| Enhancement Level | Stat Boost | EP Cost | Gold Coin Cost | Success Rate |
| :---- | :---- | :---- | :---- | :---- |
| \+1 | \+1 to each attribute | 100 EP | 5,000 | 100% |
| \+2 | \+2 overall | 200 EP | 10,000 | 100% |
| \+3 | \+3 overall | 400 EP | 20,000 | 100% |
| \+4 | \+4 overall | 800 EP | 40,000 | 85% |
| \+5 | \+5 overall \+ unlock Trait Slot | 1,500 EP | 80,000 | 75% |
| \+6 | \+6 overall | 2,500 EP | 150,000 | 65% |
| \+7 | \+7 overall \+ unlock Skill Slot | 4,000 EP | 250,000 | 55% |
| \+8 | \+8 overall | 6,000 EP | 400,000 | 45% |
| \+9 | \+9 overall | 10,000 EP | 600,000 | 35% |
| \+10 (MAX) | \+10 overall \+ unlock Evolution | 20,000 EP | 1,000,000 | 25% |

 

### **3.5.2 Upgrade Protection System**

* Breakthrough Card: Guarantees success when applied before an upgrade attempt

* Downgrade Protection: Card maintains current level on failure (does not decrease)

* Fusion Upgrade: Combine two identical cards at \+10 to unlock the next tier

* Mastery Badge: Applied to maxed cards to unlock bonus stat (+5 OVR fixed)

 

### **3.5.3 Card Evolution (Tier Breakthrough)**

When a card reaches \+10, it can undergo Evolution. Evolution transforms a card into the next rarity tier (e.g., Gold to Elite). Evolution requires:

* 1x Base Card at \+10

* 3x same player at any enhancement level (as fodder)

* Evolution Shards (obtainable via events and packs)

* Evolution cost in coins scales with tier

 

## **3.6 Player Market & Trading**

* Transfer Market: Real-time player-to-player auction marketplace

* Buy Now / Bid system with countdown timers

* Market filters: Rating, Position, League, Nation, Club, Price Range

* Price floor/ceiling enforced by system to prevent exploitation

* Trade History: Public log of recent sale prices for market analysis

* Quick Sell: Sell unwanted cards directly to system for EP \+ Coins

# **4\. Skill & Technique System (FIFA Mobile-Inspired)**

## **4.1 Overview**

The Skill System allows players to perform real football techniques during matches. It is modeled after FIFA Mobile's Skill Boost and technique system, with additional gesture-based inputs for mobile-optimized play. Each skill has a star rating (1–5) matching the FIFA skill system.

 

## **4.2 Dribbling Skills**

| Skill Name | Stars Required | Input Gesture | Effect |
| :---- | :---- | :---- | :---- |
| Step Over | 3★ | Circle gesture on joystick | Fake direction, beat defender |
| Double Step Over | 4★ | Double circle gesture | Two fakes in quick succession |
| Ball Roll | 3★ | Hold joystick sideways \+ tap skill btn | Lateral roll away from pressure |
| Elastico | 5★ | Flick right then left on joystick | Sudden direction change |
| Reverse Elastico | 5★ | Flick left then right | Opposite Elastico |
| Heel-to-Heel Flick | 4★ | Double backward tap | Quick heel flick, creates space |
| Fake Shot | 2★ | Shoot then cancel with direction | Trick defender into committing |
| Roulette | 4★ | 360° joystick spin | Spin through a tackle |
| La Croqueta | 4★ | Tap skill \+ horizontal swipe | Side-step past defender |
| Rabona Fake | 5★ | Hold shoot \+ swipe opposite foot | Trick cross / fake shot |
| Sombrero Flick | 4★ | Swipe up twice | Flick ball over defender's head |
| Rainbow Flick | 5★ | Backward then forward joystick | Lift ball over defender (aerial) |
| Nutmeg | 3★ | Flick directly at defender | Kick through defender's legs (risky) |
| First-Time Shot | 3★ | Shoot on receiving pass | Shoot without first touch |
| Scoop Turn | 3★ | Hold ball \+ flick up \+ direction | 360 scoop with direction change |
| Spin Shot | 4★ | Hold shoot \+ rotate joystick | Spinning curled effort |

 

## **4.3 Passing & Technique Skills**

* Driven Ground Pass: Power pass along the ground (R1 \+ pass on console; double-tap pass on mobile)

* No-Look Pass: Triggered when a teammate makes a run behind the passer; auto-suggests

* Backheel Pass: Swipe back on the joystick while pressing pass

* Lofted Through Ball: Hold through ball for 1 second

* Threaded Needle Pass: Precision pass through tight gaps (manual assist required)

 

## **4.4 Shooting Skills**

* Power Shot: Hold shoot for maximum power (risk of inaccuracy)

* Chip Shot: Quick upward flick on joystick while pressing shoot

* Driven Finesse: Side-foot curled shot into corners (hold aim \+ shoot)

* Outside Foot Shot: Shoot while moving away from dominant foot side

* Bicycle Kick: Auto-triggered when aerial ball arrives at right height \+ timing

* Scorpion Kick: Goalkeeper only — backward kick to clear aerial ball

 

## **4.5 Skill Star System**

Each player card has a Skill Stars rating (1–5) and Weak Foot rating (1–5). These determine which skills can be performed and how effectively.

| Stars | Skills Unlocked | Example Players |
| :---- | :---- | :---- |
| 1★ | Basic controls only | Low-tier Bronze/Silver defenders |
| 2★ | Fake Shot, Simple Dribbles | Standard midfielders |
| 3★ | Step Over, Ball Roll, Nutmeg | Good wingers, wide mids |
| 4★ | Roulette, La Croqueta, Heel Flick | Elite attacking players |
| 5★ | All skills including Elastico, Rainbow | Messi, Ronaldo, Neymar, Mbappe |

 

## **4.6 Skill Training (Boost Mode)**

Players can train specific skills in the Training Dojo to permanently boost their execution success rate and animation speed. Skill training costs Skill XP earned from:

* Completing Daily Challenges

* Playing Training Mode mini-games

* Watching skill tutorial replays

* Participating in Skill Tournaments

# **5\. 3D Player Face & Character System**

## **5.1 Face Technology Overview**

Soccer Mobile Pro uses a proprietary Face Rendering Pipeline built on UE4's physically-based rendering (PBR) system to generate photorealistic player faces that closely match real-life appearances.

 

### **5.1.1 Face Data Pipeline**

1. Reference Photo Collection: High-resolution front/side/angled photos of each player sourced from authorized press kits and licensed databases

2. Photogrammetry Reconstruction: 3D mesh is reconstructed from 2D reference images using depth estimation algorithms

3. Facial Feature Mapping: AI-assisted landmark detection places 468 facial landmarks (eyes, nose, mouth, jaw, cheeks, forehead) onto the base mesh

4. Texture Generation: 4K diffuse, normal, specular, and subsurface scattering maps generated per player

5. Blend Shape Animation: 52+ facial blend shapes per player for expression, celebration, and frustration animations

6. LOD (Level of Detail): 4 LOD levels — High (in-menu), Med (match close-up), Low (match), Minimal (distant match)

7. Real-time Update Protocol: When player transfers clubs, hairstyle/beard/skin tone updated in-game within 72 hours

 

## **5.2 Character Customization for Player Avatar**

User's personal in-game avatar (Manager / Pro Player mode) supports full customization:

* Face: Skin tone, eye color, nose shape, jaw shape, lip style

* Hair: 80+ hairstyle options, custom dye colors

* Beard: 40+ beard/facial hair options

* Body: Height, build (slim/athletic/stocky)

* Kit: Home/Away/Third kit from real licensed clubs

* Boots: 100+ boot designs with custom color options

* Accessories: Gloves, headbands, armbands, compression sleeves

 

## **5.3 Celebration Animations**

Each player has up to 5 signature celebration animations matching their real-life behavior:

* Ronaldo: Siuuu jump-spin-land

* Messi: Arms wide, looking up to sky

* Neymar: Pointing to sky \+ backflip attempt

* Mbappe: Arms crossed (pharaoh pose)

* Custom Celebrations: Players can equip unlocked generic celebrations

# **6\. Game Modes**

## **6.1 Manager Mode (Main Campaign)**

Players manage a club through a full football season. Recruit players, manage finances, set tactics, play matches, and compete for the title.

* Season Length: 38 matches (league) \+ cup competitions

* Transfer Budgets: Managed via in-game economy (not real money)

* Player Morale: Affects performance in matches

* Youth Academy: Discover and develop young players

* Stadium Upgrade: Level up home ground for revenue and atmosphere bonuses

 

## **6.2 Ultimate Team Mode (Card Collecting Hub)**

The primary competitive mode where players build their dream squad using collected player cards.

* Squad Builder: Build 11-man starting line-up from card collection

* Weekly Objectives: Challenges that reward specific card types

* Division Rivals: Weekly competitive ranked play

* Weekend League: 30-match weekend tournament for premium rewards

* Squad Battles: Ranked matches against AI-controlled real player squads

 

## **6.3 Live Season Mode (eFootball-Inspired)**

Real-world football data influences in-game content:

* Real match results affect player form ratings weekly

* Real Transfer Window: New card versions released reflecting real-world transfers

* Real Injury Alerts: Injured players have reduced in-game availability

* Live League Table mirrored in-game

 

## **6.4 Pro Evolution Mode (eFootball Inspired)**

Create your own player, develop them from youth to legend, and play across the world:

* Full custom player creation (see Section 5.2)

* Skill progression system: earn Attribute Points through performance

* Contract system: sign with clubs and rise through leagues

* Online Pro Clubs: 11-player co-op with real friends controlling individual players

 

## **6.5 Tournament Mode**

| Tournament | Type | Format | Entry | Reward |
| :---- | :---- | :---- | :---- | :---- |
| Local Cup | PvE | 8-team knockout | Free | Silver Packs |
| Division League | PvE | 10-team league | Free | Gold Packs |
| Global Tournament | PvP | 32-team bracket | Gems | Legend Pack \+ Coins |
| Champions Classic | PvP | 16-team UCL style | Gems | Icon Player Pick |
| Weekend Cup | PvP | 30 matches / weekend | Free (3 tokens/week) | Top rewards |
| Skill Competition | PvP/PvE | Skill-move challenges | Free | Skill XP \+ EP |

# **7\. Economy & Monetization**

## **7.1 In-Game Currencies**

| Currency | Symbol | Earn Method | Use |
| :---- | :---- | :---- | :---- |
| Gold Coins | GC | Matches, quests, daily login, sell cards | Transfers, enhancements, packs |
| Gems | 💎 | Purchase (IAP), event rewards, achievements | Premium packs, skip timers |
| Enhancement Points | EP | Quick-selling cards, events, objectives | Card enhancement |
| Skill XP | SXP | Training, challenges, matches | Skill training |
| Trophy Points | TP | Tournament performance | Exclusive trophy shop |
| Club Tokens | CT | Club events, weekly loyalty | Club shop items |

 

## **7.2 Pack System**

| Pack Name | Cost | Contents | Guaranteed |
| :---- | :---- | :---- | :---- |
| Starter Pack | Free (once) | 5 cards | 1x Silver minimum |
| Basic Pack | 5,000 GC | 5 cards | 3x Bronze+ |
| Gold Pack | 20,000 GC / 50 Gems | 5 cards | 1x Gold minimum |
| Premium Gold Pack | 100 Gems | 5 cards | 2x Gold \+ chance Elite |
| Elite Pack | 300 Gems | 5 cards | 1x Elite minimum |
| Legend Pack | 700 Gems | 5 cards | 1x Legend minimum |
| Icon Pack | 1,500 Gems | 3 cards | 1x Icon or better |
| Transcendence Pack | 3,000 Gems | 1 card | Transcendence tier |
| Club Special Pack | Club Tokens 500 | 5 themed cards | 2x specific club players |
| Daily Deal Pack | Varies (discounted) | 5 cards | Special rotating guarantee |

 

## **7.3 Pity System (Protection)**

A guaranteed pull system ensures fair monetization:

* Every 10 packs from Gold Pack or higher: 1 guaranteed Gold card or better

* Every 20 packs from Elite Pack or higher: 1 guaranteed Elite or better

* Every 50 packs from any pack: 1 Legend card guaranteed

* Every 200 packs from Icon Pack: 1 Icon card guaranteed

* Pity counter carries over across sessions; resets only on reward

 

## **7.4 Battle Pass**

| Tier | Type | Duration | Cost | Key Rewards |
| :---- | :---- | :---- | :---- | :---- |
| Free Track | Season Pass | Monthly | Free | Bronze/Silver cards, EP, GC |
| Pro Pass | Season Pass | Monthly | 499 Gems | Gold/Elite cards, cosmetics, 2x EP |
| Legend Pass | Season Pass | Monthly | 999 Gems | Legend card, exclusive kit, 3x everything |

 

## **7.5 IAP Store (Real Money Purchases)**

| Package | Gems | Bonus | USD Price |
| :---- | :---- | :---- | :---- |
| Starter | 50 Gems | \+5 bonus | $0.99 |
| Basic | 270 Gems | \+30 bonus | $4.99 |
| Popular | 580 Gems | \+100 bonus | $9.99 |
| Best Value | 1,250 Gems | \+300 bonus | $19.99 |
| Super | 2,700 Gems | \+800 bonus | $39.99 |
| Mega | 6,000 Gems | \+2,000 bonus | $79.99 |
| Legend Bundle | 15,000 Gems \+ Exclusive Icon | \+6,000 bonus | $199.99 |

# **8\. Progression & Reward Systems**

## **8.1 Player Rank (Account Level)**

The player's overall account progresses through ranks, unlocking new features, modes, and reward drops.

| Rank | Level Range | Unlock |
| :---- | :---- | :---- |
| Rookie | 1–10 | Basic modes, starter packs, first 5-man squad |
| Amateur | 11–20 | Transfer Market access, full 11v11 squad |
| Semi-Pro | 21–35 | Division Rivals, card evolution |
| Professional | 36–50 | Weekend League, Pro Clubs co-op |
| International | 51–70 | Icon cards, Transcendence packs |
| Legend | 71–99 | Hall of Fame, Legacy cosmetics |
| GOAT | 100 (Max) | Exclusive GOAT badge \+ GOAT avatar frame |

 

## **8.2 Division Rivals Ranking**

| Division | Tier | Points Range | Weekly Reward |
| :---- | :---- | :---- | :---- |
| Division 10 | Bronze I-III | 0–999 | 1x Silver Pack |
| Division 9 | Bronze I-III | 1000–1999 | 2x Silver Pack |
| Division 8 | Silver I-III | 2000–2999 | 1x Gold Pack |
| Division 7 | Silver I-III | 3000–3999 | 2x Gold Pack |
| Division 6 | Gold I-III | 4000–4999 | 1x Premium Gold Pack |
| Division 5 | Gold I-III | 5000–5999 | 2x Premium Gold Pack \+ EP |
| Division 4 | Elite I-III | 6000–6999 | 1x Elite Pack |
| Division 3 | Elite I-III | 7000–7999 | 2x Elite Pack |
| Division 2 | Legend | 8000–8999 | 1x Legend Pack |
| Division 1 | World Class | 9000+ | 1x Icon Pack \+ 1000 Gems |

 

## **8.3 Daily & Weekly Objectives**

* Daily Login Reward: 7-day streak system with escalating rewards (Day 7 \= Gold Pack)

* Daily Challenges: 5 tasks per day (score 2 goals, complete a match, etc.)

* Weekly Objectives: 10 tasks per week requiring mixed mode play

* Monthly Campaign: 30 objectives to unlock a special monthly player card

* Achievement System: 500+ total achievements with permanent badge rewards

# **9\. Technical Specifications (UE4)**

## **9.1 Unreal Engine 5.6.1 Architecture**

| System | Implementation |
| :---- | :---- |
| Engine | Unreal Engine 5.6.1.27.2 (final stable release) |
| Physics | Chaos Physics for ball, PhysX for player body collisions |
| Rendering | Mobile Deferred Renderer \+ PBR Materials |
| Animation | Control Rig \+ AnimGraph for full-body IK and motion blending |
| AI | Behavior Trees \+ EQS (Environmental Query System) for player AI |
| Networking | UE4 Dedicated Server \+ Epic Online Services (EOS) for matchmaking |
| Audio | MetaSounds engine for dynamic match audio |
| UI | UMG (Unreal Motion Graphics) fully designed in Blueprints \+ C++ |
| Localization | Unreal Localization Dashboard with 9 languages |
| Analytics | Epic Analytics \+ custom server-side event tracking |

 

## **9.2 Performance Targets**

| Device Tier | Target FPS | Resolution | Graphics Setting |
| :---- | :---- | :---- | :---- |
| Low-End (3GB RAM, SD 600\) | 30 FPS stable | 720p | Low — simplified shaders, reduced crowd |
| Mid-Range (4GB RAM, SD 720G+) | 60 FPS stable | 1080p | Medium — partial PBR, dynamic crowds |
| High-End (6GB RAM, SD 865+) | 60 FPS stable \+ HDR | 1080p+ | High — full PBR, real-time shadows |
| Premium (8GB+ RAM, SD 8 Gen 2+) | 90–120 FPS | 1440p | Ultra — ray-traced reflections, 8K textures |
| iOS (iPhone 13+) | 60 FPS stable | 1080p/1440p | High / Ultra adaptive |

 

## **9.3 Networking & Backend**

* Backend: AWS GameLift for dedicated match servers (Tokyo, Singapore, Frankfurt, Virginia)

* Matchmaking: Skill-Based (ELO \+ Division) with latency weighting

* Anti-Cheat: Server-authoritative match simulation; client sends inputs only

* Database: PostgreSQL for persistent player data; Redis for real-time session caching

* CDN: CloudFront for asset streaming; players download only what they own

* Live Ops API: Internal tool for event creation, balance patches, emergency toggles

 

## **9.4 Data & Storage**

* Initial Download: \~2.5 GB (base game)

* Streaming Assets: 3D faces, kits, stadiums downloaded on-demand

* Offline Cache: Last 5 played teams/squads cached for offline vs AI play

* Cloud Save: Full progress backed up to server; device swap supported

* Account Security: OAuth 2.0 / Email OTP / Google & Apple Sign-In

# **10\. Art Direction & Audio Design**

## **10.1 Visual Style**

Soccer Mobile Pro targets a photorealistic-but-stylized look — closer to broadcast TV than pure hyperrealism, to remain performant on mid-range devices.

* Player Models: 20,000–80,000 poly (depending on LOD level)

* Stadiums: 15 fully licensed stadiums at launch; 30+ planned

* Kits: Authentic licensed kits from 500+ clubs, updated seasonally

* Weather System: Clear, Overcast, Rain, Heavy Rain, Night conditions

* Crowd: 3D procedural crowd system (up to 20,000 individual fans visible)

* Ball Physics: Physically accurate spin, backspin, curve, wet surface drag

 

## **10.2 UI/UX Design**

* Dark green and gold primary palette (football premium aesthetic)

* Card UI: Holographic effects, animated backgrounds for Legend+ cards

* Lobby: 3D stadium environment as background, players warming up

* Match HUD: Minimal, semi-transparent — score, time, stamina bars, minimap

* Haptic Feedback: Impact haptics on shots, tackles, crowd reactions

* Accessibility: Full colorblind modes, text scaling, one-handed control option

 

## **10.3 Audio Design**

* Commentary: Full dual-language commentary (English \+ regional language) recorded with real commentators

* Crowd Audio: Dynamic reactive crowd system — adapts to match situation in real-time

* Ball Sounds: Distinct audio profiles per surface (wet grass, dry turf, indoor)

* Player Voices: Grunt/shout samples from 200+ players

* Soundtrack: Original orchestral \+ electronic fusion score for menus; licensed tracks for celebrations

* Diegetic Audio: Manager shouting from touchline, referee whistle physics

# **11\. Social & Community Features**

## **11.1 Clubs (Guild System)**

* Create or join a Club of up to 30 players

* Club House: Shared 3D lobby space with trophy display

* Club Objectives: Shared weekly goals for Club Tokens

* Club League: Clubs compete in a separate league table

* Club Transfer Market: Internal player trades within club members

* Club Chat: In-game text \+ emoji \+ quick sticker system

 

## **11.2 Friends & Social**

* Friend List: Up to 200 friends cross-platform

* Friend Match: Friendly matches without ladder consequences

* Gift System: Daily gift coins/EP to friends (cap per day)

* Watchlist: Follow top players' squads for inspiration

* Replay Share: Export match highlights as video (30-second clip)

 

## **11.3 Esports & Competitive**

* Official in-game Esports qualifier system

* Monthly National Championship brackets

* Annual World Championship with in-game spectator mode

* Leaderboard: Global, Regional, National, Friends rankings

* Live Match Spectating: Watch top-ranked matches live in-app

# **12\. Content Roadmap & Live Services**

## **12.1 Launch Content (Day 0\)**

* 30 fully licensed real football clubs

* 500 player cards (including 20 Icon cards)

* 15 real stadiums

* 6 game modes fully operational

* Full transfer market enabled

* Seasonal Battle Pass (Month 1\)

 

## **12.2 Monthly Update Schedule**

| Month | Content Update | Major Feature |
| :---- | :---- | :---- |
| Month 1 | TOTY Campaign, new event cards | Club League launch |
| Month 2 | Champions League event, Icon expansion | Pro Clubs online (11v11) |
| Month 3 | World Cup special edition cards | Stadium Editor beta |
| Month 4 | TOTS Campaign | Draft Champions mode |
| Month 5 | Legends expansion pack | Esports qualifier integration |
| Month 6 | End-of-season reset \+ new season cards | New league added |

 

## **12.3 Year 1 Targets**

* 10 million downloads in 90 days post-launch

* 50 licensed clubs at end of Year 1

* 1,000+ total player cards

* 30 stadiums

* Official Esports World Championship event

* Console crossplay exploration (Year 1 Q4)

# **13\. AI Development Reference Notes (for ChatGPT Codex)**

## **13.1 Repository Structure (Recommended)**

The following folder structure is recommended for the UE4 project:

* Content/ — All game assets

* Content/Characters/ — Player meshes, face textures, skeleton assets

* Content/Stadiums/ — Stadium level assets per venue

* Content/UI/ — All UMG widgets and HUD elements

* Content/Gameplay/ — Ball, pitch, physics volumes

* Source/ — All C++ source files

* Source/MatchEngine/ — Core match logic, physics callbacks, input

* Source/PlayerCard/ — Card system, enhancement logic, evolution

* Source/Economy/ — Currency, pack opening, pity system

* Source/Network/ — EOS integration, matchmaking, dedicated server

* Source/AI/ — Behavior trees, EQS queries, team AI

* Config/ — Project settings, input mappings

 

## **13.2 Key Blueprints to Generate**

8. BP\_FootballPlayer — base player pawn with AnimGraph integration

9. BP\_Ball — physics-based ball with surface material detection

10. BP\_MatchController — match state machine (kick off → half time → full time)

11. BP\_TeamAI — team formation AI, set piece logic

12. BP\_CardManager — card inventory, enhancement, evolution logic

13. BP\_PackOpener — randomized pull with pity counter

14. BP\_TransferMarket — auction listing, bidding, buy-now

15. BP\_UIMainMenu — full navigation flow

16. BP\_UIMatchHUD — real-time in-match HUD

17. BP\_UICardUpgrade — enhancement UI with animations

 

## **13.3 Key C++ Classes to Generate**

18. MSoccerPlayerCharacter — extends ACharacter, handles input, IK, skill system

19. MSoccerBall — extends AActor, PhysX ball with drag/spin calculations

20. UCardDatabase — UDataAsset managing all player card definitions

21. UEnhancementComponent — handles EP costs, success rolls, failure logic

22. UEconomyManager — subsystem tracking all currencies, transactions

23. UMatchSimulator — handles offline AI match fast-sim for Squad Battles

24. USkillSystem — gesture recognition, skill star gating, execution logic

25. AFootballGameMode — match rules, timer, score, substitutions

26. UFaceRenderComponent — PBR face texture swapping, blend shape control

 

## **13.4 External APIs & Services to Integrate**

| Service | Purpose | Notes |
| :---- | :---- | :---- |
| Epic Online Services (EOS) | Authentication, matchmaking, friends | Free tier sufficient for launch |
| AWS GameLift | Dedicated match servers | UE4 native integration available |
| Frostbite 3rd Party Face API | Face photogrammetry input | Custom pipeline — placeholder |
| Google Play Billing v5 | Android IAP | Required for Android store |
| Apple StoreKit 2 | iOS IAP | Required for iOS store |
| Firebase Analytics | Event tracking, crash reporting | Android \+ iOS cross-platform |
| Appsflyer | Attribution and ad analytics | For user acquisition campaigns |
| CheatBreaker / Easy Anti-Cheat | Anti-cheat solution | EAC has UE4 official support |

 

## **13.5 Coding Conventions**

* Language: Primarily C++ with Blueprints for rapid prototyping and UI

* Naming: UE4 standard — U prefix for UObject, A for Actor, F for struct, E for enum

* Comments: Doxygen-style comments on all public functions

* Testing: Automation Testing Plugin for unit tests on game logic

* Version Control: Git with Perforce-style branching for asset management

* Build System: Unreal Build Tool (UBT); separate iOS and Android build pipelines

# **Appendix A: Glossary**

| Term | Definition |
| :---- | :---- |
| OVR | Overall Rating — a single number (0–100) summarizing a player's total attributes |
| EP | Enhancement Points — resource used for card upgrading |
| GC | Gold Coins — primary soft currency |
| IAP | In-App Purchase — real money transaction |
| PvP | Player vs. Player — online match against a real human opponent |
| PvE | Player vs. Environment — match against AI |
| LOD | Level of Detail — system that reduces model complexity at a distance |
| PBR | Physically Based Rendering — realistic light simulation in UE4 |
| UMG | Unreal Motion Graphics — UE4's built-in UI framework |
| EOS | Epic Online Services — backend platform from Epic Games |
| EQS | Environment Query System — UE4 AI spatial reasoning module |
| TOTY | Team of the Year — annual best players special cards |
| TOTS | Team of the Season — seasonal best performers special cards |
| Pity Counter | Guaranteed-pull counter in pack system; ensures fair distribution |
| Chemistry | Bonus system rewarding thematic squad-building connections |
| Blend Shape | 3D facial animation deformation key for expressions |

 

# **Appendix B: Competitors Analysis**

| Game | Strength Borrowed | Weakness We Solve |
| :---- | :---- | :---- |
| eFootball Mobile | Deep simulation physics, manual controls, online PvP | Poor monetization UX, lack of card meta |
| FIFA Mobile | Skill system, event calendar, accessible controls | Shallow simulation, unrealistic faces |
| FIFA Online 3 | Deep card upgrade system, chemistry, market | PC-only, no mobile port |
| Dream League Soccer | Fast casual play, custom team | No licensed players, poor graphics |
| PES Club Manager | Strategy depth, formation control | Outdated engine, slow updates |

 

# **Document Control**

| Version | Date | Author | Changes |
| :---- | :---- | :---- | :---- |
| 1.0 | March 2026 | Game Design Team | Initial GDD creation for AI-assisted development |
| 1.1 | TBD | TBD | Post-prototype revision |
| 2.0 | TBD | TBD | Full pre-production GDD |

 

***END OF DOCUMENT — Soccer Mobile Pro GDD v1.0***