#include "GeometryNodesBridgeEditorStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateStyleMacros.h"

FGeometryNodesBridgeEditorStyle& FGeometryNodesBridgeEditorStyle::Get()
{
    static FGeometryNodesBridgeEditorStyle Style;
    return Style;
}

FGeometryNodesBridgeEditorStyle::FGeometryNodesBridgeEditorStyle()
    : FSlateStyleSet(TEXT("GeometryNodesBridgeEditorStyle"))
{
    const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("GeometryNodesBridge"));
    const FString ContentRoot = Plugin.IsValid()
        ? (Plugin->GetBaseDir() / TEXT("Resources"))
        : FPaths::ProjectPluginsDir() / TEXT("GeometryNodesBridge/Resources");

    SetParentStyleName(FAppStyle::GetAppStyleSetName());
    SetContentRoot(ContentRoot);
    SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

    const FVector2D Icon16x16(16.0f, 16.0f);
    const FVector2D Icon64x64(64.0f, 64.0f);
    constexpr TCHAR SourceAssetIconName[] = TEXT("icon");

    Set(TEXT("ClassIcon.GeometryNodesBridgeSource"), new IMAGE_BRUSH(SourceAssetIconName, Icon16x16));
    Set(TEXT("ClassThumbnail.GeometryNodesBridgeSource"), new IMAGE_BRUSH(SourceAssetIconName, Icon64x64));
}

FGeometryNodesBridgeEditorStyle::~FGeometryNodesBridgeEditorStyle()
{
}
