#pragma once

#include "BlenderBridgeProtocol.h"

struct FBlenderBridgeJsonMessage;
class FBlenderBridgeWorkerSession;
class UBlenderBridgeComponent;
class UBlenderBridgeSource;

class BLENDERBRIDGERUNTIME_API FBlenderBridgeSessionManager final
{
public:
    FBlenderBridgeSessionManager();
    ~FBlenderBridgeSessionManager();

    bool CreateSession();
    void StopSession();
    bool RestartSession();
    void Shutdown();

    void NotifySourceChanged(UBlenderBridgeComponent* Component);
    void NotifyTargetChanged(UBlenderBridgeComponent* Component);
    void RefreshComponentSchema(UBlenderBridgeComponent* Component, bool bForceReload);
    void RequestCook(UBlenderBridgeComponent* Component);
    void WarmSourceOnPreviewDrag(UBlenderBridgeSource* Source, UBlenderBridgeComponent* PreviewComponent);

    bool IsSessionConnected() const;
    EBlenderBridgeConnectionState GetConnectionState() const { return SessionState; }
    const FString& GetLastSessionMessage() const { return LastSessionMessage; }

private:
    enum class EBridgeManagerRequestKind : uint8
    {
        EnsureSourceLoaded,
        DescribeTarget,
        CookTarget
    };

    struct FSourceCache
    {
        FString BlendFilePath;
        TArray<FBridgeTargetDescriptor> Targets;
        bool bLoaded = false;
    };

    struct FTargetSchemaCache
    {
        TArray<FBridgeInputSchema> Schema;
    };

    struct FCookedTargetCache
    {
        FBridgeDecodedMesh DecodedMesh;
    };

    struct FPendingRequest
    {
        EBridgeManagerRequestKind Kind = EBridgeManagerRequestKind::EnsureSourceLoaded;
        TWeakObjectPtr<UBlenderBridgeComponent> Component;
        FString SourceHandle;
        FString TargetId;
        FString CookCacheKey;
        bool bForceReload = false;
        bool bRequestCookAfterDescribe = false;
        bool bPreserveInputValues = false;
    };

    void StartSessionInternal();
    void HandleSessionStateChanged(EBlenderBridgeConnectionState NewState);
    void HandleSessionMessageReceived(const FBlenderBridgeJsonMessage& Message);
    void HandleSessionLogMessage(const FString& Message);

    void RefreshAllComponentConnectionStates() const;
    TMap<FString, FString> GatherSourcesToRestore() const;
    void ApplySourceCacheToComponents(const FString& SourceHandle, bool bRequestCookForPendingComponent, UBlenderBridgeComponent* PendingComponent);
    void ClearCachedTargetData(const FString& SourceHandle);
    bool QueueEnsureSourceLoadedRequest(
        const FString& SourceHandle,
        const FString& BlendFilePath,
        UBlenderBridgeComponent* Component,
        bool bForceReload,
        bool bRequestCookAfterDescribe);

    bool RequestEnsureSourceLoaded(UBlenderBridgeComponent* Component, bool bForceReload, bool bRequestCookAfterDescribe);
    bool RequestDescribeTarget(UBlenderBridgeComponent* Component, bool bPreserveInputValues, bool bRequestCookAfterDescribe);
    bool SendCookRequest(UBlenderBridgeComponent* Component);
    bool TryApplyCachedTargetDescription(UBlenderBridgeComponent* Component, bool bPreserveInputValues, bool bRequestCookAfterDescribe);
    bool TryApplyCachedCook(UBlenderBridgeComponent* Component);
    void RestartSessionForResize(UBlenderBridgeComponent* RetryComponent, uint64 RequiredSlotBytes);

    FString MakeRequestId();
    FString ResolveWorkerScriptPath() const;

private:
    TSharedPtr<FBlenderBridgeWorkerSession> Session;
    TMap<FString, FSourceCache> SourceCaches;
    TMap<FString, FTargetSchemaCache> TargetSchemaCaches;
    TMap<FString, FCookedTargetCache> CookedTargetCaches;
    TMap<FString, FPendingRequest> PendingRequests;
    TArray<TWeakObjectPtr<UBlenderBridgeComponent>> PendingCookRetryComponents;
    TMap<FString, TWeakObjectPtr<UBlenderBridgeComponent>> PendingWarmupPreviewComponents;
    EBlenderBridgeConnectionState SessionState = EBlenderBridgeConnectionState::Disconnected;
    FString LastSessionMessage;
    uint64 NextRequestId = 1;
    bool bRestoreSourcesAfterReconnect = false;
};
