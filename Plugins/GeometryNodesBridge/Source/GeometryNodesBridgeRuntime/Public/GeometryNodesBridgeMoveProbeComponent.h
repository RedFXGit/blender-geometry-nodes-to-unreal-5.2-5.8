#pragma once

#include "Components/SceneComponent.h"

#include "GeometryNodesBridgeMoveProbeComponent.generated.h"

class UGeometryNodesBridgeComponent;

UCLASS(ClassGroup = (Blender), NotBlueprintable, NotBlueprintType, Transient)
class GEOMETRYNODESBRIDGERUNTIME_API UGeometryNodesBridgeMoveProbeComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UGeometryNodesBridgeMoveProbeComponent();

    void InitializeForObservedComponent(USceneComponent* InObservedComponent);
    void AddWatchingBridge(UGeometryNodesBridgeComponent* BridgeComponent);
    void RemoveWatchingBridge(UGeometryNodesBridgeComponent* BridgeComponent);
    bool HasWatchingBridges();
    bool MatchesObservedComponent(const USceneComponent* InObservedComponent) const;
    USceneComponent* GetObservedComponent() const { return ObservedComponent.Get(); }

protected:
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
    virtual void OnRegister() override;
    virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport = ETeleportType::None) override;

private:
    void NotifyWatchingBridges(const FTransform& PreviousTransform, const FTransform& CurrentTransform, ETeleportType Teleport);
    void PruneWatchingBridges();

    TWeakObjectPtr<USceneComponent> ObservedComponent;
    TArray<TWeakObjectPtr<UGeometryNodesBridgeComponent>> WatchingBridges;
    bool bHasCachedTransform = false;
    FTransform CachedComponentTransform = FTransform::Identity;
};
