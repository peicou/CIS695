// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CIS695_vehicle : ModuleRules
{
	public CIS695_vehicle(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Sockets",
            "Networking",
            "HeadMountedDisplay" });
	}
}
