// Copyright (c) Soccer Mobile Pro. All Rights Reserved.
// Unreal Engine 5.6 — Module Build Rules
// TECHSPEC §9.1 (AI), §4 (Input), §10 (Online), §8 (UI), §12 (HTTP/JSON)

using UnrealBuildTool;

public class SoccerMobilePro : ModuleRules
{
    public SoccerMobilePro(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // ───────────────────────────────────────────────────────────────────────────
        // Public dependencies — available to modules that depend on this module
        // ───────────────────────────────────────────────────────────────────────────
        PublicDependencyModuleNames.AddRange(new string[]
        {
            // — UE core (always required) —
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",

            // — Enhanced Input (TECHSPEC §4: gesture + button mappings) —
            "EnhancedInput",

            // — AI (TECHSPEC §9.1: BehaviorTree + EQS + NavMesh) —
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",

            // — Online Subsystem (TECHSPEC §10: matchmaking, leaderboards) —
            "OnlineSubsystem",
            "OnlineSubsystemUtils",

            // — UI (TECHSPEC §8: UMG HUD, menus, overlays) —
            "UMG",
            "SlateCore",
            "Slate",

            // — HTTP + JSON (TECHSPEC §12: analytics, cloud save, pack API) —
            "Json",
            "JsonUtilities",
            "HTTP",
        });

        // ───────────────────────────────────────────────────────────────────────────
        // Private dependencies — internal use only, not exposed to dependents
        // ───────────────────────────────────────────────────────────────────────────
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            // — Gameplay Abilities (future skill system integration) —
            // Uncomment when USkillSystem integrates GAS:
            // "GameplayAbilities",
            // "GameplayTags",
            // "GameplayMessageRuntime",

            // — Audio (crowd SFX, commentary) —
            // "AudioMixer",
            // "MetasoundEngine",

            // — Niagara (goal VFX, weather particles) —
            // "Niagara",
        });

        // ───────────────────────────────────────────────────────────────────────────
        // Platform-specific settings
        // ───────────────────────────────────────────────────────────────────────────
        if (Target.Platform == UnrealTargetPlatform.Android ||
            Target.Platform == UnrealTargetPlatform.IOS)
        {
            // Mobile-specific Online Subsystem (Google Play / Game Center)
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "OnlineSubsystemGooglePlay",  // Android
                "OnlineSubsystemIOS",          // iOS
            });
        }

        // ───────────────────────────────────────────────────────────────────────────
        // C++ standard and optimization flags
        // ───────────────────────────────────────────────────────────────────────────
        CppStandard = CppStandardVersion.Cpp20;

        // Disable unity builds during development for faster incremental compile.
        // Re-enable (bUseUnity = true) for full shipping builds.
        bUseUnity = false;
    }
}
