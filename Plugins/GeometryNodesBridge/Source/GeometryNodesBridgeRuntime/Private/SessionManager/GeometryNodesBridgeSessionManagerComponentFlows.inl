FGeometryNodesBridgeSessionManager::FGeometryNodesBridgeSessionManager() = default;

FGeometryNodesBridgeSessionManager::~FGeometryNodesBridgeSessionManager()
{
    Shutdown();
}

bool FGeometryNodesBridgeSessionManager::CreateSession()
{
    if (!IsEditorBridgeSupported())
    {
        LastSessionMessage = TEXT("Geometry Nodes Bridge shared sessions are only supported in the Unreal Editor.");
        SessionState = EGeometryNodesBridgeConnectionState::Error;
        RefreshAllComponentConnectionStates();
        return false;
    }

    if (Session.IsValid() && SessionState == EGeometryNodesBridgeConnectionState::Error)
    {
        LastSessionMessage = TEXT("Restarting the global Blender session after the previous error.");
        RestartSession();
        return Session.IsValid();
    }

    if (Session.IsValid() && SessionState != EGeometryNodesBridgeConnectionState::Disconnected)
    {
        LastSessionMessage = TEXT("Global Blender session is already running.");
        return true;
    }

    StartSessionInternal();
    return Session.IsValid();
}

void FGeometryNodesBridgeSessionManager::StopSession()
{
    PendingRequests.Reset();
    PendingCookRetryComponents.Reset();
    PendingInstanceBakeRetryComponents.Reset();
    PendingWarmupPreviewComponents.Reset();
    TargetSchemaCaches.Reset();
    CookedTargetCaches.Reset();
    TrackedComponentStateKeys.Reset();
    bRestoreSourcesAfterReconnect = false;

    if (Session.IsValid())
    {
        Session->OnStateChanged.RemoveAll(this);
        Session->OnMessageReceived.RemoveAll(this);
        Session->OnLogMessage.RemoveAll(this);
        Session->Stop();
        Session.Reset();
    }

    for (TPair<FString, FSourceCache>& Entry : SourceCaches)
    {
        Entry.Value.bLoaded = false;
    }

    SessionState = EGeometryNodesBridgeConnectionState::Disconnected;
    LastSessionMessage = TEXT("Global Blender session stopped.");
    RefreshAllComponentConnectionStates();

    ForEachBridgeComponent([](UGeometryNodesBridgeComponent* Component)
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
    });
}

bool FGeometryNodesBridgeSessionManager::RestartSession()
{
    const TArray<TWeakObjectPtr<UGeometryNodesBridgeComponent>> RetryComponents = PendingCookRetryComponents;
    const TArray<TWeakObjectPtr<UGeometryNodesBridgeComponent>> PendingInstanceRetryComponents = PendingInstanceBakeRetryComponents;
    TMap<FString, TWeakObjectPtr<UGeometryNodesBridgeComponent>> WarmupPreviewComponents;
    for (const TPair<FString, TWeakObjectPtr<UGeometryNodesBridgeComponent>>& Entry : PendingWarmupPreviewComponents)
    {
        if (Entry.Value.IsValid())
        {
            WarmupPreviewComponents.Add(Entry.Key, Entry.Value);
        }
    }
    StopSession();
    PendingCookRetryComponents = RetryComponents;
    PendingInstanceBakeRetryComponents = PendingInstanceRetryComponents;
    PendingWarmupPreviewComponents = WarmupPreviewComponents;
    bRestoreSourcesAfterReconnect = true;
    StartSessionInternal();
    return Session.IsValid();
}

void FGeometryNodesBridgeSessionManager::Shutdown()
{
    StopSession();
}

void FGeometryNodesBridgeSessionManager::ReleaseComponentState(UGeometryNodesBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    for (auto It = PendingRequests.CreateIterator(); It; ++It)
    {
        if (It.Value().Component.Get() == Component)
        {
            It.RemoveCurrent();
        }
    }

    if (!Session.IsValid() || !Session->IsConnected())
    {
        return;
    }

    Component->RefreshComponentHandle();
    const FString TrackedComponentStateKey = MakeTrackedComponentStateKey(Component);
    if (TrackedComponentStateKey.IsEmpty() || !TrackedComponentStateKeys.Contains(TrackedComponentStateKey))
    {
        return;
    }

    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(ComponentHandleFieldName, Component->GetComponentHandle());

    const FString SourceHandle = Component->GetSourceHandle();
    if (!SourceHandle.IsEmpty())
    {
        Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    }

    if (Session->QueueMessage(EGeometryNodesBridgeMessageType::ReleaseComponentState, Payload))
    {
        TrackedComponentStateKeys.Remove(TrackedComponentStateKey);
    }
}

FString FGeometryNodesBridgeSessionManager::MakeTrackedComponentStateKey(const UGeometryNodesBridgeComponent* Component) const
{
    if (!IsValid(Component))
    {
        return FString();
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FString ComponentHandle = Component->GetComponentHandle();
    if (SourceHandle.IsEmpty() || ComponentHandle.IsEmpty())
    {
        return FString();
    }

    return FString::Printf(TEXT("%s%s%s"), *SourceHandle, CacheKeyDelimiter, *ComponentHandle);
}

void FGeometryNodesBridgeSessionManager::TrackComponentState(UGeometryNodesBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    Component->RefreshComponentHandle();
    const FString TrackedComponentStateKey = MakeTrackedComponentStateKey(Component);
    if (!TrackedComponentStateKey.IsEmpty())
    {
        TrackedComponentStateKeys.Add(TrackedComponentStateKey);
    }
}

void FGeometryNodesBridgeSessionManager::NotifySourceChanged(UGeometryNodesBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    ReleaseComponentState(Component);
    Component->ResetBridgeData(true);

    if (Component->Source == nullptr || GetComponentBlendFilePath(Component).IsEmpty())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::SourceNotLoaded, TEXT("Select a GeometryNodesBridgeSource with a valid Blend File."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    RequestEnsureSourceLoaded(Component, false, false, false);
}

void FGeometryNodesBridgeSessionManager::WarmSourceOnPreviewDrag(UGeometryNodesBridgeSource* Source, UGeometryNodesBridgeComponent* PreviewComponent)
{
    if (!IsEditorBridgeSupported())
    {
        return;
    }

    FString SourceHandle;
    FString BlendFilePath;
    if (!TryResolveSourceHandleAndBlendFilePath(Source, SourceHandle, BlendFilePath))
    {
        return;
    }

    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache != nullptr && SourceCache->bLoaded)
    {
        QueueEnsureSourceLoadedRequest(SourceHandle, BlendFilePath, PreviewComponent, false, true, false);
        return;
    }

    if (Session.IsValid() && Session->IsConnected())
    {
        QueueEnsureSourceLoadedRequest(SourceHandle, BlendFilePath, PreviewComponent, false, true, false);
        return;
    }

    PendingWarmupPreviewComponents.Add(SourceHandle, PreviewComponent);

    if (SessionState == EGeometryNodesBridgeConnectionState::Disconnected || SessionState == EGeometryNodesBridgeConnectionState::Error)
    {
        CreateSession();
    }
}

void FGeometryNodesBridgeSessionManager::NotifyTargetChanged(UGeometryNodesBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    ReleaseComponentState(Component);
    Component->CancelPendingSplineCook();
    Component->SchemaTargetId.Reset();
    Component->InputSchema.Reset();
    Component->GeometryInputDescriptors.Reset();
    Component->GeometryBindings.Reset();
    Component->ApplyCurveOwnerDescription(Component->TargetId, FBridgeTargetCapabilities(), false);
    Component->RebuildInputValues(false);
    Component->UpdateTargetDisplayLabel();
    if (Component->PreviewMeshComponent != nullptr)
    {
        Component->PreviewMeshComponent->ClearAllMeshSections();
    }

    if (Component->TargetId.IsEmpty())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    Component->SetShowingBakedOutput(false);
    RequestDescribeTarget(Component, false, true, false);
}

void FGeometryNodesBridgeSessionManager::RefreshComponentSchema(UGeometryNodesBridgeComponent* Component, const bool bForceReload)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    Component->EnsurePreviewMeshComponent();

    if (Component->Source == nullptr || GetComponentBlendFilePath(Component).IsEmpty())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::SourceNotLoaded, TEXT("Select a GeometryNodesBridgeSource with a valid Blend File."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (!bForceReload && SourceCache != nullptr && SourceCache->bLoaded)
    {
        ApplySourceCacheToComponents(SourceHandle, false, false, nullptr);
        return;
    }

    RequestEnsureSourceLoaded(Component, bForceReload, false, false);
}

void FGeometryNodesBridgeSessionManager::RequestCook(UGeometryNodesBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    Component->EnsurePreviewMeshComponent();

    if (Component->Source == nullptr || GetComponentBlendFilePath(Component).IsEmpty())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::SourceNotLoaded, TEXT("Select a GeometryNodesBridgeSource with a valid Blend File."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        RequestEnsureSourceLoaded(Component, false, true, false);
        return;
    }

    ApplySourceCacheToComponents(SourceHandle, false, false, nullptr);
    SendCookRequest(Component);
}

void FGeometryNodesBridgeSessionManager::RequestInstanceBake(UGeometryNodesBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    Component->SetShowingBakedOutput(false);

    if (Component->Source == nullptr || GetComponentBlendFilePath(Component).IsEmpty())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::SourceNotLoaded, TEXT("Select a GeometryNodesBridgeSource with a valid Blend File."));
        Component->bPendingInstancedBake = false;
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        Component->bPendingInstancedBake = false;
        return;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        RequestEnsureSourceLoaded(Component, false, false, true);
        return;
    }

    ApplySourceCacheToComponents(SourceHandle, false, false, nullptr);
    SendInstanceBakeRequest(Component);
}

bool FGeometryNodesBridgeSessionManager::IsSessionConnected() const
{
    return Session.IsValid() && Session->IsConnected();
}

const FBridgeDecodedInstanceBake* FGeometryNodesBridgeSessionManager::FindCachedInstanceBake(const UGeometryNodesBridgeComponent* Component) const
{
    if (!IsValid(Component))
    {
        return nullptr;
    }

    FString CurveHash;
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        FBridgeCurveOwnerData CurveData;
        if (!Component->BuildCurveOwnerData(CurveData, CurveHash))
        {
            return nullptr;
        }
    }

    TArray<FBridgeGeometryBindingPayload> GeometryBindingPayloads;
    FString GeometryBindingsHash;
    FString GeometryBindingsError;
    if (!Component->BuildGeometryBindingPayloads(GeometryBindingPayloads, GeometryBindingsHash, GeometryBindingsError))
    {
        return nullptr;
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
        return nullptr;
    }

    const FCookedTargetCache* CachedCook = CookedTargetCaches.Find(CookCacheKey);
    if (CachedCook == nullptr || CachedCook->DecodedInstanceBake.Prototypes.IsEmpty() || CachedCook->DecodedInstanceBake.Placements.IsEmpty())
    {
        return nullptr;
    }

    return &CachedCook->DecodedInstanceBake;
}

