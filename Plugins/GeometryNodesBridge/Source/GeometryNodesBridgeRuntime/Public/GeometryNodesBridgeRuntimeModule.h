#pragma once

#include "GeometryNodesBridgeSessionManager.h"
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGeometryNodesBridge, Log, All);

class UGeometryNodesBridgeComponent;

DECLARE_DELEGATE_RetVal_OneParam(bool, FGeometryNodesBridgeBakeComponentDelegate, UGeometryNodesBridgeComponent*);
DECLARE_DELEGATE_RetVal_OneParam(bool, FGeometryNodesBridgeSelectBakedOutputDelegate, UGeometryNodesBridgeComponent*);
DECLARE_DELEGATE_RetVal_OneParam(bool, FGeometryNodesBridgeDeleteBakedOutputDelegate, UGeometryNodesBridgeComponent*);

class GEOMETRYNODESBRIDGERUNTIME_API FGeometryNodesBridgeRuntimeModule final : public IModuleInterface
{
public:
    static FGeometryNodesBridgeRuntimeModule& Get();

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    FGeometryNodesBridgeSessionManager& GetSessionManager();
    bool ExecuteBakeBridgeComponent(UGeometryNodesBridgeComponent* Component);
    bool ExecuteSelectBakedOutput(UGeometryNodesBridgeComponent* Component);
    bool ExecuteDeleteBakedOutput(UGeometryNodesBridgeComponent* Component);
    void SetBakeBridgeComponentDelegate(FGeometryNodesBridgeBakeComponentDelegate InDelegate);
    void SetSelectBakedOutputDelegate(FGeometryNodesBridgeSelectBakedOutputDelegate InDelegate);
    void SetDeleteBakedOutputDelegate(FGeometryNodesBridgeDeleteBakedOutputDelegate InDelegate);

private:
    TUniquePtr<FGeometryNodesBridgeSessionManager> SessionManager;
    FGeometryNodesBridgeBakeComponentDelegate BakeBridgeComponentDelegate;
    FGeometryNodesBridgeSelectBakedOutputDelegate SelectBakedOutputDelegate;
    FGeometryNodesBridgeDeleteBakedOutputDelegate DeleteBakedOutputDelegate;
};
