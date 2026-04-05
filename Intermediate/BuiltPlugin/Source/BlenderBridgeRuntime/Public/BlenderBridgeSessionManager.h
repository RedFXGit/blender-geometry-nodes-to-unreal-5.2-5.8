#pragma once

#include "BlenderBridgeProtocol.h"

struct FBlenderBridgeJsonMessage;
class FBlenderBridgeWorkerSession;
class UBlenderBridgeComponent;

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

    struct FPendingRequest
    {
        EBridgeManagerRequestKind Kind = EBridgeManagerRequestKind::EnsureSourceLoaded;
        TWeakObjectPtr<UBlenderBridgeComponent> Component;
        FString SourceHandle;
        FString TargetId;
        bool bRequestCookAfterDescribe = false;
        bool bPreserveInputValues = false;
    };

    void StartSessionInternal();
    void HandleSessionStateChanged(EBlenderBridgeConnectionState NewState);
    void HandleSessionMessageReceived(const FBlenderBridgeJsonMessage& Message);
    void HandleSessionLogMessage(const FString& Message);

    void RefreshAllComponentConnectionStates() const;
    void ApplySourceCacheToComponents(const FString& SourceHandle, bool bRequestCookForPendingComponent, UBlenderBridgeComponent* PendingComponent);

    bool RequestEnsureSourceLoaded(UBlenderBridgeComponent* Component, bool bForceReload, bool bRequestCookAfterDescribe);
    bool RequestDescribeTarget(UBlenderBridgeComponent* Component, bool bPreserveInputValues, bool bRequestCookAfterDescribe);
    bool SendCookRequest(UBlenderBridgeComponent* Component);
    void RestartSessionForResize(UBlenderBridgeComponent* RetryComponent, uint64 RequiredSlotBytes);

    FString MakeRequestId();
    FString ResolveWorkerScriptPath() const;

private:
    TSharedPtr<FBlenderBridgeWorkerSession> Session;
    TMap<FString, FSourceCache> SourceCaches;
    TMap<FString, FPendingRequest> PendingRequests;
    TArray<TWeakObjectPtr<UBlenderBridgeComponent>> PendingCookRetryComponents;
    EBlenderBridgeConnectionState SessionState = EBlenderBridgeConnectionState::Disconnected;
    FString LastSessionMessage;
    uint64 NextRequestId = 1;
    bool bRestoreSourcesAfterReconnect = false;
};
