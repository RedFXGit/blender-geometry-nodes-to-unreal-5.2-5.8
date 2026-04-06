#pragma once

#include "CoreMinimal.h"

class FBlenderBridgePipeClient final
{
public:
    ~FBlenderBridgePipeClient();

    bool Connect(const FString& PipeName, double TimeoutSeconds, const TAtomic<bool>* StopRequested = nullptr, bool bLogTimeout = true);
    void Disconnect();
    bool IsConnected() const { return PipeHandle != nullptr; }

    bool WriteLine(const FString& Line);
    bool ReadLine(FString& OutLine, const TAtomic<bool>* StopRequested = nullptr);

private:
    FString BuildPlatformPipePath(const FString& PipeName) const;

private:
    void* PipeHandle = nullptr;
    FCriticalSection WriteMutex;
    FString ReadBuffer;
};
