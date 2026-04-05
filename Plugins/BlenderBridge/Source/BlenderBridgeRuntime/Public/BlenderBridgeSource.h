#pragma once

#include "Engine/DataAsset.h"

#include "BlenderBridgeSource.generated.h"

class UAssetImportData;
class FAssetRegistryTagsContext;

UCLASS(BlueprintType)
class BLENDERBRIDGERUNTIME_API UBlenderBridgeSource : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender", meta = (FilePathFilter = "blend"))
    FFilePath BlendFile;

#if WITH_EDITORONLY_DATA
    UPROPERTY(VisibleAnywhere, Instanced, Category = "Import Settings")
    TObjectPtr<UAssetImportData> AssetImportData;
#endif

    void SetBlendFilePath(const FString& InBlendFilePath);

    virtual void PostInitProperties() override;
    virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
};
