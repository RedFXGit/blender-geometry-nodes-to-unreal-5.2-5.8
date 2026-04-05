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
                "BlenderBridgeRuntime",
                "PropertyEditor",
                "Slate",
                "SlateCore",
                "StructUtilsEditor",
                "UnrealEd"
            }
        );
    }
}
