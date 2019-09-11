// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PluxController : ModuleRules
{
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
    }

    public PluxController(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"PluxController/Public",
                 Path.Combine(ModuleDirectory, "../../ThirdParty/BiosignalPlux", "include"),
            }
			);  
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"PluxController/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
                "SlateCore",
                "Slate",
                "InputCore",
                "RenderCore",
                "RHI",
                "UMG"
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "InputDevice",      // Provides IInputInterface
                "Slate",
                "SlateCore"
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
            }
			);

        if(Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Add the import library
            //Import Plux library
            PublicLibraryPaths.Add(Path.Combine(ModuleDirectory, "../../ThirdParty/BiosignalPlux", "lib"));
            PublicAdditionalLibraries.Add("plux.lib");


            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("LibFT4222-64.dll");
            PublicDelayLoadDLLs.Add("ftd2xx.dll");
        }
	}
}
