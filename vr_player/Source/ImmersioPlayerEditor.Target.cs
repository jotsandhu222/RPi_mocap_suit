using UnrealBuildTool;
using System.Collections.Generic;

public class ImmersioPlayerEditorTarget : TargetRules
{
	public ImmersioPlayerEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.AddRange( new string[] { "ImmersioPlayer" } );
	}
}
