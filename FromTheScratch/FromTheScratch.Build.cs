// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FromTheScratch : ModuleRules
{
	public FromTheScratch(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "HairStrandsCore", "Niagara", "GeometryCollectionEngine", "UMG", "AIModule", "GameplayTags" });
	}
}
