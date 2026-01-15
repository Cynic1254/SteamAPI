// Copyright 2026 Cynic. All Rights Reserved.
// Licensed under the Apache License, Version 2.0.
// See LICENSE file in the project root for full license information.

using UnrealBuildTool;

public class SteamCore : ModuleRules
{
    public SteamCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "SteamShared",
                "Engine"
            }
        );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",
                "Settings"
            });
        }
        
        AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
    }
}