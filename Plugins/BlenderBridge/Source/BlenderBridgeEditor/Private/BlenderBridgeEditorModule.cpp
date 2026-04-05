#include "BlenderBridgePropertyBagDetails.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

namespace
{
    void RegisterBridgePropertyBagCustomization()
    {
        FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("StructUtilsEditor"));

        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
        PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("InstancedPropertyBag"));
        PropertyModule.RegisterCustomPropertyTypeLayout(
            TEXT("InstancedPropertyBag"),
            FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBlenderBridgePropertyBagDetails::MakeInstance));
        PropertyModule.NotifyCustomizationModuleChanged();
    }
}

class FBlenderBridgeEditorModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        RegisterBridgePropertyBagCustomization();
    }

    virtual void ShutdownModule() override
    {
        if (FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor")))
        {
            FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
            PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("InstancedPropertyBag"));

            if (FModuleManager::Get().IsModuleLoaded(TEXT("StructUtilsEditor")))
            {
            PropertyModule.RegisterCustomPropertyTypeLayout(
                TEXT("InstancedPropertyBag"),
                FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPropertyBagDetails::MakeInstance));
            }

            PropertyModule.NotifyCustomizationModuleChanged();
        }
    }
};

IMPLEMENT_MODULE(FBlenderBridgeEditorModule, BlenderBridgeEditor)
