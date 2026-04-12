#include "ActorFactoryGeometryNodesBridgeSource.h"
#include "GeometryNodesBridgeActorDetails.h"
#include "GeometryNodesBridgeBakeService.h"
#include "GeometryNodesBridgeEditorStyle.h"
#include "GeometryNodesBridgePropertyBagDetails.h"

#include "GeometryNodesBridgeRuntimeModule.h"
#include "GeometryNodesBridgeSessionManager.h"
#include "GeometryNodesBridgeSettings.h"
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
            TEXT("GeometryNodesBridgeActor"),
            FOnGetDetailCustomizationInstance::CreateStatic(&FGeometryNodesBridgeActorDetails::MakeInstance));
        PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("InstancedPropertyBag"));
        PropertyModule.RegisterCustomPropertyTypeLayout(
            TEXT("InstancedPropertyBag"),
            FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGeometryNodesBridgePropertyBagDetails::MakeInstance));
        PropertyModule.NotifyCustomizationModuleChanged();
    }

    FLinearColor GetSessionStateColor(const EGeometryNodesBridgeConnectionState ConnectionState)
    {
        switch (ConnectionState)
        {
        case EGeometryNodesBridgeConnectionState::Connected:
            return FLinearColor(0.18f, 0.72f, 0.32f);
        case EGeometryNodesBridgeConnectionState::Launching:
            return FLinearColor(0.92f, 0.73f, 0.18f);
        case EGeometryNodesBridgeConnectionState::Disconnected:
            return FLinearColor(0.85f, 0.22f, 0.22f);
        case EGeometryNodesBridgeConnectionState::Error:
            return FLinearColor(0.78f, 0.12f, 0.12f);
        default:
            return FLinearColor(0.55f, 0.55f, 0.55f);
        }
    }

    FText GetSessionStateLabel(const EGeometryNodesBridgeConnectionState ConnectionState)
    {
        switch (ConnectionState)
        {
        case EGeometryNodesBridgeConnectionState::Connected:
            return FText::FromString(TEXT("Connected"));
        case EGeometryNodesBridgeConnectionState::Launching:
            return FText::FromString(TEXT("Launching"));
        case EGeometryNodesBridgeConnectionState::Disconnected:
            return FText::FromString(TEXT("Disconnected"));
        case EGeometryNodesBridgeConnectionState::Error:
            return FText::FromString(TEXT("Error"));
        default:
            return FText::FromString(TEXT("Unknown"));
        }
    }
}

class FGeometryNodesBridgeEditorModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        FSlateStyleRegistry::RegisterSlateStyle(FGeometryNodesBridgeEditorStyle::Get());
        RegisterBridgePropertyBagCustomization();
        RegisterPostEngineInitWork();
        FGeometryNodesBridgeRuntimeModule::Get().SetBakeBridgeComponentDelegate(
            FGeometryNodesBridgeBakeComponentDelegate::CreateStatic(&FGeometryNodesBridgeBakeService::BakeComponent));
        FGeometryNodesBridgeRuntimeModule::Get().SetSelectBakedOutputDelegate(
            FGeometryNodesBridgeSelectBakedOutputDelegate::CreateStatic(&FGeometryNodesBridgeBakeService::SelectBakedOutput));
        FGeometryNodesBridgeRuntimeModule::Get().SetDeleteBakedOutputDelegate(
            FGeometryNodesBridgeDeleteBakedOutputDelegate::CreateStatic(&FGeometryNodesBridgeBakeService::DeleteBakedOutput));

        UToolMenus::RegisterStartupCallback(
            FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGeometryNodesBridgeEditorModule::RegisterMenus));
    }

    virtual void ShutdownModule() override
    {
        if (FModuleManager::Get().IsModuleLoaded(TEXT("GeometryNodesBridgeRuntime")))
        {
            FGeometryNodesBridgeRuntimeModule::Get().SetBakeBridgeComponentDelegate(FGeometryNodesBridgeBakeComponentDelegate());
            FGeometryNodesBridgeRuntimeModule::Get().SetSelectBakedOutputDelegate(FGeometryNodesBridgeSelectBakedOutputDelegate());
            FGeometryNodesBridgeRuntimeModule::Get().SetDeleteBakedOutputDelegate(FGeometryNodesBridgeDeleteBakedOutputDelegate());
        }

        FSlateStyleRegistry::UnRegisterSlateStyle(FGeometryNodesBridgeEditorStyle::Get());
        FCoreDelegates::OnPostEngineInit.RemoveAll(this);
        UToolMenus::UnRegisterStartupCallback(this);
        UToolMenus::UnregisterOwner(this);

        if (FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor")))
        {
            FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
            PropertyModule.UnregisterCustomClassLayout(TEXT("GeometryNodesBridgeActor"));
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

        FCoreDelegates::OnPostEngineInit.AddRaw(this, &FGeometryNodesBridgeEditorModule::HandlePostEngineInit);
    }

    void HandlePostEngineInit()
    {
        EnsureGeometryNodesBridgeActorFactoryRegistered();
        AutoStartSessionIfEnabled();
    }

    void AutoStartSessionIfEnabled()
    {
        if (IsRunningCommandlet() || FApp::IsUnattended() || GIsRunningUnattendedScript)
        {
            return;
        }

        const UGeometryNodesBridgeSettings* Settings = GetDefault<UGeometryNodesBridgeSettings>();
        if (Settings == nullptr || !Settings->bAutoStartSessionOnEditorLaunch)
        {
            return;
        }

        FGeometryNodesBridgeSessionManager& SessionManager = FGeometryNodesBridgeRuntimeModule::Get().GetSessionManager();
        if (SessionManager.GetConnectionState() != EGeometryNodesBridgeConnectionState::Disconnected)
        {
            return;
        }

        SessionManager.CreateSession();
    }

    void EnsureGeometryNodesBridgeActorFactoryRegistered()
    {
        if (GEditor == nullptr)
        {
            return;
        }

        const bool bAlreadyRegistered = GEditor->ActorFactories.ContainsByPredicate([](const UActorFactory* Factory)
        {
            return IsValid(Factory) && Factory->IsA<UActorFactoryGeometryNodesBridgeSource>();
        });
        if (!bAlreadyRegistered)
        {
            GEditor->ActorFactories.Add(NewObject<UActorFactoryGeometryNodesBridgeSource>());
        }
    }

    void RegisterMenus()
    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Tools"));
        FToolMenuSection& Section = Menu->FindOrAddSection(TEXT("GeometryNodesBridge"));
        Section.AddSubMenu(
            TEXT("GeometryNodesBridgeMenu"),
            FText::FromString(TEXT("Geometry Nodes Bridge")),
            FText::FromString(TEXT("Global Geometry Nodes Bridge session controls.")),
            FNewToolMenuDelegate::CreateRaw(this, &FGeometryNodesBridgeEditorModule::PopulateGeometryNodesBridgeMenu),
            false,
            FSlateIcon());
    }

    void PopulateGeometryNodesBridgeMenu(UToolMenu* Menu)
    {
        FToolMenuSection& Section = Menu->AddSection(TEXT("Session"), FText::FromString(TEXT("Session")));
        Section.AddEntry(BuildSessionStatusEntry());
        Section.AddSeparator(TEXT("GeometryNodesBridgeStatusSeparator"));

        Section.AddMenuEntry(
            TEXT("CreateSession"),
            FText::FromString(TEXT("Create Session")),
            FText::FromString(TEXT("Launch the global Geometry Nodes Bridge session.")),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &FGeometryNodesBridgeEditorModule::HandleCreateSession)));

        Section.AddMenuEntry(
            TEXT("RestartSession"),
            FText::FromString(TEXT("Restart Session")),
            FText::FromString(TEXT("Restart the global Geometry Nodes Bridge session and restore loaded sources.")),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &FGeometryNodesBridgeEditorModule::HandleRestartSession)));

        Section.AddMenuEntry(
            TEXT("StopSession"),
            FText::FromString(TEXT("Stop Session")),
            FText::FromString(TEXT("Stop the global Geometry Nodes Bridge session.")),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateRaw(this, &FGeometryNodesBridgeEditorModule::HandleStopSession)));
    }

    FToolMenuEntry BuildSessionStatusEntry() const
    {
        const FGeometryNodesBridgeSessionManager& SessionManager = FGeometryNodesBridgeRuntimeModule::Get().GetSessionManager();
        const EGeometryNodesBridgeConnectionState ConnectionState = SessionManager.GetConnectionState();
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
            TEXT("GeometryNodesBridgeSessionStatus"),
            StatusWidget,
            FText::FromString(TEXT("Connection Status")),
            true,
            false,
            false,
            ToolTipText);
    }

    void HandleCreateSession()
    {
        FGeometryNodesBridgeRuntimeModule::Get().GetSessionManager().CreateSession();
    }

    void HandleRestartSession()
    {
        FGeometryNodesBridgeRuntimeModule::Get().GetSessionManager().RestartSession();
    }

    void HandleStopSession()
    {
        FGeometryNodesBridgeRuntimeModule::Get().GetSessionManager().StopSession();
    }
};

IMPLEMENT_MODULE(FGeometryNodesBridgeEditorModule, GeometryNodesBridgeEditor)
