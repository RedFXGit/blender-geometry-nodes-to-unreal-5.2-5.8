#include "GeometryNodesBridgeSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GeometryNodesBridgeSettings)

UGeometryNodesBridgeSettings::UGeometryNodesBridgeSettings()
{
    CategoryName = TEXT("Plugins");
    SectionName = TEXT("Geometry Nodes Bridge");
}

FName UGeometryNodesBridgeSettings::GetCategoryName() const
{
    return TEXT("Plugins");
}
