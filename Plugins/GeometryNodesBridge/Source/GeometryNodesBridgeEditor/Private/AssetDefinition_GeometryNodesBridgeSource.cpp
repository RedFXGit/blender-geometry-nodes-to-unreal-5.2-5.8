#include "AssetDefinition_GeometryNodesBridgeSource.h"

#include "GeometryNodesBridgeSource.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_GeometryNodesBridgeSource)

#define LOCTEXT_NAMESPACE "AssetDefinition_GeometryNodesBridgeSource"

FText UAssetDefinition_GeometryNodesBridgeSource::GetAssetDisplayName() const
{
    return LOCTEXT("AssetDisplayName", "Geometry Nodes Bridge Source");
}

FLinearColor UAssetDefinition_GeometryNodesBridgeSource::GetAssetColor() const
{
    return FLinearColor(FColor(242, 131, 31));
}

TSoftClassPtr<UObject> UAssetDefinition_GeometryNodesBridgeSource::GetAssetClass() const
{
    return UGeometryNodesBridgeSource::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_GeometryNodesBridgeSource::GetAssetCategories() const
{
    static const auto Categories = { EAssetCategoryPaths::Misc / LOCTEXT("GeometryNodesBridgeCategory", "Geometry Nodes Bridge") };
    return Categories;
}

#undef LOCTEXT_NAMESPACE
