#include "ActorFactoryBlenderBridgeSource.h"
#include "BlenderBridgeActorDetails.h"
#include "BlenderBridgeBakeService.h"
#include "BlenderBridgeEditorStyle.h"
#include "BlenderBridgePropertyBagDetails.h"

#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSessionManager.h"
#include "Editor.h"
#include "Misc/CoreDelegates.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Styling/SlateStyleRegistry.h"
#include "ToolMenus.h"

namespace
{
    void RegisterBridgePropertyBagCustomization()
    {
        FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("StructUtilsEditor"));

        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
        PropertyModule.RegisterCustomClassLayout(
            TEXT("BlenderBridgeActor"),
            FOnGetDetailCustomizationInstance::CreateStatic(&FBlenderBridgeActorDetails::MakeInstance));
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
        FSlateStyleRegistry::RegisterSlateStyle(FBlenderBridgeEditorStyle::Get());
        RegisterBridgePropertyBagCustomization();
        RegisterBlenderBridgeActorFactory();
        FBlenderBridgeRuntimeModule::Get().SetBakeBridgeComponentDelegate(
            FBlenderBridgeBakeComponentDelegate::CreateStatic(&FBlenderBridgeBakeService::BakeComponent));
        FBlenderBridgeRuntimeModule::Get().SetSelectBakedOutputDelegate(
            FBlenderBridgeSelectBakedOutputDelegate::CreateStatic(&FBlenderBridgeBakeService::SelectBakedOutput));
        FBlenderBridgeRuntimeModule::Get().SetDeleteBakedOutputDelegate(
            FBlenderBridgeDeleteBakedOutputDelegate::CreateStatic(&FBlenderBridgeBakeService::DeleteBakedOutput));

        UToolMenus::RegisterStartupCallback(
            FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBlenderBridgeEditorModule::RegisterMenus));
    }

    virtual void ShutdownModule() override
    {
        if (FModuleManager::Get().IsModuleLoaded(TEXT("BlenderBridgeRuntime")))
        {
            FBlenderBridgeRuntimeModule::Get().SetBakeBridgeComponentDelegate(FBlenderBridgeBakeComponentDelegate());
            FBlenderBridgeRuntimeModule::Get().SetSelectBakedOutputDelegate(FBlenderBridgeSelectBakedOutputDelegate());
            FBlenderBridgeRuntimeModule::Get().SetDeleteBakedOutputDelegate(FBlenderBridgeDeleteBakedOutputDelegate());
        }

        FSlateStyleRegistry::UnRegisterSlateStyle(FBlenderBridgeEditorStyle::Get());
        FCoreDelegates::OnPostEngineInit.RemoveAll(this);
        UToolMenus::UnRegisterStartupCallback(this);
        UToolMenus::UnregisterOwner(this);

        if (FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor")))
        {
            FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
            PropertyModule.UnregisterCustomClassLayout(TEXT("BlenderBridgeActor"));
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
    void RegisterBlenderBridgeActorFactory()
    {
        if (GEditor != nullptr)
        {
            EnsureBlenderBridgeActorFactoryRegistered();
            return;
        }

        FCoreDelegates::OnPostEngineInit.AddRaw(this, &FBlenderBridgeEditorModule::EnsureBlenderBridgeActorFactoryRegistered);
    }

    void EnsureBlenderBridgeActorFactoryRegistered()
    {
        if (GEditor == nullptr)
        {
            return;
        }

        const bool bAlreadyRegistered = GEditor->ActorFactories.ContainsByPredicate([](const UActorFactory* Factory)
        {
            return IsValid(Factory) && Factory->IsA<UActorFactoryBlenderBridgeSource>();
        });
        if (!bAlreadyRegistered)
        {
            GEditor->ActorFactories.Add(NewObject<UActorFactoryBlenderBridgeSource>());
        }
    }

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
