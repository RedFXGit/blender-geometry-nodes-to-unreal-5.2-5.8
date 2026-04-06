#pragma once

#include "Styling/SlateStyle.h"

class FBlenderBridgeEditorStyle final : public FSlateStyleSet
{
public:
    static FBlenderBridgeEditorStyle& Get();

    FBlenderBridgeEditorStyle();
    virtual ~FBlenderBridgeEditorStyle() override;
};
