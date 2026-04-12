#include "GeometryNodesBridgeSource.h"

#include "Misc/Paths.h"
#include "UObject/AssetRegistryTagsContext.h"

#if WITH_EDITORONLY_DATA
#include "EditorFramework/AssetImportData.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GeometryNodesBridgeSource)

void UGeometryNodesBridgeSource::SetBlendFilePath(const FString& InBlendFilePath)
{
    BlendFile.FilePath = FPaths::ConvertRelativePathToFull(InBlendFilePath);

#if WITH_EDITORONLY_DATA
    if (AssetImportData == nullptr)
    {
        AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
    }

    if (AssetImportData != nullptr)
    {
        AssetImportData->UpdateFilenameOnly(BlendFile.FilePath);
    }
#endif
}

void UGeometryNodesBridgeSource::PostInitProperties()
{
#if WITH_EDITORONLY_DATA
    if (!HasAnyFlags(RF_ClassDefaultObject) && AssetImportData == nullptr)
    {
        AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
    }
#endif

    Super::PostInitProperties();
}

void UGeometryNodesBridgeSource::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
#if WITH_EDITORONLY_DATA
    if (AssetImportData != nullptr)
    {
        Context.AddTag(FAssetRegistryTag(SourceFileTagName(), AssetImportData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden));
    }
#endif

    Super::GetAssetRegistryTags(Context);
}
