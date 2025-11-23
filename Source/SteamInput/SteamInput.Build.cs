using UnrealBuildTool;

public class SteamInput : ModuleRules
{
    public SteamInput(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "InputDevice",
                "CoreUObject",
                
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