#include "GeometryNodesBridgeSourceFactory.h"

#include "GeometryNodesBridgeSource.h"
#include "Editor.h"
#include "Misc/Paths.h"
#include "Subsystems/ImportSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GeometryNodesBridgeSourceFactory)

UGeometryNodesBridgeSourceFactory::UGeometryNodesBridgeSourceFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bCreateNew = false;
    bEditorImport = true;
    SupportedClass = UGeometryNodesBridgeSource::StaticClass();
    ImportPriority = DefaultImportPriority;
    Formats.Add(TEXT("blend;Blender Scene"));
}

bool UGeometryNodesBridgeSourceFactory::FactoryCanImport(const FString& Filename)
{
    return FPaths::GetExtension(Filename).Equals(TEXT("blend"), ESearchCase::IgnoreCase);
}

UObject* UGeometryNodesBridgeSourceFactory::FactoryCreateFile(
    UClass* InClass,
    UObject* InParent,
    FName InName,
    EObjectFlags Flags,
    const FString& Filename,
    const TCHAR* Parms,
    FFeedbackContext* Warn,
    bool& bOutOperationCanceled)
{
    GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(
        this,
        InClass,
        InParent,
        InName,
        *FPaths::GetExtension(Filename));

    UGeometryNodesBridgeSource* SourceAsset = NewObject<UGeometryNodesBridgeSource>(InParent, InClass, InName, Flags);
    if (SourceAsset == nullptr)
    {
        GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, nullptr);
        return nullptr;
    }

    SourceAsset->SetBlendFilePath(Filename);

    GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, SourceAsset);
    return SourceAsset;
}
