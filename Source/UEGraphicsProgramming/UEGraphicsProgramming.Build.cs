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
			"GeometryCore",
			"MeshConversion",
			"GeometryFramework",
			"DynamicMesh",
			"GeometryScriptingCore",
			"MeshDescription",
			"StaticMeshDescription",
			
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
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../../Shaders"));
		//PrivateIncludePathModuleNames.Add("Shaders");

		
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