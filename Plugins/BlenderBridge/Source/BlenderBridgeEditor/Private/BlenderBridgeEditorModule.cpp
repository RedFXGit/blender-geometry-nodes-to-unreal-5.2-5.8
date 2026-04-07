#include "ActorFactoryBlenderBridgeSource.h"
#include "BlenderBridgeActorDetails.h"
#include "BlenderBridgeBakeService.h"
#include "BlenderBridgeEditorStyle.h"
#include "BlenderBridgePropertyBagDetails.h"

#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSessionManager.h"
#include "BlenderBridgeSettings.h"
#include "CoreGlobals.h"
#include "Editor.h"
#include "Misc/CoreDelegates.h"
#include "Misc/App.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "ToolMenus.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

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

    FLinearColor GetSessionStateColor(const EBlenderBridgeConnectionState ConnectionState)
    {
        switch (ConnectionState)
        {
        case EBlenderBridgeConnectionState::Connected:
            return FLinearColor(0.18f, 0.72f, 0.32f);
        case EBlenderBridgeConnectionState::Launching:
            return FLinearColor(0.92f, 0.73f, 0.18f);
        case EBlenderBridgeConnectionState::Disconnected:
            return FLinearColor(0.85f, 0.22f, 0.22f);
        case EBlenderBridgeConnectionState::Error:
            return FLinearColor(0.78f, 0.12f, 0.12f);
        default:
            return FLinearColor(0.55f, 0.55f, 0.55f);
        }
    }

    FText GetSessionStateLabel(const EBlenderBridgeConnectionState ConnectionState)
    {
        switch (ConnectionState)
        {
        case EBlenderBridgeConnectionState::Connected:
            return FText::FromString(TEXT("Connected"));
        case EBlenderBridgeConnectionState::Launching:
            return FText::FromString(TEXT("Launching"));
        case EBlenderBridgeConnectionState::Disconnected:
            return FText::FromString(TEXT("Disconnected"));
        case EBlenderBridgeConnectionState::Error:
            return FText::FromString(TEXT("Error"));
        default:
            return FText::FromString(TEXT("Unknown"));
        }
    }
}

class FBlenderBridgeEditorModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        FSlateStyleRegistry::RegisterSlateStyle(FBlenderBridgeEditorStyle::Get());
        RegisterBridgePropertyBagCustomization();
        RegisterPostEngineInitWork();
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
    void RegisterPostEngineInitWork()
    {
        if (GEditor != nullptr)
        {
            HandlePostEngineInit();
            return;
        }

        FCoreDelegates::OnPostEngineInit.AddRaw(this, &FBlenderBridgeEditorModule::HandlePostEngineInit);
    }

    void HandlePostEngineInit()
    {
        EnsureBlenderBridgeActorFactoryRegistered();
        AutoStartSessionIfEnabled();
    }

    void AutoStartSessionIfEnabled()
    {
        if (IsRunningCommandlet() || FApp::IsUnattended() || GIsRunningUnattendedScript)
        {
            return;
        }

        const UBlenderBridgeSettings* Settings = GetDefault<UBlenderBridgeSettings>();
        if (Settings == nullptr || !Settings->bAutoStartSessionOnEditorLaunch)
        {
            return;
        }

        FBlenderBridgeSessionManager& SessionManager = FBlenderBridgeRuntimeModule::Get().GetSessionManager();
        if (SessionManager.GetConnectionState() != EBlenderBridgeConnectionState::Disconnected)
        {
            return;
        }

        SessionManager.CreateSession();
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
        Section.AddEntry(BuildSessionStatusEntry());
        Section.AddSeparator(TEXT("BlenderBridgeStatusSeparator"));

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

    FToolMenuEntry BuildSessionStatusEntry() const
    {
        const FBlenderBridgeSessionManager& SessionManager = FBlenderBridgeRuntimeModule::Get().GetSessionManager();
        const EBlenderBridgeConnectionState ConnectionState = SessionManager.GetConnectionState();
        const FLinearColor StatusColor = GetSessionStateColor(ConnectionState);
        const FText StatusLabel = GetSessionStateLabel(ConnectionState);
        const FText ToolTipText = FText::FromString(
            SessionManager.GetLastSessionMessage().IsEmpty()
                ? StatusLabel.ToString()
                : SessionManager.GetLastSessionMessage());

        TSharedRef<SHorizontalBox> StatusWidget =
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(0.f, 0.f, 8.f, 0.f)
            [
                SNew(SBox)
                .WidthOverride(10.f)
                .HeightOverride(10.f)
                [
                    SNew(SBorder)
                    .BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush")))
                    .BorderBackgroundColor(StatusColor)
                    .Padding(0.f)
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(0.f, 0.f, 10.f, 0.f)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("Connection Status")))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(StatusLabel)
                .ColorAndOpacity(FSlateColor(StatusColor))
            ];

        return FToolMenuEntry::InitWidget(
            TEXT("BlenderBridgeSessionStatus"),
            StatusWidget,
            FText::FromString(TEXT("Connection Status")),
            true,
            false,
            false,
            ToolTipText);
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
