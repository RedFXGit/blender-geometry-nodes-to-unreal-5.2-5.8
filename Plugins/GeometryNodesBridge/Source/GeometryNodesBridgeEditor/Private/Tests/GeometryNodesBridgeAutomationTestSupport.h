#pragma once

#include "GeometryNodesBridgeProtocol.h"
#include "Misc/AutomationTest.h"
#include "UObject/StrongObjectPtr.h"

#if WITH_DEV_AUTOMATION_TESTS

class AActor;
class AGeometryNodesBridgeActor;
class UGeometryNodesBridgeComponent;
class UGeometryNodesBridgeSource;
class UWorld;

namespace GeometryNodesBridgeAutomation
{
    constexpr double DefaultScenarioTimeoutSeconds = 120.0;

    struct FBridgeAutomationScenarioDefinition
    {
        FString Id;
        FString DisplayName;
        FString BlendFile;
        FString TargetId;
        bool bUseAutomationScratchBakeRoot = false;
    };

    struct FBridgeAutomationPerfScenarioDefinition : public FBridgeAutomationScenarioDefinition
    {
        FString Action;
        int32 WarmupCount = 0;
        int32 MeasureCount = 0;
        bool bRequiresBakeCleanup = false;
        double BaselineMedianMs = 0.0;
        double AllowedRegressionPercent = 0.0;
        double AllowedRegressionMs = 0.0;
    };

    struct FBridgeAutomationE2EScenarioDefinition : public FBridgeAutomationScenarioDefinition
    {
        FString ScenarioType;
    };

    FString GetPluginConfigPath(const TCHAR* RelativeConfigPath);
    FString GetAutomationRunLabelFromCommandLine();

    bool LoadPerfScenarioManifest(
        const TCHAR* RelativeConfigPath,
        TArray<FBridgeAutomationPerfScenarioDefinition>& OutScenarios,
        FString& OutError);

    bool LoadE2EScenarioManifest(
        const TCHAR* RelativeConfigPath,
        TArray<FBridgeAutomationE2EScenarioDefinition>& OutScenarios,
        FString& OutError);

    bool TryFindPerfScenarioDefinition(
        const TCHAR* RelativeConfigPath,
        const FString& ScenarioId,
        FBridgeAutomationPerfScenarioDefinition& OutScenario,
        FString& OutError);

    bool TryFindE2EScenarioDefinition(
        const TCHAR* RelativeConfigPath,
        const FString& ScenarioId,
        FBridgeAutomationE2EScenarioDefinition& OutScenario,
        FString& OutError);

    bool HasAvailableTarget(const UGeometryNodesBridgeComponent* Component, const FString& TargetId);
    bool HasCookedPreview(const UGeometryNodesBridgeComponent* Component);
    bool HasInstancedBakeOutput(const UGeometryNodesBridgeComponent* Component);
    bool HasAnyBakedOutput(const UGeometryNodesBridgeComponent* Component);
    bool CleanupAutomationScratchBakeRoot(FString& OutError);
    uint32 HashCookedMeshSignature(const FBridgeDecodedMesh& Mesh);
    TArray<FString> CollectPrototypeMeshPaths(const UGeometryNodesBridgeComponent* Component);
    int32 CountHismComponents(const AActor* Actor);
    int32 CountInstancesAcrossHismComponents(const AActor* Actor);

    class FGeometryNodesBridgeAutomationScenarioCommandBase : public IAutomationLatentCommand
    {
    protected:
        enum class EBaseStep : uint8
        {
            CreateWorld,
            EnsureSession,
            WaitForSession,
            ConfigureSource,
            WaitForTargets,
            ConfigureTarget,
            WaitForInitialCook,
            RunScenarioBody,
            CleanupScenario,
            Done
        };

    public:
        FGeometryNodesBridgeAutomationScenarioCommandBase(
            FAutomationTestBase* InTest,
            FBridgeAutomationScenarioDefinition InScenario,
            FString InRunLabel,
            double InTimeoutSeconds = DefaultScenarioTimeoutSeconds);

        virtual bool Update() override;

    protected:
        virtual void ResetScenarioState() {}
        virtual void UpdateScenarioBody() = 0;
        virtual void CleanupScenarioState() {}

        UWorld* GetWorld() const { return World; }
        UGeometryNodesBridgeSource* GetPrimarySource() const { return PrimarySource.Get(); }
        AGeometryNodesBridgeActor* GetPrimaryActor() const { return PrimaryActor.Get(); }
        UGeometryNodesBridgeComponent* GetPrimaryComponent() const { return PrimaryComponent.Get(); }
        const FBridgeAutomationScenarioDefinition& GetScenario() const { return Scenario; }
        const FString& GetRunLabel() const { return RunLabel; }

        bool HasTimedOut(const FString& Message);
        bool Fail(const FString& Message);
        void LogProgress(const FString& Message) const;
        void BeginCleanup();
        void ResetStepTimeout();

        bool SpawnTrackedActor(
            UGeometryNodesBridgeSource* InSource,
            const FString& LabelSuffix,
            TWeakObjectPtr<AGeometryNodesBridgeActor>& OutActor,
            TWeakObjectPtr<UGeometryNodesBridgeComponent>& OutComponent,
            bool bNotifySourceChanged = true);

        UGeometryNodesBridgeSource* CreateTrackedSource(const FString& BlendFilePath);

        bool ConfigureTargetAndRequestInitialCook(
            UGeometryNodesBridgeComponent* Component,
            const FString& TargetId,
            bool bUseAutomationScratchBakeRoot);

        bool StartCookAction(
            UGeometryNodesBridgeComponent* Component,
            bool bMeasured,
            int32 IterationIndex,
            const FString& ActionKind = TEXT("request_cook"),
            bool bUseAutomationScratchBakeRoot = false);

        bool StartStaticBakeAction(
            UGeometryNodesBridgeComponent* Component,
            bool bMeasured,
            int32 IterationIndex,
            const FString& ActionKind = TEXT("static_bake"),
            bool bUseAutomationScratchBakeRoot = false);

        bool StartInstancedBakeAction(
            UGeometryNodesBridgeComponent* Component,
            bool bMeasured,
            int32 IterationIndex,
            const FString& ActionKind = TEXT("instanced_bake"),
            bool bUseAutomationScratchBakeRoot = false);

        void ClearAutomationContext(UGeometryNodesBridgeComponent* Component) const;
        void ClearAllAutomationContexts() const;
        void ClearWorldDirtyState() const;
        void DestroyManagedActors();
        void ResetManagedObjects();

        static FString ResolveFixturePath(const FString& RelativeBlendFile, FString& OutError);

    private:
        bool HandleCreateWorld();
        bool HandleEnsureSession();
        bool HandleWaitForSession();
        bool HandleConfigureSource();
        bool HandleWaitForTargets();
        bool HandleConfigureTarget();
        bool HandleWaitForInitialCook();
        bool HandleCleanupScenario();

    protected:
        FAutomationTestBase* Test = nullptr;
        FBridgeAutomationScenarioDefinition Scenario;
        FString RunLabel;
        UWorld* World = nullptr;
        TWeakObjectPtr<UGeometryNodesBridgeSource> PrimarySource;
        TWeakObjectPtr<AGeometryNodesBridgeActor> PrimaryActor;
        TWeakObjectPtr<UGeometryNodesBridgeComponent> PrimaryComponent;
        EBaseStep Step = EBaseStep::CreateWorld;
        double StepStartTimeSeconds = 0.0;
        bool bFinished = false;

    private:
        TArray<TStrongObjectPtr<UGeometryNodesBridgeSource>> ManagedSources;
        TArray<TWeakObjectPtr<AGeometryNodesBridgeActor>> ManagedActors;
        TArray<TWeakObjectPtr<UGeometryNodesBridgeComponent>> ManagedComponents;
        double TimeoutSeconds = DefaultScenarioTimeoutSeconds;
    };
}

#endif
