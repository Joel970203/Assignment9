// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SampleChat : ModuleRules
{
	public SampleChat(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" , "SlateCore"});


        PrivateDependencyModuleNames.AddRange(new string[] {  });

    }
}
