#include "BlenderBridgeSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlenderBridgeSettings)

UBlenderBridgeSettings::UBlenderBridgeSettings()
{
    CategoryName = TEXT("Plugins");
    SectionName = TEXT("Blender Bridge");
}

FName UBlenderBridgeSettings::GetCategoryName() const
{
    return TEXT("Plugins");
}
