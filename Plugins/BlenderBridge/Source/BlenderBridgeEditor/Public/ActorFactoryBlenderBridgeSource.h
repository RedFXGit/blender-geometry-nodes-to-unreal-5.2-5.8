#pragma once

#include "ActorFactories/ActorFactory.h"

#include "ActorFactoryBlenderBridgeSource.generated.h"

UCLASS(Transient)
class BLENDERBRIDGEEDITOR_API UActorFactoryBlenderBridgeSource : public UActorFactory
{
    GENERATED_BODY()

public:
    UActorFactoryBlenderBridgeSource(const FObjectInitializer& ObjectInitializer);

    virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
    virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
    virtual UObject* GetAssetFromActorInstance(AActor* ActorInstance) override;
};
