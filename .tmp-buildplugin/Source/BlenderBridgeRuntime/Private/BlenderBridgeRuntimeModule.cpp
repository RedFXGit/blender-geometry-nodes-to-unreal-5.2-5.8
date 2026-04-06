#include "BlenderBridgeRuntimeModule.h"

#include "BlenderBridgeSessionManager.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogBlenderBridge);

FBlenderBridgeRuntimeModule& FBlenderBridgeRuntimeModule::Get()
{
    return FModuleManager::LoadModuleChecked<FBlenderBridgeRuntimeModule>(TEXT("BlenderBridgeRuntime"));
}

void FBlenderBridgeRuntimeModule::StartupModule()
{
    SessionManager = MakeUnique<FBlenderBridgeSessionManager>();
}

void FBlenderBridgeRuntimeModule::ShutdownModule()
{
    if (SessionManager)
    {
        SessionManager->Shutdown();
        SessionManager.Reset();
    }
}

FBlenderBridgeSessionManager& FBlenderBridgeRuntimeModule::GetSessionManager()
{
    if (!SessionManager)
    {
        SessionManager = MakeUnique<FBlenderBridgeSessionManager>();
    }

    return *SessionManager;
}

bool FBlenderBridgeRuntimeModule::ExecuteBakeBridgeComponent(UBlenderBridgeComponent* Component)
{
    return BakeBridgeComponentDelegate.IsBound() ? BakeBridgeComponentDelegate.Execute(Component) : false;
}

bool FBlenderBridgeRuntimeModule::ExecuteSelectBakedOutput(UBlenderBridgeComponent* Component)
{
    return SelectBakedOutputDelegate.IsBound() ? SelectBakedOutputDelegate.Execute(Component) : false;
}

bool FBlenderBridgeRuntimeModule::ExecuteDeleteBakedOutput(UBlenderBridgeComponent* Component)
{
    return DeleteBakedOutputDelegate.IsBound() ? DeleteBakedOutputDelegate.Execute(Component) : false;
}

void FBlenderBridgeRuntimeModule::SetBakeBridgeComponentDelegate(FBlenderBridgeBakeComponentDelegate InDelegate)
{
    BakeBridgeComponentDelegate = MoveTemp(InDelegate);
}

void FBlenderBridgeRuntimeModule::SetSelectBakedOutputDelegate(FBlenderBridgeSelectBakedOutputDelegate InDelegate)
{
    SelectBakedOutputDelegate = MoveTemp(InDelegate);
}

void FBlenderBridgeRuntimeModule::SetDeleteBakedOutputDelegate(FBlenderBridgeDeleteBakedOutputDelegate InDelegate)
{
    DeleteBakedOutputDelegate = MoveTemp(InDelegate);
}

IMPLEMENT_MODULE(FBlenderBridgeRuntimeModule, BlenderBridgeRuntime)
