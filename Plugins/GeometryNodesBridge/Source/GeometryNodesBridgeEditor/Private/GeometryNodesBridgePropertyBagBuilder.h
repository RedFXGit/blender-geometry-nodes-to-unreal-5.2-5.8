#pragma once

#include "CoreMinimal.h"

class IDetailCategoryBuilder;
class IDetailChildrenBuilder;
class IPropertyHandle;
class IPropertyUtilities;

namespace GeometryNodesBridge::PropertyBagBuilder
{
    bool ShouldUseGroupedLayout(const TSharedRef<IPropertyHandle>& StructPropertyHandle);

    void AddGroupedPropertyBagToChildren(
        const TSharedRef<IPropertyHandle>& StructPropertyHandle,
        IDetailChildrenBuilder& StructBuilder,
        const TSharedPtr<IPropertyUtilities>& PropertyUtilities);

    void AddGroupedPropertyBagToCategory(
        const TSharedRef<IPropertyHandle>& StructPropertyHandle,
        IDetailCategoryBuilder& CategoryBuilder,
        const TArray<TWeakObjectPtr<UObject>>& CustomizedObjects,
        const TSharedPtr<IPropertyUtilities>& PropertyUtilities);
}
