#include "ActorFactoryGeometryNodesBridgeSource.h"

#include "AssetRegistry/AssetData.h"
#include "GeometryNodesBridgeActor.h"
#include "GeometryNodesBridgeComponent.h"
#include "GeometryNodesBridgeRuntimeModule.h"
#include "GeometryNodesBridgeSource.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ActorFactoryGeometryNodesBridgeSource)

#define LOCTEXT_NAMESPACE "ActorFactoryGeometryNodesBridgeSource"

UActorFactoryGeometryNodesBridgeSource::UActorFactoryGeometryNodesBridgeSource(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    DisplayName = LOCTEXT("DisplayName", "Geometry Nodes Bridge Actor");
    NewActorClass = AGeometryNodesBridgeActor::StaticClass();
}

bool UActorFactoryGeometryNodesBridgeSource::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
    if (!AssetData.IsValid() || !AssetData.IsInstanceOf(UGeometryNodesBridgeSource::StaticClass()))
    {
        OutErrorMsg = LOCTEXT("InvalidSource", "A valid GeometryNodesBridgeSource asset must be specified.");
        return false;
    }

    return true;
}

bool UActorFactoryGeometryNodesBridgeSource::PreSpawnActor(UObject* Asset, FTransform& InOutLocation)
{
    return Cast<UGeometryNodesBridgeSource>(Asset) != nullptr;
}

void UActorFactoryGeometryNodesBridgeSource::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
    UGeometryNodesBridgeSource* SourceAsset = Cast<UGeometryNodesBridgeSource>(Asset);
    AGeometryNodesBridgeActor* BridgeActor = Cast<AGeometryNodesBridgeActor>(NewActor);
    if (SourceAsset == nullptr || BridgeActor == nullptr)
    {
        return;
    }

    if (UGeometryNodesBridgeComponent* BridgeComponent = BridgeActor->GetBridgeComponent())
    {
        if (BridgeComponent->Source == SourceAsset)
        {
            return;
        }

        BridgeComponent->AssignSourceFromActorFactory(SourceAsset);

        FGeometryNodesBridgeSessionManager& SessionManager = FGeometryNodesBridgeRuntimeModule::Get().GetSessionManager();
        if (BridgeActor->bIsEditorPreviewActor)
        {
            SessionManager.WarmSourceOnPreviewDrag(SourceAsset, BridgeComponent);
            return;
        }

        if (SessionManager.IsSessionConnected())
        {
            BridgeComponent->RequestCook();
        }
        else
        {
            SessionManager.NotifySourceChanged(BridgeComponent);
        }
    }
}

UObject* UActorFactoryGeometryNodesBridgeSource::GetAssetFromActorInstance(AActor* ActorInstance)
{
    const AGeometryNodesBridgeActor* BridgeActor = Cast<AGeometryNodesBridgeActor>(ActorInstance);
    if (BridgeActor == nullptr)
    {
        return nullptr;
    }

    if (const UGeometryNodesBridgeComponent* BridgeComponent = BridgeActor->GetBridgeComponent())
    {
        return BridgeComponent->Source;
    }

    return nullptr;
}

#undef LOCTEXT_NAMESPACE
