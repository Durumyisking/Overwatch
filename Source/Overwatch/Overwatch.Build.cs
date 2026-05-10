// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Overwatch : ModuleRules
{
	public Overwatch(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[]
		{
			ModuleDirectory
		});
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"PhysicsCore",
			"InputCore", 
			"EnhancedInput", 
			"ModularGameplay",
			"GameFeatures",
			"ModularGameplayActors",
			"NetCore",

            "GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",

			"CommonUI",

			"CommonUser"

        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		SetupIrisSupport(Target);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
