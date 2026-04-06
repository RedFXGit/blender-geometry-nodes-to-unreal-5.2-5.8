#pragma once

#include "BlenderBridgeSessionManager.h"
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBlenderBridge, Log, All);

class UBlenderBridgeComponent;

DECLARE_DELEGATE_RetVal_OneParam(bool, FBlenderBridgeBakeComponentDelegate, UBlenderBridgeComponent*);
DECLARE_DELEGATE_RetVal_OneParam(bool, FBlenderBridgeSelectBakedOutputDelegate, UBlenderBridgeComponent*);
DECLARE_DELEGATE_RetVal_OneParam(bool, FBlenderBridgeDeleteBakedOutputDelegate, UBlenderBridgeComponent*);

class BLENDERBRIDGERUNTIME_API FBlenderBridgeRuntimeModule final : public IModuleInterface
{
public:
    static FBlenderBridgeRuntimeModule& Get();

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    FBlenderBridgeSessionManager& GetSessionManager();
    bool ExecuteBakeBridgeComponent(UBlenderBridgeComponent* Component);
    bool ExecuteSelectBakedOutput(UBlenderBridgeComponent* Component);
    bool ExecuteDeleteBakedOutput(UBlenderBridgeComponent* Component);
    void SetBakeBridgeComponentDelegate(FBlenderBridgeBakeComponentDelegate InDelegate);
    void SetSelectBakedOutputDelegate(FBlenderBridgeSelectBakedOutputDelegate InDelegate);
    void SetDeleteBakedOutputDelegate(FBlenderBridgeDeleteBakedOutputDelegate InDelegate);

private:
    TUniquePtr<FBlenderBridgeSessionManager> SessionManager;
    FBlenderBridgeBakeComponentDelegate BakeBridgeComponentDelegate;
    FBlenderBridgeSelectBakedOutputDelegate SelectBakedOutputDelegate;
    FBlenderBridgeDeleteBakedOutputDelegate DeleteBakedOutputDelegate;
};
