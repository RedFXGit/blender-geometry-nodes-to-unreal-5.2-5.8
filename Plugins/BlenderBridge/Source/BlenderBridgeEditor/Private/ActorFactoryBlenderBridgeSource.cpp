#include "ActorFactoryBlenderBridgeSource.h"

#include "AssetRegistry/AssetData.h"
#include "BlenderBridgeActor.h"
#include "BlenderBridgeComponent.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSource.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ActorFactoryBlenderBridgeSource)

#define LOCTEXT_NAMESPACE "ActorFactoryBlenderBridgeSource"

UActorFactoryBlenderBridgeSource::UActorFactoryBlenderBridgeSource(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    DisplayName = LOCTEXT("DisplayName", "Blender Bridge Actor");
    NewActorClass = ABlenderBridgeActor::StaticClass();
}

bool UActorFactoryBlenderBridgeSource::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
    if (!AssetData.IsValid() || !AssetData.IsInstanceOf(UBlenderBridgeSource::StaticClass()))
    {
        OutErrorMsg = LOCTEXT("InvalidSource", "A valid BlenderBridgeSource asset must be specified.");
        return false;
    }

    return true;
}

void UActorFactoryBlenderBridgeSource::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
    Super::PostSpawnActor(Asset, NewActor);

    UBlenderBridgeSource* SourceAsset = Cast<UBlenderBridgeSource>(Asset);
    ABlenderBridgeActor* BridgeActor = Cast<ABlenderBridgeActor>(NewActor);
    if (SourceAsset == nullptr || BridgeActor == nullptr)
    {
        return;
    }

    if (UBlenderBridgeComponent* BridgeComponent = BridgeActor->GetBridgeComponent())
    {
        BridgeComponent->Source = SourceAsset;
        auto& SessionManager = FBlenderBridgeRuntimeModule::Get().GetSessionManager();
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

UObject* UActorFactoryBlenderBridgeSource::GetAssetFromActorInstance(AActor* ActorInstance)
{
    const ABlenderBridgeActor* BridgeActor = Cast<ABlenderBridgeActor>(ActorInstance);
    if (BridgeActor == nullptr)
    {
        return nullptr;
    }

    if (const UBlenderBridgeComponent* BridgeComponent = BridgeActor->GetBridgeComponent())
    {
        return BridgeComponent->Source;
    }

    return nullptr;
}

#undef LOCTEXT_NAMESPACE
