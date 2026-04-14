#include "GeometryNodesBridgePropertyBagDetails.h"

#include "GeometryNodesBridgePropertyBagBuilder.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"

TSharedRef<IPropertyTypeCustomization> FGeometryNodesBridgePropertyBagDetails::MakeInstance()
{
    return MakeShared<FGeometryNodesBridgePropertyBagDetails>();
}

void FGeometryNodesBridgePropertyBagDetails::CustomizeHeader(
    TSharedRef<IPropertyHandle> StructPropertyHandle,
    FDetailWidgetRow& HeaderRow,
    IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    FPropertyBagDetails::CustomizeHeader(StructPropertyHandle, HeaderRow, StructCustomizationUtils);
}

void FGeometryNodesBridgePropertyBagDetails::CustomizeChildren(
    TSharedRef<IPropertyHandle> StructPropertyHandle,
    IDetailChildrenBuilder& StructBuilder,
    IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    if (!GeometryNodesBridge::PropertyBagBuilder::ShouldUseGroupedLayout(StructPropertyHandle))
    {
        FPropertyBagDetails::CustomizeChildren(StructPropertyHandle, StructBuilder, StructCustomizationUtils);
        return;
    }

    GeometryNodesBridge::PropertyBagBuilder::AddGroupedPropertyBagToChildren(
        StructPropertyHandle,
        StructBuilder,
        StructCustomizationUtils.GetPropertyUtilities());
}
