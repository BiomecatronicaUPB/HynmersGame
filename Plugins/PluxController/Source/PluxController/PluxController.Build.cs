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
            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "amd64" : "x86";
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "v1.8", "lib", PlatformString, "Kinect10.lib"));

            PublicIncludePaths.AddRange(
                new string[] {
                    Path.Combine(ThirdPartyPath, "v1.8/inc")
                });
    
        }
	}
}
