// Copyright (c) Soccer Mobile Pro. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SoccerMobileProEditorTarget : TargetRules
{
    public SoccerMobileProEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.Add("SoccerMobilePro");
    }
}
