// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HynmersGame : ModuleRules
{
	public HynmersGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "OculusHMD" });

        PrivateDependencyModuleNames.AddRange(new string[] {"Engine", "OculusHMD", "KinectInput" });

        PublicIncludePaths.AddRange(
            new string[] {"OculusHMD", "KinectInput/Public" });

    }
}
