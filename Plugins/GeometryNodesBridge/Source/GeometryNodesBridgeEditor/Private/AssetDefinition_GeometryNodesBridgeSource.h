#pragma once

#include "AssetDefinitionDefault.h"

#include "AssetDefinition_GeometryNodesBridgeSource.generated.h"

UCLASS()
class UAssetDefinition_GeometryNodesBridgeSource : public UAssetDefinitionDefault
{
    GENERATED_BODY()

public:
    virtual FText GetAssetDisplayName() const override;
    virtual FLinearColor GetAssetColor() const override;
    virtual TSoftClassPtr<UObject> GetAssetClass() const override;
    virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
};
