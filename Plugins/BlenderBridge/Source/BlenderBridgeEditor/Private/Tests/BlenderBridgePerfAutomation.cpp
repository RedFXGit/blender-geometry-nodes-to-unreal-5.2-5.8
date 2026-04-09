#include "BlenderBridgeActor.h"
#include "BlenderBridgeComponent.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSessionManager.h"
#include "BlenderBridgeSettings.h"
#include "BlenderBridgeSource.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Tests/AutomationEditorCommon.h"
#include "UObject/StrongObjectPtr.h"

#if WITH_DEV_AUTOMATION_TESTS

DEFINE_LOG_CATEGORY_STATIC(LogBlenderBridgePerfAutomation, Log, All);

namespace
{
    constexpr double PerfScenarioTimeoutSeconds = 120.0;

    struct FBridgePerfScenarioDefinition
    {
        FString Id;
        FString DisplayName;
        FString BlendFile;
        FString TargetId;
        FString Action;
        int32 WarmupCount = 0;
        int32 MeasureCount = 0;
        bool bRequiresBakeCleanup = false;
        bool bUseAutomationScratchBakeRoot = false;
    };

    FString GetPerfManifestPath()
    {
        const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("BlenderBridge"));
        return Plugin.IsValid()
            ? FPaths::Combine(Plugin->GetBaseDir(), TEXT("Config/PerfBenchmarks.json"))
            : FString();
    }

    FString GetPerfRunLabelFromCommandLine()
    {
        FString RunLabel;
        if (FParse::Value(FCommandLine::Get(), TEXT("BlenderBridgePerfRunLabel="), RunLabel))
        {
            RunLabel.TrimQuotesInline();
        }

        return RunLabel.IsEmpty() ? TEXT("manual") : RunLabel;
    }

    bool LoadPerfBenchmarksManifest(TArray<FBridgePerfScenarioDefinition>& OutScenarios, FString& OutError)
    {
        OutScenarios.Reset();
        OutError.Reset();

        const FString ManifestPath = GetPerfManifestPath();
        if (ManifestPath.IsEmpty())
        {
            OutError = TEXT("Could not resolve the Blender Bridge perf benchmark manifest path.");
            return false;
        }

        FString ManifestJson;
        if (!FFileHelper::LoadFileToString(ManifestJson, *ManifestPath))
        {
            OutError = FString::Printf(TEXT("Failed to read perf benchmark manifest '%s'."), *ManifestPath);
            return false;
        }

        TSharedPtr<FJsonObject> RootObject;
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ManifestJson);
        if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
        {
            OutError = FString::Printf(TEXT("Failed to parse perf benchmark manifest '%s' as JSON."), *ManifestPath);
            return false;
        }

        const TArray<TSharedPtr<FJsonValue>>* ScenarioArray = nullptr;
        if (!RootObject->TryGetArrayField(TEXT("scenarios"), ScenarioArray) || ScenarioArray == nullptr)
        {
            OutError = FString::Printf(TEXT("Perf benchmark manifest '%s' is missing a scenarios array."), *ManifestPath);
            return false;
        }

        for (const TSharedPtr<FJsonValue>& ScenarioValue : *ScenarioArray)
        {
            const TSharedPtr<FJsonObject>* ScenarioObject = nullptr;
            if (!ScenarioValue.IsValid() || !ScenarioValue->TryGetObject(ScenarioObject) || ScenarioObject == nullptr || !ScenarioObject->IsValid())
            {
                continue;
            }

            FBridgePerfScenarioDefinition Scenario;
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
            (*ScenarioObject)->TryGetBoolField(TEXT("requires_bake_cleanup"), Scenario.bRequiresBakeCleanup);
            (*ScenarioObject)->TryGetBoolField(TEXT("use_automation_scratch_bake_root"), Scenario.bUseAutomationScratchBakeRoot);

            if (!Scenario.Id.IsEmpty())
            {
                OutScenarios.Add(MoveTemp(Scenario));
            }
        }

        if (OutScenarios.IsEmpty())
        {
            OutError = FString::Printf(TEXT("Perf benchmark manifest '%s' did not define any scenarios."), *ManifestPath);
            return false;
        }

        return true;
    }

    bool TryFindScenarioDefinition(const FString& ScenarioId, FBridgePerfScenarioDefinition& OutScenario, FString& OutError)
    {
        TArray<FBridgePerfScenarioDefinition> Scenarios;
        if (!LoadPerfBenchmarksManifest(Scenarios, OutError))
        {
            return false;
        }

        for (const FBridgePerfScenarioDefinition& Scenario : Scenarios)
        {
            if (Scenario.Id == ScenarioId)
            {
                OutScenario = Scenario;
                return true;
            }
        }

        OutError = FString::Printf(TEXT("Perf scenario '%s' was not found in the benchmark manifest."), *ScenarioId);
        return false;
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

    class FRunBlenderBridgePerfScenarioCommand final : public IAutomationLatentCommand
    {
    public:
        FRunBlenderBridgePerfScenarioCommand(
            FAutomationTestBase* InTest,
            FBridgePerfScenarioDefinition InScenario,
            FString InRunLabel)
            : Test(InTest)
            , Scenario(MoveTemp(InScenario))
            , RunLabel(MoveTemp(InRunLabel))
        {
        }

        virtual bool Update() override
        {
            if (bFinished)
            {
                return true;
            }

            switch (Step)
            {
            case EStep::CreateWorld:
                return HandleCreateWorld();
            case EStep::EnsureSession:
                return HandleEnsureSession();
            case EStep::WaitForSession:
                return HandleWaitForSession();
            case EStep::ConfigureSource:
                return HandleConfigureSource();
            case EStep::WaitForTargets:
                return HandleWaitForTargets();
            case EStep::ConfigureTarget:
                return HandleConfigureTarget();
            case EStep::WaitForInitialCook:
                return HandleWaitForInitialCook();
            case EStep::StartWarmup:
                return HandleStartWarmup();
            case EStep::WaitForWarmup:
                return HandleWaitForWarmup();
            case EStep::CleanupWarmupBake:
                return HandleCleanupWarmupBake();
            case EStep::StartMeasure:
                return HandleStartMeasure();
            case EStep::WaitForMeasure:
                return HandleWaitForMeasure();
            case EStep::CleanupMeasuredBake:
                return HandleCleanupMeasuredBake();
            case EStep::CleanupScenario:
                return HandleCleanupScenario();
            case EStep::Done:
            default:
                bFinished = true;
                return true;
            }
        }

    private:
        enum class EStep : uint8
        {
            CreateWorld,
            EnsureSession,
            WaitForSession,
            ConfigureSource,
            WaitForTargets,
            ConfigureTarget,
            WaitForInitialCook,
            StartWarmup,
            WaitForWarmup,
            CleanupWarmupBake,
            StartMeasure,
            WaitForMeasure,
            CleanupMeasuredBake,
            CleanupScenario,
            Done
        };

        bool HandleCreateWorld()
        {
            LogProgress(FString::Printf(TEXT("Starting Blender Bridge perf scenario '%s'."), *Scenario.Id));
            World = FAutomationEditorCommonUtils::CreateNewMap();
            if (World == nullptr)
            {
                return Fail(TEXT("Failed to create a fresh editor world for the perf scenario."));
            }

            if (UPackage* WorldPackage = World->GetOutermost())
            {
                WorldPackage->SetDirtyFlag(false);
            }

            Step = EStep::EnsureSession;
            StepStartTimeSeconds = FPlatformTime::Seconds();
            return false;
        }

        bool HandleEnsureSession()
        {
            FBlenderBridgeSessionManager& SessionManager = FBlenderBridgeRuntimeModule::Get().GetSessionManager();
            if (SessionManager.IsSessionConnected())
            {
                Step = EStep::ConfigureSource;
                StepStartTimeSeconds = FPlatformTime::Seconds();
                return false;
            }

            if (!SessionManager.CreateSession())
            {
                LogProgress(TEXT("CreateSession returned false; waiting to see whether the shared editor session still connects."));
            }

            Step = EStep::WaitForSession;
            StepStartTimeSeconds = FPlatformTime::Seconds();
            return false;
        }

        bool HandleWaitForSession()
        {
            FBlenderBridgeSessionManager& SessionManager = FBlenderBridgeRuntimeModule::Get().GetSessionManager();
            if (SessionManager.IsSessionConnected())
            {
                Step = EStep::ConfigureSource;
                StepStartTimeSeconds = FPlatformTime::Seconds();
                return false;
            }

            if (HasTimedOut(TEXT("Timed out waiting for the Blender Bridge session to connect.")))
            {
                return true;
            }

            return false;
        }

        bool HandleConfigureSource()
        {
            const UBlenderBridgeSettings* Settings = GetDefault<UBlenderBridgeSettings>();
            if (Settings == nullptr)
            {
                return Fail(TEXT("Blender Bridge settings are unavailable."));
            }

            const FString FixtureRoot = FPaths::ConvertRelativePathToFull(Settings->AutomationFixtureRootDirectory.Path);
            if (FixtureRoot.IsEmpty())
            {
                return Fail(TEXT("Automation Fixture Root Directory is empty."));
            }

            const FString BlendFilePath = FPaths::Combine(FixtureRoot, Scenario.BlendFile);
            if (!FPaths::FileExists(BlendFilePath))
            {
                return Fail(FString::Printf(TEXT("Fixture file '%s' does not exist."), *BlendFilePath));
            }

            FixtureSource = TStrongObjectPtr<UBlenderBridgeSource>(NewObject<UBlenderBridgeSource>(GetTransientPackage(), NAME_None, RF_Transient));
            if (!FixtureSource.IsValid())
            {
                return Fail(TEXT("Failed to allocate a transient BlenderBridgeSource."));
            }

            FixtureSource->SetBlendFilePath(BlendFilePath);

            FActorSpawnParameters SpawnParameters;
            SpawnParameters.ObjectFlags |= RF_Transient;
            SpawnParameters.OverrideLevel = World->PersistentLevel;
            SpawnParameters.bTemporaryEditorActor = true;
            SpawnParameters.bHideFromSceneOutliner = true;
            SpawnParameters.bCreateActorPackage = false;
            BridgeActor = World->SpawnActor<ABlenderBridgeActor>(ABlenderBridgeActor::StaticClass(), FTransform::Identity, SpawnParameters);
            if (!BridgeActor.IsValid())
            {
                return Fail(TEXT("Failed to spawn a transient BlenderBridgeActor."));
            }

#if WITH_EDITOR
            BridgeActor->SetActorLabel(FString::Printf(TEXT("BridgePerf_%s"), *Scenario.Id), false);
#endif

            BridgeComponent = BridgeActor->GetBridgeComponent();
            if (!BridgeComponent.IsValid())
            {
                return Fail(TEXT("Spawned BlenderBridgeActor did not expose a bridge component."));
            }

            BridgeComponent->AssignSourceFromActorFactory(FixtureSource.Get());
            BridgeComponent->ClearAutomationRequestContext();
            FBlenderBridgeRuntimeModule::Get().GetSessionManager().NotifySourceChanged(BridgeComponent.Get());
            LogProgress(FString::Printf(TEXT("Assigned fixture '%s' for scenario '%s'."), *BlendFilePath, *Scenario.Id));

            Step = EStep::WaitForTargets;
            StepStartTimeSeconds = FPlatformTime::Seconds();
            return false;
        }

        bool HandleWaitForTargets()
        {
            if (!BridgeComponent.IsValid())
            {
                return Fail(TEXT("Bridge component became invalid while waiting for available targets."));
            }

            if (BridgeComponent->ComponentStatus == EBlenderBridgeComponentStatus::Error)
            {
                return Fail(FString::Printf(TEXT("Bridge reported an error while loading the source: %s"), *BridgeComponent->LastStatusMessage));
            }

            if (HasAvailableTarget(BridgeComponent.Get(), Scenario.TargetId))
            {
                Step = EStep::ConfigureTarget;
                StepStartTimeSeconds = FPlatformTime::Seconds();
                return false;
            }

            if (HasTimedOut(FString::Printf(TEXT("Timed out waiting for target '%s' to appear."), *Scenario.TargetId)))
            {
                return true;
            }

            return false;
        }

        bool HandleConfigureTarget()
        {
            if (!BridgeComponent.IsValid())
            {
                return Fail(TEXT("Bridge component became invalid before the target could be selected."));
            }

            BridgeComponent->TargetId = Scenario.TargetId;
            FBlenderBridgeRuntimeModule::Get().GetSessionManager().NotifyTargetChanged(BridgeComponent.Get());
            LogProgress(FString::Printf(TEXT("Selected target '%s' for scenario '%s'."), *Scenario.TargetId, *Scenario.Id));

            Step = EStep::WaitForInitialCook;
            StepStartTimeSeconds = FPlatformTime::Seconds();
            return false;
        }

        bool HandleWaitForInitialCook()
        {
            if (!BridgeComponent.IsValid())
            {
                return Fail(TEXT("Bridge component became invalid during initial target warm-up."));
            }

            if (BridgeComponent->ComponentStatus == EBlenderBridgeComponentStatus::Error)
            {
                return Fail(FString::Printf(TEXT("Initial target warm-up failed: %s"), *BridgeComponent->LastStatusMessage));
            }

            if (HasCookedPreview(BridgeComponent.Get()))
            {
                LogProgress(FString::Printf(TEXT("Initial preview cook completed for scenario '%s'."), *Scenario.Id));
                Step = EStep::StartWarmup;
                StepStartTimeSeconds = FPlatformTime::Seconds();
                return false;
            }

            if (HasTimedOut(TEXT("Timed out waiting for the initial preview cook after selecting the target.")))
            {
                return true;
            }

            return false;
        }

        bool HandleStartWarmup()
        {
            if (WarmupIterationIndex >= Scenario.WarmupCount)
            {
                Step = EStep::StartMeasure;
                StepStartTimeSeconds = FPlatformTime::Seconds();
                return false;
            }

            if (!StartScenarioAction(false, WarmupIterationIndex))
            {
                return true;
            }

            LogProgress(FString::Printf(TEXT("Started warm-up iteration %d for scenario '%s'."), WarmupIterationIndex, *Scenario.Id));
            Step = EStep::WaitForWarmup;
            StepStartTimeSeconds = FPlatformTime::Seconds();
            return false;
        }

        bool HandleWaitForWarmup()
        {
            if (!BridgeComponent.IsValid())
            {
                return Fail(TEXT("Bridge component became invalid during the warm-up run."));
            }

            if (BridgeComponent->ComponentStatus == EBlenderBridgeComponentStatus::Error)
            {
                return Fail(FString::Printf(TEXT("Warm-up iteration %d failed: %s"), WarmupIterationIndex, *BridgeComponent->LastStatusMessage));
            }

            const bool bCompleted = Scenario.Action == TEXT("instanced_bake")
                ? HasInstancedBakeOutput(BridgeComponent.Get())
                : HasCookedPreview(BridgeComponent.Get());
            if (bCompleted)
            {
                BridgeComponent->ClearAutomationRequestContext();
                LogProgress(FString::Printf(TEXT("Completed warm-up iteration %d for scenario '%s'."), WarmupIterationIndex, *Scenario.Id));
                ++WarmupIterationIndex;

                if (Scenario.Action == TEXT("instanced_bake") && Scenario.bRequiresBakeCleanup)
                {
                    Step = EStep::CleanupWarmupBake;
                }
                else
                {
                    Step = EStep::StartWarmup;
                }

                StepStartTimeSeconds = FPlatformTime::Seconds();
                return false;
            }

            if (HasTimedOut(FString::Printf(TEXT("Timed out waiting for warm-up iteration %d to finish."), WarmupIterationIndex)))
            {
                return true;
            }

            return false;
        }

        bool HandleCleanupWarmupBake()
        {
            if (!BridgeComponent.IsValid())
            {
                return Fail(TEXT("Bridge component became invalid before warm-up bake cleanup."));
            }

            if (HasAnyBakedOutput(BridgeComponent.Get()))
            {
                BridgeComponent->DeleteBakedOutput();
                if (BridgeComponent->ComponentStatus == EBlenderBridgeComponentStatus::Error)
                {
                    return Fail(FString::Printf(TEXT("Warm-up bake cleanup failed: %s"), *BridgeComponent->LastStatusMessage));
                }
            }

            Step = EStep::StartWarmup;
            StepStartTimeSeconds = FPlatformTime::Seconds();
            return false;
        }

        bool HandleStartMeasure()
        {
            if (MeasuredIterationIndex >= Scenario.MeasureCount)
            {
                Step = EStep::CleanupScenario;
                StepStartTimeSeconds = FPlatformTime::Seconds();
                return false;
            }

            if (!StartScenarioAction(true, MeasuredIterationIndex))
            {
                return true;
            }

            LogProgress(FString::Printf(TEXT("Started measured iteration %d for scenario '%s'."), MeasuredIterationIndex, *Scenario.Id));
            Step = EStep::WaitForMeasure;
            StepStartTimeSeconds = FPlatformTime::Seconds();
            return false;
        }

        bool HandleWaitForMeasure()
        {
            if (!BridgeComponent.IsValid())
            {
                return Fail(TEXT("Bridge component became invalid during a measured iteration."));
            }

            if (BridgeComponent->ComponentStatus == EBlenderBridgeComponentStatus::Error)
            {
                return Fail(FString::Printf(TEXT("Measured iteration %d failed: %s"), MeasuredIterationIndex, *BridgeComponent->LastStatusMessage));
            }

            const bool bCompleted = Scenario.Action == TEXT("instanced_bake")
                ? HasInstancedBakeOutput(BridgeComponent.Get())
                : HasCookedPreview(BridgeComponent.Get());
            if (bCompleted)
            {
                BridgeComponent->ClearAutomationRequestContext();
                LogProgress(FString::Printf(TEXT("Completed measured iteration %d for scenario '%s'."), MeasuredIterationIndex, *Scenario.Id));
                ++MeasuredIterationIndex;

                if (Scenario.Action == TEXT("instanced_bake") && Scenario.bRequiresBakeCleanup)
                {
                    Step = EStep::CleanupMeasuredBake;
                }
                else
                {
                    Step = EStep::StartMeasure;
                }

                StepStartTimeSeconds = FPlatformTime::Seconds();
                return false;
            }

            if (HasTimedOut(FString::Printf(TEXT("Timed out waiting for measured iteration %d to finish."), MeasuredIterationIndex)))
            {
                return true;
            }

            return false;
        }

        bool HandleCleanupMeasuredBake()
        {
            if (!BridgeComponent.IsValid())
            {
                return Fail(TEXT("Bridge component became invalid before measured bake cleanup."));
            }

            if (HasAnyBakedOutput(BridgeComponent.Get()))
            {
                BridgeComponent->DeleteBakedOutput();
                if (BridgeComponent->ComponentStatus == EBlenderBridgeComponentStatus::Error)
                {
                    return Fail(FString::Printf(TEXT("Measured bake cleanup failed: %s"), *BridgeComponent->LastStatusMessage));
                }
            }

            Step = EStep::StartMeasure;
            StepStartTimeSeconds = FPlatformTime::Seconds();
            return false;
        }

        bool HandleCleanupScenario()
        {
            if (BridgeComponent.IsValid())
            {
                BridgeComponent->ClearAutomationRequestContext();
                if (HasAnyBakedOutput(BridgeComponent.Get()))
                {
                    BridgeComponent->DeleteBakedOutput();
                }
            }

            if (BridgeActor.IsValid())
            {
                BridgeActor->Destroy();
            }

            ClearWorldDirtyState();
            LogProgress(FString::Printf(TEXT("Completed Blender Bridge perf scenario '%s'."), *Scenario.Id));
            Step = EStep::Done;
            StepStartTimeSeconds = FPlatformTime::Seconds();
            return false;
        }

        bool StartScenarioAction(const bool bMeasured, const int32 IterationIndex)
        {
            if (!BridgeComponent.IsValid())
            {
                return Fail(TEXT("Bridge component became invalid before an automation action could start."));
            }

            const FString ContextRunLabel = bMeasured ? RunLabel : FString();
            const FString ContextScenarioId = bMeasured ? Scenario.Id : FString();
            const FString ContextActionKind = bMeasured ? Scenario.Action : FString();
            const int32 ContextIterationIndex = bMeasured ? IterationIndex : INDEX_NONE;
            BridgeComponent->SetAutomationRequestContext(
                ContextRunLabel,
                ContextScenarioId,
                ContextIterationIndex,
                ContextActionKind,
                true,
                Scenario.bUseAutomationScratchBakeRoot);

            if (Scenario.Action == TEXT("instanced_bake"))
            {
                BridgeComponent->BakeOutputMode = EBlenderBridgeBakeOutputMode::InstancedHierarchy;
                BridgeComponent->BakeToStaticMesh();
            }
            else
            {
                BridgeComponent->RequestCook();
            }

            if (BridgeComponent->ComponentStatus == EBlenderBridgeComponentStatus::Error)
            {
                return Fail(FString::Printf(TEXT("Failed to start action '%s': %s"), *Scenario.Action, *BridgeComponent->LastStatusMessage));
            }

            return true;
        }

        bool HasTimedOut(const FString& Message)
        {
            if ((FPlatformTime::Seconds() - StepStartTimeSeconds) < PerfScenarioTimeoutSeconds)
            {
                return false;
            }

            return Fail(Message);
        }

        bool Fail(const FString& Message)
        {
            LogProgress(FString::Printf(TEXT("Scenario '%s' failed: %s"), *Scenario.Id, *Message));

            if (Test != nullptr)
            {
                Test->AddError(Message);
            }

            if (BridgeComponent.IsValid())
            {
                BridgeComponent->ClearAutomationRequestContext();
            }

            if (BridgeActor.IsValid())
            {
                BridgeActor->Destroy();
            }

            ClearWorldDirtyState();
            bFinished = true;
            Step = EStep::Done;
            return true;
        }

        void LogProgress(const FString& Message) const
        {
            if (Test != nullptr)
            {
                Test->AddInfo(Message);
            }

            UE_LOG(LogBlenderBridgePerfAutomation, Display, TEXT("%s"), *Message);
        }

        void ClearWorldDirtyState() const
        {
            if (BridgeActor.IsValid())
            {
                if (UPackage* ActorPackage = BridgeActor->GetOutermost())
                {
                    ActorPackage->SetDirtyFlag(false);
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

    private:
        FAutomationTestBase* Test = nullptr;
        FBridgePerfScenarioDefinition Scenario;
        FString RunLabel;
        UWorld* World = nullptr;
        TStrongObjectPtr<UBlenderBridgeSource> FixtureSource;
        TWeakObjectPtr<ABlenderBridgeActor> BridgeActor;
        TWeakObjectPtr<UBlenderBridgeComponent> BridgeComponent;
        EStep Step = EStep::CreateWorld;
        int32 WarmupIterationIndex = 0;
        int32 MeasuredIterationIndex = 0;
        double StepStartTimeSeconds = 0.0;
        bool bFinished = false;
    };

    IMPLEMENT_COMPLEX_AUTOMATION_TEST(
        FBlenderBridgePerfAutomationTest,
        "BlenderBridge.Perf",
        EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

    void FBlenderBridgePerfAutomationTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
    {
        TArray<FBridgePerfScenarioDefinition> Scenarios;
        FString Error;
        if (!LoadPerfBenchmarksManifest(Scenarios, Error))
        {
            OutBeautifiedNames.Add(TEXT("ManifestError"));
            OutTestCommands.Add(TEXT("__manifest_error__"));
            return;
        }

        for (const FBridgePerfScenarioDefinition& Scenario : Scenarios)
        {
            OutBeautifiedNames.Add(Scenario.DisplayName.IsEmpty() ? Scenario.Id : Scenario.DisplayName);
            OutTestCommands.Add(Scenario.Id);
        }
    }

    bool FBlenderBridgePerfAutomationTest::RunTest(const FString& Parameters)
    {
        if (Parameters == TEXT("__manifest_error__"))
        {
            TArray<FBridgePerfScenarioDefinition> Scenarios;
            FString Error;
            LoadPerfBenchmarksManifest(Scenarios, Error);
            AddError(Error.IsEmpty() ? TEXT("The Blender Bridge perf benchmark manifest failed to load.") : Error);
            return false;
        }

        FBridgePerfScenarioDefinition Scenario;
        FString Error;
        if (!TryFindScenarioDefinition(Parameters, Scenario, Error))
        {
            AddError(Error);
            return false;
        }

        ADD_LATENT_AUTOMATION_COMMAND(FRunBlenderBridgePerfScenarioCommand(this, Scenario, GetPerfRunLabelFromCommandLine()));
        return true;
    }
}

#endif
