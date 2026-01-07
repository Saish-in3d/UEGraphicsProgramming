// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class UEGraphicsProgramming : ModuleRules
{
	public UEGraphicsProgramming(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"RenderCore",
			"Renderer",
			"RHI",
			"Projects",
			"MediaAssets",
		});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		
		string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);

		PrivateIncludePaths.AddRange(
			new string[] {
				// Including to Find shader utilities path
				Path.Combine(EngineDir, "Source/Runtime/Renderer/Private"),
				Path.Combine(EngineDir, "Source/Runtime/Renderer/Internal")
			}
		);
	}
}