// Copyright (c) Soccer Mobile Pro. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SoccerMobileProTarget : TargetRules
{
    public SoccerMobileProTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.Add("SoccerMobilePro");
    }
}
