#pragma once

#include "GeometryNodesBridgeProtocol.h"

struct FGeometryNodesBridgeJsonMessage;
class FGeometryNodesBridgeWorkerSession;
class UGeometryNodesBridgeComponent;
class UGeometryNodesBridgeSource;

class GEOMETRYNODESBRIDGERUNTIME_API FGeometryNodesBridgeSessionManager final
{
public:
    FGeometryNodesBridgeSessionManager();
    ~FGeometryNodesBridgeSessionManager();

    bool CreateSession();
    void StopSession();
    bool RestartSession();
    void Shutdown();

    void NotifySourceChanged(UGeometryNodesBridgeComponent* Component);
    void NotifyTargetChanged(UGeometryNodesBridgeComponent* Component);
    void RefreshComponentSchema(UGeometryNodesBridgeComponent* Component, bool bForceReload);
    void RequestCook(UGeometryNodesBridgeComponent* Component);
    void RequestInstanceBake(UGeometryNodesBridgeComponent* Component);
    void ReleaseComponentState(UGeometryNodesBridgeComponent* Component);
    void WarmSourceOnPreviewDrag(UGeometryNodesBridgeSource* Source, UGeometryNodesBridgeComponent* PreviewComponent);

    bool IsSessionConnected() const;
    EGeometryNodesBridgeConnectionState GetConnectionState() const { return SessionState; }
    const FString& GetLastSessionMessage() const { return LastSessionMessage; }
    const FBridgeDecodedInstanceBake* FindCachedInstanceBake(const UGeometryNodesBridgeComponent* Component) const;

private:
    enum class EBridgeManagerRequestKind : uint8
    {
        EnsureSourceLoaded,
        DescribeTarget,
        CookTarget,
        RequestInstanceBake
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
        TArray<FBridgeGeometryInputDescriptor> GeometryInputs;
        FBridgeTargetCapabilities Capabilities;
    };

    struct FCookedTargetCache
    {
        FBridgeDecodedMesh DecodedMesh;
        FBridgeDecodedInstanceBake DecodedInstanceBake;
    };

    struct FPendingRequest
    {
        EBridgeManagerRequestKind Kind = EBridgeManagerRequestKind::EnsureSourceLoaded;
        TWeakObjectPtr<UGeometryNodesBridgeComponent> Component;
        FString SourceHandle;
        FString TargetId;
        FString CookCacheKey;
        double SentTimeSeconds = 0.0;
        FString RunLabel;
        FString ScenarioId;
        FString ActionKind;
        int32 IterationIndex = INDEX_NONE;
        bool bForceReload = false;
        bool bRequestCookAfterDescribe = false;
        bool bRequestInstanceBakeAfterDescribe = false;
        bool bPreserveInputValues = false;
    };

    void StartSessionInternal();
    void HandleSessionStateChanged(EGeometryNodesBridgeConnectionState NewState);
    void HandleSessionMessageReceived(const FGeometryNodesBridgeJsonMessage& Message);
    void HandleSessionLogMessage(const FString& Message);

    void RefreshAllComponentConnectionStates() const;
    TMap<FString, FString> GatherSourcesToRestore() const;
    void ApplySourceCacheToComponents(
        const FString& SourceHandle,
        bool bRequestCookForPendingComponent,
        bool bRequestInstanceBakeForPendingComponent,
        UGeometryNodesBridgeComponent* PendingComponent);
    void ClearCachedTargetData(const FString& SourceHandle);
    bool QueueEnsureSourceLoadedRequest(
        const FString& SourceHandle,
        const FString& BlendFilePath,
        UGeometryNodesBridgeComponent* Component,
        bool bForceReload,
        bool bRequestCookAfterDescribe,
        bool bRequestInstanceBakeAfterDescribe);

    bool RequestEnsureSourceLoaded(
        UGeometryNodesBridgeComponent* Component,
        bool bForceReload,
        bool bRequestCookAfterDescribe,
        bool bRequestInstanceBakeAfterDescribe);
    bool RequestDescribeTarget(
        UGeometryNodesBridgeComponent* Component,
        bool bPreserveInputValues,
        bool bRequestCookAfterDescribe,
        bool bRequestInstanceBakeAfterDescribe);
    bool SendCookRequest(UGeometryNodesBridgeComponent* Component);
    bool SendInstanceBakeRequest(UGeometryNodesBridgeComponent* Component);
    bool TryApplyCachedTargetDescription(
        UGeometryNodesBridgeComponent* Component,
        bool bPreserveInputValues,
        bool bRequestCookAfterDescribe,
        bool bRequestInstanceBakeAfterDescribe);
    bool TryApplyCachedCook(UGeometryNodesBridgeComponent* Component);
    bool TryApplyCachedInstanceBake(UGeometryNodesBridgeComponent* Component);
    void RestartSessionForResize(UGeometryNodesBridgeComponent* RetryComponent, EBridgeManagerRequestKind RetryKind, uint64 RequiredSlotBytes);
    FString MakeTrackedComponentStateKey(const UGeometryNodesBridgeComponent* Component) const;
    void TrackComponentState(UGeometryNodesBridgeComponent* Component);

    FString MakeRequestId();
    FString ResolveWorkerScriptPath() const;

private:
    TSharedPtr<FGeometryNodesBridgeWorkerSession> Session;
    TMap<FString, FSourceCache> SourceCaches;
    TMap<FString, FTargetSchemaCache> TargetSchemaCaches;
    TMap<FString, FCookedTargetCache> CookedTargetCaches;
    TMap<FString, FPendingRequest> PendingRequests;
    TSet<FString> TrackedComponentStateKeys;
    TArray<TWeakObjectPtr<UGeometryNodesBridgeComponent>> PendingCookRetryComponents;
    TArray<TWeakObjectPtr<UGeometryNodesBridgeComponent>> PendingInstanceBakeRetryComponents;
    TMap<FString, TWeakObjectPtr<UGeometryNodesBridgeComponent>> PendingWarmupPreviewComponents;
    EGeometryNodesBridgeConnectionState SessionState = EGeometryNodesBridgeConnectionState::Disconnected;
    FString LastSessionMessage;
    uint64 NextRequestId = 1;
    bool bRestoreSourcesAfterReconnect = false;
};
