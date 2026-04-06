#pragma once

#include "AssetDefinitionDefault.h"

#include "AssetDefinition_BlenderBridgeSource.generated.h"

UCLASS()
class UAssetDefinition_BlenderBridgeSource : public UAssetDefinitionDefault
{
    GENERATED_BODY()

public:
    virtual FText GetAssetDisplayName() const override;
    virtual FLinearColor GetAssetColor() const override;
    virtual TSoftClassPtr<UObject> GetAssetClass() const override;
    virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
};
