// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PALTarget : TargetRules
{
	public PALTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		CppStandard = CppStandardVersion.Default;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "PAL" } );
	}
}
