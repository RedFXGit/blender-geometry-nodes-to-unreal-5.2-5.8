#include "Tests/BlenderBridgeAutomationTestSupport.h"

#include "BlenderBridgeActor.h"
#include "BlenderBridgeComponent.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSessionManager.h"
#include "BlenderBridgeSettings.h"
#include "BlenderBridgeSource.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Crc.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "ObjectTools.h"
#include "Serialization/JsonSerializer.h"
#include "Tests/AutomationEditorCommon.h"
#include "UObject/GarbageCollection.h"

#if WITH_DEV_AUTOMATION_TESTS

DEFINE_LOG_CATEGORY_STATIC(LogBlenderBridgeAutomation, Log, All);

namespace
{
    using namespace BlenderBridgeAutomation;

    bool LoadManifestRootObject(
        const TCHAR* RelativeConfigPath,
        TSharedPtr<FJsonObject>& OutRootObject,
        FString& OutError)
    {
        OutRootObject.Reset();
        OutError.Reset();

        const FString ManifestPath = GetPluginConfigPath(RelativeConfigPath);
        if (ManifestPath.IsEmpty())
        {
            OutError = FString::Printf(TEXT("Could not resolve the Blender Bridge manifest path '%s'."), RelativeConfigPath);
            return false;
        }

        FString ManifestJson;
        if (!FFileHelper::LoadFileToString(ManifestJson, *ManifestPath))
        {
            OutError = FString::Printf(TEXT("Failed to read manifest '%s'."), *ManifestPath);
            return false;
        }

        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ManifestJson);
        if (!FJsonSerializer::Deserialize(Reader, OutRootObject) || !OutRootObject.IsValid())
        {
            OutError = FString::Printf(TEXT("Failed to parse manifest '%s' as JSON."), *ManifestPath);
            return false;
        }

        return true;
    }

    template <typename ScenarioType>
    bool FindScenarioById(
        const TArray<ScenarioType>& Scenarios,
        const FString& ScenarioId,
        ScenarioType& OutScenario,
        FString& OutError)
    {
        for (const ScenarioType& Scenario : Scenarios)
        {
            if (Scenario.Id == ScenarioId)
            {
                OutScenario = Scenario;
                return true;
            }
        }

        OutError = FString::Printf(TEXT("Scenario '%s' was not found in the manifest."), *ScenarioId);
        return false;
    }
}

namespace BlenderBridgeAutomation
{
    FString GetPluginConfigPath(const TCHAR* RelativeConfigPath)
    {
        const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("BlenderBridge"));
        return Plugin.IsValid()
            ? FPaths::Combine(Plugin->GetBaseDir(), RelativeConfigPath)
            : FString();
    }

    FString GetAutomationRunLabelFromCommandLine()
    {
        FString RunLabel;
        if (FParse::Value(FCommandLine::Get(), TEXT("BlenderBridgeAutomationRunLabel="), RunLabel)
            || FParse::Value(FCommandLine::Get(), TEXT("BlenderBridgeSmokeRunLabel="), RunLabel)
            || FParse::Value(FCommandLine::Get(), TEXT("BlenderBridgePerfRunLabel="), RunLabel))
        {
            RunLabel.TrimQuotesInline();
        }

        return RunLabel.IsEmpty() ? TEXT("manual") : RunLabel;
    }

    bool LoadPerfScenarioManifest(
        const TCHAR* RelativeConfigPath,
        TArray<FBridgeAutomationPerfScenarioDefinition>& OutScenarios,
        FString& OutError)
    {
        OutScenarios.Reset();
        OutError.Reset();

        TSharedPtr<FJsonObject> RootObject;
        if (!LoadManifestRootObject(RelativeConfigPath, RootObject, OutError))
        {
            return false;
        }

        const TArray<TSharedPtr<FJsonValue>>* ScenarioArray = nullptr;
        if (!RootObject->TryGetArrayField(TEXT("scenarios"), ScenarioArray) || ScenarioArray == nullptr)
        {
            OutError = FString::Printf(TEXT("Manifest '%s' is missing a scenarios array."), RelativeConfigPath);
            return false;
        }

        for (const TSharedPtr<FJsonValue>& ScenarioValue : *ScenarioArray)
        {
            const TSharedPtr<FJsonObject>* ScenarioObject = nullptr;
            if (!ScenarioValue.IsValid() || !ScenarioValue->TryGetObject(ScenarioObject) || ScenarioObject == nullptr || !ScenarioObject->IsValid())
            {
                continue;
            }

            FBridgeAutomationPerfScenarioDefinition Scenario;
            double NumericValue = 0.0;
            (*ScenarioObject)->TryGetStringField(TEXT("id"), Scenario.Id);
            (*ScenarioObject)->TryGetStringField(TEXT("display_name"), Scenario.DisplayName);
            (*ScenarioObject)->TryGetStringField(TEXT("blend_file"), Scenario.BlendFile);
            (*ScenarioObject)->TryGetStringField(TEXT("target_id"), Scenario.TargetId);
            (*ScenarioObject)->TryGetStringField(TEXT("action"), Scenario.Action);
            if ((*ScenarioObject)->TryGetNumberField(TEXT("warmup_count"), NumericValue))
            {
                Scenario.WarmupCount = static_cast<int32>(NumericValue);
            }
            if ((*ScenarioObject)->TryGetNumberField(TEXT("measure_count"), NumericValue))
            {
                Scenario.MeasureCount = static_cast<int32>(NumericValue);
            }
            if ((*ScenarioObject)->TryGetNumberField(TEXT("baseline_median_ms"), NumericValue))
            {
                Scenario.BaselineMedianMs = NumericValue;
            }
            if ((*ScenarioObject)->TryGetNumberField(TEXT("allowed_regression_percent"), NumericValue))
            {
                Scenario.AllowedRegressionPercent = NumericValue;
            }
            if ((*ScenarioObject)->TryGetNumberField(TEXT("allowed_regression_ms"), NumericValue))
            {
                Scenario.AllowedRegressionMs = NumericValue;
            }
            (*ScenarioObject)->TryGetBoolField(TEXT("requires_bake_cleanup"), Scenario.bRequiresBakeCleanup);
            (*ScenarioObject)->TryGetBoolField(TEXT("use_automation_scratch_bake_root"), Scenario.bUseAutomationScratchBakeRoot);

            if (!Scenario.Id.IsEmpty())
            {
                OutScenarios.Add(MoveTemp(Scenario));
            }
        }

        if (OutScenarios.IsEmpty())
        {
            OutError = FString::Printf(TEXT("Manifest '%s' did not define any scenarios."), RelativeConfigPath);
            return false;
        }

        return true;
    }

    bool LoadE2EScenarioManifest(
        const TCHAR* RelativeConfigPath,
        TArray<FBridgeAutomationE2EScenarioDefinition>& OutScenarios,
        FString& OutError)
    {
        OutScenarios.Reset();
        OutError.Reset();

        TSharedPtr<FJsonObject> RootObject;
        if (!LoadManifestRootObject(RelativeConfigPath, RootObject, OutError))
        {
            return false;
        }

        const TArray<TSharedPtr<FJsonValue>>* ScenarioArray = nullptr;
        if (!RootObject->TryGetArrayField(TEXT("scenarios"), ScenarioArray) || ScenarioArray == nullptr)
        {
            OutError = FString::Printf(TEXT("Manifest '%s' is missing a scenarios array."), RelativeConfigPath);
            return false;
        }

        for (const TSharedPtr<FJsonValue>& ScenarioValue : *ScenarioArray)
        {
            const TSharedPtr<FJsonObject>* ScenarioObject = nullptr;
            if (!ScenarioValue.IsValid() || !ScenarioValue->TryGetObject(ScenarioObject) || ScenarioObject == nullptr || !ScenarioObject->IsValid())
            {
                continue;
            }

            FBridgeAutomationE2EScenarioDefinition Scenario;
            (*ScenarioObject)->TryGetStringField(TEXT("id"), Scenario.Id);
            (*ScenarioObject)->TryGetStringField(TEXT("display_name"), Scenario.DisplayName);
            (*ScenarioObject)->TryGetStringField(TEXT("blend_file"), Scenario.BlendFile);
            (*ScenarioObject)->TryGetStringField(TEXT("target_id"), Scenario.TargetId);
            (*ScenarioObject)->TryGetStringField(TEXT("scenario_type"), Scenario.ScenarioType);
            (*ScenarioObject)->TryGetBoolField(TEXT("use_automation_scratch_bake_root"), Scenario.bUseAutomationScratchBakeRoot);

            if (!Scenario.Id.IsEmpty())
            {
                OutScenarios.Add(MoveTemp(Scenario));
            }
        }

        if (OutScenarios.IsEmpty())
        {
            OutError = FString::Printf(TEXT("Manifest '%s' did not define any scenarios."), RelativeConfigPath);
            return false;
        }

        return true;
    }

    bool TryFindPerfScenarioDefinition(
        const TCHAR* RelativeConfigPath,
        const FString& ScenarioId,
        FBridgeAutomationPerfScenarioDefinition& OutScenario,
        FString& OutError)
    {
        TArray<FBridgeAutomationPerfScenarioDefinition> Scenarios;
        if (!LoadPerfScenarioManifest(RelativeConfigPath, Scenarios, OutError))
        {
            return false;
        }

        return FindScenarioById(Scenarios, ScenarioId, OutScenario, OutError);
    }

    bool TryFindE2EScenarioDefinition(
        const TCHAR* RelativeConfigPath,
        const FString& ScenarioId,
        FBridgeAutomationE2EScenarioDefinition& OutScenario,
        FString& OutError)
    {
        TArray<FBridgeAutomationE2EScenarioDefinition> Scenarios;
        if (!LoadE2EScenarioManifest(RelativeConfigPath, Scenarios, OutError))
        {
            return false;
        }

        return FindScenarioById(Scenarios, ScenarioId, OutScenario, OutError);
    }

    bool HasAvailableTarget(const UBlenderBridgeComponent* Component, const FString& TargetId)
    {
        if (Component == nullptr)
        {
            return false;
        }

        for (const FBridgeTargetDescriptor& Target : Component->AvailableTargets)
        {
            if (Target.TargetId == TargetId)
            {
                return true;
            }
        }

        return false;
    }

    bool HasCookedPreview(const UBlenderBridgeComponent* Component)
    {
        if (Component == nullptr)
        {
            return false;
        }

        const FBridgeDecodedMesh& Mesh = Component->GetLastCookedMesh();
        return Component->ComponentStatus == EBlenderBridgeComponentStatus::Cooked
            && Mesh.Positions.Num() > 0
            && Mesh.Triangles.Num() > 0;
    }

    bool HasInstancedBakeOutput(const UBlenderBridgeComponent* Component)
    {
        return Component != nullptr
            && Component->ComponentStatus == EBlenderBridgeComponentStatus::Cooked
            && !Component->BakedPrototypeMeshes.IsEmpty()
            && Component->BakedActor.Get() != nullptr;
    }

    bool HasAnyBakedOutput(const UBlenderBridgeComponent* Component)
    {
        return Component != nullptr
            && (Component->BakedActor.Get() != nullptr
                || !Component->BakedPrototypeMeshes.IsEmpty()
                || !Component->BakedStaticMesh.ToSoftObjectPath().IsNull());
    }

    bool CleanupAutomationScratchBakeRoot(FString& OutError)
    {
        OutError.Reset();

        const UBlenderBridgeSettings* Settings = GetDefault<UBlenderBridgeSettings>();
        if (Settings == nullptr || Settings->AutomationPerfScratchBakeRoot.IsEmpty())
        {
            return true;
        }

        const FName ScratchRootPath(*Settings->AutomationPerfScratchBakeRoot);
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

        TArray<FAssetData> ScratchAssets;
        AssetRegistryModule.Get().GetAssetsByPath(ScratchRootPath, ScratchAssets, true);

        TArray<UObject*> AssetsToDelete;
        AssetsToDelete.Reserve(ScratchAssets.Num());
        for (const FAssetData& AssetData : ScratchAssets)
        {
            if (UObject* Asset = AssetData.GetAsset())
            {
                AssetsToDelete.Add(Asset);
            }
        }

        if (!AssetsToDelete.IsEmpty())
        {
            ObjectTools::DeleteObjects(AssetsToDelete, false);
        }

        return true;
    }

    uint32 HashCookedMeshSignature(const FBridgeDecodedMesh& Mesh)
    {
        const int32 PositionCount = Mesh.Positions.Num();
        const int32 TriangleCount = Mesh.Triangles.Num();

        uint32 Hash = FCrc::MemCrc32(&PositionCount, sizeof(PositionCount));
        Hash = FCrc::MemCrc32(&TriangleCount, sizeof(TriangleCount), Hash);

        if (!Mesh.Positions.IsEmpty())
        {
            Hash = FCrc::MemCrc32(Mesh.Positions.GetData(), Mesh.Positions.Num() * sizeof(FVector), Hash);
        }

        if (!Mesh.Triangles.IsEmpty())
        {
            Hash = FCrc::MemCrc32(Mesh.Triangles.GetData(), Mesh.Triangles.Num() * sizeof(int32), Hash);
        }

        return Hash;
    }

    TArray<FString> CollectPrototypeMeshPaths(const UBlenderBridgeComponent* Component)
    {
        TArray<FString> Paths;
        if (Component == nullptr)
        {
            return Paths;
        }

        for (const FBlenderBridgeBakedPrototypeMeshReference& PrototypeReference : Component->BakedPrototypeMeshes)
        {
            const FString Path = PrototypeReference.Mesh.ToSoftObjectPath().ToString();
            if (!Path.IsEmpty())
            {
                Paths.Add(Path);
            }
        }

        Paths.Sort();
        return Paths;
    }

    int32 CountHismComponents(const AActor* Actor)
    {
        if (Actor == nullptr)
        {
            return 0;
        }

        TInlineComponentArray<UHierarchicalInstancedStaticMeshComponent*> HismComponents(Actor);
        return HismComponents.Num();
    }

    int32 CountInstancesAcrossHismComponents(const AActor* Actor)
    {
        if (Actor == nullptr)
        {
            return 0;
        }

        int32 TotalInstances = 0;
        TInlineComponentArray<UHierarchicalInstancedStaticMeshComponent*> HismComponents(Actor);
        for (const UHierarchicalInstancedStaticMeshComponent* HismComponent : HismComponents)
        {
            if (HismComponent != nullptr)
            {
                TotalInstances += HismComponent->GetInstanceCount();
            }
        }

        return TotalInstances;
    }

    FBlenderBridgeAutomationScenarioCommandBase::FBlenderBridgeAutomationScenarioCommandBase(
        FAutomationTestBase* InTest,
        FBridgeAutomationScenarioDefinition InScenario,
        FString InRunLabel,
        const double InTimeoutSeconds)
        : Test(InTest)
        , Scenario(MoveTemp(InScenario))
        , RunLabel(MoveTemp(InRunLabel))
        , TimeoutSeconds(InTimeoutSeconds)
    {
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::Update()
    {
        if (bFinished)
        {
            return true;
        }

        switch (Step)
        {
        case EBaseStep::CreateWorld:
            return HandleCreateWorld();
        case EBaseStep::EnsureSession:
            return HandleEnsureSession();
        case EBaseStep::WaitForSession:
            return HandleWaitForSession();
        case EBaseStep::ConfigureSource:
            return HandleConfigureSource();
        case EBaseStep::WaitForTargets:
            return HandleWaitForTargets();
        case EBaseStep::ConfigureTarget:
            return HandleConfigureTarget();
        case EBaseStep::WaitForInitialCook:
            return HandleWaitForInitialCook();
        case EBaseStep::RunScenarioBody:
            UpdateScenarioBody();
            return bFinished;
        case EBaseStep::CleanupScenario:
            return HandleCleanupScenario();
        case EBaseStep::Done:
        default:
            bFinished = true;
            return true;
        }
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::HandleCreateWorld()
    {
        LogProgress(FString::Printf(TEXT("Starting Blender Bridge automation scenario '%s'."), *Scenario.Id));
        World = FAutomationEditorCommonUtils::CreateNewMap();
        if (World == nullptr)
        {
            return Fail(TEXT("Failed to create a fresh editor world for the automation scenario."));
        }

        if (UPackage* WorldPackage = World->GetOutermost())
        {
            WorldPackage->SetDirtyFlag(false);
        }

        Step = EBaseStep::EnsureSession;
        ResetStepTimeout();
        return false;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::HandleEnsureSession()
    {
        FBlenderBridgeSessionManager& SessionManager = FBlenderBridgeRuntimeModule::Get().GetSessionManager();
        if (SessionManager.IsSessionConnected())
        {
            Step = EBaseStep::ConfigureSource;
            ResetStepTimeout();
            return false;
        }

        if (!SessionManager.CreateSession())
        {
            LogProgress(TEXT("CreateSession returned false; waiting to see whether the shared editor session still connects."));
        }

        Step = EBaseStep::WaitForSession;
        ResetStepTimeout();
        return false;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::HandleWaitForSession()
    {
        if (FBlenderBridgeRuntimeModule::Get().GetSessionManager().IsSessionConnected())
        {
            Step = EBaseStep::ConfigureSource;
            ResetStepTimeout();
            return false;
        }

        if (HasTimedOut(TEXT("Timed out waiting for the Blender Bridge session to connect.")))
        {
            return true;
        }

        return false;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::HandleConfigureSource()
    {
        if (Scenario.bUseAutomationScratchBakeRoot)
        {
            FString CleanupError;
            if (!CleanupAutomationScratchBakeRoot(CleanupError))
            {
                return Fail(CleanupError.IsEmpty()
                    ? TEXT("Failed to clean the automation scratch bake root before the scenario started.")
                    : CleanupError);
            }
        }

        FString FixtureError;
        const FString BlendFilePath = ResolveFixturePath(Scenario.BlendFile, FixtureError);
        if (BlendFilePath.IsEmpty())
        {
            return Fail(FixtureError.IsEmpty()
                ? TEXT("Failed to resolve the fixture path for the automation scenario.")
                : FixtureError);
        }

        UBlenderBridgeSource* Source = CreateTrackedSource(BlendFilePath);
        if (Source == nullptr)
        {
            return Fail(TEXT("Failed to allocate a transient BlenderBridgeSource."));
        }

        TWeakObjectPtr<ABlenderBridgeActor> Actor;
        TWeakObjectPtr<UBlenderBridgeComponent> Component;
        if (!SpawnTrackedActor(Source, Scenario.Id, Actor, Component, true))
        {
            return true;
        }

        PrimarySource = Source;
        PrimaryActor = Actor;
        PrimaryComponent = Component;
        LogProgress(FString::Printf(TEXT("Assigned fixture '%s' for scenario '%s'."), *BlendFilePath, *Scenario.Id));

        Step = EBaseStep::WaitForTargets;
        ResetStepTimeout();
        return false;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::HandleWaitForTargets()
    {
        if (!PrimaryComponent.IsValid())
        {
            return Fail(TEXT("Bridge component became invalid while waiting for available targets."));
        }

        if (PrimaryComponent->ComponentStatus == EBlenderBridgeComponentStatus::Error)
        {
            return Fail(FString::Printf(TEXT("Bridge reported an error while loading the source: %s"), *PrimaryComponent->LastStatusMessage));
        }

        if (HasAvailableTarget(PrimaryComponent.Get(), Scenario.TargetId))
        {
            Step = EBaseStep::ConfigureTarget;
            ResetStepTimeout();
            return false;
        }

        if (HasTimedOut(FString::Printf(TEXT("Timed out waiting for target '%s' to appear."), *Scenario.TargetId)))
        {
            return true;
        }

        return false;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::HandleConfigureTarget()
    {
        if (!PrimaryComponent.IsValid())
        {
            return Fail(TEXT("Bridge component became invalid before the target could be selected."));
        }

        if (!ConfigureTargetAndRequestInitialCook(PrimaryComponent.Get(), Scenario.TargetId, Scenario.bUseAutomationScratchBakeRoot))
        {
            return true;
        }

        LogProgress(FString::Printf(TEXT("Selected target '%s' for scenario '%s'."), *Scenario.TargetId, *Scenario.Id));
        Step = EBaseStep::WaitForInitialCook;
        ResetStepTimeout();
        return false;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::HandleWaitForInitialCook()
    {
        if (!PrimaryComponent.IsValid())
        {
            return Fail(TEXT("Bridge component became invalid during initial target warm-up."));
        }

        if (PrimaryComponent->ComponentStatus == EBlenderBridgeComponentStatus::Error)
        {
            return Fail(FString::Printf(TEXT("Initial target warm-up failed: %s"), *PrimaryComponent->LastStatusMessage));
        }

        if (HasCookedPreview(PrimaryComponent.Get()))
        {
            ClearAutomationContext(PrimaryComponent.Get());
            LogProgress(FString::Printf(TEXT("Initial preview cook completed for scenario '%s'."), *Scenario.Id));
            ResetScenarioState();
            Step = EBaseStep::RunScenarioBody;
            ResetStepTimeout();
            return false;
        }

        if (HasTimedOut(TEXT("Timed out waiting for the initial preview cook after selecting the target.")))
        {
            return true;
        }

        return false;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::HandleCleanupScenario()
    {
        CleanupScenarioState();
        ClearAllAutomationContexts();

        for (const TWeakObjectPtr<UBlenderBridgeComponent>& ComponentPtr : ManagedComponents)
        {
            if (UBlenderBridgeComponent* Component = ComponentPtr.Get())
            {
                if (HasAnyBakedOutput(Component))
                {
                    Component->DeleteBakedOutput();
                }
            }
        }

        if (Scenario.bUseAutomationScratchBakeRoot)
        {
            FString CleanupError;
            if (!CleanupAutomationScratchBakeRoot(CleanupError))
            {
                return Fail(CleanupError.IsEmpty()
                    ? TEXT("Failed to clean the automation scratch bake root during scenario cleanup.")
                    : CleanupError);
            }
        }

        DestroyManagedActors();
        ClearWorldDirtyState();
        ResetManagedObjects();
        CollectGarbage(RF_NoFlags);
        LogProgress(FString::Printf(TEXT("Completed Blender Bridge automation scenario '%s'."), *Scenario.Id));
        Step = EBaseStep::Done;
        bFinished = true;
        return true;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::HasTimedOut(const FString& Message)
    {
        if ((FPlatformTime::Seconds() - StepStartTimeSeconds) < TimeoutSeconds)
        {
            return false;
        }

        return Fail(Message);
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::Fail(const FString& Message)
    {
        LogProgress(FString::Printf(TEXT("Scenario '%s' failed: %s"), *Scenario.Id, *Message));

        if (Test != nullptr)
        {
            Test->AddError(Message);
        }

        ClearAllAutomationContexts();
        DestroyManagedActors();
        ClearWorldDirtyState();
        ResetManagedObjects();
        CollectGarbage(RF_NoFlags);
        bFinished = true;
        Step = EBaseStep::Done;
        return true;
    }

    void FBlenderBridgeAutomationScenarioCommandBase::LogProgress(const FString& Message) const
    {
        if (Test != nullptr)
        {
            Test->AddInfo(Message);
        }

        UE_LOG(LogBlenderBridgeAutomation, Display, TEXT("%s"), *Message);
    }

    void FBlenderBridgeAutomationScenarioCommandBase::BeginCleanup()
    {
        Step = EBaseStep::CleanupScenario;
        ResetStepTimeout();
    }

    void FBlenderBridgeAutomationScenarioCommandBase::ResetStepTimeout()
    {
        StepStartTimeSeconds = FPlatformTime::Seconds();
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::SpawnTrackedActor(
        UBlenderBridgeSource* InSource,
        const FString& LabelSuffix,
        TWeakObjectPtr<ABlenderBridgeActor>& OutActor,
        TWeakObjectPtr<UBlenderBridgeComponent>& OutComponent,
        const bool bNotifySourceChanged)
    {
        OutActor.Reset();
        OutComponent.Reset();

        if (World == nullptr)
        {
            return Fail(TEXT("Automation scenario does not have a valid editor world."));
        }

        FActorSpawnParameters SpawnParameters;
        SpawnParameters.ObjectFlags |= RF_Transient;
        SpawnParameters.OverrideLevel = World->PersistentLevel;
        SpawnParameters.bTemporaryEditorActor = true;
        SpawnParameters.bHideFromSceneOutliner = true;
        SpawnParameters.bCreateActorPackage = false;

        ABlenderBridgeActor* Actor = World->SpawnActor<ABlenderBridgeActor>(ABlenderBridgeActor::StaticClass(), FTransform::Identity, SpawnParameters);
        if (Actor == nullptr)
        {
            return Fail(TEXT("Failed to spawn a transient BlenderBridgeActor."));
        }

#if WITH_EDITOR
        Actor->SetActorLabel(FString::Printf(TEXT("BridgeAutomation_%s"), *LabelSuffix), false);
#endif

        UBlenderBridgeComponent* Component = Actor->GetBridgeComponent();
        if (Component == nullptr)
        {
            Actor->Destroy();
            return Fail(TEXT("Spawned BlenderBridgeActor did not expose a bridge component."));
        }

        ManagedActors.Add(Actor);
        ManagedComponents.Add(Component);

        if (InSource != nullptr)
        {
            Component->AssignSourceFromActorFactory(InSource);
            Component->ClearAutomationRequestContext();

            if (bNotifySourceChanged)
            {
                FBlenderBridgeRuntimeModule::Get().GetSessionManager().NotifySourceChanged(Component);
            }
        }

        OutActor = Actor;
        OutComponent = Component;
        return true;
    }

    UBlenderBridgeSource* FBlenderBridgeAutomationScenarioCommandBase::CreateTrackedSource(const FString& BlendFilePath)
    {
        UBlenderBridgeSource* Source = NewObject<UBlenderBridgeSource>(GetTransientPackage(), NAME_None, RF_Transient);
        if (Source == nullptr)
        {
            return nullptr;
        }

        Source->SetBlendFilePath(BlendFilePath);
        ManagedSources.Emplace(Source);
        return Source;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::ConfigureTargetAndRequestInitialCook(
        UBlenderBridgeComponent* Component,
        const FString& TargetId,
        const bool bUseAutomationScratchBakeRoot)
    {
        if (Component == nullptr)
        {
            return Fail(TEXT("Bridge component became invalid before the target could be configured."));
        }

        Component->SetAutomationRequestContext(
            GetRunLabel(),
            Scenario.Id,
            INDEX_NONE,
            TEXT("initial_target_warmup"),
            true,
            bUseAutomationScratchBakeRoot);
        Component->TargetId = TargetId;
        FBlenderBridgeRuntimeModule::Get().GetSessionManager().NotifyTargetChanged(Component);
        if (Component->ComponentStatus == EBlenderBridgeComponentStatus::Error)
        {
            return Fail(FString::Printf(TEXT("Failed to select target '%s': %s"), *TargetId, *Component->LastStatusMessage));
        }

        return true;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::StartCookAction(
        UBlenderBridgeComponent* Component,
        const bool bMeasured,
        const int32 IterationIndex,
        const FString& ActionKind,
        const bool bUseAutomationScratchBakeRoot)
    {
        if (Component == nullptr)
        {
            return Fail(TEXT("Bridge component became invalid before a cook action could start."));
        }

        Component->SetAutomationRequestContext(
            GetRunLabel(),
            Scenario.Id,
            bMeasured ? IterationIndex : INDEX_NONE,
            ActionKind,
            true,
            bUseAutomationScratchBakeRoot);
        Component->RequestCook();
        if (Component->ComponentStatus == EBlenderBridgeComponentStatus::Error)
        {
            return Fail(FString::Printf(TEXT("Failed to start action '%s': %s"), *ActionKind, *Component->LastStatusMessage));
        }

        return true;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::StartStaticBakeAction(
        UBlenderBridgeComponent* Component,
        const bool bMeasured,
        const int32 IterationIndex,
        const FString& ActionKind,
        const bool bUseAutomationScratchBakeRoot)
    {
        if (Component == nullptr)
        {
            return Fail(TEXT("Bridge component became invalid before a static bake could start."));
        }

        Component->BakeOutputMode = EBlenderBridgeBakeOutputMode::StaticMesh;
        Component->SetAutomationRequestContext(
            GetRunLabel(),
            Scenario.Id,
            bMeasured ? IterationIndex : INDEX_NONE,
            ActionKind,
            true,
            bUseAutomationScratchBakeRoot);
        Component->BakeToStaticMesh();
        if (Component->ComponentStatus == EBlenderBridgeComponentStatus::Error)
        {
            return Fail(FString::Printf(TEXT("Failed to start action '%s': %s"), *ActionKind, *Component->LastStatusMessage));
        }

        return true;
    }

    bool FBlenderBridgeAutomationScenarioCommandBase::StartInstancedBakeAction(
        UBlenderBridgeComponent* Component,
        const bool bMeasured,
        const int32 IterationIndex,
        const FString& ActionKind,
        const bool bUseAutomationScratchBakeRoot)
    {
        if (Component == nullptr)
        {
            return Fail(TEXT("Bridge component became invalid before an instanced bake could start."));
        }

        Component->BakeOutputMode = EBlenderBridgeBakeOutputMode::InstancedHierarchy;
        Component->SetAutomationRequestContext(
            GetRunLabel(),
            Scenario.Id,
            bMeasured ? IterationIndex : INDEX_NONE,
            ActionKind,
            true,
            bUseAutomationScratchBakeRoot);
        Component->BakeToStaticMesh();
        if (Component->ComponentStatus == EBlenderBridgeComponentStatus::Error)
        {
            return Fail(FString::Printf(TEXT("Failed to start action '%s': %s"), *ActionKind, *Component->LastStatusMessage));
        }

        return true;
    }

    void FBlenderBridgeAutomationScenarioCommandBase::ClearAutomationContext(UBlenderBridgeComponent* Component) const
    {
        if (Component != nullptr)
        {
            Component->ClearAutomationRequestContext();
        }
    }

    void FBlenderBridgeAutomationScenarioCommandBase::ClearAllAutomationContexts() const
    {
        for (const TWeakObjectPtr<UBlenderBridgeComponent>& ComponentPtr : ManagedComponents)
        {
            if (UBlenderBridgeComponent* Component = ComponentPtr.Get())
            {
                Component->ClearAutomationRequestContext();
            }
        }
    }

    void FBlenderBridgeAutomationScenarioCommandBase::ClearWorldDirtyState() const
    {
        for (const TWeakObjectPtr<ABlenderBridgeActor>& ActorPtr : ManagedActors)
        {
            if (AActor* Actor = ActorPtr.Get())
            {
                if (UPackage* ActorPackage = Actor->GetOutermost())
                {
                    ActorPackage->SetDirtyFlag(false);
                }
            }
        }

        if (World != nullptr)
        {
            if (UPackage* WorldPackage = World->GetOutermost())
            {
                WorldPackage->SetDirtyFlag(false);
            }
        }
    }

    void FBlenderBridgeAutomationScenarioCommandBase::DestroyManagedActors()
    {
        for (const TWeakObjectPtr<ABlenderBridgeActor>& ActorPtr : ManagedActors)
        {
            if (ABlenderBridgeActor* Actor = ActorPtr.Get())
            {
                Actor->Destroy();
            }
        }
    }

    void FBlenderBridgeAutomationScenarioCommandBase::ResetManagedObjects()
    {
        PrimarySource.Reset();
        PrimaryActor.Reset();
        PrimaryComponent.Reset();
        ManagedComponents.Reset();
        ManagedActors.Reset();
        ManagedSources.Reset();
        World = nullptr;
    }

    FString FBlenderBridgeAutomationScenarioCommandBase::ResolveFixturePath(const FString& RelativeBlendFile, FString& OutError)
    {
        OutError.Reset();

        const UBlenderBridgeSettings* Settings = GetDefault<UBlenderBridgeSettings>();
        if (Settings == nullptr)
        {
            OutError = TEXT("Blender Bridge settings are unavailable.");
            return FString();
        }

        const FString FixtureRoot = FPaths::ConvertRelativePathToFull(Settings->AutomationFixtureRootDirectory.Path);
        if (FixtureRoot.IsEmpty())
        {
            OutError = TEXT("Automation Fixture Root Directory is empty.");
            return FString();
        }

        const FString BlendFilePath = FPaths::Combine(FixtureRoot, RelativeBlendFile);
        if (!FPaths::FileExists(BlendFilePath))
        {
            OutError = FString::Printf(TEXT("Fixture file '%s' does not exist."), *BlendFilePath);
            return FString();
        }

        return BlendFilePath;
    }
}

#endif
