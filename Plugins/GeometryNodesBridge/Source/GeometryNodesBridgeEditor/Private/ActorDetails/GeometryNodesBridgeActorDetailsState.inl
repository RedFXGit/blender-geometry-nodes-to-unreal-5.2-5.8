TArray<UObject*> FGeometryNodesBridgeActorDetails::GetSelectedBridgeComponentObjects() const
{
    TArray<UObject*> BridgeComponents;
    BridgeComponents.Reserve(SelectedActors.Num());

    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponents.Add(BridgeComponent);
            }
        }
    }

    return BridgeComponents;
}

AGeometryNodesBridgeActor* FGeometryNodesBridgeActorDetails::GetSingleSelectedActor() const
{
    return SelectedActors.Num() == 1 ? SelectedActors[0].Get() : nullptr;
}

UGeometryNodesBridgeComponent* FGeometryNodesBridgeActorDetails::GetSingleSelectedBridgeComponent() const
{
    if (AGeometryNodesBridgeActor* Actor = GetSingleSelectedActor())
    {
        return Actor->GetBridgeComponent();
    }

    return nullptr;
}

bool FGeometryNodesBridgeActorDetails::IsSingleSelection() const
{
    return GetSingleSelectedActor() != nullptr;
}

bool FGeometryNodesBridgeActorDetails::HasSelectedActors() const
{
    return !SelectedActors.IsEmpty();
}

bool FGeometryNodesBridgeActorDetails::AnySelectedActorsHaveBakedOutput() const
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (const AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (HasBakedOutput(Actor->GetBridgeComponent()))
            {
                return true;
            }
        }
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowCurveCategory() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->TargetCapabilities.TargetKind == EGeometryNodesBridgeTargetKind::CurveOwner
            || (GetSingleSelectedActor() != nullptr && GetSingleSelectedActor()->GetBridgeSplineComponent() != nullptr);
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowGeometryCategory() const
{
    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowGeometryProperties() const
{
    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowCurveSplineProperty() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr || IsExternalSplineBindingActive(BridgeComponent))
    {
        return false;
    }

    return GetSingleSelectedActor() != nullptr && GetSingleSelectedActor()->GetBridgeSplineComponent() != nullptr;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowMaterialProperties() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->LastCookedMaterialSlots.IsEmpty();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowParameterProperties() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->InputSchema.IsEmpty();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowBakedActorProperty() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->BakedActor.IsValid();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowBakedStaticMeshProperty() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->BakedStaticMesh.IsNull();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowBakedPrototypeProperties() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->BakedPrototypeMeshes.IsEmpty();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowGeneratedBakeMaterialsProperty() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->GeneratedBakeMaterials.IsEmpty();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowInstancedBakeNote() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    return BridgeComponent != nullptr
        && BridgeComponent->BakeOutputMode == EGeometryNodesBridgeBakeOutputMode::InstancedHierarchy;
}

bool FGeometryNodesBridgeActorDetails::CanSelectOrDeleteBakedOutput() const
{
    return AnySelectedActorsHaveBakedOutput();
}

bool FGeometryNodesBridgeActorDetails::HasCachedInstanceBakePayload() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return false;
    }

    if (!FModuleManager::Get().IsModuleLoaded(TEXT("GeometryNodesBridgeRuntime")))
    {
        return false;
    }

    return FGeometryNodesBridgeRuntimeModule::Get().GetSessionManager().FindCachedInstanceBake(BridgeComponent) != nullptr;
}

EVisibility FGeometryNodesBridgeActorDetails::GetSingleSelectionVisibility() const
{
    return IsSingleSelection() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetMultiSelectionVisibility() const
{
    return SelectedActors.Num() > 1 ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetSetupHintVisibility() const
{
    return GetSetupHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetParametersHintVisibility() const
{
    return GetParametersHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetParametersPropertyVisibility() const
{
    return ShouldShowParameterProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetGeometryHintVisibility() const
{
    return GetGeometryHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetGeometryPropertiesVisibility() const
{
    return ShouldShowGeometryProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetCurveHintVisibility() const
{
    return GetCurveHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetCurveSplinePropertyVisibility() const
{
    return ShouldShowCurveSplineProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetMaterialsHintVisibility() const
{
    return GetMaterialsHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetMaterialPropertiesVisibility() const
{
    return ShouldShowMaterialProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetInstancedBakeNoteVisibility() const
{
    return ShouldShowInstancedBakeNote() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetBakedActorPropertyVisibility() const
{
    return ShouldShowBakedActorProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetBakedStaticMeshPropertyVisibility() const
{
    return ShouldShowBakedStaticMeshProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetBakedPrototypePropertiesVisibility() const
{
    return ShouldShowBakedPrototypeProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetGeneratedBakeMaterialsPropertyVisibility() const
{
    return ShouldShowGeneratedBakeMaterialsProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

FSlateColor FGeometryNodesBridgeActorDetails::GetStatusStripColor() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        if (BridgeComponent->ConnectionState != EGeometryNodesBridgeConnectionState::Connected)
        {
            return FSlateColor(GetConnectionStateColor(BridgeComponent->ConnectionState));
        }

        return FSlateColor(GetComponentStatusColor(BridgeComponent->ComponentStatus));
    }

    if (HasSelectedActors())
    {
        return FSlateColor(GetConnectionStateColor(EGeometryNodesBridgeConnectionState::Connected));
    }

    return FSlateColor(FLinearColor(0.55f, 0.55f, 0.55f));
}

FText FGeometryNodesBridgeActorDetails::GetStatusStripText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return FText::Format(
            FText::FromString(TEXT("{0} | {1}")),
            GetEnumDisplayText(BridgeComponent->ConnectionState),
            GetEnumDisplayText(BridgeComponent->ComponentStatus));
    }

    if (SelectedActors.Num() > 1)
    {
        return FText::FromString(TEXT("Batch Actions Available"));
    }

    return FText::FromString(TEXT("No Bridge Actor Selected"));
}

FText FGeometryNodesBridgeActorDetails::GetStatusStripToolTipText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->LastStatusMessage.IsEmpty()
            ? GetStatusStripText()
            : FText::FromString(BridgeComponent->LastStatusMessage);
    }

    return FText::FromString(TEXT("Select one bridge actor to inspect source, targets, parameters, and outputs."));
}

FText FGeometryNodesBridgeActorDetails::GetOverviewSelectionSummaryText() const
{
    if (IsSingleSelection())
    {
        return FText::FromString(TEXT("One bridge actor selected. The controls below edit the live bridge component directly."));
    }

    if (SelectedActors.Num() > 1)
    {
        return FText::Format(
            FText::FromString(TEXT("{0} bridge actors selected. Batch actions stay available here; select one actor to edit parameters and outputs.")),
            FText::AsNumber(SelectedActors.Num()));
    }

    return FText::FromString(TEXT("Select a Geometry Nodes Bridge actor to manage its source, target, parameters, bake settings, and outputs."));
}

FText FGeometryNodesBridgeActorDetails::GetSourceSummaryText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->Source != nullptr
            ? FText::FromString(BridgeComponent->Source->GetName())
            : FText::FromString(TEXT("None"));
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetTargetSummaryText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        if (!BridgeComponent->TargetDisplayLabel.IsEmpty())
        {
            return FText::FromString(BridgeComponent->TargetDisplayLabel);
        }

        if (!BridgeComponent->TargetId.IsEmpty())
        {
            return FText::FromString(BridgeComponent->TargetId);
        }

        return FText::FromString(TEXT("No target selected"));
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetBridgeBuildSummaryText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return FText::FromString(
            BridgeComponent->BridgeBuildVersion.IsEmpty()
                ? TEXT("Unknown build")
                : BridgeComponent->BridgeBuildVersion);
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetLastStatusSummaryText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return FText::FromString(
            BridgeComponent->LastStatusMessage.IsEmpty()
                ? TEXT("No status message yet.")
                : BridgeComponent->LastStatusMessage);
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetMultiSelectionSummaryText() const
{
    if (SelectedActors.Num() <= 1)
    {
        return FText::GetEmpty();
    }

    return FText::FromString(
        TEXT("Batch actions stay on the actor surface for multi-selection. Choose a single bridge actor when you want to edit source assignment, target selection, parameters, bake settings, or outputs."));
}

FText FGeometryNodesBridgeActorDetails::GetSetupHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->Source == nullptr)
    {
        return FText::FromString(TEXT("Assign a Geometry Nodes Bridge source to load targets and reflect parameters."));
    }

    if (BridgeComponent->AvailableTargets.IsEmpty())
    {
        return FText::FromString(TEXT("No targets are available yet. Use Refresh Input Schema to load the source and populate target options."));
    }

    if (BridgeComponent->TargetId.IsEmpty())
    {
        return FText::FromString(TEXT("Choose a target to reflect its schema and unlock parameters, materials, and bake output details."));
    }

    const bool bTargetExists = BridgeComponent->AvailableTargets.ContainsByPredicate([BridgeComponent](const FBridgeTargetDescriptor& TargetDescriptor)
    {
        return TargetDescriptor.TargetId == BridgeComponent->TargetId;
    });

    if (!bTargetExists)
    {
        return FText::FromString(TEXT("The current target is not available in the loaded source. Pick a valid target or refresh the source data."));
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetParametersHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->Source == nullptr || BridgeComponent->TargetId.IsEmpty())
    {
        return FText::FromString(TEXT("Parameters appear after a source and target are selected."));
    }

    if (BridgeComponent->ComponentStatus == EGeometryNodesBridgeComponentStatus::SchemaPending)
    {
        return FText::FromString(TEXT("The target schema is still loading."));
    }

    if (BridgeComponent->InputSchema.IsEmpty())
    {
        if (!BridgeComponent->GeometryInputDescriptors.IsEmpty())
        {
            return FText::FromString(TEXT("This target does not expose scalar inputs. Optional reference actor overrides are available in Setup."));
        }

        return FText::FromString(TEXT("This target does not expose any editable inputs."));
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetGeometryHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->Source == nullptr || BridgeComponent->TargetId.IsEmpty())
    {
        return FText::FromString(TEXT("Geometry bindings appear after a source and target are selected."));
    }

    if (BridgeComponent->ComponentStatus == EGeometryNodesBridgeComponentStatus::SchemaPending)
    {
        return FText::FromString(TEXT("Geometry binding options are still loading."));
    }

    if (BridgeComponent->GeometryInputDescriptors.IsEmpty())
    {
        return FText::FromString(TEXT("This target does not expose any bindable geometry inputs."));
    }

    const bool bSupportsStaticMeshBinding = BridgeComponent->GeometryInputDescriptors.ContainsByPredicate([](const FBridgeGeometryInputDescriptor& Descriptor)
    {
        return Descriptor.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent);
    });

    const bool bSupportsSplineBinding = BridgeComponent->GeometryInputDescriptors.ContainsByPredicate([](const FBridgeGeometryInputDescriptor& Descriptor)
    {
        return DescriptorSupportsSplineBinding(Descriptor);
    });

    if (bSupportsStaticMeshBinding && !bSupportsSplineBinding)
    {
        return FText::FromString(TEXT("Pick a level StaticMeshComponent here to override owner-geometry inputs such as floorplan meshes. Leaving it empty keeps the blend file's original geometry."));
    }

    if (bSupportsSplineBinding)
    {
        return FText::FromString(TEXT("Leave the picker empty to use the embedded bridge spline, or assign a level spline component to override it from Unreal."));
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetCurveHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->TargetCapabilities.TargetKind != EGeometryNodesBridgeTargetKind::CurveOwner)
    {
        return FText::GetEmpty();
    }

    if (!BridgeComponent->TargetCapabilities.bSupportsCurveOwner)
    {
        return BridgeComponent->TargetCapabilities.CompatibilityMessage.IsEmpty()
            ? FText::FromString(TEXT("This target is curve-owner based, but embedded spline editing is unavailable."))
            : FText::FromString(FString::Printf(
                TEXT("Embedded spline editing is unavailable for this target: %s"),
                *BridgeComponent->TargetCapabilities.CompatibilityMessage));
    }

    if (IsExternalSplineBindingActive(BridgeComponent))
    {
        if (HasValidExternalSplineBinding(BridgeComponent, GetSingleSelectedActor()))
        {
            return FText::FromString(TEXT("This target is being driven by an external spline component selected in Setup. Edit that spline in the viewport and cooks will follow its shape."));
        }

        return FText::FromString(TEXT("This target is set to use an external spline component, but the current binding is invalid. Pick a valid spline actor override in Setup to cook this target."));
    }

    if (!ShouldShowCurveSplineProperty())
    {
        return FText::FromString(TEXT("This target is curve-owner compatible, but the embedded spline is not currently available."));
    }

    return FText::FromString(TEXT("Edit the embedded spline in the viewport to drive this target. Spline edits continue to queue cooks using the existing manual workflow."));
}

FText FGeometryNodesBridgeActorDetails::GetMaterialsHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->LastCookedMaterialSlots.IsEmpty())
    {
        return FText::FromString(TEXT("No cooked material slots are available yet. Request Cook to populate slot mappings and override materials."));
    }

    return FText::Format(
        FText::FromString(TEXT("{0} cooked material slot(s) are available. Material overrides apply to preview and bake output; generated bake materials appear in Outputs after baking.")),
        FText::AsNumber(BridgeComponent->LastCookedMaterialSlots.Num()));
}

FText FGeometryNodesBridgeActorDetails::GetBakeModeDescriptionText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->BakeOutputMode == EGeometryNodesBridgeBakeOutputMode::InstancedHierarchy)
    {
        return FText::FromString(TEXT("InstancedHierarchy preserves recoverable repeated elements as HISM output and shares prototype meshes across duplicate actors from the same blend source."));
    }

    return FText::FromString(TEXT("StaticMesh preserves the existing one-mesh bake flow by collapsing the cooked result into a single baked mesh asset."));
}

FText FGeometryNodesBridgeActorDetails::GetInstancedBakeNoteText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr || BridgeComponent->BakeOutputMode != EGeometryNodesBridgeBakeOutputMode::InstancedHierarchy)
    {
        return FText::GetEmpty();
    }

    if (!BridgeComponent->BakedPrototypeMeshes.IsEmpty())
    {
        return FText::Format(
            FText::FromString(TEXT("This actor currently references {0} shared prototype mesh(es) from the last instanced bake.")),
            FText::AsNumber(BridgeComponent->BakedPrototypeMeshes.Num()));
    }

    if (HasCachedInstanceBakePayload())
    {
        return FText::FromString(TEXT("An instance-bake payload is already cached for this actor and ready for Bake Output."));
    }

    return FText::FromString(TEXT("No instance-bake payload is cached yet. Bake Output will request dedicated instance data from Blender when you run it."));
}

FText FGeometryNodesBridgeActorDetails::GetOutputsSummaryText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (!HasBakedOutput(BridgeComponent))
    {
        return FText::FromString(TEXT("No baked output exists yet. Use Bake Output to create actor assets from the current bridge state."));
    }

    if (!BridgeComponent->BakedPrototypeMeshes.IsEmpty())
    {
        return FText::Format(
            FText::FromString(TEXT("Instanced bake output is active. This actor has a baked actor reference, {0} shared prototype mesh(es), and {1} generated bake material(s).")),
            FText::AsNumber(BridgeComponent->BakedPrototypeMeshes.Num()),
            FText::AsNumber(BridgeComponent->GeneratedBakeMaterials.Num()));
    }

    return FText::Format(
        FText::FromString(TEXT("Static bake output is active. This actor references a baked actor, {0} baked static mesh asset, and {1} generated bake material(s).")),
        FText::AsNumber(BridgeComponent->BakedStaticMesh.IsNull() ? 0 : 1),
        FText::AsNumber(BridgeComponent->GeneratedBakeMaterials.Num()));
}

FText FGeometryNodesBridgeActorDetails::GetAdvancedHintText() const
{
    return FText::FromString(TEXT("Raw implementation details live here for debugging and escape-hatch workflows. The sections above are the intended day-to-day control surface."));
}

