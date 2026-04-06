#include "BlenderBridgeActorDetails.h"

#include "BlenderBridgeActor.h"
#include "BlenderBridgeComponent.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"

TSharedRef<IDetailCustomization> FBlenderBridgeActorDetails::MakeInstance()
{
    return MakeShared<FBlenderBridgeActorDetails>();
}

void FBlenderBridgeActorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
    DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

    SelectedActors.Reset();
    for (const TWeakObjectPtr<UObject>& CustomizedObject : CustomizedObjects)
    {
        if (ABlenderBridgeActor* Actor = Cast<ABlenderBridgeActor>(CustomizedObject.Get()))
        {
            SelectedActors.Add(Actor);
        }
    }

    IDetailCategoryBuilder& ActionsCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Actions"),
        FText::FromString(TEXT("Blender Bridge Actions")),
        ECategoryPriority::Important);
    ActionsCategory.SetSortOrder(-100);

    ActionsCategory.AddCustomRow(FText::FromString(TEXT("Bridge Actions")))
    .WholeRowContent()
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(0.0f, 0.0f, 6.0f, 0.0f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Refresh Input Schema")))
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleRefreshInputSchemaClicked)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Request Cook")))
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleRequestCookClicked)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(6.0f, 0.0f, 0.0f, 0.0f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Bake To Static Mesh")))
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleBakeToStaticMeshClicked)
        ]
    ];

    ActionsCategory.AddCustomRow(FText::FromString(TEXT("Baked Output Actions")))
    .WholeRowContent()
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(0.0f, 0.0f, 6.0f, 0.0f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Select Baked Output")))
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleSelectBakedOutputClicked)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Delete Baked Output")))
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleDeleteBakedOutputClicked)
        ]
    ];
}

FReply FBlenderBridgeActorDetails::HandleRefreshInputSchemaClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->RefreshInputSchema();
            }
        }
    }

    return FReply::Handled();
}

FReply FBlenderBridgeActorDetails::HandleRequestCookClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->RequestCook();
            }
        }
    }

    return FReply::Handled();
}

FReply FBlenderBridgeActorDetails::HandleBakeToStaticMeshClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->BakeToStaticMesh();
            }
        }
    }

    return FReply::Handled();
}

FReply FBlenderBridgeActorDetails::HandleSelectBakedOutputClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->SelectBakedOutput();
            }
        }
    }

    return FReply::Handled();
}

FReply FBlenderBridgeActorDetails::HandleDeleteBakedOutputClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->DeleteBakedOutput();
            }
        }
    }

    return FReply::Handled();
}
