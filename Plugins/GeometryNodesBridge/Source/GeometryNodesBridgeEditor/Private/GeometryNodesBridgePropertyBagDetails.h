#pragma once

#include "PropertyBagDetails.h"

class FGeometryNodesBridgePropertyBagDetails final : public FPropertyBagDetails
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
    virtual void CustomizeHeader(
        TSharedRef<IPropertyHandle> StructPropertyHandle,
        FDetailWidgetRow& HeaderRow,
        IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

    virtual void CustomizeChildren(
        TSharedRef<IPropertyHandle> StructPropertyHandle,
        IDetailChildrenBuilder& StructBuilder,
        IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};
