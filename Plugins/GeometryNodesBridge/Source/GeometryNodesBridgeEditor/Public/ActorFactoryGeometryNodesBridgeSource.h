#pragma once

#include "ActorFactories/ActorFactory.h"

#include "ActorFactoryGeometryNodesBridgeSource.generated.h"

UCLASS(Transient)
class GEOMETRYNODESBRIDGEEDITOR_API UActorFactoryGeometryNodesBridgeSource : public UActorFactory
{
    GENERATED_BODY()

public:
    UActorFactoryGeometryNodesBridgeSource(const FObjectInitializer& ObjectInitializer);

    virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
    virtual bool PreSpawnActor(UObject* Asset, FTransform& InOutLocation) override;
    virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
    virtual UObject* GetAssetFromActorInstance(AActor* ActorInstance) override;
};
