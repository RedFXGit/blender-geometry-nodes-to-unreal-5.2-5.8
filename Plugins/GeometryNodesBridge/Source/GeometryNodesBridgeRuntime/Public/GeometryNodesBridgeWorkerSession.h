#pragma once

#include "GeometryNodesBridgeMessages.h"
#include "GeometryNodesBridgePipeClient.h"
#include "GeometryNodesBridgeSharedMemory.h"
#include "Async/Future.h"
#include "HAL/PlatformProcess.h"

class FJsonObject;

struct FGeometryNodesBridgeWorkerConfig
{
    FString BlenderExecutablePath;
    FString WorkerScriptPath;
    uint64 SharedMemoryBytes = GeometryNodesBridge::DefaultSharedMemoryBytes;
};

struct FGeometryNodesBridgePendingMessage
{
    EGeometryNodesBridgeMessageType Type = EGeometryNodesBridgeMessageType::Unknown;
    FString SerializedLine;
};

class FGeometryNodesBridgeWorkerSession final : public TSharedFromThis<FGeometryNodesBridgeWorkerSession>
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateChanged, EGeometryNodesBridgeConnectionState);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FGeometryNodesBridgeJsonMessage&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLogMessage, const FString&);

    ~FGeometryNodesBridgeWorkerSession();

    bool Start(const FGeometryNodesBridgeWorkerConfig& InConfig);
    void Stop();

    bool QueueMessage(EGeometryNodesBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload);
    void SendSerializedMessagesAsync(TArray<FGeometryNodesBridgePendingMessage> Messages);

    bool IsConnected() const { return State == EGeometryNodesBridgeConnectionState::Connected && PipeClient.IsConnected(); }
    EGeometryNodesBridgeConnectionState GetState() const { return State; }

    FGeometryNodesBridgeSharedMemoryRegion& GetSharedMemory() { return SharedMemory; }
    const FGeometryNodesBridgeSharedMemoryRegion& GetSharedMemory() const { return SharedMemory; }
    FGeometryNodesBridgeInstanceBakeSharedMemoryRegion& GetInstanceBakeSharedMemory() { return InstanceBakeSharedMemory; }
    const FGeometryNodesBridgeInstanceBakeSharedMemoryRegion& GetInstanceBakeSharedMemory() const { return InstanceBakeSharedMemory; }

    FOnStateChanged OnStateChanged;
    FOnMessageReceived OnMessageReceived;
    FOnLogMessage OnLogMessage;

private:
    void StopInternal(bool bAllowCallbacks);
    void StartOnBackgroundThread();
    bool WaitForPipeConnection();
    void CleanupFailedStartup();
    void ListenLoop();
    void HandleMessageOnGameThread(const FGeometryNodesBridgeJsonMessage& Message);
    bool SendMessage(EGeometryNodesBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload);
    void BroadcastLogOnGameThread(const FString& Message);
    void SetState(EGeometryNodesBridgeConnectionState NewState);
    void EmitLog(const FString& Message);
    FString MakeUniqueName(const TCHAR* Prefix) const;
    FString MakeSharedMemoryName() const;
    FString BuildBlenderArguments() const;

private:
    FGeometryNodesBridgeWorkerConfig Config;
    FString PipeName;
    FString SharedMemoryName;
    FString InstanceBakeSharedMemoryName;
    FProcHandle ProcessHandle;
    FGeometryNodesBridgePipeClient PipeClient;
    FGeometryNodesBridgeSharedMemoryRegion SharedMemory;
    FGeometryNodesBridgeInstanceBakeSharedMemoryRegion InstanceBakeSharedMemory;
    TFuture<void> StartupTask;
    TFuture<void> ListenerTask;
    TFuture<void> SendTask;
    FCriticalSection SendTaskMutex;
    TAtomic<bool> bStopRequested = false;
    TAtomic<bool> bIsShuttingDown = false;
    TAtomic<bool> bHasStopped = true;
    EGeometryNodesBridgeConnectionState State = EGeometryNodesBridgeConnectionState::Disconnected;
};
