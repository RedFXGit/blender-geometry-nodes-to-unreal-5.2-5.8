#pragma once

#include "Styling/SlateStyle.h"

class FGeometryNodesBridgeEditorStyle final : public FSlateStyleSet
{
public:
    static FGeometryNodesBridgeEditorStyle& Get();

    FGeometryNodesBridgeEditorStyle();
    virtual ~FGeometryNodesBridgeEditorStyle() override;
};
