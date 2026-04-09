#pragma once

#include "BlenderBridgeProtocol.h"
#include "Engine/DeveloperSettings.h"
#include "BlenderBridgeSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, DefaultConfig, meta = (DisplayName = "Blender Bridge"))
class BLENDERBRIDGERUNTIME_API UBlenderBridgeSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UBlenderBridgeSettings();

    virtual FName GetCategoryName() const override;

    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Session",
        meta = (
            DisplayName = "Automatically Start Session On Editor Launch",
            ToolTip = "When enabled, Blender Bridge will attempt to launch the global Blender session after the Unreal Editor finishes starting."
        ))
    bool bAutoStartSessionOnEditorLaunch = true;

    UPROPERTY(EditAnywhere, Config, Category = "Session")
    FFilePath BlenderExecutablePath;

    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Session",
        meta = (
            ClampMin = "1048576",
            DisplayName = "Shared Memory Bytes (Total)",
            ToolTip = "Total shared-memory budget across both bridge slots. The default is 250 MiB per slot, 500 MiB total."
        ))
    int64 SharedMemoryBytes = static_cast<int64>(BlenderBridge::DefaultSharedMemoryBytes);

    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Bake",
        meta = (
            DisplayName = "Bake Root Content Path",
            ToolTip = "Content root where baked Blender Bridge outputs, shared prototype meshes, and generated materials will be written."
        ))
    FString BakeRootContentPath = TEXT("/Game/BlenderBridge/Baked");

    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Automation",
        meta = (
            DisplayName = "Automation Fixture Root Directory",
            ToolTip = "Root folder for local Blender fixture files consumed by the automated performance loop."
        ))
    FDirectoryPath AutomationFixtureRootDirectory = { TEXT("C:/Users/Lassiter/Downloads/Blender_Geo") };

    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Automation",
        meta = (
            DisplayName = "Automation Perf Scratch Bake Root",
            ToolTip = "Content root reserved for automated performance bake runs so the loop does not pollute normal baked output content."
        ))
    FString AutomationPerfScratchBakeRoot = TEXT("/Game/BlenderBridge/PerfScratch");

    UPROPERTY(
        EditAnywhere,
        Config,
        Category = "Diagnostics",
        meta = (
            DisplayName = "Timing History Output Directory",
            ToolTip = "Directory where Blender Bridge appends the timing history CSV when blenderbridge.LogCookTimings is enabled. Leave empty to write to the Unreal project root."
        ))
    FDirectoryPath TimingHistoryOutputDirectory;
};
