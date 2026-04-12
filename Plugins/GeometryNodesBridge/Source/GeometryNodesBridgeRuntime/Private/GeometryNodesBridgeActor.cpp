#include "GeometryNodesBridgeActor.h"

#include "GeometryNodesBridgeComponent.h"
#include "Components/SplineComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GeometryNodesBridgeActor)

AGeometryNodesBridgeActor::AGeometryNodesBridgeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    BridgeComponent = CreateDefaultSubobject<UGeometryNodesBridgeComponent>(TEXT("GeometryNodesBridgeComponent"));
    SetRootComponent(BridgeComponent);
}

void AGeometryNodesBridgeActor::RefreshInputSchema()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->RefreshInputSchema();
    }
}

void AGeometryNodesBridgeActor::RequestCook()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->RequestCook();
    }
}

void AGeometryNodesBridgeActor::BakeToStaticMesh()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->BakeToStaticMesh();
    }
}

void AGeometryNodesBridgeActor::SelectBakedOutput()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->SelectBakedOutput();
    }
}

void AGeometryNodesBridgeActor::DeleteBakedOutput()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->DeleteBakedOutput();
    }
}

UGeometryNodesBridgeComponent* AGeometryNodesBridgeActor::GetBridgeComponent() const
{
    return BridgeComponent;
}

USplineComponent* AGeometryNodesBridgeActor::GetBridgeSplineComponent() const
{
    return BridgeSplineComponent;
}

USplineComponent* AGeometryNodesBridgeActor::EnsureBridgeSplineComponent()
{
    if (BridgeSplineComponent != nullptr)
    {
        return BridgeSplineComponent;
    }

    TInlineComponentArray<USplineComponent*> ExistingSplineComponents(this);
    for (USplineComponent* ExistingSplineComponent : ExistingSplineComponents)
    {
        if (ExistingSplineComponent != nullptr && ExistingSplineComponent->GetFName() == TEXT("BridgeSplineComponent"))
        {
            BridgeSplineComponent = ExistingSplineComponent;
            return BridgeSplineComponent;
        }
    }

    BridgeSplineComponent = NewObject<USplineComponent>(this, TEXT("BridgeSplineComponent"), RF_Transactional);
    if (BridgeSplineComponent == nullptr)
    {
        return nullptr;
    }

    AddInstanceComponent(BridgeSplineComponent);
    BridgeSplineComponent->SetupAttachment(BridgeComponent);
    BridgeSplineComponent->SetMobility(EComponentMobility::Movable);
    BridgeSplineComponent->SetUsingAbsoluteLocation(false);
    BridgeSplineComponent->SetUsingAbsoluteRotation(false);
    BridgeSplineComponent->SetUsingAbsoluteScale(false);
    BridgeSplineComponent->ComponentTags.AddUnique(TEXT("GeometryNodesBridgeOwnedSpline"));
    BridgeSplineComponent->RegisterComponent();
    return BridgeSplineComponent;
}

void AGeometryNodesBridgeActor::RemoveBridgeSplineComponent()
{
    USplineComponent* SplineComponent = BridgeSplineComponent;
    if (SplineComponent == nullptr)
    {
        TInlineComponentArray<USplineComponent*> ExistingSplineComponents(this);
        for (USplineComponent* ExistingSplineComponent : ExistingSplineComponents)
        {
            if (ExistingSplineComponent != nullptr && ExistingSplineComponent->GetFName() == TEXT("BridgeSplineComponent"))
            {
                SplineComponent = ExistingSplineComponent;
                break;
            }
        }
    }

    BridgeSplineComponent = nullptr;
    if (SplineComponent == nullptr)
    {
        return;
    }

    RemoveInstanceComponent(SplineComponent);
    SplineComponent->DestroyComponent();
}
