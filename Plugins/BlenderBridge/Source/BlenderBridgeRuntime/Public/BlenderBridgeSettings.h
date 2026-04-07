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
            ToolTip = "Content root where baked Blender Bridge static meshes and generated materials will be written."
        ))
    FString BakeRootContentPath = TEXT("/Game/BlenderBridge/Baked");
};
