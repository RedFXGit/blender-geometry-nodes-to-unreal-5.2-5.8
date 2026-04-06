#include "BlenderBridgeEditorStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateStyleMacros.h"

FBlenderBridgeEditorStyle& FBlenderBridgeEditorStyle::Get()
{
    static FBlenderBridgeEditorStyle Style;
    return Style;
}

FBlenderBridgeEditorStyle::FBlenderBridgeEditorStyle()
    : FSlateStyleSet(TEXT("BlenderBridgeEditorStyle"))
{
    const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("BlenderBridge"));
    const FString ContentRoot = Plugin.IsValid()
        ? (Plugin->GetBaseDir() / TEXT("Resources"))
        : FPaths::ProjectPluginsDir() / TEXT("BlenderBridge/Resources");

    SetParentStyleName(FAppStyle::GetAppStyleSetName());
    SetContentRoot(ContentRoot);
    SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

    const FVector2D Icon16x16(16.0f, 16.0f);
    const FVector2D Icon64x64(64.0f, 64.0f);
    constexpr TCHAR BlenderBadgeIconName[] = TEXT("blender_community_badge_white");

    Set(TEXT("ClassIcon.BlenderBridgeSource"), new IMAGE_BRUSH_SVG(BlenderBadgeIconName, Icon16x16));
    Set(TEXT("ClassThumbnail.BlenderBridgeSource"), new IMAGE_BRUSH_SVG(BlenderBadgeIconName, Icon64x64));
}

FBlenderBridgeEditorStyle::~FBlenderBridgeEditorStyle()
{
}
