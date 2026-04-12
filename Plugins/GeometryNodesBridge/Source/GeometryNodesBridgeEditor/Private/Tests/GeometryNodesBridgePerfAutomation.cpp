#include "Tests/GeometryNodesBridgeAutomationTestSupport.h"

#include "GeometryNodesBridgeComponent.h"
#include "Misc/AutomationTest.h"
#include "UObject/GarbageCollection.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    constexpr TCHAR PerfManifestRelativePath[] = TEXT("Config/PerfBenchmarks.json");

    class FRunGeometryNodesBridgePerfScenarioCommand final
        : public GeometryNodesBridgeAutomation::FGeometryNodesBridgeAutomationScenarioCommandBase
    {
    public:
        FRunGeometryNodesBridgePerfScenarioCommand(
            FAutomationTestBase* InTest,
            GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition InScenario,
            FString InRunLabel)
            : FGeometryNodesBridgeAutomationScenarioCommandBase(InTest, InScenario, MoveTemp(InRunLabel))
            , PerfScenario(MoveTemp(InScenario))
        {
        }

    protected:
        virtual void ResetScenarioState() override
        {
            BodyStep = EPerfStep::StartWarmup;
            WarmupIterationIndex = 0;
            MeasuredIterationIndex = 0;
        }

        virtual void UpdateScenarioBody() override
        {
            switch (BodyStep)
            {
            case EPerfStep::StartWarmup:
                HandleStartWarmup();
                return;
            case EPerfStep::WaitForWarmup:
                HandleWaitForWarmup();
                return;
            case EPerfStep::CleanupWarmupBake:
                HandleCleanupWarmupBake();
                return;
            case EPerfStep::StartMeasure:
                HandleStartMeasure();
                return;
            case EPerfStep::WaitForMeasure:
                HandleWaitForMeasure();
                return;
            case EPerfStep::CleanupMeasuredBake:
                HandleCleanupMeasuredBake();
                return;
            case EPerfStep::Complete:
            default:
                BeginCleanup();
                return;
            }
        }

    private:
        enum class EPerfStep : uint8
        {
            StartWarmup,
            WaitForWarmup,
            CleanupWarmupBake,
            StartMeasure,
            WaitForMeasure,
            CleanupMeasuredBake,
            Complete
        };

        bool IsActionComplete() const
        {
            return PerfScenario.Action == TEXT("instanced_bake")
                ? GeometryNodesBridgeAutomation::HasInstancedBakeOutput(GetPrimaryComponent())
                : GeometryNodesBridgeAutomation::HasCookedPreview(GetPrimaryComponent());
        }

        bool StartScenarioAction(const bool bMeasured, const int32 IterationIndex)
        {
            CollectGarbage(RF_NoFlags);

            if (PerfScenario.Action == TEXT("instanced_bake"))
            {
                return StartInstancedBakeAction(
                    GetPrimaryComponent(),
                    bMeasured,
                    IterationIndex,
                    TEXT("instanced_bake"),
                    PerfScenario.bUseAutomationScratchBakeRoot);
            }

            return StartCookAction(
                GetPrimaryComponent(),
                bMeasured,
                IterationIndex,
                TEXT("request_cook"),
                PerfScenario.bUseAutomationScratchBakeRoot);
        }

        void HandleStartWarmup()
        {
            if (WarmupIterationIndex >= PerfScenario.WarmupCount)
            {
                BodyStep = EPerfStep::StartMeasure;
                ResetStepTimeout();
                return;
            }

            if (!StartScenarioAction(false, WarmupIterationIndex))
            {
                return;
            }

            LogProgress(FString::Printf(TEXT("Started warm-up iteration %d for scenario '%s'."), WarmupIterationIndex, *PerfScenario.Id));
            BodyStep = EPerfStep::WaitForWarmup;
            ResetStepTimeout();
        }

        void HandleWaitForWarmup()
        {
            UGeometryNodesBridgeComponent* Component = GetPrimaryComponent();
            if (Component == nullptr)
            {
                Fail(TEXT("Bridge component became invalid during the warm-up run."));
                return;
            }

            if (Component->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
            {
                Fail(FString::Printf(TEXT("Warm-up iteration %d failed: %s"), WarmupIterationIndex, *Component->LastStatusMessage));
                return;
            }

            if (IsActionComplete())
            {
                ClearAutomationContext(Component);
                LogProgress(FString::Printf(TEXT("Completed warm-up iteration %d for scenario '%s'."), WarmupIterationIndex, *PerfScenario.Id));
                ++WarmupIterationIndex;
                BodyStep = (PerfScenario.Action == TEXT("instanced_bake") && PerfScenario.bRequiresBakeCleanup)
                    ? EPerfStep::CleanupWarmupBake
                    : EPerfStep::StartWarmup;
                ResetStepTimeout();
                return;
            }

            HasTimedOut(FString::Printf(TEXT("Timed out waiting for warm-up iteration %d to finish."), WarmupIterationIndex));
        }

        void HandleCleanupWarmupBake()
        {
            UGeometryNodesBridgeComponent* Component = GetPrimaryComponent();
            if (Component == nullptr)
            {
                Fail(TEXT("Bridge component became invalid before warm-up bake cleanup."));
                return;
            }

            if (GeometryNodesBridgeAutomation::HasAnyBakedOutput(Component))
            {
                Component->DeleteBakedOutput();
                if (Component->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                {
                    Fail(FString::Printf(TEXT("Warm-up bake cleanup failed: %s"), *Component->LastStatusMessage));
                    return;
                }
            }

            BodyStep = EPerfStep::StartWarmup;
            ResetStepTimeout();
        }

        void HandleStartMeasure()
        {
            if (MeasuredIterationIndex >= PerfScenario.MeasureCount)
            {
                BodyStep = EPerfStep::Complete;
                ResetStepTimeout();
                return;
            }

            if (!StartScenarioAction(true, MeasuredIterationIndex))
            {
                return;
            }

            LogProgress(FString::Printf(TEXT("Started measured iteration %d for scenario '%s'."), MeasuredIterationIndex, *PerfScenario.Id));
            BodyStep = EPerfStep::WaitForMeasure;
            ResetStepTimeout();
        }

        void HandleWaitForMeasure()
        {
            UGeometryNodesBridgeComponent* Component = GetPrimaryComponent();
            if (Component == nullptr)
            {
                Fail(TEXT("Bridge component became invalid during a measured iteration."));
                return;
            }

            if (Component->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
            {
                Fail(FString::Printf(TEXT("Measured iteration %d failed: %s"), MeasuredIterationIndex, *Component->LastStatusMessage));
                return;
            }

            if (IsActionComplete())
            {
                ClearAutomationContext(Component);
                LogProgress(FString::Printf(TEXT("Completed measured iteration %d for scenario '%s'."), MeasuredIterationIndex, *PerfScenario.Id));
                ++MeasuredIterationIndex;
                BodyStep = (PerfScenario.Action == TEXT("instanced_bake") && PerfScenario.bRequiresBakeCleanup)
                    ? EPerfStep::CleanupMeasuredBake
                    : EPerfStep::StartMeasure;
                ResetStepTimeout();
                return;
            }

            HasTimedOut(FString::Printf(TEXT("Timed out waiting for measured iteration %d to finish."), MeasuredIterationIndex));
        }

        void HandleCleanupMeasuredBake()
        {
            UGeometryNodesBridgeComponent* Component = GetPrimaryComponent();
            if (Component == nullptr)
            {
                Fail(TEXT("Bridge component became invalid before measured bake cleanup."));
                return;
            }

            if (GeometryNodesBridgeAutomation::HasAnyBakedOutput(Component))
            {
                Component->DeleteBakedOutput();
                if (Component->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                {
                    Fail(FString::Printf(TEXT("Measured bake cleanup failed: %s"), *Component->LastStatusMessage));
                    return;
                }
            }

            BodyStep = EPerfStep::StartMeasure;
            ResetStepTimeout();
        }

    private:
        GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition PerfScenario;
        EPerfStep BodyStep = EPerfStep::StartWarmup;
        int32 WarmupIterationIndex = 0;
        int32 MeasuredIterationIndex = 0;
    };

    IMPLEMENT_COMPLEX_AUTOMATION_TEST(
        FGeometryNodesBridgePerfAutomationTest,
        "GeometryNodesBridge.Perf",
        EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

    void FGeometryNodesBridgePerfAutomationTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
    {
        TArray<GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition> Scenarios;
        FString Error;
        if (!GeometryNodesBridgeAutomation::LoadPerfScenarioManifest(PerfManifestRelativePath, Scenarios, Error))
        {
            OutBeautifiedNames.Add(TEXT("ManifestError"));
            OutTestCommands.Add(TEXT("__manifest_error__"));
            return;
        }

        for (const GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition& Scenario : Scenarios)
        {
            OutBeautifiedNames.Add(Scenario.DisplayName.IsEmpty() ? Scenario.Id : Scenario.DisplayName);
            OutTestCommands.Add(Scenario.Id);
        }
    }

    bool FGeometryNodesBridgePerfAutomationTest::RunTest(const FString& Parameters)
    {
        if (Parameters == TEXT("__manifest_error__"))
        {
            TArray<GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition> Scenarios;
            FString Error;
            GeometryNodesBridgeAutomation::LoadPerfScenarioManifest(PerfManifestRelativePath, Scenarios, Error);
            AddError(Error.IsEmpty() ? TEXT("The Geometry Nodes Bridge perf benchmark manifest failed to load.") : Error);
            return false;
        }

        GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition Scenario;
        FString Error;
        if (!GeometryNodesBridgeAutomation::TryFindPerfScenarioDefinition(PerfManifestRelativePath, Parameters, Scenario, Error))
        {
            AddError(Error);
            return false;
        }

        ADD_LATENT_AUTOMATION_COMMAND(FRunGeometryNodesBridgePerfScenarioCommand(this, Scenario, GeometryNodesBridgeAutomation::GetAutomationRunLabelFromCommandLine()));
        return true;
    }
}

#endif
