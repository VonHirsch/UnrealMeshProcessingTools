// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RuntimeGeometryDemo : ModuleRules
{
	public RuntimeGeometryDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//#pragma warning supress C4125 - Doesn't Work

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"HeadMountedDisplay",
				"ProceduralMeshComponent",
				"GeometricObjects",
				"DynamicMesh",
				"RuntimeGeometryUtils",
				"ModelingComponents",
				"Protobuf"
			});

		// Protobuf Plugin Settings
		bEnableUndefinedIdentifierWarnings = false;		// Enable warnings for using undefined identifiers in #if expressions
		bEnableExceptions = true;						// Enable exception handling

	}
}
