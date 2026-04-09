#include "BlenderBridgeActor.h"

#include "BlenderBridgeComponent.h"
#include "Components/SplineComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BlenderBridgeActor)

ABlenderBridgeActor::ABlenderBridgeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    BridgeComponent = CreateDefaultSubobject<UBlenderBridgeComponent>(TEXT("BlenderBridgeComponent"));
    SetRootComponent(BridgeComponent);
}

void ABlenderBridgeActor::RefreshInputSchema()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->RefreshInputSchema();
    }
}

void ABlenderBridgeActor::RequestCook()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->RequestCook();
    }
}

void ABlenderBridgeActor::BakeToStaticMesh()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->BakeToStaticMesh();
    }
}

void ABlenderBridgeActor::SelectBakedOutput()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->SelectBakedOutput();
    }
}

void ABlenderBridgeActor::DeleteBakedOutput()
{
    if (BridgeComponent != nullptr)
    {
        BridgeComponent->DeleteBakedOutput();
    }
}

UBlenderBridgeComponent* ABlenderBridgeActor::GetBridgeComponent() const
{
    return BridgeComponent;
}

USplineComponent* ABlenderBridgeActor::GetBridgeSplineComponent() const
{
    return BridgeSplineComponent;
}

USplineComponent* ABlenderBridgeActor::EnsureBridgeSplineComponent()
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
    BridgeSplineComponent->ComponentTags.AddUnique(TEXT("BlenderBridgeOwnedSpline"));
    BridgeSplineComponent->RegisterComponent();
    return BridgeSplineComponent;
}

void ABlenderBridgeActor::RemoveBridgeSplineComponent()
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
