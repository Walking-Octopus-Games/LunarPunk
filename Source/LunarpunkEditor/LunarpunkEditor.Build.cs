// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class LunarpunkEditor : ModuleRules
{
	public LunarpunkEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Lunarpunk" });

		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "EditorStyle", "Slate", "SlateCore", "GraphEditor", "AppFramework", "PropertyEditor", "MainFrame" });

	}
}
