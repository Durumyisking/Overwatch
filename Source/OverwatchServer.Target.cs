// Source/OverwatchServer.Target.cs
using UnrealBuildTool;
using System.Collections.Generic;

public class OverwatchServerTarget : TargetRules
{
	public OverwatchServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("Overwatch");
	}
}
