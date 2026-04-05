#pragma once

#include "BlenderBridgeSessionManager.h"
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBlenderBridge, Log, All);

class BLENDERBRIDGERUNTIME_API FBlenderBridgeRuntimeModule final : public IModuleInterface
{
public:
    static FBlenderBridgeRuntimeModule& Get();

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    FBlenderBridgeSessionManager& GetSessionManager();

private:
    TUniquePtr<FBlenderBridgeSessionManager> SessionManager;
};
