#include "Tests/GeometryNodesBridgeAutomationTestSupport.h"

#include "GeometryNodesBridgeActor.h"
#include "GeometryNodesBridgeComponent.h"
#include "Components/SplineComponent.h"
#include "Misc/AutomationTest.h"
#include "UObject/GarbageCollection.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    constexpr TCHAR E2EManifestRelativePath[] = TEXT("Config/E2ESmokeScenarios.json");
    constexpr TCHAR PerfSmokeManifestRelativePath[] = TEXT("Config/PerfSmokeBenchmarks.json");

    bool HasStaticBakeOutput(const UGeometryNodesBridgeComponent* Component)
    {
        return Component != nullptr
            && Component->BakedActor.Get() != nullptr
            && !Component->BakedStaticMesh.ToSoftObjectPath().IsNull();
    }

    class FRunGeometryNodesBridgeE2EScenarioCommand final
        : public GeometryNodesBridgeAutomation::FGeometryNodesBridgeAutomationScenarioCommandBase
    {
    public:
        FRunGeometryNodesBridgeE2EScenarioCommand(
            FAutomationTestBase* InTest,
            GeometryNodesBridgeAutomation::FBridgeAutomationE2EScenarioDefinition InScenario,
            FString InRunLabel)
            : FGeometryNodesBridgeAutomationScenarioCommandBase(InTest, InScenario, MoveTemp(InRunLabel))
            , E2EScenario(MoveTemp(InScenario))
        {
        }

    protected:
        virtual void ResetScenarioState() override
        {
            HouseStep = EHouseStep::ValidateInitial;
            CurveStep = ECurveStep::ValidateInitial;
            InstancesStep = EInstancesStep::ValidateInitial;
            InitialMeshHash = 0;
            SplinePointIndex = INDEX_NONE;
            PrimaryPrototypeMeshPaths.Reset();
            SecondaryActor.Reset();
            SecondaryComponent.Reset();
        }

        virtual void UpdateScenarioBody() override
        {
            if (E2EScenario.ScenarioType == TEXT("house_static_bake"))
            {
                UpdateHouseScenario();
                return;
            }

            if (E2EScenario.ScenarioType == TEXT("curve_spline_recook"))
            {
                UpdateCurveScenario();
                return;
            }

            if (E2EScenario.ScenarioType == TEXT("instances_instanced_bake_sharing"))
            {
                UpdateInstancesScenario();
                return;
            }

            Fail(FString::Printf(TEXT("Unknown E2E scenario type '%s'."), *E2EScenario.ScenarioType));
        }

    private:
        enum class EHouseStep : uint8
        {
            ValidateInitial,
            WaitForBake,
            VerifyCleanup
        };

        enum class ECurveStep : uint8
        {
            ValidateInitial,
            WaitForRecook
        };

        enum class EInstancesStep : uint8
        {
            ValidateInitial,
            WaitForPrimaryBake,
            WaitForSecondaryTargets,
            WaitForSecondaryInitialCook,
            WaitForSecondaryBake,
            VerifyCleanup
        };

        void UpdateHouseScenario()
        {
            UGeometryNodesBridgeComponent* Component = GetPrimaryComponent();
            AGeometryNodesBridgeActor* Actor = GetPrimaryActor();
            if (Component == nullptr || Actor == nullptr)
            {
                Fail(TEXT("Primary bridge actor became invalid during the house E2E scenario."));
                return;
            }

            switch (HouseStep)
            {
            case EHouseStep::ValidateInitial:
                if (Actor->GetBridgeSplineComponent() != nullptr)
                {
                    Fail(TEXT("House static-bake scenario unexpectedly created a bridge spline component."));
                    return;
                }

                if (Component->TargetCapabilities.bSupportsCurveOwner)
                {
                    Fail(TEXT("House static-bake scenario unexpectedly reported curve-owner compatibility."));
                    return;
                }

                if (!GeometryNodesBridgeAutomation::HasCookedPreview(Component))
                {
                    Fail(TEXT("House static-bake scenario did not produce an initial cooked preview."));
                    return;
                }

                if (Component->InputSchema.IsEmpty())
                {
                    Fail(TEXT("House static-bake scenario did not reflect any input schema entries."));
                    return;
                }

                if (Component->LastCookedMaterialSlots.IsEmpty())
                {
                    Fail(TEXT("House static-bake scenario did not report any cooked material slots."));
                    return;
                }

                if (!StartStaticBakeAction(Component, false, INDEX_NONE, TEXT("static_bake_smoke"), E2EScenario.bUseAutomationScratchBakeRoot))
                {
                    return;
                }

                HouseStep = EHouseStep::WaitForBake;
                ResetStepTimeout();
                return;

            case EHouseStep::WaitForBake:
                if (Component->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                {
                    Fail(FString::Printf(TEXT("House static bake failed: %s"), *Component->LastStatusMessage));
                    return;
                }

                if (HasStaticBakeOutput(Component))
                {
                    ClearAutomationContext(Component);

                    if (!Component->BakedPrototypeMeshes.IsEmpty())
                    {
                        Fail(TEXT("Static bake unexpectedly populated shared prototype meshes."));
                        return;
                    }

                    Component->DeleteBakedOutput();
                    if (Component->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                    {
                        Fail(FString::Printf(TEXT("House static-bake cleanup failed: %s"), *Component->LastStatusMessage));
                        return;
                    }

                    HouseStep = EHouseStep::VerifyCleanup;
                    ResetStepTimeout();
                    return;
                }

                HasTimedOut(TEXT("Timed out waiting for the static bake output."));
                return;

            case EHouseStep::VerifyCleanup:
            default:
                if (GeometryNodesBridgeAutomation::HasAnyBakedOutput(Component))
                {
                    Fail(TEXT("House static-bake cleanup left baked output references behind."));
                    return;
                }

                BeginCleanup();
                return;
            }
        }

        void UpdateCurveScenario()
        {
            UGeometryNodesBridgeComponent* Component = GetPrimaryComponent();
            AGeometryNodesBridgeActor* Actor = GetPrimaryActor();
            if (Component == nullptr || Actor == nullptr)
            {
                Fail(TEXT("Primary bridge actor became invalid during the curve E2E scenario."));
                return;
            }

            switch (CurveStep)
            {
            case ECurveStep::ValidateInitial:
            {
                if (!Component->TargetCapabilities.bSupportsCurveOwner)
                {
                    Fail(TEXT("Curve E2E scenario did not report curve-owner compatibility."));
                    return;
                }

                USplineComponent* SplineComponent = Actor->GetBridgeSplineComponent();
                if (SplineComponent == nullptr)
                {
                    Fail(TEXT("Curve E2E scenario did not create a bridge spline component."));
                    return;
                }

                const int32 NumSplinePoints = SplineComponent->GetNumberOfSplinePoints();
                if (NumSplinePoints <= 0)
                {
                    Fail(TEXT("Curve E2E scenario did not import any spline points."));
                    return;
                }

                InitialMeshHash = GeometryNodesBridgeAutomation::HashCookedMeshSignature(Component->GetLastCookedMesh());
                SplinePointIndex = NumSplinePoints > 1 ? 1 : 0;

                Component->SetAutomationRequestContext(
                    GetRunLabel(),
                    E2EScenario.Id,
                    INDEX_NONE,
                    TEXT("curve_recook"),
                    true,
                    false);

                const FVector OriginalLocation =
                    SplineComponent->GetLocationAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::Local);
                SplineComponent->SetLocationAtSplinePoint(
                    SplinePointIndex,
                    OriginalLocation + FVector(0.0f, 0.0f, 150.0f),
                    ESplineCoordinateSpace::Local,
                    true);

                CurveStep = ECurveStep::WaitForRecook;
                ResetStepTimeout();
                return;
            }

            case ECurveStep::WaitForRecook:
            default:
                if (Component->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                {
                    Fail(FString::Printf(TEXT("Curve recook failed: %s"), *Component->LastStatusMessage));
                    return;
                }

                if (GeometryNodesBridgeAutomation::HasCookedPreview(Component))
                {
                    const uint32 UpdatedMeshHash =
                        GeometryNodesBridgeAutomation::HashCookedMeshSignature(Component->GetLastCookedMesh());
                    if (UpdatedMeshHash != InitialMeshHash)
                    {
                        ClearAutomationContext(Component);
                        BeginCleanup();
                        return;
                    }
                }

                HasTimedOut(TEXT("Timed out waiting for the spline edit to trigger a changed cooked mesh."));
                return;
            }
        }

        void UpdateInstancesScenario()
        {
            UGeometryNodesBridgeComponent* Primary = GetPrimaryComponent();
            if (Primary == nullptr)
            {
                Fail(TEXT("Primary bridge component became invalid during the instances E2E scenario."));
                return;
            }

            switch (InstancesStep)
            {
            case EInstancesStep::ValidateInitial:
                if (!GeometryNodesBridgeAutomation::HasCookedPreview(Primary))
                {
                    Fail(TEXT("Instances E2E scenario did not produce an initial cooked preview."));
                    return;
                }

                if (!StartInstancedBakeAction(Primary, false, INDEX_NONE, TEXT("instanced_bake_smoke"), E2EScenario.bUseAutomationScratchBakeRoot))
                {
                    return;
                }

                InstancesStep = EInstancesStep::WaitForPrimaryBake;
                ResetStepTimeout();
                return;

            case EInstancesStep::WaitForPrimaryBake:
                if (Primary->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                {
                    Fail(FString::Printf(TEXT("Primary instanced bake failed: %s"), *Primary->LastStatusMessage));
                    return;
                }

                if (GeometryNodesBridgeAutomation::HasInstancedBakeOutput(Primary))
                {
                    ClearAutomationContext(Primary);

                    if (!Primary->BakedStaticMesh.ToSoftObjectPath().IsNull())
                    {
                        Fail(TEXT("Instanced bake unexpectedly populated the baked static mesh reference."));
                        return;
                    }

                    if (GeometryNodesBridgeAutomation::CountHismComponents(Primary->BakedActor.Get()) <= 0)
                    {
                        Fail(TEXT("Primary instanced bake did not create any HISM components."));
                        return;
                    }

                    if (GeometryNodesBridgeAutomation::CountInstancesAcrossHismComponents(Primary->BakedActor.Get()) <= 0)
                    {
                        Fail(TEXT("Primary instanced bake did not create any HISM instances."));
                        return;
                    }

                    PrimaryPrototypeMeshPaths = GeometryNodesBridgeAutomation::CollectPrototypeMeshPaths(Primary);
                    if (PrimaryPrototypeMeshPaths.IsEmpty())
                    {
                        Fail(TEXT("Primary instanced bake did not store any shared prototype mesh references."));
                        return;
                    }

                    if (!SpawnTrackedActor(
                            GetPrimarySource(),
                            FString::Printf(TEXT("%s_Duplicate"), *E2EScenario.Id),
                            SecondaryActor,
                            SecondaryComponent,
                            true))
                    {
                        return;
                    }

                    InstancesStep = EInstancesStep::WaitForSecondaryTargets;
                    ResetStepTimeout();
                    return;
                }

                HasTimedOut(TEXT("Timed out waiting for the primary instanced bake output."));
                return;

            case EInstancesStep::WaitForSecondaryTargets:
                if (!SecondaryComponent.IsValid())
                {
                    Fail(TEXT("Secondary bridge component became invalid before target discovery completed."));
                    return;
                }

                if (SecondaryComponent->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                {
                    Fail(FString::Printf(TEXT("Secondary source load failed: %s"), *SecondaryComponent->LastStatusMessage));
                    return;
                }

                if (GeometryNodesBridgeAutomation::HasAvailableTarget(SecondaryComponent.Get(), E2EScenario.TargetId))
                {
                    if (!ConfigureTargetAndRequestInitialCook(
                            SecondaryComponent.Get(),
                            E2EScenario.TargetId,
                            E2EScenario.bUseAutomationScratchBakeRoot))
                    {
                        return;
                    }

                    LogProgress(FString::Printf(TEXT("Selected duplicate target '%s' for scenario '%s'."), *E2EScenario.TargetId, *E2EScenario.Id));
                    InstancesStep = EInstancesStep::WaitForSecondaryInitialCook;
                    ResetStepTimeout();
                    return;
                }

                HasTimedOut(TEXT("Timed out waiting for the duplicate actor to discover the target."));
                return;

            case EInstancesStep::WaitForSecondaryInitialCook:
                if (!SecondaryComponent.IsValid())
                {
                    Fail(TEXT("Secondary bridge component became invalid during duplicate target warm-up."));
                    return;
                }

                if (SecondaryComponent->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                {
                    Fail(FString::Printf(TEXT("Secondary initial cook failed: %s"), *SecondaryComponent->LastStatusMessage));
                    return;
                }

                if (GeometryNodesBridgeAutomation::HasCookedPreview(SecondaryComponent.Get()))
                {
                    ClearAutomationContext(SecondaryComponent.Get());

                    if (!StartInstancedBakeAction(
                            SecondaryComponent.Get(),
                            false,
                            INDEX_NONE,
                            TEXT("instanced_bake_smoke"),
                            E2EScenario.bUseAutomationScratchBakeRoot))
                    {
                        return;
                    }

                    InstancesStep = EInstancesStep::WaitForSecondaryBake;
                    ResetStepTimeout();
                    return;
                }

                HasTimedOut(TEXT("Timed out waiting for the duplicate actor's initial cook."));
                return;

            case EInstancesStep::WaitForSecondaryBake:
                if (!SecondaryComponent.IsValid())
                {
                    Fail(TEXT("Secondary bridge component became invalid during duplicate instanced bake."));
                    return;
                }

                if (SecondaryComponent->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                {
                    Fail(FString::Printf(TEXT("Secondary instanced bake failed: %s"), *SecondaryComponent->LastStatusMessage));
                    return;
                }

                if (GeometryNodesBridgeAutomation::HasInstancedBakeOutput(SecondaryComponent.Get()))
                {
                    ClearAutomationContext(SecondaryComponent.Get());

                    if (!SecondaryComponent->BakedStaticMesh.ToSoftObjectPath().IsNull())
                    {
                        Fail(TEXT("Duplicate instanced bake unexpectedly populated a baked static mesh reference."));
                        return;
                    }

                    if (GeometryNodesBridgeAutomation::CountHismComponents(SecondaryComponent->BakedActor.Get()) <= 0)
                    {
                        Fail(TEXT("Duplicate instanced bake did not create any HISM components."));
                        return;
                    }

                    if (GeometryNodesBridgeAutomation::CountInstancesAcrossHismComponents(SecondaryComponent->BakedActor.Get()) <= 0)
                    {
                        Fail(TEXT("Duplicate instanced bake did not create any HISM instances."));
                        return;
                    }

                    const TArray<FString> SecondaryPrototypeMeshPaths =
                        GeometryNodesBridgeAutomation::CollectPrototypeMeshPaths(SecondaryComponent.Get());
                    if (SecondaryPrototypeMeshPaths.IsEmpty())
                    {
                        Fail(TEXT("Duplicate instanced bake did not store any shared prototype mesh references."));
                        return;
                    }

                    if (SecondaryPrototypeMeshPaths != PrimaryPrototypeMeshPaths)
                    {
                        Fail(TEXT("Duplicate instanced bake did not reuse the shared prototype mesh assets."));
                        return;
                    }

                    Primary->DeleteBakedOutput();
                    SecondaryComponent->DeleteBakedOutput();
                    if (Primary->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error
                        || SecondaryComponent->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
                    {
                        Fail(TEXT("Instanced bake cleanup failed after verifying shared prototype reuse."));
                        return;
                    }

                    InstancesStep = EInstancesStep::VerifyCleanup;
                    ResetStepTimeout();
                    return;
                }

                HasTimedOut(TEXT("Timed out waiting for the duplicate instanced bake output."));
                return;

            case EInstancesStep::VerifyCleanup:
            default:
                if (GeometryNodesBridgeAutomation::HasAnyBakedOutput(Primary))
                {
                    Fail(TEXT("Primary instanced bake cleanup left baked output references behind."));
                    return;
                }

                if (SecondaryComponent.IsValid() && GeometryNodesBridgeAutomation::HasAnyBakedOutput(SecondaryComponent.Get()))
                {
                    Fail(TEXT("Duplicate instanced bake cleanup left baked output references behind."));
                    return;
                }

                BeginCleanup();
                return;
            }
        }

    private:
        GeometryNodesBridgeAutomation::FBridgeAutomationE2EScenarioDefinition E2EScenario;
        EHouseStep HouseStep = EHouseStep::ValidateInitial;
        ECurveStep CurveStep = ECurveStep::ValidateInitial;
        EInstancesStep InstancesStep = EInstancesStep::ValidateInitial;
        uint32 InitialMeshHash = 0;
        int32 SplinePointIndex = INDEX_NONE;
        TArray<FString> PrimaryPrototypeMeshPaths;
        TWeakObjectPtr<AGeometryNodesBridgeActor> SecondaryActor;
        TWeakObjectPtr<UGeometryNodesBridgeComponent> SecondaryComponent;
    };

    class FRunGeometryNodesBridgePerfSmokeScenarioCommand final
        : public GeometryNodesBridgeAutomation::FGeometryNodesBridgeAutomationScenarioCommandBase
    {
    public:
        FRunGeometryNodesBridgePerfSmokeScenarioCommand(
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
            case EPerfStep::StartMeasure:
                HandleStartMeasure();
                return;
            case EPerfStep::WaitForMeasure:
                HandleWaitForMeasure();
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
            StartMeasure,
            WaitForMeasure,
            Complete
        };

        bool StartMeasuredCook(const bool bMeasured, const int32 IterationIndex)
        {
            CollectGarbage(RF_NoFlags);
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

            if (!StartMeasuredCook(false, WarmupIterationIndex))
            {
                return;
            }

            LogProgress(FString::Printf(TEXT("Started perf-smoke warm-up iteration %d for scenario '%s'."), WarmupIterationIndex, *PerfScenario.Id));
            BodyStep = EPerfStep::WaitForWarmup;
            ResetStepTimeout();
        }

        void HandleWaitForWarmup()
        {
            UGeometryNodesBridgeComponent* Component = GetPrimaryComponent();
            if (Component == nullptr)
            {
                Fail(TEXT("Bridge component became invalid during the perf-smoke warm-up run."));
                return;
            }

            if (Component->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
            {
                Fail(FString::Printf(TEXT("Perf-smoke warm-up iteration %d failed: %s"), WarmupIterationIndex, *Component->LastStatusMessage));
                return;
            }

            if (GeometryNodesBridgeAutomation::HasCookedPreview(Component))
            {
                ClearAutomationContext(Component);
                LogProgress(FString::Printf(TEXT("Completed perf-smoke warm-up iteration %d for scenario '%s'."), WarmupIterationIndex, *PerfScenario.Id));
                ++WarmupIterationIndex;
                BodyStep = EPerfStep::StartWarmup;
                ResetStepTimeout();
                return;
            }

            HasTimedOut(FString::Printf(TEXT("Timed out waiting for perf-smoke warm-up iteration %d to finish."), WarmupIterationIndex));
        }

        void HandleStartMeasure()
        {
            if (MeasuredIterationIndex >= PerfScenario.MeasureCount)
            {
                BodyStep = EPerfStep::Complete;
                ResetStepTimeout();
                return;
            }

            if (!StartMeasuredCook(true, MeasuredIterationIndex))
            {
                return;
            }

            LogProgress(FString::Printf(TEXT("Started perf-smoke measured iteration %d for scenario '%s'."), MeasuredIterationIndex, *PerfScenario.Id));
            BodyStep = EPerfStep::WaitForMeasure;
            ResetStepTimeout();
        }

        void HandleWaitForMeasure()
        {
            UGeometryNodesBridgeComponent* Component = GetPrimaryComponent();
            if (Component == nullptr)
            {
                Fail(TEXT("Bridge component became invalid during a perf-smoke measured iteration."));
                return;
            }

            if (Component->ComponentStatus == EGeometryNodesBridgeComponentStatus::Error)
            {
                Fail(FString::Printf(TEXT("Perf-smoke measured iteration %d failed: %s"), MeasuredIterationIndex, *Component->LastStatusMessage));
                return;
            }

            if (GeometryNodesBridgeAutomation::HasCookedPreview(Component))
            {
                ClearAutomationContext(Component);
                LogProgress(FString::Printf(TEXT("Completed perf-smoke measured iteration %d for scenario '%s'."), MeasuredIterationIndex, *PerfScenario.Id));
                ++MeasuredIterationIndex;
                BodyStep = EPerfStep::StartMeasure;
                ResetStepTimeout();
                return;
            }

            HasTimedOut(FString::Printf(TEXT("Timed out waiting for perf-smoke measured iteration %d to finish."), MeasuredIterationIndex));
        }

    private:
        GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition PerfScenario;
        EPerfStep BodyStep = EPerfStep::StartWarmup;
        int32 WarmupIterationIndex = 0;
        int32 MeasuredIterationIndex = 0;
    };

    IMPLEMENT_COMPLEX_AUTOMATION_TEST(
        FGeometryNodesBridgeE2EAutomationTest,
        "GeometryNodesBridge.E2E",
        EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

    IMPLEMENT_COMPLEX_AUTOMATION_TEST(
        FGeometryNodesBridgePerfSmokeAutomationTest,
        "GeometryNodesBridge.PerfSmoke",
        EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

    void FGeometryNodesBridgeE2EAutomationTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
    {
        TArray<GeometryNodesBridgeAutomation::FBridgeAutomationE2EScenarioDefinition> Scenarios;
        FString Error;
        if (!GeometryNodesBridgeAutomation::LoadE2EScenarioManifest(E2EManifestRelativePath, Scenarios, Error))
        {
            OutBeautifiedNames.Add(TEXT("ManifestError"));
            OutTestCommands.Add(TEXT("__manifest_error__"));
            return;
        }

        for (const GeometryNodesBridgeAutomation::FBridgeAutomationE2EScenarioDefinition& Scenario : Scenarios)
        {
            OutBeautifiedNames.Add(Scenario.DisplayName.IsEmpty() ? Scenario.Id : Scenario.DisplayName);
            OutTestCommands.Add(Scenario.Id);
        }
    }

    bool FGeometryNodesBridgeE2EAutomationTest::RunTest(const FString& Parameters)
    {
        if (Parameters == TEXT("__manifest_error__"))
        {
            TArray<GeometryNodesBridgeAutomation::FBridgeAutomationE2EScenarioDefinition> Scenarios;
            FString Error;
            GeometryNodesBridgeAutomation::LoadE2EScenarioManifest(E2EManifestRelativePath, Scenarios, Error);
            AddError(Error.IsEmpty() ? TEXT("The Geometry Nodes Bridge E2E smoke manifest failed to load.") : Error);
            return false;
        }

        GeometryNodesBridgeAutomation::FBridgeAutomationE2EScenarioDefinition Scenario;
        FString Error;
        if (!GeometryNodesBridgeAutomation::TryFindE2EScenarioDefinition(E2EManifestRelativePath, Parameters, Scenario, Error))
        {
            AddError(Error);
            return false;
        }

        ADD_LATENT_AUTOMATION_COMMAND(FRunGeometryNodesBridgeE2EScenarioCommand(this, Scenario, GeometryNodesBridgeAutomation::GetAutomationRunLabelFromCommandLine()));
        return true;
    }

    void FGeometryNodesBridgePerfSmokeAutomationTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
    {
        TArray<GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition> Scenarios;
        FString Error;
        if (!GeometryNodesBridgeAutomation::LoadPerfScenarioManifest(PerfSmokeManifestRelativePath, Scenarios, Error))
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

    bool FGeometryNodesBridgePerfSmokeAutomationTest::RunTest(const FString& Parameters)
    {
        if (Parameters == TEXT("__manifest_error__"))
        {
            TArray<GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition> Scenarios;
            FString Error;
            GeometryNodesBridgeAutomation::LoadPerfScenarioManifest(PerfSmokeManifestRelativePath, Scenarios, Error);
            AddError(Error.IsEmpty() ? TEXT("The Geometry Nodes Bridge perf-smoke manifest failed to load.") : Error);
            return false;
        }

        GeometryNodesBridgeAutomation::FBridgeAutomationPerfScenarioDefinition Scenario;
        FString Error;
        if (!GeometryNodesBridgeAutomation::TryFindPerfScenarioDefinition(PerfSmokeManifestRelativePath, Parameters, Scenario, Error))
        {
            AddError(Error);
            return false;
        }

        ADD_LATENT_AUTOMATION_COMMAND(FRunGeometryNodesBridgePerfSmokeScenarioCommand(this, Scenario, GeometryNodesBridgeAutomation::GetAutomationRunLabelFromCommandLine()));
        return true;
    }
}

#endif
