#pragma once

#include "BlenderBridgeMessages.h"
#include "BlenderBridgePipeClient.h"
#include "BlenderBridgeSharedMemory.h"
#include "Async/Future.h"
#include "HAL/PlatformProcess.h"

struct FBlenderBridgeWorkerConfig
{
    FString BlenderExecutablePath;
    FString WorkerScriptPath;
    FString BlendFilePath;
    FString ObjectName;
    FString ModifierName;
    uint64 SharedMemoryBytes = BlenderBridge::DefaultSharedMemoryBytes;
    bool bHelloWorldMode = true;
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
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnSchemaReceived, const TArray<FBridgeInputSchema>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCookComplete, uint32);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLogMessage, const FString&);

    ~FBlenderBridgeWorkerSession();

    bool Start(const FBlenderBridgeWorkerConfig& InConfig);
    void Stop();

    bool RequestSchema();
    bool PushInputs(const TArray<FBridgeInputSchema>& Schema, const FInstancedPropertyBag& InputValues);
    bool RequestCook();
    void SendSerializedMessagesAsync(TArray<FBlenderBridgePendingMessage> Messages);

    bool IsConnected() const { return State == EBlenderBridgeConnectionState::Connected && PipeClient.IsConnected(); }
    EBlenderBridgeConnectionState GetState() const { return State; }

    FBlenderBridgeSharedMemoryRegion& GetSharedMemory() { return SharedMemory; }
    const FBlenderBridgeSharedMemoryRegion& GetSharedMemory() const { return SharedMemory; }

    FOnStateChanged OnStateChanged;
    FOnSchemaReceived OnSchemaReceived;
    FOnCookComplete OnCookComplete;
    FOnLogMessage OnLogMessage;

private:
    void StopInternal(bool bAllowCallbacks);
    void StartOnBackgroundThread();
    void ListenLoop();
    void HandleMessageOnGameThread(const FBlenderBridgeJsonMessage& Message);
    bool SendMessage(EBlenderBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload);
    bool QueueMessage(EBlenderBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload);
    void QueueStartupMessages();
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
    FProcHandle ProcessHandle;
    FBlenderBridgePipeClient PipeClient;
    FBlenderBridgeSharedMemoryRegion SharedMemory;
    TFuture<void> StartupTask;
    TFuture<void> ListenerTask;
    TFuture<void> SendTask;
    FCriticalSection SendTaskMutex;
    TAtomic<bool> bStopRequested = false;
    TAtomic<bool> bIsShuttingDown = false;
    TAtomic<bool> bHasStopped = true;
    bool bStartupMessagesQueued = false;
    EBlenderBridgeConnectionState State = EBlenderBridgeConnectionState::Disconnected;
};
