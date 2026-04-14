bool FGeometryNodesBridgeSessionManager::RequestEnsureSourceLoaded(
    UGeometryNodesBridgeComponent* Component,
    const bool bForceReload,
    const bool bRequestCookAfterDescribe,
    const bool bRequestInstanceBakeAfterDescribe)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FString BlendFilePath = GetComponentBlendFilePath(Component);
    if (SourceHandle.IsEmpty() || BlendFilePath.IsEmpty())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::SourceNotLoaded, TEXT("Select a GeometryNodesBridgeSource with a valid Blend File."));
        return false;
    }

    FSourceCache& SourceCache = SourceCaches.FindOrAdd(SourceHandle);
    SourceCache.BlendFilePath = BlendFilePath;

    return QueueEnsureSourceLoadedRequest(
        SourceHandle,
        BlendFilePath,
        Component,
        bForceReload,
        bRequestCookAfterDescribe,
        bRequestInstanceBakeAfterDescribe);
}

bool FGeometryNodesBridgeSessionManager::QueueEnsureSourceLoadedRequest(
    const FString& SourceHandle,
    const FString& BlendFilePath,
    UGeometryNodesBridgeComponent* Component,
    const bool bForceReload,
    const bool bRequestCookAfterDescribe,
    const bool bRequestInstanceBakeAfterDescribe)
{
    if (!Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    if (SourceHandle.IsEmpty() || BlendFilePath.IsEmpty())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::SourceNotLoaded, TEXT("Select a GeometryNodesBridgeSource with a valid Blend File."));
        }
        return false;
    }

    FSourceCache& SourceCache = SourceCaches.FindOrAdd(SourceHandle);
    SourceCache.BlendFilePath = BlendFilePath;

    if (SourceCache.bLoaded && !bForceReload)
    {
        if (IsValid(Component))
        {
            ApplySourceCacheToComponents(SourceHandle, bRequestCookAfterDescribe, bRequestInstanceBakeAfterDescribe, Component);
        }
        return true;
    }

    for (TPair<FString, FPendingRequest>& Entry : PendingRequests)
    {
        FPendingRequest& ExistingRequest = Entry.Value;
        if (ExistingRequest.Kind != EBridgeManagerRequestKind::EnsureSourceLoaded
            || ExistingRequest.SourceHandle != SourceHandle)
        {
            continue;
        }

        if (bForceReload && !ExistingRequest.bForceReload)
        {
            continue;
        }

        if (!ExistingRequest.Component.IsValid() && IsValid(Component))
        {
            ExistingRequest.Component = Component;
        }

        ExistingRequest.bForceReload |= bForceReload;
        ExistingRequest.bRequestCookAfterDescribe |= bRequestCookAfterDescribe;
        ExistingRequest.bRequestInstanceBakeAfterDescribe |= bRequestInstanceBakeAfterDescribe;
        return true;
    }

    const FString RequestId = MakeRequestId();
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(RequestIdFieldName, RequestId);
    Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    Payload->SetStringField(BlendFileFieldName, BlendFilePath);
    Payload->SetBoolField(ForceReloadFieldName, bForceReload);

    FPendingRequest PendingRequest;
    PendingRequest.Kind = EBridgeManagerRequestKind::EnsureSourceLoaded;
    PendingRequest.Component = Component;
    PendingRequest.SourceHandle = SourceHandle;
    PendingRequest.bForceReload = bForceReload;
    PendingRequest.bRequestCookAfterDescribe = bRequestCookAfterDescribe;
    PendingRequest.bRequestInstanceBakeAfterDescribe = bRequestInstanceBakeAfterDescribe;
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    if (IsValid(Component))
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::SchemaPending, TEXT("Loading source into the global Blender session."));
    }

    return Session->QueueMessage(EGeometryNodesBridgeMessageType::EnsureSourceLoaded, Payload);
}

bool FGeometryNodesBridgeSessionManager::RequestDescribeTarget(
    UGeometryNodesBridgeComponent* Component,
    const bool bPreserveInputValues,
    const bool bRequestCookAfterDescribe,
    const bool bRequestInstanceBakeAfterDescribe)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        return RequestEnsureSourceLoaded(Component, false, bRequestCookAfterDescribe, bRequestInstanceBakeAfterDescribe);
    }

    if (Component->TargetId.IsEmpty() || Component->FindTargetDescriptor(Component->TargetId) == nullptr)
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return false;
    }

    if (TryApplyCachedTargetDescription(
        Component,
        bPreserveInputValues,
        bRequestCookAfterDescribe,
        bRequestInstanceBakeAfterDescribe))
    {
        return true;
    }

    for (TPair<FString, FPendingRequest>& Entry : PendingRequests)
    {
        FPendingRequest& ExistingRequest = Entry.Value;
        if (ExistingRequest.Kind != EBridgeManagerRequestKind::DescribeTarget
            || ExistingRequest.Component.Get() != Component
            || ExistingRequest.SourceHandle != SourceHandle
            || ExistingRequest.TargetId != Component->TargetId)
        {
            continue;
        }

        ExistingRequest.bPreserveInputValues |= bPreserveInputValues;
        ExistingRequest.bRequestCookAfterDescribe |= bRequestCookAfterDescribe;
        ExistingRequest.bRequestInstanceBakeAfterDescribe |= bRequestInstanceBakeAfterDescribe;
        return true;
    }

    const FString RequestId = MakeRequestId();
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(RequestIdFieldName, RequestId);
    Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    Payload->SetStringField(TargetIdFieldName, Component->TargetId);

    FPendingRequest PendingRequest;
    PendingRequest.Kind = EBridgeManagerRequestKind::DescribeTarget;
    PendingRequest.Component = Component;
    PendingRequest.SourceHandle = SourceHandle;
    PendingRequest.TargetId = Component->TargetId;
    PendingRequest.bPreserveInputValues = bPreserveInputValues;
    PendingRequest.bRequestCookAfterDescribe = bRequestCookAfterDescribe;
    PendingRequest.bRequestInstanceBakeAfterDescribe = bRequestInstanceBakeAfterDescribe;
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::SchemaPending, TEXT("Refreshing target schema from Blender."));
    return Session->QueueMessage(EGeometryNodesBridgeMessageType::DescribeTarget, Payload);
}

bool FGeometryNodesBridgeSessionManager::SendCookRequest(UGeometryNodesBridgeComponent* Component)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        return RequestEnsureSourceLoaded(Component, false, true, false);
    }

    if (Component->TargetId.IsEmpty() || Component->FindTargetDescriptor(Component->TargetId) == nullptr)
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return false;
    }

    // A target can legitimately expose zero editable inputs. Once we've described the
    // currently selected target, we should still allow a cook with an empty values payload.
    if (Component->SchemaTargetId != Component->TargetId)
    {
        return RequestDescribeTarget(Component, Component->SchemaTargetId == Component->TargetId, true, false);
    }

    FBridgeCurveOwnerData CurveOwnerData;
    FString CurveHash;
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        if (!Component->BuildCurveOwnerData(CurveOwnerData, CurveHash))
        {
            Component->SetBridgeStatus(
                EGeometryNodesBridgeComponentStatus::Error,
                TEXT("Curve-owner target requires a valid spline source before it can cook."));
            return false;
        }
    }

    TArray<FBridgeGeometryBindingPayload> GeometryBindingPayloads;
    FString GeometryBindingsHash;
    FString GeometryBindingsError;
    if (!Component->BuildGeometryBindingPayloads(GeometryBindingPayloads, GeometryBindingsHash, GeometryBindingsError))
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, GeometryBindingsError);
        return false;
    }

    if (TryApplyCachedCook(Component))
    {
        return true;
    }

    Component->RefreshComponentHandle();
    const FString RequestId = MakeRequestId();
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(RequestIdFieldName, RequestId);
    Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    Payload->SetStringField(TargetIdFieldName, Component->TargetId);
    Payload->SetStringField(ComponentHandleFieldName, Component->GetComponentHandle());
    GeometryNodesBridge::AddInputValuesToPayload(Component->InputSchema, Component->InputValues, Payload);
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        Payload->SetStringField(CurveHashFieldName, CurveHash);
        GeometryNodesBridge::AddCurveOwnerDataToPayload(CurveOwnerData, Payload);
    }
    if (!GeometryBindingPayloads.IsEmpty())
    {
        GeometryNodesBridge::AddGeometryBindingPayloadsToPayload(GeometryBindingPayloads, Payload);
    }

    FPendingRequest PendingRequest;
    PendingRequest.Kind = EBridgeManagerRequestKind::CookTarget;
    PendingRequest.Component = Component;
    PendingRequest.SourceHandle = SourceHandle;
    PendingRequest.TargetId = Component->TargetId;
    PendingRequest.RunLabel = Component->GetAutomationRunLabel();
    PendingRequest.ScenarioId = Component->GetAutomationScenarioId();
    PendingRequest.ActionKind = Component->GetAutomationActionKind();
    PendingRequest.IterationIndex = Component->GetAutomationIterationIndex();
    BuildCookCacheKey(
        SourceHandle,
        Component->TargetId,
        Component->InputSchema,
        Component->InputValues,
        CurveHash,
        GeometryBindingsHash,
        PendingRequest.CookCacheKey);
    PendingRequest.SentTimeSeconds = FPlatformTime::Seconds();
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Cooking, TEXT("Cooking selected target."));
    if (Session->QueueMessage(EGeometryNodesBridgeMessageType::CookTarget, Payload))
    {
        TrackComponentState(Component);
        return true;
    }

    return false;
}

bool FGeometryNodesBridgeSessionManager::SendInstanceBakeRequest(UGeometryNodesBridgeComponent* Component)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->bPendingInstancedBake = false;
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        return RequestEnsureSourceLoaded(Component, false, false, true);
    }

    if (Component->TargetId.IsEmpty() || Component->FindTargetDescriptor(Component->TargetId) == nullptr)
    {
        Component->bPendingInstancedBake = false;
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return false;
    }

    if (Component->SchemaTargetId != Component->TargetId)
    {
        return RequestDescribeTarget(Component, Component->SchemaTargetId == Component->TargetId, false, true);
    }

    if (TryApplyCachedInstanceBake(Component))
    {
        const double BakeExecuteStartSeconds = FPlatformTime::Seconds();
        const bool bBakeSucceeded = FGeometryNodesBridgeRuntimeModule::Get().ExecuteBakeBridgeComponent(Component);
        const double BakeExecuteMs = (FPlatformTime::Seconds() - BakeExecuteStartSeconds) * 1000.0;

        if (ShouldLogCookTimings())
        {
            UE_LOG(
                LogGeometryNodesBridge,
                Log,
                TEXT("[GeometryNodesBridgeTiming] kind=instance_bake cache_hit=true actor=%s target=%s bake_execute=%s result=%s"),
                *GetNameSafe(Component->GetOwner()),
                *Component->TargetId,
                *FormatTimingMs(BakeExecuteMs),
                bBakeSucceeded ? TEXT("success") : TEXT("failure"));

            FBridgeTimingHistoryRow HistoryRow;
            HistoryRow.TimestampUtc = FDateTime::UtcNow().ToIso8601();
            HistoryRow.Kind = TEXT("instance_bake");
            HistoryRow.bCacheHit = true;
            PopulateTimingHistoryMetadataFromComponent(Component, HistoryRow);
            HistoryRow.ActorName = GetNameSafe(Component->GetOwner());
            HistoryRow.TargetId = Component->TargetId;
            HistoryRow.Result = bBakeSucceeded ? TEXT("success") : TEXT("failure");
            HistoryRow.BakeExecuteMs = BakeExecuteMs;
            AppendTimingHistoryRow(HistoryRow);
        }

        if (bBakeSucceeded)
        {
            Component->bPendingInstancedBake = false;
            return true;
        }

        Component->bPendingInstancedBake = false;
        if (Component->ComponentStatus != EGeometryNodesBridgeComponentStatus::Error)
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to bake the bridge output as instanced geometry."));
        }
        return false;
    }

    FBridgeCurveOwnerData CurveOwnerData;
    FString CurveHash;
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        if (!Component->BuildCurveOwnerData(CurveOwnerData, CurveHash))
        {
            Component->bPendingInstancedBake = false;
            Component->SetBridgeStatus(
                EGeometryNodesBridgeComponentStatus::Error,
                TEXT("Curve-owner target requires a valid spline source before it can bake instances."));
            return false;
        }
    }

    TArray<FBridgeGeometryBindingPayload> GeometryBindingPayloads;
    FString GeometryBindingsHash;
    FString GeometryBindingsError;
    if (!Component->BuildGeometryBindingPayloads(GeometryBindingPayloads, GeometryBindingsHash, GeometryBindingsError))
    {
        Component->bPendingInstancedBake = false;
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, GeometryBindingsError);
        return false;
    }

    Component->RefreshComponentHandle();
    const FString RequestId = MakeRequestId();
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(RequestIdFieldName, RequestId);
    Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    Payload->SetStringField(TargetIdFieldName, Component->TargetId);
    Payload->SetStringField(ComponentHandleFieldName, Component->GetComponentHandle());
    GeometryNodesBridge::AddInputValuesToPayload(Component->InputSchema, Component->InputValues, Payload);
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        Payload->SetStringField(CurveHashFieldName, CurveHash);
        GeometryNodesBridge::AddCurveOwnerDataToPayload(CurveOwnerData, Payload);
    }
    if (!GeometryBindingPayloads.IsEmpty())
    {
        GeometryNodesBridge::AddGeometryBindingPayloadsToPayload(GeometryBindingPayloads, Payload);
    }

    FPendingRequest PendingRequest;
    PendingRequest.Kind = EBridgeManagerRequestKind::RequestInstanceBake;
    PendingRequest.Component = Component;
    PendingRequest.SourceHandle = SourceHandle;
    PendingRequest.TargetId = Component->TargetId;
    PendingRequest.RunLabel = Component->GetAutomationRunLabel();
    PendingRequest.ScenarioId = Component->GetAutomationScenarioId();
    PendingRequest.ActionKind = Component->GetAutomationActionKind();
    PendingRequest.IterationIndex = Component->GetAutomationIterationIndex();
    BuildCookCacheKey(
        SourceHandle,
        Component->TargetId,
        Component->InputSchema,
        Component->InputValues,
        CurveHash,
        GeometryBindingsHash,
        PendingRequest.CookCacheKey);
    PendingRequest.SentTimeSeconds = FPlatformTime::Seconds();
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Cooking, TEXT("Preparing instanced bake payload."));
    if (Session->QueueMessage(EGeometryNodesBridgeMessageType::RequestInstanceBake, Payload))
    {
        TrackComponentState(Component);
        return true;
    }

    Component->bPendingInstancedBake = false;
    return false;
}

void FGeometryNodesBridgeSessionManager::RestartSessionForResize(
    UGeometryNodesBridgeComponent* RetryComponent,
    const EBridgeManagerRequestKind RetryKind,
    const uint64 RequiredSlotBytes)
{
    UGeometryNodesBridgeSettings* Settings = GetMutableDefault<UGeometryNodesBridgeSettings>();
    const uint64 CurrentTotalBytes = Settings != nullptr
        ? static_cast<uint64>(FMath::Max<int64>(Settings->SharedMemoryBytes, static_cast<int64>(GeometryNodesBridge::DefaultSharedMemoryBytes)))
        : GeometryNodesBridge::DefaultSharedMemoryBytes;
    const uint64 ExpandedSharedMemoryBytes = ComputeExpandedSharedMemoryBytes(CurrentTotalBytes, RequiredSlotBytes);
    if (ExpandedSharedMemoryBytes <= CurrentTotalBytes)
    {
        if (IsValid(RetryComponent))
        {
            RetryComponent->SetBridgeStatus(
                EGeometryNodesBridgeComponentStatus::Error,
                FString::Printf(
                    TEXT("ResizeRequired requested %llu bytes per slot, but the current shared-memory budget of %llu bytes is already large enough."),
                    RequiredSlotBytes,
                    CurrentTotalBytes));
        }
        return;
    }

    if (Settings != nullptr)
    {
        Settings->SharedMemoryBytes = static_cast<int64>(ExpandedSharedMemoryBytes);
        Settings->SaveConfig();
    }

    if (IsValid(RetryComponent))
    {
        if (RetryKind == EBridgeManagerRequestKind::RequestInstanceBake)
        {
            PendingInstanceBakeRetryComponents.Add(RetryComponent);
        }
        else
        {
            PendingCookRetryComponents.Add(RetryComponent);
        }
        RetryComponent->SetBridgeStatus(
            EGeometryNodesBridgeComponentStatus::SchemaPending,
            FString::Printf(
                TEXT("Growing shared memory from %llu to %llu bytes. Restarting the global session and retrying the pending bridge request."),
                CurrentTotalBytes,
                ExpandedSharedMemoryBytes));
    }

    bRestoreSourcesAfterReconnect = true;
    RestartSession();
}

FString FGeometryNodesBridgeSessionManager::MakeRequestId()
{
    return FString::Printf(TEXT("Request_%llu"), NextRequestId++);
}

FString FGeometryNodesBridgeSessionManager::ResolveWorkerScriptPath(FString& OutResolutionNote) const
{
    OutResolutionNote.Reset();

    FString ConfiguredPath;
    if (const UGeometryNodesBridgeSettings* Settings = GetDefault<UGeometryNodesBridgeSettings>())
    {
        ConfiguredPath = Settings->WorkerScriptPath.FilePath;
    }

    ConfiguredPath = ConfiguredPath.TrimStartAndEnd();
    if (!ConfiguredPath.IsEmpty())
    {
        if (FPaths::IsRelative(ConfiguredPath))
        {
            ConfiguredPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), ConfiguredPath);
        }

        FPaths::NormalizeFilename(ConfiguredPath);
        if (FPaths::DirectoryExists(ConfiguredPath))
        {
            ConfiguredPath = FPaths::Combine(ConfiguredPath, TEXT("geometry_nodes_bridge_server.py"));
            FPaths::NormalizeFilename(ConfiguredPath);
        }

        if (IFileManager::Get().FileExists(*ConfiguredPath))
        {
            return ConfiguredPath;
        }

        OutResolutionNote = FString::Printf(
            TEXT("Configured Bridge Worker Script Path does not exist: %s"),
            *ConfiguredPath);
        return FString();
    }

    if (const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("GeometryNodesBridge")))
    {
        FString BundledPath = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Scripts/geometry_nodes_bridge_server.py"));
        FPaths::NormalizeFilename(BundledPath);
        if (IFileManager::Get().FileExists(*BundledPath))
        {
            return BundledPath;
        }

        OutResolutionNote = FString::Printf(
            TEXT("Bundled Blender bridge worker script was not found: %s"),
            *BundledPath);
        return FString();
    }

    OutResolutionNote = TEXT("Could not locate the GeometryNodesBridge plugin to resolve the bundled Blender bridge worker script.");
    return FString();
}
