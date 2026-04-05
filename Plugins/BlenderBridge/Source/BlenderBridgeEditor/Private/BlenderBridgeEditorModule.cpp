#include "BlenderBridgePropertyBagDetails.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

namespace
{
    DEFINE_LOG_CATEGORY_STATIC(LogBlenderBridgeEditor, Log, All);

    void RegisterBridgePropertyBagCustomization()
    {
        FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("StructUtilsEditor"));

        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
        PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("InstancedPropertyBag"));
        PropertyModule.RegisterCustomPropertyTypeLayout(
            TEXT("InstancedPropertyBag"),
            FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBlenderBridgePropertyBagDetails::MakeInstance));
        PropertyModule.NotifyCustomizationModuleChanged();

        UE_LOG(LogBlenderBridgeEditor, Log, TEXT("Registered BlenderBridge InstancedPropertyBag customization after StructUtilsEditor."));
    }
}

class FBlenderBridgeEditorModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        ModulesChangedHandle = FModuleManager::Get().OnModulesChanged().AddRaw(this, &FBlenderBridgeEditorModule::HandleModulesChanged);

        UE_LOG(LogBlenderBridgeEditor, Log, TEXT("BlenderBridgeEditor startup. StructUtilsEditor loaded before registration: %s"),
            FModuleManager::Get().IsModuleLoaded(TEXT("StructUtilsEditor")) ? TEXT("yes") : TEXT("no"));

        RegisterBridgePropertyBagCustomization();
    }

    virtual void ShutdownModule() override
    {
        if (ModulesChangedHandle.IsValid())
        {
            FModuleManager::Get().OnModulesChanged().Remove(ModulesChangedHandle);
            ModulesChangedHandle.Reset();
        }

        if (FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor")))
        {
            FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
            PropertyModule.RegisterCustomPropertyTypeLayout(
                TEXT("InstancedPropertyBag"),
                FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPropertyBagDetails::MakeInstance));
            PropertyModule.NotifyCustomizationModuleChanged();
        }

        UE_LOG(LogBlenderBridgeEditor, Log, TEXT("BlenderBridgeEditor shutdown."));
    }

private:
    void HandleModulesChanged(FName ModuleName, EModuleChangeReason Reason)
    {
        if (Reason != EModuleChangeReason::ModuleLoaded)
        {
            return;
        }

        if (ModuleName == TEXT("StructUtilsEditor") || ModuleName == TEXT("PropertyEditor"))
        {
            UE_LOG(LogBlenderBridgeEditor, Log, TEXT("Module change detected for %s. Re-registering BlenderBridge property bag customization."), *ModuleName.ToString());
            RegisterBridgePropertyBagCustomization();
        }
    }

    FDelegateHandle ModulesChangedHandle;
};

IMPLEMENT_MODULE(FBlenderBridgeEditorModule, BlenderBridgeEditor)
