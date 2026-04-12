#include "GeometryNodesBridgePipeClient.h"

#include "GeometryNodesBridgeRuntimeModule.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformTime.h"

#if GEOMETRY_NODES_BRIDGE_WITH_WINDOWS_IPC
#include "Windows/WindowsHWrapper.h"
#endif

FGeometryNodesBridgePipeClient::~FGeometryNodesBridgePipeClient()
{
    Disconnect();
}

bool FGeometryNodesBridgePipeClient::Connect(
    const FString& PipeName,
    const double TimeoutSeconds,
    const TAtomic<bool>* StopRequested,
    const bool bLogTimeout)
{
#if !GEOMETRY_NODES_BRIDGE_WITH_WINDOWS_IPC
    return false;
#else
    Disconnect();

    const FString PipePath = BuildPlatformPipePath(PipeName);
    const double Deadline = FPlatformTime::Seconds() + TimeoutSeconds;

    while (FPlatformTime::Seconds() < Deadline)
    {
        if (StopRequested != nullptr && StopRequested->Load())
        {
            return false;
        }

        if (::WaitNamedPipeW(*PipePath, 250))
        {
            HANDLE Handle = ::CreateFileW(
                *PipePath,
                GENERIC_READ | GENERIC_WRITE,
                0,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr);

            if (Handle != INVALID_HANDLE_VALUE)
            {
                DWORD ReadMode = PIPE_READMODE_BYTE;
                ::SetNamedPipeHandleState(Handle, &ReadMode, nullptr, nullptr);
                PipeHandle = Handle;
                return true;
            }
        }

        FPlatformProcess::Sleep(0.05f);
    }

    if (bLogTimeout)
    {
        UE_LOG(LogGeometryNodesBridge, Error, TEXT("Timed out connecting to named pipe %s"), *PipePath);
    }
    return false;
#endif
}

void FGeometryNodesBridgePipeClient::Disconnect()
{
#if GEOMETRY_NODES_BRIDGE_WITH_WINDOWS_IPC
    if (PipeHandle != nullptr)
    {
        ::CloseHandle(static_cast<HANDLE>(PipeHandle));
        PipeHandle = nullptr;
    }
#else
    PipeHandle = nullptr;
#endif

    ReadBuffer.Reset();
}

bool FGeometryNodesBridgePipeClient::WriteLine(const FString& Line)
{
#if !GEOMETRY_NODES_BRIDGE_WITH_WINDOWS_IPC
    return false;
#else
    if (PipeHandle == nullptr)
    {
        return false;
    }

    FString LineWithNewline = Line;
    LineWithNewline.AppendChar(TEXT('\n'));

    const FTCHARToUTF8 UTF8Line(*LineWithNewline);
    const uint8* Data = reinterpret_cast<const uint8*>(UTF8Line.Get());
    int32 RemainingBytes = UTF8Line.Length();

    FScopeLock Lock(&WriteMutex);

    while (RemainingBytes > 0)
    {
        DWORD BytesWritten = 0;
        const BOOL bWriteSucceeded = ::WriteFile(
            static_cast<HANDLE>(PipeHandle),
            Data,
            RemainingBytes,
            &BytesWritten,
            nullptr);

        if (!bWriteSucceeded || BytesWritten == 0)
        {
            UE_LOG(LogGeometryNodesBridge, Error, TEXT("Failed writing to named pipe"));
            Disconnect();
            return false;
        }

        Data += BytesWritten;
        RemainingBytes -= static_cast<int32>(BytesWritten);
    }

    return true;
#endif
}

bool FGeometryNodesBridgePipeClient::ReadLine(FString& OutLine, const TAtomic<bool>* StopRequested)
{
#if !GEOMETRY_NODES_BRIDGE_WITH_WINDOWS_IPC
    return false;
#else
    OutLine.Reset();

    while (true)
    {
        int32 NewlineIndex = INDEX_NONE;
        if (ReadBuffer.FindChar(TEXT('\n'), NewlineIndex))
        {
            OutLine = ReadBuffer.Left(NewlineIndex).TrimStartAndEnd();
            ReadBuffer.RightChopInline(NewlineIndex + 1, EAllowShrinking::No);
            return true;
        }

        if (StopRequested != nullptr && StopRequested->Load())
        {
            return false;
        }

        DWORD TotalBytesAvailable = 0;
        const BOOL bPeekSucceeded = ::PeekNamedPipe(static_cast<HANDLE>(PipeHandle), nullptr, 0, nullptr, &TotalBytesAvailable, nullptr);
        if (!bPeekSucceeded)
        {
            Disconnect();
            return false;
        }

        if (TotalBytesAvailable == 0)
        {
            FPlatformProcess::Sleep(0.01f);
            continue;
        }

        ANSICHAR Buffer[4096];
        DWORD BytesRead = 0;
        const DWORD BytesToRead = FMath::Min<DWORD>(static_cast<DWORD>(sizeof(Buffer)), TotalBytesAvailable);
        const BOOL bReadSucceeded = ::ReadFile(static_cast<HANDLE>(PipeHandle), Buffer, BytesToRead, &BytesRead, nullptr);
        if (!bReadSucceeded || BytesRead == 0)
        {
            Disconnect();
            return false;
        }

        const FUTF8ToTCHAR Converted(Buffer, BytesRead);
        ReadBuffer.AppendChars(Converted.Get(), Converted.Length());
    }
#endif
}

FString FGeometryNodesBridgePipeClient::BuildPlatformPipePath(const FString& PipeName) const
{
    return FString::Printf(TEXT("\\\\.\\pipe\\%s"), *PipeName);
}
