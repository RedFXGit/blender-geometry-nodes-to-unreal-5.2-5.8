#include "BlenderBridgeMoveProbeComponent.h"

#include "BlenderBridgeComponent.h"
#include "GameFramework/Actor.h"

UBlenderBridgeMoveProbeComponent::UBlenderBridgeMoveProbeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bTickInEditor = false;
    bWantsOnUpdateTransform = true;
    SetIsVisualizationComponent(true);
    SetHiddenInGame(true);
    SetCanEverAffectNavigation(false);
}

void UBlenderBridgeMoveProbeComponent::InitializeForObservedComponent(USceneComponent* InObservedComponent)
{
    ObservedComponent = InObservedComponent;
    CachedComponentTransform = GetComponentTransform();
    bHasCachedTransform = true;
}

void UBlenderBridgeMoveProbeComponent::AddWatchingBridge(UBlenderBridgeComponent* BridgeComponent)
{
    if (BridgeComponent == nullptr)
    {
        return;
    }

    WatchingBridges.AddUnique(BridgeComponent);
}

void UBlenderBridgeMoveProbeComponent::RemoveWatchingBridge(UBlenderBridgeComponent* BridgeComponent)
{
    WatchingBridges.RemoveAll([BridgeComponent](const TWeakObjectPtr<UBlenderBridgeComponent>& WatchingBridge)
    {
        return WatchingBridge.Get() == nullptr || WatchingBridge.Get() == BridgeComponent;
    });
}

bool UBlenderBridgeMoveProbeComponent::HasWatchingBridges()
{
    PruneWatchingBridges();
    return !WatchingBridges.IsEmpty();
}

bool UBlenderBridgeMoveProbeComponent::MatchesObservedComponent(const USceneComponent* InObservedComponent) const
{
    return ObservedComponent.Get() == InObservedComponent;
}

void UBlenderBridgeMoveProbeComponent::OnComponentDestroyed(const bool bDestroyingHierarchy)
{
    WatchingBridges.Reset();
    ObservedComponent.Reset();
    bHasCachedTransform = false;
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UBlenderBridgeMoveProbeComponent::OnRegister()
{
    Super::OnRegister();
    CachedComponentTransform = GetComponentTransform();
    bHasCachedTransform = true;
}

void UBlenderBridgeMoveProbeComponent::OnUpdateTransform(const EUpdateTransformFlags UpdateTransformFlags, const ETeleportType Teleport)
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

void UBlenderBridgeMoveProbeComponent::NotifyWatchingBridges(
    const FTransform& PreviousTransform,
    const FTransform& CurrentTransform,
    const ETeleportType Teleport)
{
    for (int32 WatcherIndex = WatchingBridges.Num() - 1; WatcherIndex >= 0; --WatcherIndex)
    {
        UBlenderBridgeComponent* WatchingBridge = WatchingBridges[WatcherIndex].Get();
        if (WatchingBridge == nullptr)
        {
            WatchingBridges.RemoveAtSwap(WatcherIndex);
            continue;
        }

        WatchingBridge->HandleObservedMoveProbeTransformUpdated(this, PreviousTransform, CurrentTransform, Teleport);
    }
}

void UBlenderBridgeMoveProbeComponent::PruneWatchingBridges()
{
    WatchingBridges.RemoveAll([](const TWeakObjectPtr<UBlenderBridgeComponent>& WatchingBridge)
    {
        return !WatchingBridge.IsValid();
    });
}
