using UnrealBuildTool;

public class SteamDebug : ModuleRules
{
    public SteamDebug(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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