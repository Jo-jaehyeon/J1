// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class J1 : ModuleRules
{
	public J1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", "CoreUObject", "Engine", 
            "InputCore", "EnhancedInput", 
            "UMG", "SlateCore", 
            "Sockets", "Networking" });

		PrivateDependencyModuleNames.AddRange(new string[] { "ThirdParty" });

        PrivateIncludePaths.AddRange(new string[]
{
            "J1/",
            "J1/Network/",
});

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
