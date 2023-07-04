// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Lunarpunk : ModuleRules
{
	public Lunarpunk(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
            "Core"
            , "CoreUObject"
            , "Engine"
            , "InputCore"
            , "UMG"
            , "Niagara" 
            , "NavigationSystem"
            , "ApexDestruction"
            , "MediaAssets"
            , "MovieScene"
            , "LevelSequence"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { 
            "Json" ,
            "FMODStudio"
        });

        PublicIncludePaths.AddRange(
        new string[] {
            "Lunarpunk"
        }
        );

    }
}
