#include "BlenderBridgePropertyBagDetails.h"

#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSessionManager.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "ToolMenus.h"

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

        UToolMenus::RegisterStartupCallback(
            FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBlenderBridgeEditorModule::RegisterMenus));
    }

    virtual void ShutdownModule() override
    {
        UToolMenus::UnRegisterStartupCallback(this);
        UToolMenus::UnregisterOwner(this);

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

private:
    void RegisterMenus()
    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Tools"));
        FToolMenuSection& Section = Menu->FindOrAddSection(TEXT("BlenderBridge"));
        Section.AddSubMenu(
            TEXT("BlenderBridgeMenu"),
            FText::FromString(TEXT("Blender Bridge")),
            FText::FromString(TEXT("Global Blender Bridge session controls.")),
            FNewToolMenuDelegate::CreateRaw(this, &FBlenderBridgeEditorModule::PopulateBlenderBridgeMenu),
            false,
            FSlateIcon());
    }

    void PopulateBlenderBridgeMenu(UToolMenu* Menu)
    {
        FToolMenuSection& Section = Menu->AddSection(TEXT("Session"), FText::FromString(TEXT("Session")));

        Section.AddMenuEntry(
            TEXT("CreateSession"),
            FText::FromString(TEXT("Create Session")),
            FText::FromString(TEXT("Launch the global Blender Bridge session.")),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &FBlenderBridgeEditorModule::HandleCreateSession)));

        Section.AddMenuEntry(
            TEXT("RestartSession"),
            FText::FromString(TEXT("Restart Session")),
            FText::FromString(TEXT("Restart the global Blender Bridge session and restore loaded sources.")),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &FBlenderBridgeEditorModule::HandleRestartSession)));

        Section.AddMenuEntry(
            TEXT("StopSession"),
            FText::FromString(TEXT("Stop Session")),
            FText::FromString(TEXT("Stop the global Blender Bridge session.")),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &FBlenderBridgeEditorModule::HandleStopSession)));
    }

    void HandleCreateSession()
    {
        FBlenderBridgeRuntimeModule::Get().GetSessionManager().CreateSession();
    }

    void HandleRestartSession()
    {
        FBlenderBridgeRuntimeModule::Get().GetSessionManager().RestartSession();
    }

    void HandleStopSession()
    {
        FBlenderBridgeRuntimeModule::Get().GetSessionManager().StopSession();
    }
};

IMPLEMENT_MODULE(FBlenderBridgeEditorModule, BlenderBridgeEditor)
