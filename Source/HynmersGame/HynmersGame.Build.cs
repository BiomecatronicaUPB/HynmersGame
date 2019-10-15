// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HynmersGame : ModuleRules
{
	public HynmersGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "OculusHMD", "UMG", "SlateCore"});

        PrivateDependencyModuleNames.AddRange(new string[] {"OculusHMD" });

        PublicIncludePaths.AddRange(
            new string[] {"OculusHMD" });

    }
}
