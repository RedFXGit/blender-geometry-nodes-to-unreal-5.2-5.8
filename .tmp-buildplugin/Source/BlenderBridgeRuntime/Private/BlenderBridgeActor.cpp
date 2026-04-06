#include "BlenderBridgeActor.h"

#include "BlenderBridgeComponent.h"

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
