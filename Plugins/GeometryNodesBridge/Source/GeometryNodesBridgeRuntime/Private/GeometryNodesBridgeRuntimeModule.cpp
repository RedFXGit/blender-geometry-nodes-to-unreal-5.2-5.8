#include "GeometryNodesBridgeRuntimeModule.h"

#include "GeometryNodesBridgeSessionManager.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogGeometryNodesBridge);

FGeometryNodesBridgeRuntimeModule& FGeometryNodesBridgeRuntimeModule::Get()
{
    return FModuleManager::LoadModuleChecked<FGeometryNodesBridgeRuntimeModule>(TEXT("GeometryNodesBridgeRuntime"));
}

void FGeometryNodesBridgeRuntimeModule::StartupModule()
{
    SessionManager = MakeUnique<FGeometryNodesBridgeSessionManager>();
}

void FGeometryNodesBridgeRuntimeModule::ShutdownModule()
{
    if (SessionManager)
    {
        SessionManager->Shutdown();
        SessionManager.Reset();
    }
}

FGeometryNodesBridgeSessionManager& FGeometryNodesBridgeRuntimeModule::GetSessionManager()
{
    if (!SessionManager)
    {
        SessionManager = MakeUnique<FGeometryNodesBridgeSessionManager>();
    }

    return *SessionManager;
}

bool FGeometryNodesBridgeRuntimeModule::ExecuteBakeBridgeComponent(UGeometryNodesBridgeComponent* Component)
{
    return BakeBridgeComponentDelegate.IsBound() ? BakeBridgeComponentDelegate.Execute(Component) : false;
}

bool FGeometryNodesBridgeRuntimeModule::ExecuteSelectBakedOutput(UGeometryNodesBridgeComponent* Component)
{
    return SelectBakedOutputDelegate.IsBound() ? SelectBakedOutputDelegate.Execute(Component) : false;
}

bool FGeometryNodesBridgeRuntimeModule::ExecuteDeleteBakedOutput(UGeometryNodesBridgeComponent* Component)
{
    return DeleteBakedOutputDelegate.IsBound() ? DeleteBakedOutputDelegate.Execute(Component) : false;
}

void FGeometryNodesBridgeRuntimeModule::SetBakeBridgeComponentDelegate(FGeometryNodesBridgeBakeComponentDelegate InDelegate)
{
    BakeBridgeComponentDelegate = MoveTemp(InDelegate);
}

void FGeometryNodesBridgeRuntimeModule::SetSelectBakedOutputDelegate(FGeometryNodesBridgeSelectBakedOutputDelegate InDelegate)
{
    SelectBakedOutputDelegate = MoveTemp(InDelegate);
}

void FGeometryNodesBridgeRuntimeModule::SetDeleteBakedOutputDelegate(FGeometryNodesBridgeDeleteBakedOutputDelegate InDelegate)
{
    DeleteBakedOutputDelegate = MoveTemp(InDelegate);
}

IMPLEMENT_MODULE(FGeometryNodesBridgeRuntimeModule, GeometryNodesBridgeRuntime)
