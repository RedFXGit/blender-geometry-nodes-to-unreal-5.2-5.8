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

IMPLEMENT_MODULE(FBlenderBridgeRuntimeModule, BlenderBridgeRuntime)
