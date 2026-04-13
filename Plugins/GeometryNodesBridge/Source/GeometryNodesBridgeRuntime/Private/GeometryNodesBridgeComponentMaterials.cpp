#include "GeometryNodesBridgeComponent.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"

void UGeometryNodesBridgeComponent::SyncMaterialOverridesFromCookedSlots()
{
    TMap<FString, TObjectPtr<UMaterialInterface>> ExistingOverrides;
    for (const FGeometryNodesBridgeMaterialOverride& Entry : MaterialOverrides)
    {
        if (!Entry.SlotId.IsEmpty())
        {
            ExistingOverrides.Add(Entry.SlotId, Entry.Material);
        }
    }

    TArray<FGeometryNodesBridgeMaterialOverride> UpdatedOverrides;
    UpdatedOverrides.Reserve(LastCookedMaterialSlots.Num());
    for (const FBridgeMaterialSlotDescriptor& Slot : LastCookedMaterialSlots)
    {
        FGeometryNodesBridgeMaterialOverride OverrideEntry;
        OverrideEntry.SlotId = Slot.SlotId;
        OverrideEntry.SlotName = Slot.SlotName;
        OverrideEntry.BlenderMaterialName = Slot.BlenderMaterialName;
        if (const TObjectPtr<UMaterialInterface>* ExistingMaterial = ExistingOverrides.Find(Slot.SlotId))
        {
            OverrideEntry.Material = *ExistingMaterial;
        }

        UpdatedOverrides.Add(MoveTemp(OverrideEntry));
    }

    MaterialOverrides = MoveTemp(UpdatedOverrides);
}

void UGeometryNodesBridgeComponent::QueueShowBakedOutputOnNextTick()
{
    CancelPendingBakedOutputVisibilitySwap();

    PendingShowBakedOutputTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateWeakLambda(
            this,
            [this](const float)
            {
                PendingShowBakedOutputTickerHandle.Reset();
                SetShowingBakedOutput(true);
                return false;
            }),
        0.0f);
}

void UGeometryNodesBridgeComponent::CancelPendingBakedOutputVisibilitySwap()
{
    if (PendingShowBakedOutputTickerHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(PendingShowBakedOutputTickerHandle);
        PendingShowBakedOutputTickerHandle.Reset();
    }
}

void UGeometryNodesBridgeComponent::SetShowingBakedOutput(const bool bInShowingBakedOutput)
{
    if (!bInShowingBakedOutput)
    {
        CancelPendingBakedOutputVisibilitySwap();
    }

    bShowingBakedOutput = bInShowingBakedOutput;
    UpdatePreviewVisibility();
    UpdateBakedOutputVisibility();
}

void UGeometryNodesBridgeComponent::UpdatePreviewVisibility()
{
    if (PreviewMeshComponent == nullptr)
    {
        return;
    }

    const bool bShouldHidePreview = bHidePreviewAfterBake && bShowingBakedOutput && BakedActor.IsValid();
    PreviewMeshComponent->SetVisibility(!bShouldHidePreview);
    PreviewMeshComponent->SetHiddenInGame(bShouldHidePreview);
}

void UGeometryNodesBridgeComponent::UpdateBakedOutputVisibility()
{
    if (AActor* ExistingBakedActor = BakedActor.Get())
    {
        const bool bShouldShowBakedOutput = bShowingBakedOutput;
#if WITH_EDITOR
        ExistingBakedActor->SetIsTemporarilyHiddenInEditor(!bShouldShowBakedOutput);
#endif
        ExistingBakedActor->SetActorHiddenInGame(!bShouldShowBakedOutput);

        TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents(ExistingBakedActor);
        for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
        {
            if (PrimitiveComponent == nullptr)
            {
                continue;
            }

            PrimitiveComponent->SetVisibility(bShouldShowBakedOutput);
            PrimitiveComponent->SetHiddenInGame(!bShouldShowBakedOutput);
        }
    }
}

void UGeometryNodesBridgeComponent::ApplyPreviewMaterials()
{
    if (PreviewMeshComponent == nullptr)
    {
        return;
    }

    for (const FBridgeMeshSectionDescriptor& Section : LastCookedMesh.Sections)
    {
        PreviewMeshComponent->SetMaterial(Section.SectionIndex, FindOverrideMaterialBySlotId(Section.SlotId));
    }
}

UMaterialInterface* UGeometryNodesBridgeComponent::FindOverrideMaterialBySlotId(const FString& SlotId) const
{
    for (const FGeometryNodesBridgeMaterialOverride& Entry : MaterialOverrides)
    {
        if (Entry.SlotId == SlotId)
        {
            return Entry.Material;
        }
    }

    return nullptr;
}

bool UGeometryNodesBridgeComponent::HasCookedMesh() const
{
    return bHasValidCookedMesh && LastCookedMesh.Positions.Num() > 0 && LastCookedMesh.Triangles.Num() > 0;
}

bool UGeometryNodesBridgeComponent::HasBakedOutput() const
{
    return BakedActor.IsValid() || !BakedStaticMesh.IsNull() || !BakedPrototypeMeshes.IsEmpty();
}
