// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Crystalline : ModuleRules
{
	public Crystalline(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "Slate", "SlateCore" });
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		PublicDependencyModuleNames.AddRange(new string[] { 
		"OnlineSubsystem",
		"OnlineSubsystemUtils"
		});
	}
	
	
	
	
}
