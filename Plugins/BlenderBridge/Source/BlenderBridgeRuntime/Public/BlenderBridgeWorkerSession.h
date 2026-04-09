#pragma once

#include "BlenderBridgeMessages.h"
#include "BlenderBridgePipeClient.h"
#include "BlenderBridgeSharedMemory.h"
#include "Async/Future.h"
#include "HAL/PlatformProcess.h"

class FJsonObject;

struct FBlenderBridgeWorkerConfig
{
    FString BlenderExecutablePath;
    FString WorkerScriptPath;
    uint64 SharedMemoryBytes = BlenderBridge::DefaultSharedMemoryBytes;
};

struct FBlenderBridgePendingMessage
{
    EBlenderBridgeMessageType Type = EBlenderBridgeMessageType::Unknown;
    FString SerializedLine;
};

class FBlenderBridgeWorkerSession final : public TSharedFromThis<FBlenderBridgeWorkerSession>
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateChanged, EBlenderBridgeConnectionState);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FBlenderBridgeJsonMessage&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLogMessage, const FString&);

    ~FBlenderBridgeWorkerSession();

    bool Start(const FBlenderBridgeWorkerConfig& InConfig);
    void Stop();

    bool QueueMessage(EBlenderBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload);
    void SendSerializedMessagesAsync(TArray<FBlenderBridgePendingMessage> Messages);

    bool IsConnected() const { return State == EBlenderBridgeConnectionState::Connected && PipeClient.IsConnected(); }
    EBlenderBridgeConnectionState GetState() const { return State; }

    FBlenderBridgeSharedMemoryRegion& GetSharedMemory() { return SharedMemory; }
    const FBlenderBridgeSharedMemoryRegion& GetSharedMemory() const { return SharedMemory; }
    FBlenderBridgeInstanceBakeSharedMemoryRegion& GetInstanceBakeSharedMemory() { return InstanceBakeSharedMemory; }
    const FBlenderBridgeInstanceBakeSharedMemoryRegion& GetInstanceBakeSharedMemory() const { return InstanceBakeSharedMemory; }

    FOnStateChanged OnStateChanged;
    FOnMessageReceived OnMessageReceived;
    FOnLogMessage OnLogMessage;

private:
    void StopInternal(bool bAllowCallbacks);
    void StartOnBackgroundThread();
    bool WaitForPipeConnection();
    void CleanupFailedStartup();
    void ListenLoop();
    void HandleMessageOnGameThread(const FBlenderBridgeJsonMessage& Message);
    bool SendMessage(EBlenderBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload);
    void BroadcastLogOnGameThread(const FString& Message);
    void SetState(EBlenderBridgeConnectionState NewState);
    void EmitLog(const FString& Message);
    FString MakeUniqueName(const TCHAR* Prefix) const;
    FString MakeSharedMemoryName() const;
    FString BuildBlenderArguments() const;

private:
    FBlenderBridgeWorkerConfig Config;
    FString PipeName;
    FString SharedMemoryName;
    FString InstanceBakeSharedMemoryName;
    FProcHandle ProcessHandle;
    FBlenderBridgePipeClient PipeClient;
    FBlenderBridgeSharedMemoryRegion SharedMemory;
    FBlenderBridgeInstanceBakeSharedMemoryRegion InstanceBakeSharedMemory;
    TFuture<void> StartupTask;
    TFuture<void> ListenerTask;
    TFuture<void> SendTask;
    FCriticalSection SendTaskMutex;
    TAtomic<bool> bStopRequested = false;
    TAtomic<bool> bIsShuttingDown = false;
    TAtomic<bool> bHasStopped = true;
    EBlenderBridgeConnectionState State = EBlenderBridgeConnectionState::Disconnected;
};
