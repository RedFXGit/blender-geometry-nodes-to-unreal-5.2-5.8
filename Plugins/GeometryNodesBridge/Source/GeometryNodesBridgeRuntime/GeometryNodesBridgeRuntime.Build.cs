using System;
using UnrealBuildTool;

public class GeometryNodesBridgeRuntime : ModuleRules
{
    public GeometryNodesBridgeRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;

        string buildVersion = $"{DateTime.UtcNow:yyyy-MM-dd HH:mm:ss} UTC | {Target.Name} {Target.Platform} {Target.Configuration}";
        PublicDefinitions.Add($"GEOMETRY_NODES_BRIDGE_BUILD_VERSION=TEXT(\"{EscapeDefinitionString(buildVersion)}\")");

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "DeveloperSettings",
                "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "Json",
                "JsonUtilities",
                "ProceduralMeshComponent",
                "Projects",
                "RenderCore",
                "RHI"
            }
        );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDefinitions.Add("GEOMETRY_NODES_BRIDGE_WITH_WINDOWS_IPC=1");
        }
        else
        {
            PublicDefinitions.Add("GEOMETRY_NODES_BRIDGE_WITH_WINDOWS_IPC=0");
        }
    }

    private static string EscapeDefinitionString(string value)
    {
        return value.Replace("\\", "\\\\").Replace("\"", "\\\"");
    }
}
