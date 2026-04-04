#include "BlenderBridgeRuntimeModule.h"

#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogBlenderBridge);

void FBlenderBridgeRuntimeModule::StartupModule()
{
}

void FBlenderBridgeRuntimeModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FBlenderBridgeRuntimeModule, BlenderBridgeRuntime)
