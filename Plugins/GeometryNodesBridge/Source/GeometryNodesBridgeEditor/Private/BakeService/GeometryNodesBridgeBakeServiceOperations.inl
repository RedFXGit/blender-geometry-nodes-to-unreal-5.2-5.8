bool FGeometryNodesBridgeBakeService::BakeComponent(UGeometryNodesBridgeComponent* Component)
{
    if (Component == nullptr || Component->GetOwner() == nullptr)
    {
        return false;
    }

    const FBakedOutputSnapshot PreviousOutput = CaptureBakedOutputSnapshot(Component);
    const bool bWasShowingBakedOutput = Component->bShowingBakedOutput;
    bool bBakedSuccessfully = false;

    if (Component->BakeOutputMode == EGeometryNodesBridgeBakeOutputMode::StaticMesh)
    {
        if (!Component->HasCookedMesh())
        {
            return false;
        }

        const FString BakeFolderPath = BuildBakeFolderPath(Component);
        const FString MaterialsFolderPath = FString::Printf(TEXT("%s/Materials"), *BakeFolderPath);

        FResolvedBakeMaterials ResolvedMaterialState;
        ResolveMaterialsForSlots(
            Component,
            Component->LastCookedMaterialSlots,
            MaterialsFolderPath,
            EnsurePlaceholderParentMaterial(BakeFolderPath),
            ResolvedMaterialState);

        const TArray<UMaterialInterface*> ResolvedMaterials =
            BuildResolvedMaterialsForSlotList(Component->LastCookedMaterialSlots, ResolvedMaterialState);
        UStaticMesh* StaticMesh = BuildOrUpdateStaticMeshAsset(
            FString::Printf(TEXT("%s/Meshes"), *BakeFolderPath),
            FString::Printf(TEXT("SM_%s"), *GetOwnerSegment(Component)),
            Component->GetLastCookedMesh(),
            Component->LastCookedMaterialSlots,
            ResolvedMaterials);
        if (StaticMesh == nullptr)
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to build a baked static mesh asset."));
            return false;
        }

        AStaticMeshActor* BakedActor = SpawnOrUpdateBakedActor(Component, StaticMesh);
        if (BakedActor == nullptr)
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to spawn or update the baked static mesh actor."));
            return false;
        }

        Component->BakedStaticMesh = StaticMesh;
        Component->BakedActor = BakedActor;
        Component->BakedPrototypeMeshes.Reset();
        Component->GeneratedBakeMaterials = MoveTemp(ResolvedMaterialState.GeneratedMaterials);
        Component->SetBridgeStatus(
            EGeometryNodesBridgeComponentStatus::Cooked,
            FString::Printf(TEXT("Baked static mesh output to %s."), *StaticMesh->GetPathName()));
        bBakedSuccessfully = true;
    }
    else
    {
        const FBridgeDecodedInstanceBake* InstanceBake =
            FGeometryNodesBridgeRuntimeModule::Get().GetSessionManager().FindCachedInstanceBake(Component);
        if (InstanceBake == nullptr || InstanceBake->Prototypes.IsEmpty() || InstanceBake->Placements.IsEmpty())
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("No instanced bake payload is available yet."));
            return false;
        }

        const FString ActorBakeFolderPath = BuildBakeFolderPath(Component);
        const FString MaterialsFolderPath = FString::Printf(TEXT("%s/Materials"), *ActorBakeFolderPath);
        const FString SharedPrototypeMeshesFolderPath = FString::Printf(TEXT("%s/Meshes"), *BuildSharedPrototypeFolderPath(Component));

        TArray<FBridgeMaterialSlotDescriptor> MaterialSlots;
        GatherInstanceBakeMaterialSlots(*InstanceBake, MaterialSlots);

        FResolvedBakeMaterials ResolvedMaterialState;
        ResolveMaterialsForSlots(
            Component,
            MaterialSlots,
            MaterialsFolderPath,
            EnsurePlaceholderParentMaterial(ActorBakeFolderPath),
            ResolvedMaterialState);

        TMap<int32, TArray<FTransform>> PlacementsByPrototypeIndex;
        for (const FBridgeInstanceBakePlacement& Placement : InstanceBake->Placements)
        {
            if (Placement.PrototypeIndex >= 0 && Placement.PrototypeIndex < InstanceBake->Prototypes.Num())
            {
                PlacementsByPrototypeIndex.FindOrAdd(Placement.PrototypeIndex).Add(Placement.Transform);
            }
        }

        AActor* BakedActor = SpawnAttachedActor(Component, AActor::StaticClass());
        if (BakedActor == nullptr)
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to spawn the baked instanced actor."));
            return false;
        }

        USceneComponent* RootComponent = EnsureActorRootComponent(BakedActor, GetSourceMobility(Component));
        if (RootComponent == nullptr)
        {
            if (UWorld* World = BakedActor->GetWorld())
            {
                World->EditorDestroyActor(BakedActor, true);
            }
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to initialize the baked instanced actor root."));
            return false;
        }

        const FString SourceScopeKey = BuildSourceScopeKey(Component);
        TArray<FGeometryNodesBridgeBakedPrototypeMeshReference> PrototypeReferences;
        PrototypeReferences.Reserve(InstanceBake->Prototypes.Num());

        for (int32 PrototypeIndex = 0; PrototypeIndex < InstanceBake->Prototypes.Num(); ++PrototypeIndex)
        {
            const FBridgeInstanceBakePrototype& Prototype = InstanceBake->Prototypes[PrototypeIndex];
            if (Prototype.PrototypeKey.IsEmpty())
            {
                continue;
            }

            TArray<UMaterialInterface*> NullMaterials;
            NullMaterials.SetNum(Prototype.Mesh.MaterialSlots.Num());
            UStaticMesh* SharedMesh = BuildOrUpdateStaticMeshAsset(
                SharedPrototypeMeshesFolderPath,
                FString::Printf(TEXT("SMI_%s"), *SanitizePackageSegment(Prototype.PrototypeKey)),
                Prototype.Mesh,
                Prototype.Mesh.MaterialSlots,
                NullMaterials);
            if (SharedMesh == nullptr)
            {
                if (UWorld* World = BakedActor->GetWorld())
                {
                    World->EditorDestroyActor(BakedActor, true);
                }
                Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to build a shared prototype mesh asset."));
                return false;
            }

            FGeometryNodesBridgeBakedPrototypeMeshReference PrototypeReference;
            PrototypeReference.SourceScopeKey = SourceScopeKey;
            PrototypeReference.PrototypeKey = Prototype.PrototypeKey;
            PrototypeReference.Mesh = SharedMesh;
            PrototypeReferences.Add(MoveTemp(PrototypeReference));

            const TArray<FTransform>* InstanceTransforms = PlacementsByPrototypeIndex.Find(PrototypeIndex);
            if (InstanceTransforms == nullptr || InstanceTransforms->IsEmpty())
            {
                continue;
            }

            UHierarchicalInstancedStaticMeshComponent* HismComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(
                BakedActor,
                *FString::Printf(TEXT("%s_%d"), BakedHismComponentNamePrefix, PrototypeIndex),
                RF_Transactional);
            if (HismComponent == nullptr)
            {
                if (UWorld* World = BakedActor->GetWorld())
                {
                    World->EditorDestroyActor(BakedActor, true);
                }
                Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to allocate a baked HISM component."));
                return false;
            }

            BakedActor->AddInstanceComponent(HismComponent);
            HismComponent->SetupAttachment(RootComponent);
            HismComponent->SetMobility(GetSourceMobility(Component));
            HismComponent->SetCanEverAffectNavigation(false);
            HismComponent->SetStaticMesh(SharedMesh);
            for (int32 SlotIndex = 0; SlotIndex < Prototype.Mesh.MaterialSlots.Num(); ++SlotIndex)
            {
                const FBridgeMaterialSlotDescriptor& Slot = Prototype.Mesh.MaterialSlots[SlotIndex];
                if (const TObjectPtr<UMaterialInterface>* Material = ResolvedMaterialState.MaterialsBySlotId.Find(Slot.SlotId))
                {
                    HismComponent->SetMaterial(SlotIndex, Material->Get());
                }
            }

            HismComponent->RegisterComponent();
            for (const FTransform& Transform : *InstanceTransforms)
            {
                HismComponent->AddInstance(Transform);
            }
        }

        Component->BakedActor = BakedActor;
        Component->BakedStaticMesh = nullptr;
        Component->BakedPrototypeMeshes = MoveTemp(PrototypeReferences);
        Component->GeneratedBakeMaterials = MoveTemp(ResolvedMaterialState.GeneratedMaterials);
        Component->SetBridgeStatus(
            EGeometryNodesBridgeComponentStatus::Cooked,
            FString::Printf(
                TEXT("Baked %d shared prototype meshes across %d instances."),
                Component->BakedPrototypeMeshes.Num(),
                InstanceBake->Placements.Num()));
        bBakedSuccessfully = true;
    }

    if (!bBakedSuccessfully)
    {
        return false;
    }

    if (bWasShowingBakedOutput)
    {
        Component->SetShowingBakedOutput(true);
    }
    else
    {
        Component->SetShowingBakedOutput(false);
        Component->QueueShowBakedOutputOnNextTick();
    }

    CleanupReplacedBakedOutput(Component, PreviousOutput);
    return true;
}

