#pragma once

#include "PropertyBagDetails.h"

class FBlenderBridgePropertyBagDetails final : public FPropertyBagDetails
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

private:
    static bool ShouldUseGroupedLayout(const TSharedRef<IPropertyHandle>& StructPropertyHandle);
};
