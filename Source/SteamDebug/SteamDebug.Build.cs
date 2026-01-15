// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

using UnrealBuildTool;

public class SteamDebug : ModuleRules
{
    public SteamDebug(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "SteamInput", 
                "EnhancedInput"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "ToolMenus",
                "InputCore"
            }
        );
        
        AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
    }
}