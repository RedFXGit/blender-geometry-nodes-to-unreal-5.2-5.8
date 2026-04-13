void FGeometryNodesBridgeSessionManager::StartSessionInternal()
{
    UGeometryNodesBridgeSettings* Settings = GetMutableDefault<UGeometryNodesBridgeSettings>();
    const FString WorkerScriptPath = ResolveWorkerScriptPath();
    if (WorkerScriptPath.IsEmpty())
    {
        LastSessionMessage = TEXT("Could not resolve Blender bridge worker script path.");
        SessionState = EGeometryNodesBridgeConnectionState::Error;
        RefreshAllComponentConnectionStates();
        return;
    }

    FString BlenderExecutableResolutionNote;
    const FString BlenderExecutablePath = ResolveBlenderExecutablePath(Settings, BlenderExecutableResolutionNote);
    if (BlenderExecutablePath.IsEmpty())
    {
        LastSessionMessage = BlenderExecutableResolutionNote.IsEmpty()
            ? TEXT("Blender executable path is not configured.")
            : BlenderExecutableResolutionNote;
        SessionState = EGeometryNodesBridgeConnectionState::Error;
        RefreshAllComponentConnectionStates();
        return;
    }

    FGeometryNodesBridgeWorkerConfig Config;
    Config.BlenderExecutablePath = BlenderExecutablePath;
    Config.WorkerScriptPath = WorkerScriptPath;
    Config.SharedMemoryBytes = Settings != nullptr
        ? static_cast<uint64>(FMath::Max<int64>(Settings->SharedMemoryBytes, static_cast<int64>(GeometryNodesBridge::DefaultSharedMemoryBytes)))
        : GeometryNodesBridge::DefaultSharedMemoryBytes;

    Session = MakeShared<FGeometryNodesBridgeWorkerSession>();
    Session->OnStateChanged.AddRaw(this, &FGeometryNodesBridgeSessionManager::HandleSessionStateChanged);
    Session->OnMessageReceived.AddRaw(this, &FGeometryNodesBridgeSessionManager::HandleSessionMessageReceived);
    Session->OnLogMessage.AddRaw(this, &FGeometryNodesBridgeSessionManager::HandleSessionLogMessage);
    if (!Session->Start(Config))
    {
        Session->OnStateChanged.RemoveAll(this);
        Session->OnMessageReceived.RemoveAll(this);
        Session->OnLogMessage.RemoveAll(this);
        Session.Reset();
        SessionState = EGeometryNodesBridgeConnectionState::Error;
        LastSessionMessage = TEXT("Failed to start the global Blender session.");
        RefreshAllComponentConnectionStates();
        return;
    }

    SessionState = EGeometryNodesBridgeConnectionState::Launching;
    LastSessionMessage = BlenderExecutableResolutionNote.IsEmpty()
        ? TEXT("Launching global Blender session.")
        : BlenderExecutableResolutionNote;
    RefreshAllComponentConnectionStates();
}

void FGeometryNodesBridgeSessionManager::HandleSessionStateChanged(const EGeometryNodesBridgeConnectionState NewState)
{
    SessionState = NewState;

    switch (NewState)
    {
    case EGeometryNodesBridgeConnectionState::Connected:
        LastSessionMessage = TEXT("Global Blender session connected.");
        break;
    case EGeometryNodesBridgeConnectionState::Launching:
        LastSessionMessage = TEXT("Launching global Blender session.");
        break;
    case EGeometryNodesBridgeConnectionState::Disconnected:
        LastSessionMessage = TEXT("Global Blender session disconnected.");
        break;
    case EGeometryNodesBridgeConnectionState::Error:
        LastSessionMessage = TEXT("Global Blender session encountered an error.");
        break;
    default:
        break;
    }

    RefreshAllComponentConnectionStates();

    if (NewState == EGeometryNodesBridgeConnectionState::Connected && bRestoreSourcesAfterReconnect)
    {
        bRestoreSourcesAfterReconnect = false;

        const TMap<FString, FString> SourcesToRestore = GatherSourcesToRestore();
        for (const TPair<FString, FString>& Entry : SourcesToRestore)
        {
            QueueEnsureSourceLoadedRequest(Entry.Key, Entry.Value, nullptr, false, false, false);
        }
    }

    if (NewState == EGeometryNodesBridgeConnectionState::Connected && !PendingWarmupPreviewComponents.IsEmpty())
    {
        TMap<FString, TWeakObjectPtr<UGeometryNodesBridgeComponent>> WarmupPreviewComponents = MoveTemp(PendingWarmupPreviewComponents);
        PendingWarmupPreviewComponents.Reset();

        for (const TPair<FString, TWeakObjectPtr<UGeometryNodesBridgeComponent>>& Entry : WarmupPreviewComponents)
        {
            if (!Entry.Value.IsValid())
            {
                continue;
            }

            QueueEnsureSourceLoadedRequest(Entry.Key, Entry.Key, Entry.Value.Get(), false, Entry.Value.IsValid(), false);
        }
    }
}

