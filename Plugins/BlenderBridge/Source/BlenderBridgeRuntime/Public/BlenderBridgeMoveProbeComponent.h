#pragma once

#include "Components/SceneComponent.h"

#include "BlenderBridgeMoveProbeComponent.generated.h"

class UBlenderBridgeComponent;

UCLASS(ClassGroup = (Blender), NotBlueprintable, NotBlueprintType, Transient)
class BLENDERBRIDGERUNTIME_API UBlenderBridgeMoveProbeComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UBlenderBridgeMoveProbeComponent();

    void InitializeForObservedComponent(USceneComponent* InObservedComponent);
    void AddWatchingBridge(UBlenderBridgeComponent* BridgeComponent);
    void RemoveWatchingBridge(UBlenderBridgeComponent* BridgeComponent);
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
    TArray<TWeakObjectPtr<UBlenderBridgeComponent>> WatchingBridges;
    bool bHasCachedTransform = false;
    FTransform CachedComponentTransform = FTransform::Identity;
};
