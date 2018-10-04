// Copyright 2017 Zhongqi Shan. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class PluginMapViewImporting : ModuleRules
    {
        public PluginMapViewImporting(TargetInfo Target)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "UnrealEd",
                    "XmlParser",
                    "AssetTools",
                    "Projects",
                    "Slate",
                    "EditorStyle",
                    "SlateCore",
                    "PropertyEditor",
                    "RenderCore",
                    "ShaderCore",
                    "RHI",
                    "RawMesh",
                    "AssetTools",
                    "AssetRegistry",
                    "PluginMapViewRuntime"
                }
            );
        }
    }
}
