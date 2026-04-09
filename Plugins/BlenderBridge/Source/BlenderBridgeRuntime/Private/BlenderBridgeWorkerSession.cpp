#include "BlenderBridgeWorkerSession.h"

#include "Async/Async.h"
#include "BlenderBridgeRuntimeModule.h"
#include "Dom/JsonObject.h"
#include "Misc/Guid.h"

namespace
{
    constexpr double BlenderPipeConnectAttemptSeconds = 1.0;
    constexpr double BlenderPipeConnectTimeoutSeconds = 45.0;
    constexpr double BlenderPipeConnectProgressLogIntervalSeconds = 5.0;
}

FBlenderBridgeWorkerSession::~FBlenderBridgeWorkerSession()
{
    StopInternal(false);
}

bool FBlenderBridgeWorkerSession::Start(const FBlenderBridgeWorkerConfig& InConfig)
{
    Stop();

    Config = InConfig;
    PipeName = MakeUniqueName(TEXT("BlenderBridgePipe"));
    SharedMemoryName = MakeSharedMemoryName();
    InstanceBakeSharedMemoryName = MakeSharedMemoryName();

    if (Config.BlenderExecutablePath.IsEmpty())
    {
        Config.BlenderExecutablePath = TEXT("blender.exe");
    }

    bStopRequested = false;
    bIsShuttingDown = false;
    bHasStopped = false;
    SetState(EBlenderBridgeConnectionState::Launching);

    const TWeakPtr<FBlenderBridgeWorkerSession> WeakSession = AsShared();
    StartupTask = Async(EAsyncExecution::Thread, [WeakSession]()
    {
        if (const TSharedPtr<FBlenderBridgeWorkerSession> Pinned = WeakSession.Pin())
        {
            Pinned->StartOnBackgroundThread();
        }
    });
    return true;
}

void FBlenderBridgeWorkerSession::Stop()
{
    StopInternal(true);
}

void FBlenderBridgeWorkerSession::StopInternal(const bool bAllowCallbacks)
{
    if (bHasStopped.Exchange(true))
    {
        if (!bAllowCallbacks)
        {
            bIsShuttingDown = true;
        }
        return;
    }

    if (!bAllowCallbacks)
    {
        bIsShuttingDown = true;
    }

    bStopRequested = true;

    if (bAllowCallbacks && PipeClient.IsConnected())
    {
        SendMessage(EBlenderBridgeMessageType::Shutdown, MakeShared<FJsonObject>());
    }

    PipeClient.Disconnect();

    if (StartupTask.IsValid())
    {
        StartupTask.Wait();
        StartupTask = TFuture<void>();
    }

    if (ListenerTask.IsValid())
    {
        ListenerTask.Wait();
        ListenerTask = TFuture<void>();
    }

    {
        FScopeLock Lock(&SendTaskMutex);
        if (SendTask.IsValid())
        {
            SendTask.Wait();
            SendTask = TFuture<void>();
        }
    }

    if (ProcessHandle.IsValid())
    {
        const double Deadline = FPlatformTime::Seconds() + 2.0;
        while (FPlatformProcess::IsProcRunning(ProcessHandle) && FPlatformTime::Seconds() < Deadline)
        {
            FPlatformProcess::Sleep(0.05f);
        }

        if (FPlatformProcess::IsProcRunning(ProcessHandle))
        {
            FPlatformProcess::TerminateProc(ProcessHandle, true);
        }

        FPlatformProcess::CloseProc(ProcessHandle);
        ProcessHandle.Reset();
    }

    SharedMemory.Reset();
    InstanceBakeSharedMemory.Reset();

    if (bAllowCallbacks)
    {
        SetState(EBlenderBridgeConnectionState::Disconnected);
    }
    else
    {
        State = EBlenderBridgeConnectionState::Disconnected;
    }
}

void FBlenderBridgeWorkerSession::StartOnBackgroundThread()
{
    if (!SharedMemory.Create(SharedMemoryName, Config.SharedMemoryBytes))
    {
        SetState(EBlenderBridgeConnectionState::Error);
        EmitLog(TEXT("Failed to create shared memory region."));
        return;
    }

    if (!InstanceBakeSharedMemory.Create(InstanceBakeSharedMemoryName, Config.SharedMemoryBytes))
    {
        SetState(EBlenderBridgeConnectionState::Error);
        EmitLog(TEXT("Failed to create instance-bake shared memory region."));
        SharedMemory.Reset();
        return;
    }

    const FString Arguments = BuildBlenderArguments();
    uint32 ProcessId = 0;
    ProcessHandle = FPlatformProcess::CreateProc(
        *Config.BlenderExecutablePath,
        *Arguments,
        true,
        false,
        false,
        &ProcessId,
        0,
        nullptr,
        nullptr);

    if (!ProcessHandle.IsValid())
    {
        SetState(EBlenderBridgeConnectionState::Error);
        EmitLog(FString::Printf(TEXT("Failed to launch Blender worker: %s"), *Config.BlenderExecutablePath));
        SharedMemory.Reset();
        InstanceBakeSharedMemory.Reset();
        return;
    }

    if (!WaitForPipeConnection())
    {
        if (!bStopRequested)
        {
            CleanupFailedStartup();
            SetState(EBlenderBridgeConnectionState::Error);
        }
        return;
    }

    if (bStopRequested)
    {
        return;
    }

    const TWeakPtr<FBlenderBridgeWorkerSession> WeakSession = AsShared();
    ListenerTask = Async(EAsyncExecution::Thread, [WeakSession]()
    {
        if (const TSharedPtr<FBlenderBridgeWorkerSession> Pinned = WeakSession.Pin())
        {
            Pinned->ListenLoop();
        }
    });
    EmitLog(TEXT("Pipe connected. Waiting for Blender worker hello."));
}

bool FBlenderBridgeWorkerSession::WaitForPipeConnection()
{
    const double StartTime = FPlatformTime::Seconds();
    const double Deadline = StartTime + BlenderPipeConnectTimeoutSeconds;
    double NextProgressLogTime = StartTime + BlenderPipeConnectProgressLogIntervalSeconds;

    EmitLog(FString::Printf(TEXT("Waiting for Blender worker to create named pipe %s."), *PipeName));

    while (FPlatformTime::Seconds() < Deadline)
    {
        if (bStopRequested)
        {
            return false;
        }

        const double RemainingSeconds = Deadline - FPlatformTime::Seconds();
        const double AttemptTimeoutSeconds = FMath::Min(BlenderPipeConnectAttemptSeconds, RemainingSeconds);
        if (AttemptTimeoutSeconds > 0.0 && PipeClient.Connect(PipeName, AttemptTimeoutSeconds, &bStopRequested, false))
        {
            const double ElapsedSeconds = FPlatformTime::Seconds() - StartTime;
            EmitLog(FString::Printf(TEXT("Connected to Blender named pipe after %.1f seconds."), ElapsedSeconds));
            return true;
        }

        if (bStopRequested)
        {
            return false;
        }

        if (!ProcessHandle.IsValid() || !FPlatformProcess::IsProcRunning(ProcessHandle))
        {
            int32 ExitCode = 0;
            if (ProcessHandle.IsValid() && FPlatformProcess::GetProcReturnCode(ProcessHandle, &ExitCode))
            {
                EmitLog(FString::Printf(
                    TEXT("Blender worker exited before creating its named pipe (exit code %d)."),
                    ExitCode));
            }
            else
            {
                EmitLog(TEXT("Blender worker exited before creating its named pipe."));
            }
            return false;
        }

        const double Now = FPlatformTime::Seconds();
        if (Now >= NextProgressLogTime)
        {
            EmitLog(FString::Printf(
                TEXT("Still waiting for Blender worker named pipe after %.1f seconds. Blender is still running."),
                Now - StartTime));
            NextProgressLogTime = Now + BlenderPipeConnectProgressLogIntervalSeconds;
        }
    }

    EmitLog(FString::Printf(
        TEXT("Timed out after %.1f seconds waiting for Blender worker to create named pipe %s."),
        BlenderPipeConnectTimeoutSeconds,
        *PipeName));
    return false;
}

void FBlenderBridgeWorkerSession::CleanupFailedStartup()
{
    PipeClient.Disconnect();

    if (ProcessHandle.IsValid())
    {
        if (FPlatformProcess::IsProcRunning(ProcessHandle))
        {
            FPlatformProcess::TerminateProc(ProcessHandle, true);
        }

        FPlatformProcess::CloseProc(ProcessHandle);
        ProcessHandle.Reset();
    }

    SharedMemory.Reset();
    InstanceBakeSharedMemory.Reset();
}

void FBlenderBridgeWorkerSession::SendSerializedMessagesAsync(TArray<FBlenderBridgePendingMessage> Messages)
{
    TFuture<void> PreviousSendTask;
    {
        FScopeLock Lock(&SendTaskMutex);
        if (SendTask.IsValid())
        {
            PreviousSendTask = MoveTemp(SendTask);
        }
    }

    const TWeakPtr<FBlenderBridgeWorkerSession> WeakSession = AsShared();
    TFuture<void> NewSendTask = Async(EAsyncExecution::Thread, [WeakSession, PreviousSendTask = MoveTemp(PreviousSendTask), Messages = MoveTemp(Messages)]() mutable
    {
        if (PreviousSendTask.IsValid())
        {
            PreviousSendTask.Wait();
        }

        const TSharedPtr<FBlenderBridgeWorkerSession> Pinned = WeakSession.Pin();
        if (!Pinned.IsValid())
        {
            return;
        }

        for (const FBlenderBridgePendingMessage& Message : Messages)
        {
            if (!Pinned->PipeClient.IsConnected())
            {
                Pinned->EmitLog(FString::Printf(TEXT("Cannot send %s because the pipe is disconnected."), BlenderBridge::LexToString(Message.Type)));
                return;
            }

            UE_LOG(LogBlenderBridge, Verbose, TEXT("Attempting async send of %s to Blender worker."), BlenderBridge::LexToString(Message.Type));
            const bool bSuccess = Pinned->PipeClient.WriteLine(Message.SerializedLine);
            if (bSuccess)
            {
                Pinned->EmitLog(FString::Printf(TEXT("Sent %s to Blender worker."), BlenderBridge::LexToString(Message.Type)));
            }
            else
            {
                Pinned->EmitLog(FString::Printf(TEXT("Failed sending %s to Blender worker."), BlenderBridge::LexToString(Message.Type)));
                return;
            }
        }
    });

    {
        FScopeLock Lock(&SendTaskMutex);
        SendTask = MoveTemp(NewSendTask);
    }
}

void FBlenderBridgeWorkerSession::ListenLoop()
{
    while (!bStopRequested)
    {
        FString Line;
        if (!PipeClient.ReadLine(Line, &bStopRequested))
        {
            break;
        }

        FBlenderBridgeJsonMessage Message;
        if (!BlenderBridge::DeserializeBridgeMessage(Line, Message))
        {
            continue;
        }

        const TWeakPtr<FBlenderBridgeWorkerSession> WeakSession = AsShared();
        AsyncTask(ENamedThreads::GameThread, [WeakSession, Message]()
        {
            if (const TSharedPtr<FBlenderBridgeWorkerSession> Pinned = WeakSession.Pin())
            {
                Pinned->HandleMessageOnGameThread(Message);
            }
        });
    }

    if (!bStopRequested)
    {
        const TWeakPtr<FBlenderBridgeWorkerSession> WeakSession = AsShared();
        AsyncTask(ENamedThreads::GameThread, [WeakSession]()
        {
            if (const TSharedPtr<FBlenderBridgeWorkerSession> Pinned = WeakSession.Pin())
            {
                Pinned->EmitLog(TEXT("Blender worker pipe disconnected."));
                Pinned->SetState(EBlenderBridgeConnectionState::Error);
            }
        });
    }
}

void FBlenderBridgeWorkerSession::HandleMessageOnGameThread(const FBlenderBridgeJsonMessage& Message)
{
    if (Message.Type != EBlenderBridgeMessageType::Unknown)
    {
        EmitLog(FString::Printf(TEXT("Received %s from Blender worker."), BlenderBridge::LexToString(Message.Type)));
    }

    switch (Message.Type)
    {
    case EBlenderBridgeMessageType::Hello:
    {
        FString WorkerMode;
        if (Message.Payload.IsValid())
        {
            Message.Payload->TryGetStringField(TEXT("mode"), WorkerMode);
        }

        if (!WorkerMode.IsEmpty())
        {
            EmitLog(FString::Printf(TEXT("Worker reported mode: %s"), *WorkerMode));
        }

        SetState(EBlenderBridgeConnectionState::Connected);
        break;
    }

    case EBlenderBridgeMessageType::Error:
    {
        FString ErrorText;
        if (Message.Payload.IsValid())
        {
            Message.Payload->TryGetStringField(TEXT("message"), ErrorText);
        }
        EmitLog(FString::Printf(TEXT("Worker error: %s"), *ErrorText));
        OnMessageReceived.Broadcast(Message);
        break;
    }

    default:
        if (Message.Type != EBlenderBridgeMessageType::Unknown)
        {
            OnMessageReceived.Broadcast(Message);
        }
        break;
    }
}

bool FBlenderBridgeWorkerSession::SendMessage(const EBlenderBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload)
{
    if (!PipeClient.IsConnected())
    {
        EmitLog(FString::Printf(TEXT("Cannot send %s because the pipe is disconnected."), BlenderBridge::LexToString(Type)));
        return false;
    }

    FString Line;
    if (!BlenderBridge::SerializeBridgeMessage(Type, Payload, Line))
    {
        EmitLog(FString::Printf(TEXT("Failed to serialize %s for Blender worker."), BlenderBridge::LexToString(Type)));
        return false;
    }

    UE_LOG(LogBlenderBridge, Verbose, TEXT("Attempting send of %s to Blender worker."), BlenderBridge::LexToString(Type));
    const bool bSuccess = PipeClient.WriteLine(Line);
    if (bSuccess)
    {
        EmitLog(FString::Printf(TEXT("Sent %s to Blender worker."), BlenderBridge::LexToString(Type)));
    }
    else
    {
        EmitLog(FString::Printf(TEXT("Failed sending %s to Blender worker."), BlenderBridge::LexToString(Type)));
    }

    return bSuccess;
}

bool FBlenderBridgeWorkerSession::QueueMessage(const EBlenderBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload)
{
    FString Line;
    if (!BlenderBridge::SerializeBridgeMessage(Type, Payload, Line))
    {
        EmitLog(FString::Printf(TEXT("Failed to serialize %s for Blender worker."), BlenderBridge::LexToString(Type)));
        return false;
    }

    FBlenderBridgePendingMessage PendingMessage;
    PendingMessage.Type = Type;
    PendingMessage.SerializedLine = MoveTemp(Line);

    TArray<FBlenderBridgePendingMessage> PendingMessages;
    PendingMessages.Add(MoveTemp(PendingMessage));
    SendSerializedMessagesAsync(MoveTemp(PendingMessages));
    return true;
}

void FBlenderBridgeWorkerSession::BroadcastLogOnGameThread(const FString& Message)
{
    check(IsInGameThread());
    OnLogMessage.Broadcast(Message);
}

void FBlenderBridgeWorkerSession::SetState(const EBlenderBridgeConnectionState NewState)
{
    if (!IsInGameThread())
    {
        if (bIsShuttingDown.Load())
        {
            State = NewState;
            return;
        }

        const TWeakPtr<FBlenderBridgeWorkerSession> WeakSession = AsShared();
        AsyncTask(ENamedThreads::GameThread, [WeakSession, NewState]()
        {
            if (const TSharedPtr<FBlenderBridgeWorkerSession> Pinned = WeakSession.Pin())
            {
                Pinned->SetState(NewState);
            }
        });
        return;
    }

    if (State != NewState)
    {
        State = NewState;
        OnStateChanged.Broadcast(State);
    }
}

void FBlenderBridgeWorkerSession::EmitLog(const FString& Message)
{
    UE_LOG(LogBlenderBridge, Log, TEXT("%s"), *Message);

    if (!IsInGameThread())
    {
        if (bIsShuttingDown.Load())
        {
            return;
        }

        const TWeakPtr<FBlenderBridgeWorkerSession> WeakSession = AsShared();
        AsyncTask(ENamedThreads::GameThread, [WeakSession, Message]()
        {
            if (const TSharedPtr<FBlenderBridgeWorkerSession> Pinned = WeakSession.Pin())
            {
                Pinned->BroadcastLogOnGameThread(Message);
            }
        });
        return;
    }

    BroadcastLogOnGameThread(Message);
}

FString FBlenderBridgeWorkerSession::MakeUniqueName(const TCHAR* Prefix) const
{
    return FString::Printf(TEXT("%s_%s"), Prefix, *FGuid::NewGuid().ToString(EGuidFormats::Digits));
}

FString FBlenderBridgeWorkerSession::MakeSharedMemoryName() const
{
    // Unreal's Windows shared-memory helper avoids the privileged Global\ namespace
    // when the name is a GUID in braces, which keeps the mapping usable for a normal
    // editor-launched desktop process pair.
    return FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphensInBraces);
}

FString FBlenderBridgeWorkerSession::BuildBlenderArguments() const
{
    TArray<FString> Tokens;
    Tokens.Reserve(14);

    Tokens.Add(TEXT("--background"));
    Tokens.Add(TEXT("--python"));
    Tokens.Add(FString::Printf(TEXT("\"%s\""), *Config.WorkerScriptPath));
    Tokens.Add(TEXT("--"));
    Tokens.Add(TEXT("--pipe-name"));
    Tokens.Add(FString::Printf(TEXT("\"%s\""), *PipeName));
    Tokens.Add(TEXT("--shared-memory-name"));
    Tokens.Add(FString::Printf(TEXT("\"%s\""), *SharedMemoryName));
    Tokens.Add(TEXT("--shared-memory-bytes"));
    Tokens.Add(LexToString(Config.SharedMemoryBytes));
    Tokens.Add(TEXT("--instance-memory-name"));
    Tokens.Add(FString::Printf(TEXT("\"%s\""), *InstanceBakeSharedMemoryName));
    Tokens.Add(TEXT("--instance-memory-bytes"));
    Tokens.Add(LexToString(Config.SharedMemoryBytes));

    return FString::Join(Tokens, TEXT(" "));
}
