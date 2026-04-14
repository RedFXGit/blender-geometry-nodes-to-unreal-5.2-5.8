void FGeometryNodesBridgeSessionManager::RefreshAllComponentConnectionStates() const
{
    ForEachBridgeComponent([this](UGeometryNodesBridgeComponent* Component)
    {
        Component->ConnectionState = SessionState;
    });
}

TMap<FString, FString> FGeometryNodesBridgeSessionManager::GatherSourcesToRestore() const
{
    TMap<FString, FString> SourcesToRestore;

    ForEachBridgeComponent([&SourcesToRestore](UGeometryNodesBridgeComponent* Component)
    {
        const FString SourceHandle = Component->GetSourceHandle();
        if (SourceHandle.IsEmpty())
        {
            return;
        }

        SourcesToRestore.FindOrAdd(SourceHandle, SourceHandle);
    });

    return SourcesToRestore;
}

void FGeometryNodesBridgeSessionManager::ClearCachedTargetData(const FString& SourceHandle)
{
    if (SourceHandle.IsEmpty())
    {
        return;
    }

    const FString SourcePrefix = FString::Printf(TEXT("%s%s"), *SourceHandle, CacheKeyDelimiter);

    for (auto It = TargetSchemaCaches.CreateIterator(); It; ++It)
    {
        if (It.Key().StartsWith(SourcePrefix))
        {
            It.RemoveCurrent();
        }
    }

    for (auto It = CookedTargetCaches.CreateIterator(); It; ++It)
    {
        if (It.Key().StartsWith(SourcePrefix))
        {
            It.RemoveCurrent();
        }
    }
}

bool FGeometryNodesBridgeSessionManager::TryApplyCachedTargetDescription(
    UGeometryNodesBridgeComponent* Component,
    const bool bPreserveInputValues,
    const bool bRequestCookAfterDescribe,
    const bool bRequestInstanceBakeAfterDescribe)
{
    if (!IsValid(Component) || Component->TargetId.IsEmpty())
    {
        return false;
    }

    const FTargetSchemaCache* CachedSchema = TargetSchemaCaches.Find(MakeTargetSchemaCacheKey(Component->GetSourceHandle(), Component->TargetId));
    if (CachedSchema == nullptr)
    {
        return false;
    }

    Component->InputSchema = CachedSchema->Schema;
    Component->GeometryInputDescriptors = CachedSchema->GeometryInputs;
    Component->SchemaTargetId = Component->TargetId;
    Component->RebuildInputValues(bPreserveInputValues);
    Component->RebuildGeometryBindings(bPreserveInputValues);
    const FString CurveOwnerStatus = Component->ApplyCurveOwnerDescription(Component->TargetId, CachedSchema->Capabilities, bPreserveInputValues);
    Component->SetBridgeStatus(
        EGeometryNodesBridgeComponentStatus::Ready,
        FString::Printf(
            TEXT("Ready. Reused %d cached bridge inputs and %d geometry binding target(s).%s"),
            Component->InputSchema.Num(),
            Component->GeometryInputDescriptors.Num(),
            *CurveOwnerStatus));

    if (bRequestCookAfterDescribe)
    {
        return SendCookRequest(Component);
    }

    if (bRequestInstanceBakeAfterDescribe)
    {
        return SendInstanceBakeRequest(Component);
    }

    return true;
}

bool FGeometryNodesBridgeSessionManager::TryApplyCachedCook(UGeometryNodesBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return false;
    }

    if (Component->ShouldBypassCookCacheForAutomation())
    {
        return false;
    }

    FString CurveHash;
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        FBridgeCurveOwnerData CurveData;
        if (!Component->BuildCurveOwnerData(CurveData, CurveHash))
        {
            return false;
        }
    }

    TArray<FBridgeGeometryBindingPayload> GeometryBindingPayloads;
    FString GeometryBindingsHash;
    FString GeometryBindingsError;
    if (!Component->BuildGeometryBindingPayloads(GeometryBindingPayloads, GeometryBindingsHash, GeometryBindingsError))
    {
        return false;
    }

    FString CookCacheKey;
    if (!BuildCookCacheKey(
            Component->GetSourceHandle(),
            Component->TargetId,
            Component->InputSchema,
            Component->InputValues,
            CurveHash,
            GeometryBindingsHash,
            CookCacheKey))
    {
        return false;
    }

    const FCookedTargetCache* CachedCook = CookedTargetCaches.Find(CookCacheKey);
    if (CachedCook == nullptr)
    {
        return false;
    }

    const double PreviewApplyStartSeconds = FPlatformTime::Seconds();
    FBridgeDecodedMesh CachedDecodedMesh = CachedCook->DecodedMesh;
    Component->ApplyDecodedMesh(MoveTemp(CachedDecodedMesh));
    const double PreviewApplyMs = (FPlatformTime::Seconds() - PreviewApplyStartSeconds) * 1000.0;
    Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Cooked, TEXT("Applied cached bridge mesh."));

    if (ShouldLogCookTimings())
    {
        UE_LOG(
            LogGeometryNodesBridge,
            Log,
            TEXT("[GeometryNodesBridgeTiming] kind=cook cache_hit=true actor=%s target=%s preview_apply=%s"),
            *GetNameSafe(Component->GetOwner()),
            *Component->TargetId,
            *FormatTimingMs(PreviewApplyMs));

        FBridgeTimingHistoryRow HistoryRow;
        HistoryRow.TimestampUtc = FDateTime::UtcNow().ToIso8601();
        HistoryRow.Kind = TEXT("cook");
        HistoryRow.bCacheHit = true;
        PopulateTimingHistoryMetadataFromComponent(Component, HistoryRow);
        HistoryRow.ActorName = GetNameSafe(Component->GetOwner());
        HistoryRow.TargetId = Component->TargetId;
        HistoryRow.PreviewApplyMs = PreviewApplyMs;
        AppendTimingHistoryRow(HistoryRow);
    }
    return true;
}

bool FGeometryNodesBridgeSessionManager::TryApplyCachedInstanceBake(UGeometryNodesBridgeComponent* Component)
{
    if (IsValid(Component) && Component->ShouldBypassCookCacheForAutomation())
    {
        return false;
    }

    return FindCachedInstanceBake(Component) != nullptr;
}

void FGeometryNodesBridgeSessionManager::ApplySourceCacheToComponents(
    const FString& SourceHandle,
    const bool bRequestCookForPendingComponent,
    const bool bRequestInstanceBakeForPendingComponent,
    UGeometryNodesBridgeComponent* PendingComponent)
{
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr)
    {
        return;
    }

    auto ProcessComponent = [&](UGeometryNodesBridgeComponent* Component)
    {
        if (Component->GetSourceHandle() != SourceHandle)
        {
            return;
        }

        const FString PreviousTargetId = Component->TargetId;
        const FString PreviousSchemaTargetId = Component->SchemaTargetId;
        bool bCarriedEnsureCookRequest = false;
        bool bCarriedEnsureInstanceBakeRequest = false;

        for (const TPair<FString, FPendingRequest>& PendingEntry : PendingRequests)
        {
            const FPendingRequest& PendingRequest = PendingEntry.Value;
            if (PendingRequest.Kind != EBridgeManagerRequestKind::EnsureSourceLoaded
                || PendingRequest.Component.Get() != Component
                || PendingRequest.SourceHandle != SourceHandle)
            {
                continue;
            }

            bCarriedEnsureCookRequest |= PendingRequest.bRequestCookAfterDescribe;
            bCarriedEnsureInstanceBakeRequest |= PendingRequest.bRequestInstanceBakeAfterDescribe;
        }

        Component->ConnectionState = SessionState;

        Component->AvailableTargets = SourceCache->Targets;

        if (Component->FindTargetDescriptor(Component->TargetId) == nullptr)
        {
            if (!SourceCache->Targets.IsEmpty())
            {
                Component->TargetId = SourceCache->Targets[0].TargetId;
            }
            else
            {
                Component->TargetId.Reset();
            }
        }

        Component->UpdateTargetDisplayLabel();

        if (Component->TargetId != PreviousTargetId)
        {
            ReleaseComponentState(Component);
            Component->CancelPendingSplineCook();
            Component->SchemaTargetId.Reset();
            Component->InputSchema.Reset();
            Component->GeometryInputDescriptors.Reset();
            Component->GeometryBindings.Reset();
            Component->ApplyCurveOwnerDescription(Component->TargetId, FBridgeTargetCapabilities(), false);
            Component->RebuildInputValues(false);
            if (Component->PreviewMeshComponent != nullptr)
            {
                Component->PreviewMeshComponent->ClearAllMeshSections();
            }
        }

        if (SourceCache->Targets.IsEmpty())
        {
            Component->SchemaTargetId.Reset();
            Component->InputSchema.Reset();
            Component->GeometryInputDescriptors.Reset();
            Component->GeometryBindings.Reset();
            Component->ApplyCurveOwnerDescription(Component->TargetId, FBridgeTargetCapabilities(), false);
            Component->RebuildInputValues(false);
            if (Component->PreviewMeshComponent != nullptr)
            {
                Component->PreviewMeshComponent->ClearAllMeshSections();
            }
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::SourceNotLoaded, TEXT("No Geometry Nodes targets were found in this blend source."));
            return;
        }

        if (Component->TargetId.IsEmpty())
        {
            Component->SchemaTargetId.Reset();
            Component->InputSchema.Reset();
            Component->GeometryInputDescriptors.Reset();
            Component->GeometryBindings.Reset();
            Component->ApplyCurveOwnerDescription(Component->TargetId, FBridgeTargetCapabilities(), false);
            Component->RebuildInputValues(false);
            if (Component->PreviewMeshComponent != nullptr)
            {
                Component->PreviewMeshComponent->ClearAllMeshSections();
            }
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
            return;
        }

        const bool bRequestCook = (PendingComponent != nullptr && PendingComponent == Component && bRequestCookForPendingComponent)
            || bCarriedEnsureCookRequest
            || IsRetryComponent(PendingCookRetryComponents, Component);
        const bool bRequestInstanceBake =
            (PendingComponent != nullptr && PendingComponent == Component && bRequestInstanceBakeForPendingComponent)
            || bCarriedEnsureInstanceBakeRequest
            || IsRetryComponent(PendingInstanceBakeRetryComponents, Component);
        const bool bPreserveInputValues = (PreviousSchemaTargetId == Component->TargetId);

        PendingCookRetryComponents.RemoveAll([Component](const TWeakObjectPtr<UGeometryNodesBridgeComponent>& Entry)
        {
            return !Entry.IsValid() || Entry.Get() == Component;
        });
        PendingInstanceBakeRetryComponents.RemoveAll([Component](const TWeakObjectPtr<UGeometryNodesBridgeComponent>& Entry)
        {
            return !Entry.IsValid() || Entry.Get() == Component;
        });

        UE_LOG(
            LogGeometryNodesBridge,
            Log,
            TEXT("[GeometryNodesBridgeFlow] source_cache applied source='%s' component=%s target_count=%d selected_target='%s' preserve_input_values=%s request_cook=%s request_instance_bake=%s carried_ensure_cook=%s carried_ensure_instance_bake=%s"),
            *SourceHandle,
            *DescribeBridgeComponentForLog(Component),
            SourceCache->Targets.Num(),
            Component->TargetId.IsEmpty() ? TEXT("<none>") : *Component->TargetId,
            BoolToString(bPreserveInputValues),
            BoolToString(bRequestCook),
            BoolToString(bRequestInstanceBake),
            BoolToString(bCarriedEnsureCookRequest),
            BoolToString(bCarriedEnsureInstanceBakeRequest));

        RequestDescribeTarget(Component, bPreserveInputValues, bRequestCook, bRequestInstanceBake);
    };

    bool bProcessedPendingComponent = false;
    ForEachBridgeComponent([&](UGeometryNodesBridgeComponent* Component)
    {
        if (Component == PendingComponent)
        {
            bProcessedPendingComponent = true;
        }

        ProcessComponent(Component);
    });

    if (IsValid(PendingComponent) && !bProcessedPendingComponent)
    {
        ProcessComponent(PendingComponent);
    }
}

