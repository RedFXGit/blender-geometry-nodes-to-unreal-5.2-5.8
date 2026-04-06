#include "BlenderBridgeSessionManager.h"

#include "BlenderBridgeComponent.h"
#include "BlenderBridgeMessages.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSettings.h"
#include "BlenderBridgeSource.h"
#include "BlenderBridgeWorkerSession.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "ProceduralMeshComponent.h"
#include "UObject/UObjectIterator.h"

namespace
{
    constexpr TCHAR RequestIdFieldName[] = TEXT("request_id");
    constexpr TCHAR SourceHandleFieldName[] = TEXT("source_handle");
    constexpr TCHAR BlendFileFieldName[] = TEXT("blend_file");
    constexpr TCHAR TargetIdFieldName[] = TEXT("target_id");
    constexpr TCHAR ForceReloadFieldName[] = TEXT("force_reload");

    uint64 ComputeExpandedSharedMemoryBytes(const uint64 CurrentTotalBytes, const uint64 RequiredSlotBytes)
    {
        const uint64 MinRequiredTotalBytes = sizeof(FBridgeSharedHeader) + (RequiredSlotBytes * BlenderBridge::SharedSlotCount);
        uint64 ExpandedBytes = FMath::Max(CurrentTotalBytes, BlenderBridge::DefaultSharedMemoryBytes);

        while (ExpandedBytes < MinRequiredTotalBytes)
        {
            ExpandedBytes *= 2ull;
        }

        return ExpandedBytes;
    }

    template <typename CallbackType>
    void ForEachBridgeComponent(const CallbackType& Callback)
    {
        for (TObjectIterator<UBlenderBridgeComponent> It; It; ++It)
        {
            UBlenderBridgeComponent* Component = *It;
            if (!IsValid(Component) || Component->HasAnyFlags(RF_ClassDefaultObject))
            {
                continue;
            }

            Callback(Component);
        }
    }

    FString GetComponentBlendFilePath(const UBlenderBridgeComponent* Component)
    {
        if (Component == nullptr || Component->Source == nullptr)
        {
            return FString();
        }

        return FPaths::ConvertRelativePathToFull(Component->Source->BlendFile.FilePath);
    }

    bool TryGetRequestId(const TSharedPtr<FJsonObject>& Payload, FString& OutRequestId)
    {
        OutRequestId.Reset();
        return Payload.IsValid() && Payload->TryGetStringField(RequestIdFieldName, OutRequestId) && !OutRequestId.IsEmpty();
    }

    bool IsRetryComponent(const TArray<TWeakObjectPtr<UBlenderBridgeComponent>>& PendingRetryComponents, const UBlenderBridgeComponent* Component)
    {
        return PendingRetryComponents.ContainsByPredicate([Component](const TWeakObjectPtr<UBlenderBridgeComponent>& Entry)
        {
            return Entry.IsValid() && Entry.Get() == Component;
        });
    }

    bool NormalizeExecutableCandidate(FString& InOutPath)
    {
        InOutPath = InOutPath.TrimStartAndEnd();
        if (InOutPath.IsEmpty())
        {
            return false;
        }

        FPaths::NormalizeFilename(InOutPath);
        if (FPaths::DirectoryExists(InOutPath))
        {
            InOutPath = FPaths::Combine(InOutPath, TEXT("blender.exe"));
        }

        return IFileManager::Get().FileExists(*InOutPath);
    }

    TArray<int32> ParseVersionKey(const FString& DirectoryName)
    {
        FString VersionText = DirectoryName;
        VersionText.RemoveFromStart(TEXT("Blender "));

        TArray<FString> Parts;
        VersionText.ParseIntoArray(Parts, TEXT("."), true);

        TArray<int32> VersionParts;
        VersionParts.Reserve(Parts.Num());
        for (const FString& Part : Parts)
        {
            VersionParts.Add(FCString::Atoi(*Part));
        }

        return VersionParts;
    }

    bool IsVersionKeyGreater(const TArray<int32>& Left, const TArray<int32>& Right)
    {
        const int32 MaxCount = FMath::Max(Left.Num(), Right.Num());
        for (int32 Index = 0; Index < MaxCount; ++Index)
        {
            const int32 LeftValue = Left.IsValidIndex(Index) ? Left[Index] : 0;
            const int32 RightValue = Right.IsValidIndex(Index) ? Right[Index] : 0;
            if (LeftValue != RightValue)
            {
                return LeftValue > RightValue;
            }
        }

        return false;
    }

    FString FindDefaultBlenderExecutable()
    {
        const FString BaseDirectory = TEXT("C:/Program Files/Blender Foundation");
        if (!IFileManager::Get().DirectoryExists(*BaseDirectory))
        {
            return FString();
        }

        TArray<FString> CandidateDirectories;
        IFileManager::Get().FindFiles(CandidateDirectories, *(BaseDirectory / TEXT("Blender *")), false, true);

        FString BestExecutablePath;
        TArray<int32> BestVersionKey;
        for (const FString& DirectoryName : CandidateDirectories)
        {
            FString CandidatePath = FPaths::Combine(BaseDirectory, DirectoryName, TEXT("blender.exe"));
            if (!IFileManager::Get().FileExists(*CandidatePath))
            {
                continue;
            }

            const TArray<int32> CandidateVersionKey = ParseVersionKey(DirectoryName);
            if (BestExecutablePath.IsEmpty() || IsVersionKeyGreater(CandidateVersionKey, BestVersionKey))
            {
                BestExecutablePath = MoveTemp(CandidatePath);
                BestVersionKey = CandidateVersionKey;
            }
        }

        return BestExecutablePath;
    }

    FString ResolveBlenderExecutablePath(UBlenderBridgeSettings* Settings, FString& OutResolutionNote)
    {
        OutResolutionNote.Reset();

        FString ConfiguredPath = Settings != nullptr ? Settings->BlenderExecutablePath.FilePath : FString();
        if (NormalizeExecutableCandidate(ConfiguredPath))
        {
            return ConfiguredPath;
        }

        const FString AutoDetectedPath = FindDefaultBlenderExecutable();
        if (!AutoDetectedPath.IsEmpty())
        {
            OutResolutionNote = FString::Printf(TEXT("Auto-detected Blender executable: %s"), *AutoDetectedPath);

            if (Settings != nullptr)
            {
                Settings->BlenderExecutablePath.FilePath = AutoDetectedPath;
                Settings->SaveConfig();
            }

            return AutoDetectedPath;
        }

        OutResolutionNote = TEXT("Blender executable path is not configured, and no default Blender install was found.");
        return FString();
    }

    bool IsEditorBridgeSupported()
    {
        return GIsEditor;
    }

    void SetEditorOnlyUnsupported(UBlenderBridgeComponent* Component)
    {
        if (IsValid(Component))
        {
            Component->ComponentStatus = EBlenderBridgeComponentStatus::Error;
            Component->LastStatusMessage = TEXT("Blender Bridge shared sessions are only supported in the Unreal Editor.");
        }
    }
}

FBlenderBridgeSessionManager::FBlenderBridgeSessionManager() = default;

FBlenderBridgeSessionManager::~FBlenderBridgeSessionManager()
{
    Shutdown();
}

bool FBlenderBridgeSessionManager::CreateSession()
{
    if (!IsEditorBridgeSupported())
    {
        LastSessionMessage = TEXT("Blender Bridge shared sessions are only supported in the Unreal Editor.");
        SessionState = EBlenderBridgeConnectionState::Error;
        RefreshAllComponentConnectionStates();
        return false;
    }

    if (Session.IsValid() && SessionState == EBlenderBridgeConnectionState::Error)
    {
        LastSessionMessage = TEXT("Restarting the global Blender session after the previous error.");
        RestartSession();
        return Session.IsValid();
    }

    if (Session.IsValid() && SessionState != EBlenderBridgeConnectionState::Disconnected)
    {
        LastSessionMessage = TEXT("Global Blender session is already running.");
        return true;
    }

    StartSessionInternal();
    return Session.IsValid();
}

void FBlenderBridgeSessionManager::StopSession()
{
    PendingRequests.Reset();
    PendingCookRetryComponents.Reset();
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

    SessionState = EBlenderBridgeConnectionState::Disconnected;
    LastSessionMessage = TEXT("Global Blender session stopped.");
    RefreshAllComponentConnectionStates();

    ForEachBridgeComponent([](UBlenderBridgeComponent* Component)
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
    });
}

bool FBlenderBridgeSessionManager::RestartSession()
{
    const TArray<TWeakObjectPtr<UBlenderBridgeComponent>> RetryComponents = PendingCookRetryComponents;
    StopSession();
    PendingCookRetryComponents = RetryComponents;
    bRestoreSourcesAfterReconnect = true;
    StartSessionInternal();
    return Session.IsValid();
}

void FBlenderBridgeSessionManager::Shutdown()
{
    StopSession();
}

void FBlenderBridgeSessionManager::NotifySourceChanged(UBlenderBridgeComponent* Component)
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

    Component->ResetBridgeData(true);

    if (Component->Source == nullptr || GetComponentBlendFilePath(Component).IsEmpty())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    RequestEnsureSourceLoaded(Component, false, false);
}

void FBlenderBridgeSessionManager::NotifyTargetChanged(UBlenderBridgeComponent* Component)
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

    Component->SchemaTargetId.Reset();
    Component->InputSchema.Reset();
    Component->RebuildInputValues(false);
    Component->UpdateTargetDisplayLabel();
    if (Component->PreviewMeshComponent != nullptr)
    {
        Component->PreviewMeshComponent->ClearAllMeshSections();
    }

    if (Component->TargetId.IsEmpty())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    Component->SetShowingBakedOutput(false);
    RequestDescribeTarget(Component, false, true);
}

void FBlenderBridgeSessionManager::RefreshComponentSchema(UBlenderBridgeComponent* Component, const bool bForceReload)
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
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (!bForceReload && SourceCache != nullptr && SourceCache->bLoaded)
    {
        ApplySourceCacheToComponents(SourceHandle, false, nullptr);
        return;
    }

    RequestEnsureSourceLoaded(Component, bForceReload, false);
}

void FBlenderBridgeSessionManager::RequestCook(UBlenderBridgeComponent* Component)
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
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        RequestEnsureSourceLoaded(Component, false, true);
        return;
    }

    ApplySourceCacheToComponents(SourceHandle, false, nullptr);
    SendCookRequest(Component);
}

bool FBlenderBridgeSessionManager::IsSessionConnected() const
{
    return Session.IsValid() && Session->IsConnected();
}

void FBlenderBridgeSessionManager::StartSessionInternal()
{
    UBlenderBridgeSettings* Settings = GetMutableDefault<UBlenderBridgeSettings>();
    const FString WorkerScriptPath = ResolveWorkerScriptPath();
    if (WorkerScriptPath.IsEmpty())
    {
        LastSessionMessage = TEXT("Could not resolve Blender bridge worker script path.");
        SessionState = EBlenderBridgeConnectionState::Error;
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
        SessionState = EBlenderBridgeConnectionState::Error;
        RefreshAllComponentConnectionStates();
        return;
    }

    FBlenderBridgeWorkerConfig Config;
    Config.BlenderExecutablePath = BlenderExecutablePath;
    Config.WorkerScriptPath = WorkerScriptPath;
    Config.SharedMemoryBytes = Settings != nullptr
        ? static_cast<uint64>(FMath::Max<int64>(Settings->SharedMemoryBytes, static_cast<int64>(BlenderBridge::DefaultSharedMemoryBytes)))
        : BlenderBridge::DefaultSharedMemoryBytes;

    Session = MakeShared<FBlenderBridgeWorkerSession>();
    Session->OnStateChanged.AddRaw(this, &FBlenderBridgeSessionManager::HandleSessionStateChanged);
    Session->OnMessageReceived.AddRaw(this, &FBlenderBridgeSessionManager::HandleSessionMessageReceived);
    Session->OnLogMessage.AddRaw(this, &FBlenderBridgeSessionManager::HandleSessionLogMessage);
    if (!Session->Start(Config))
    {
        Session->OnStateChanged.RemoveAll(this);
        Session->OnMessageReceived.RemoveAll(this);
        Session->OnLogMessage.RemoveAll(this);
        Session.Reset();
        SessionState = EBlenderBridgeConnectionState::Error;
        LastSessionMessage = TEXT("Failed to start the global Blender session.");
        RefreshAllComponentConnectionStates();
        return;
    }

    SessionState = EBlenderBridgeConnectionState::Launching;
    LastSessionMessage = BlenderExecutableResolutionNote.IsEmpty()
        ? TEXT("Launching global Blender session.")
        : BlenderExecutableResolutionNote;
    RefreshAllComponentConnectionStates();
}

void FBlenderBridgeSessionManager::HandleSessionStateChanged(const EBlenderBridgeConnectionState NewState)
{
    SessionState = NewState;

    switch (NewState)
    {
    case EBlenderBridgeConnectionState::Connected:
        LastSessionMessage = TEXT("Global Blender session connected.");
        break;
    case EBlenderBridgeConnectionState::Launching:
        LastSessionMessage = TEXT("Launching global Blender session.");
        break;
    case EBlenderBridgeConnectionState::Disconnected:
        LastSessionMessage = TEXT("Global Blender session disconnected.");
        break;
    case EBlenderBridgeConnectionState::Error:
        LastSessionMessage = TEXT("Global Blender session encountered an error.");
        break;
    default:
        break;
    }

    RefreshAllComponentConnectionStates();

    if (NewState == EBlenderBridgeConnectionState::Connected && bRestoreSourcesAfterReconnect)
    {
        bRestoreSourcesAfterReconnect = false;

        for (const TPair<FString, FSourceCache>& Entry : SourceCaches)
        {
            TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
            const FString RequestId = MakeRequestId();
            Payload->SetStringField(RequestIdFieldName, RequestId);
            Payload->SetStringField(SourceHandleFieldName, Entry.Key);
            Payload->SetStringField(BlendFileFieldName, Entry.Value.BlendFilePath);
            Payload->SetBoolField(ForceReloadFieldName, false);

            FPendingRequest PendingRequest;
            PendingRequest.Kind = EBridgeManagerRequestKind::EnsureSourceLoaded;
            PendingRequest.SourceHandle = Entry.Key;
            PendingRequests.Add(RequestId, MoveTemp(PendingRequest));
            Session->QueueMessage(EBlenderBridgeMessageType::EnsureSourceLoaded, Payload);
        }
    }
}

void FBlenderBridgeSessionManager::HandleSessionMessageReceived(const FBlenderBridgeJsonMessage& Message)
{
    FString RequestId;
    TryGetRequestId(Message.Payload, RequestId);

    if (Message.Type == EBlenderBridgeMessageType::SourceLoaded)
    {
        FPendingRequest PendingRequest;
        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                PendingRequest = *ExistingRequest;
                PendingRequests.Remove(RequestId);
            }
        }

        FString SourceHandle;
        if (!Message.Payload.IsValid() || !Message.Payload->TryGetStringField(SourceHandleFieldName, SourceHandle) || SourceHandle.IsEmpty())
        {
            LastSessionMessage = TEXT("SourceLoaded response was missing source_handle.");
            return;
        }

        TArray<FBridgeTargetDescriptor> Targets;
        BlenderBridge::ParseTargetDescriptors(Message.Payload, Targets);

        FSourceCache& SourceCache = SourceCaches.FindOrAdd(SourceHandle);
        if (SourceCache.BlendFilePath.IsEmpty())
        {
            Message.Payload->TryGetStringField(BlendFileFieldName, SourceCache.BlendFilePath);
        }
        SourceCache.Targets = MoveTemp(Targets);
        SourceCache.bLoaded = true;

        ApplySourceCacheToComponents(SourceHandle, PendingRequest.bRequestCookAfterDescribe, PendingRequest.Component.Get());
        return;
    }

    if (Message.Type == EBlenderBridgeMessageType::TargetDescription)
    {
        FPendingRequest PendingRequest;
        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                PendingRequest = *ExistingRequest;
                PendingRequests.Remove(RequestId);
            }
        }

        UBlenderBridgeComponent* Component = PendingRequest.Component.Get();
        if (!IsValid(Component))
        {
            return;
        }

        TArray<FBridgeInputSchema> Schema;
        if (!BlenderBridge::ParseInputSchema(Message.Payload, Schema))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to parse target schema from Blender."));
            return;
        }

        FString TargetId;
        if (Message.Payload.IsValid())
        {
            Message.Payload->TryGetStringField(TargetIdFieldName, TargetId);
        }

        Component->InputSchema = MoveTemp(Schema);
        Component->SchemaTargetId = TargetId;
        Component->RebuildInputValues(PendingRequest.bPreserveInputValues);
        Component->SetBridgeStatus(
            EBlenderBridgeComponentStatus::Ready,
            FString::Printf(TEXT("Ready. Received %d bridge inputs."), Component->InputSchema.Num()));

        if (PendingRequest.bRequestCookAfterDescribe)
        {
            SendCookRequest(Component);
        }
        return;
    }

    if (Message.Type == EBlenderBridgeMessageType::CookComplete)
    {
        FPendingRequest PendingRequest;
        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                PendingRequest = *ExistingRequest;
                PendingRequests.Remove(RequestId);
            }
        }

        UBlenderBridgeComponent* Component = PendingRequest.Component.Get();
        if (!IsValid(Component) || !Session.IsValid())
        {
            return;
        }

        double SlotNumber = 0.0;
        if (!Message.Payload.IsValid() || !Message.Payload->TryGetNumberField(TEXT("slot"), SlotNumber))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("CookComplete was missing a shared-memory slot."));
            return;
        }

        FBridgeDecodedMesh DecodedMesh;
        if (!Session->GetSharedMemory().ReadMesh(static_cast<uint32>(SlotNumber), DecodedMesh))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to read cooked mesh from shared memory."));
            return;
        }

        BlenderBridge::ParseCookedMeshMetadata(Message.Payload, DecodedMesh.MaterialSlots, DecodedMesh.Sections);
        Component->ApplyDecodedMesh(MoveTemp(DecodedMesh));
        Component->SetBridgeStatus(
            EBlenderBridgeComponentStatus::Cooked,
            FString::Printf(TEXT("Applied bridge mesh from slot %u"), static_cast<uint32>(SlotNumber)));
        return;
    }

    if (Message.Type == EBlenderBridgeMessageType::ResizeRequired)
    {
        uint64 RequiredSlotBytes = 0;
        double RequiredBytes = 0.0;
        if (Message.Payload.IsValid() && Message.Payload->TryGetNumberField(TEXT("required_bytes"), RequiredBytes))
        {
            RequiredSlotBytes = static_cast<uint64>(FMath::Max(RequiredBytes, 0.0));
        }

        UBlenderBridgeComponent* RetryComponent = nullptr;
        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                RetryComponent = ExistingRequest->Component.Get();
                PendingRequests.Remove(RequestId);
            }
        }

        RestartSessionForResize(RetryComponent, RequiredSlotBytes);
        return;
    }

    if (Message.Type == EBlenderBridgeMessageType::Error)
    {
        FString ErrorMessage = TEXT("Blender worker returned an error.");
        if (Message.Payload.IsValid())
        {
            Message.Payload->TryGetStringField(TEXT("message"), ErrorMessage);
        }

        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                if (UBlenderBridgeComponent* Component = ExistingRequest->Component.Get())
                {
                    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, ErrorMessage);
                }
                PendingRequests.Remove(RequestId);
                return;
            }
        }

        LastSessionMessage = ErrorMessage;
        return;
    }
}

void FBlenderBridgeSessionManager::HandleSessionLogMessage(const FString& Message)
{
    LastSessionMessage = Message;
}

void FBlenderBridgeSessionManager::RefreshAllComponentConnectionStates() const
{
    ForEachBridgeComponent([this](UBlenderBridgeComponent* Component)
    {
        Component->ConnectionState = SessionState;
    });
}

void FBlenderBridgeSessionManager::ApplySourceCacheToComponents(
    const FString& SourceHandle,
    const bool bRequestCookForPendingComponent,
    UBlenderBridgeComponent* PendingComponent)
{
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr)
    {
        return;
    }

    ForEachBridgeComponent([&](UBlenderBridgeComponent* Component)
    {
        if (Component->GetSourceHandle() != SourceHandle)
        {
            return;
        }

        const FString PreviousTargetId = Component->TargetId;
        const FString PreviousSchemaTargetId = Component->SchemaTargetId;

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
            Component->SchemaTargetId.Reset();
            Component->InputSchema.Reset();
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
            Component->RebuildInputValues(false);
            if (Component->PreviewMeshComponent != nullptr)
            {
                Component->PreviewMeshComponent->ClearAllMeshSections();
            }
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("No Geometry Nodes targets were found in this blend source."));
            return;
        }

        if (Component->TargetId.IsEmpty())
        {
            Component->SchemaTargetId.Reset();
            Component->InputSchema.Reset();
            Component->RebuildInputValues(false);
            if (Component->PreviewMeshComponent != nullptr)
            {
                Component->PreviewMeshComponent->ClearAllMeshSections();
            }
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
            return;
        }

        const bool bRequestCook = (PendingComponent != nullptr && PendingComponent == Component && bRequestCookForPendingComponent)
            || IsRetryComponent(PendingCookRetryComponents, Component);
        const bool bPreserveInputValues = (PreviousSchemaTargetId == Component->TargetId);

        PendingCookRetryComponents.RemoveAll([Component](const TWeakObjectPtr<UBlenderBridgeComponent>& Entry)
        {
            return !Entry.IsValid() || Entry.Get() == Component;
        });

        RequestDescribeTarget(Component, bPreserveInputValues, bRequestCook);
    });
}

bool FBlenderBridgeSessionManager::RequestEnsureSourceLoaded(
    UBlenderBridgeComponent* Component,
    const bool bForceReload,
    const bool bRequestCookAfterDescribe)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FString BlendFilePath = GetComponentBlendFilePath(Component);
    if (SourceHandle.IsEmpty() || BlendFilePath.IsEmpty())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        return false;
    }

    FSourceCache& SourceCache = SourceCaches.FindOrAdd(SourceHandle);
    SourceCache.BlendFilePath = BlendFilePath;

    if (SourceCache.bLoaded && !bForceReload)
    {
        ApplySourceCacheToComponents(SourceHandle, bRequestCookAfterDescribe, Component);
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
    PendingRequest.bRequestCookAfterDescribe = bRequestCookAfterDescribe;
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SchemaPending, TEXT("Loading source into the global Blender session."));
    return Session->QueueMessage(EBlenderBridgeMessageType::EnsureSourceLoaded, Payload);
}

bool FBlenderBridgeSessionManager::RequestDescribeTarget(
    UBlenderBridgeComponent* Component,
    const bool bPreserveInputValues,
    const bool bRequestCookAfterDescribe)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        return RequestEnsureSourceLoaded(Component, false, bRequestCookAfterDescribe);
    }

    if (Component->TargetId.IsEmpty() || Component->FindTargetDescriptor(Component->TargetId) == nullptr)
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return false;
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
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SchemaPending, TEXT("Refreshing target schema from Blender."));
    return Session->QueueMessage(EBlenderBridgeMessageType::DescribeTarget, Payload);
}

bool FBlenderBridgeSessionManager::SendCookRequest(UBlenderBridgeComponent* Component)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        return RequestEnsureSourceLoaded(Component, false, true);
    }

    if (Component->TargetId.IsEmpty() || Component->FindTargetDescriptor(Component->TargetId) == nullptr)
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return false;
    }

    if (Component->SchemaTargetId != Component->TargetId || Component->InputSchema.IsEmpty())
    {
        return RequestDescribeTarget(Component, Component->SchemaTargetId == Component->TargetId, true);
    }

    const FString RequestId = MakeRequestId();
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(RequestIdFieldName, RequestId);
    Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    Payload->SetStringField(TargetIdFieldName, Component->TargetId);
    BlenderBridge::AddInputValuesToPayload(Component->InputSchema, Component->InputValues, Payload);

    FPendingRequest PendingRequest;
    PendingRequest.Kind = EBridgeManagerRequestKind::CookTarget;
    PendingRequest.Component = Component;
    PendingRequest.SourceHandle = SourceHandle;
    PendingRequest.TargetId = Component->TargetId;
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Cooking, TEXT("Cooking selected target."));
    return Session->QueueMessage(EBlenderBridgeMessageType::CookTarget, Payload);
}

void FBlenderBridgeSessionManager::RestartSessionForResize(UBlenderBridgeComponent* RetryComponent, const uint64 RequiredSlotBytes)
{
    UBlenderBridgeSettings* Settings = GetMutableDefault<UBlenderBridgeSettings>();
    const uint64 CurrentTotalBytes = Settings != nullptr
        ? static_cast<uint64>(FMath::Max<int64>(Settings->SharedMemoryBytes, static_cast<int64>(BlenderBridge::DefaultSharedMemoryBytes)))
        : BlenderBridge::DefaultSharedMemoryBytes;
    const uint64 ExpandedSharedMemoryBytes = ComputeExpandedSharedMemoryBytes(CurrentTotalBytes, RequiredSlotBytes);
    if (ExpandedSharedMemoryBytes <= CurrentTotalBytes)
    {
        if (IsValid(RetryComponent))
        {
            RetryComponent->SetBridgeStatus(
                EBlenderBridgeComponentStatus::Error,
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
        PendingCookRetryComponents.Add(RetryComponent);
        RetryComponent->SetBridgeStatus(
            EBlenderBridgeComponentStatus::SchemaPending,
            FString::Printf(
                TEXT("Growing shared memory from %llu to %llu bytes. Restarting the global session and retrying the cook."),
                CurrentTotalBytes,
                ExpandedSharedMemoryBytes));
    }

    bRestoreSourcesAfterReconnect = true;
    RestartSession();
}

FString FBlenderBridgeSessionManager::MakeRequestId()
{
    return FString::Printf(TEXT("Request_%llu"), NextRequestId++);
}

FString FBlenderBridgeSessionManager::ResolveWorkerScriptPath() const
{
    if (const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("BlenderBridge")))
    {
        return FPaths::Combine(Plugin->GetBaseDir(), TEXT("Scripts/bridge_server.py"));
    }

    return FString();
}
