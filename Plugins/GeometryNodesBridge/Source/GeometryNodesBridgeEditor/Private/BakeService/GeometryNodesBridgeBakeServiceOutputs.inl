bool FGeometryNodesBridgeBakeService::SelectBakedOutput(UGeometryNodesBridgeComponent* Component)
{
    if (Component == nullptr)
    {
        return false;
    }

    if (AActor* BakedActor = Component->BakedActor.Get())
    {
        GEditor->SelectNone(false, true, false);
        GEditor->SelectActor(BakedActor, true, true, true, true);
        return true;
    }

    if (UStaticMesh* BakedMesh = Component->BakedStaticMesh.LoadSynchronous())
    {
        TArray<UObject*> ObjectsToSync;
        ObjectsToSync.Add(BakedMesh);
        GEditor->SyncBrowserToObjects(ObjectsToSync);
        return true;
    }

    TArray<UObject*> ObjectsToSync;
    for (const FGeometryNodesBridgeBakedPrototypeMeshReference& PrototypeReference : Component->BakedPrototypeMeshes)
    {
        if (UStaticMesh* Mesh = PrototypeReference.Mesh.LoadSynchronous())
        {
            ObjectsToSync.AddUnique(Mesh);
        }
    }

    if (!ObjectsToSync.IsEmpty())
    {
        GEditor->SyncBrowserToObjects(ObjectsToSync);
        return true;
    }

    return false;
}

bool FGeometryNodesBridgeBakeService::DeleteBakedOutput(UGeometryNodesBridgeComponent* Component)
{
    if (Component == nullptr || !Component->HasBakedOutput())
    {
        return false;
    }

    const FBakedOutputSnapshot PreviousOutput = CaptureBakedOutputSnapshot(Component);
    Component->BakedActor = nullptr;
    Component->BakedStaticMesh = nullptr;
    Component->BakedPrototypeMeshes.Reset();
    Component->GeneratedBakeMaterials.Reset();
    Component->SetShowingBakedOutput(false);
    CleanupReplacedBakedOutput(Component, PreviousOutput);
    Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Cooked, TEXT("Deleted baked bridge output."));
    return true;
}
