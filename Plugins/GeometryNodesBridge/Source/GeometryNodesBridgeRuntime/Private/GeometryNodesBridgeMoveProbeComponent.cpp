#include "GeometryNodesBridgeMoveProbeComponent.h"

#include "GeometryNodesBridgeComponent.h"
#include "GameFramework/Actor.h"

UGeometryNodesBridgeMoveProbeComponent::UGeometryNodesBridgeMoveProbeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bTickInEditor = false;
    bWantsOnUpdateTransform = true;
    SetIsVisualizationComponent(true);
    SetHiddenInGame(true);
    SetCanEverAffectNavigation(false);
}

void UGeometryNodesBridgeMoveProbeComponent::InitializeForObservedComponent(USceneComponent* InObservedComponent)
{
    ObservedComponent = InObservedComponent;
    CachedComponentTransform = GetComponentTransform();
    bHasCachedTransform = true;
}

void UGeometryNodesBridgeMoveProbeComponent::AddWatchingBridge(UGeometryNodesBridgeComponent* BridgeComponent)
{
    if (BridgeComponent == nullptr)
    {
        return;
    }

    WatchingBridges.AddUnique(BridgeComponent);
}

void UGeometryNodesBridgeMoveProbeComponent::RemoveWatchingBridge(UGeometryNodesBridgeComponent* BridgeComponent)
{
    WatchingBridges.RemoveAll([BridgeComponent](const TWeakObjectPtr<UGeometryNodesBridgeComponent>& WatchingBridge)
    {
        return WatchingBridge.Get() == nullptr || WatchingBridge.Get() == BridgeComponent;
    });
}

bool UGeometryNodesBridgeMoveProbeComponent::HasWatchingBridges()
{
    PruneWatchingBridges();
    return !WatchingBridges.IsEmpty();
}

bool UGeometryNodesBridgeMoveProbeComponent::MatchesObservedComponent(const USceneComponent* InObservedComponent) const
{
    return ObservedComponent.Get() == InObservedComponent;
}

void UGeometryNodesBridgeMoveProbeComponent::OnComponentDestroyed(const bool bDestroyingHierarchy)
{
    WatchingBridges.Reset();
    ObservedComponent.Reset();
    bHasCachedTransform = false;
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UGeometryNodesBridgeMoveProbeComponent::OnRegister()
{
    Super::OnRegister();
    CachedComponentTransform = GetComponentTransform();
    bHasCachedTransform = true;
}

void UGeometryNodesBridgeMoveProbeComponent::OnUpdateTransform(const EUpdateTransformFlags UpdateTransformFlags, const ETeleportType Teleport)
{
    Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

    const FTransform CurrentTransform = GetComponentTransform();
    if (!bHasCachedTransform)
    {
        CachedComponentTransform = CurrentTransform;
        bHasCachedTransform = true;
        return;
    }

    if (CachedComponentTransform.Equals(CurrentTransform, KINDA_SMALL_NUMBER))
    {
        return;
    }

    const FTransform PreviousTransform = CachedComponentTransform;
    CachedComponentTransform = CurrentTransform;
    NotifyWatchingBridges(PreviousTransform, CurrentTransform, Teleport);
}

void UGeometryNodesBridgeMoveProbeComponent::NotifyWatchingBridges(
    const FTransform& PreviousTransform,
    const FTransform& CurrentTransform,
    const ETeleportType Teleport)
{
    for (int32 WatcherIndex = WatchingBridges.Num() - 1; WatcherIndex >= 0; --WatcherIndex)
    {
        UGeometryNodesBridgeComponent* WatchingBridge = WatchingBridges[WatcherIndex].Get();
        if (WatchingBridge == nullptr)
        {
            WatchingBridges.RemoveAtSwap(WatcherIndex);
            continue;
        }

        WatchingBridge->HandleObservedMoveProbeTransformUpdated(this, PreviousTransform, CurrentTransform, Teleport);
    }
}

void UGeometryNodesBridgeMoveProbeComponent::PruneWatchingBridges()
{
    WatchingBridges.RemoveAll([](const TWeakObjectPtr<UGeometryNodesBridgeComponent>& WatchingBridge)
    {
        return !WatchingBridge.IsValid();
    });
}
