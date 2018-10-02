// Copyright 2017 Zhongqi Shan. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class PluginMapViewRuntime : ModuleRules
	{
        public PluginMapViewRuntime(TargetInfo Target)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[] {
                    "Core",
					"CoreUObject",
					"Engine",
					"RHI",
					"RenderCore",
					"ShaderCore"
				}
			);
		}
	}
}
