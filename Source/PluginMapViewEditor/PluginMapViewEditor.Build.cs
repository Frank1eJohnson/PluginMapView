// Copyright 2017 Zhongqi Shan. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class PluginMapViewEditor : ModuleRules
    {
        public PluginMapViewEditor(TargetInfo Target)
        {
            PrivateIncludePaths.Add("PluginMapViewEditor/Private");
            PublicIncludePaths.Add("PluginMapViewEditor/Public");

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Slate",
                    "EditorStyle",
                    "SlateCore",
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "UnrealEd",
                    "PropertyEditor",
                    "RenderCore",
                    "ShaderCore",
                    "RHI",
                    "RawMesh",
                    "AssetTools",
                    "AssetRegistry",
                    "PluginMapViewRuntime",
                    "Projects"
                }
                );
        }
    }
}
