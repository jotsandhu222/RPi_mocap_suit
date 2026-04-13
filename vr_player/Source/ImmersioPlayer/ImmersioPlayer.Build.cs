using UnrealBuildTool;

public class ImmersioPlayer : ModuleRules
{
	public ImmersioPlayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Will add Plugin dependencies later once they are created.
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreMedia",
				"VRRendering",
				"UI",
				"Network"
			}
		);
	}
}
