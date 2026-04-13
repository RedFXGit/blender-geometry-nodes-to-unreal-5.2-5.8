using UnrealBuildTool;

public class GeometryNodesBridgeEditor : ModuleRules
{
    public GeometryNodesBridgeEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "GeometryNodesBridgeRuntime",
                "Json",
                "JsonUtilities",
                "MaterialEditor",
                "MeshDescription",
                "MeshConversion",
                "ProceduralMeshComponent",
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
