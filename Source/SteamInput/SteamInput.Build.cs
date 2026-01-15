// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

using UnrealBuildTool;

public class SteamInput : ModuleRules
{
    public SteamInput(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "InputDevice",
                "CoreUObject",
                "UMG"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Engine",
                "SteamCore",
                "InputCore",
                "ApplicationCore",
                "Slate",
                "SlateCore",
                "SteamShared"
            }
        );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",
                "Settings",
                "EditorStyle"
            });
        }
        
        AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
    }
}