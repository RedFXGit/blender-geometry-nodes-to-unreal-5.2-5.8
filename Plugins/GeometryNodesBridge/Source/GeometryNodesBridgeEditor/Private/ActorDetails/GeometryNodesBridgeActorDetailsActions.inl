FReply FGeometryNodesBridgeActorDetails::HandleRefreshInputSchemaClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->RefreshInputSchema();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FGeometryNodesBridgeActorDetails::HandleRequestCookClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->RequestCook();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FGeometryNodesBridgeActorDetails::HandleBakeToStaticMeshClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->BakeToStaticMesh();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FGeometryNodesBridgeActorDetails::HandleSelectBakedOutputClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->SelectBakedOutput();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FGeometryNodesBridgeActorDetails::HandleDeleteBakedOutputClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->DeleteBakedOutput();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}
