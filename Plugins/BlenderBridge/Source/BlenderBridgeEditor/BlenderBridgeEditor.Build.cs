using UnrealBuildTool;

public class BlenderBridgeEditor : ModuleRules
{
    public BlenderBridgeEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "AssetRegistry",
                "AssetTools",
                "AssetDefinition",
                "BlenderBridgeRuntime",
                "MaterialEditor",
                "MeshDescription",
                "MeshConversion",
                "PropertyEditor",
                "Projects",
                "RenderCore",
                "Slate",
                "SlateCore",
                "StaticMeshDescription",
                "StructUtilsEditor",
                "ToolMenus",
                "UnrealEd"
            }
        );
    }
}
