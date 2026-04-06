#include "AssetDefinition_BlenderBridgeSource.h"

#include "BlenderBridgeSource.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_BlenderBridgeSource)

#define LOCTEXT_NAMESPACE "AssetDefinition_BlenderBridgeSource"

FText UAssetDefinition_BlenderBridgeSource::GetAssetDisplayName() const
{
    return LOCTEXT("AssetDisplayName", "Blender Bridge Source");
}

FLinearColor UAssetDefinition_BlenderBridgeSource::GetAssetColor() const
{
    return FLinearColor(FColor(242, 131, 31));
}

TSoftClassPtr<UObject> UAssetDefinition_BlenderBridgeSource::GetAssetClass() const
{
    return UBlenderBridgeSource::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_BlenderBridgeSource::GetAssetCategories() const
{
    static const auto Categories = { EAssetCategoryPaths::Misc / LOCTEXT("BlenderBridgeCategory", "Blender Bridge") };
    return Categories;
}

#undef LOCTEXT_NAMESPACE
