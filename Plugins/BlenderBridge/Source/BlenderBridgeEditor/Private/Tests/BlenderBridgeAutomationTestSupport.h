#pragma once

#include "BlenderBridgeProtocol.h"
#include "Misc/AutomationTest.h"
#include "UObject/StrongObjectPtr.h"

#if WITH_DEV_AUTOMATION_TESTS

class AActor;
class ABlenderBridgeActor;
class UBlenderBridgeComponent;
class UBlenderBridgeSource;
class UWorld;

namespace BlenderBridgeAutomation
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

    bool HasAvailableTarget(const UBlenderBridgeComponent* Component, const FString& TargetId);
    bool HasCookedPreview(const UBlenderBridgeComponent* Component);
    bool HasInstancedBakeOutput(const UBlenderBridgeComponent* Component);
    bool HasAnyBakedOutput(const UBlenderBridgeComponent* Component);
    bool CleanupAutomationScratchBakeRoot(FString& OutError);
    uint32 HashCookedMeshSignature(const FBridgeDecodedMesh& Mesh);
    TArray<FString> CollectPrototypeMeshPaths(const UBlenderBridgeComponent* Component);
    int32 CountHismComponents(const AActor* Actor);
    int32 CountInstancesAcrossHismComponents(const AActor* Actor);

    class FBlenderBridgeAutomationScenarioCommandBase : public IAutomationLatentCommand
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
        FBlenderBridgeAutomationScenarioCommandBase(
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
        UBlenderBridgeSource* GetPrimarySource() const { return PrimarySource.Get(); }
        ABlenderBridgeActor* GetPrimaryActor() const { return PrimaryActor.Get(); }
        UBlenderBridgeComponent* GetPrimaryComponent() const { return PrimaryComponent.Get(); }
        const FBridgeAutomationScenarioDefinition& GetScenario() const { return Scenario; }
        const FString& GetRunLabel() const { return RunLabel; }

        bool HasTimedOut(const FString& Message);
        bool Fail(const FString& Message);
        void LogProgress(const FString& Message) const;
        void BeginCleanup();
        void ResetStepTimeout();

        bool SpawnTrackedActor(
            UBlenderBridgeSource* InSource,
            const FString& LabelSuffix,
            TWeakObjectPtr<ABlenderBridgeActor>& OutActor,
            TWeakObjectPtr<UBlenderBridgeComponent>& OutComponent,
            bool bNotifySourceChanged = true);

        UBlenderBridgeSource* CreateTrackedSource(const FString& BlendFilePath);

        bool ConfigureTargetAndRequestInitialCook(
            UBlenderBridgeComponent* Component,
            const FString& TargetId,
            bool bUseAutomationScratchBakeRoot);

        bool StartCookAction(
            UBlenderBridgeComponent* Component,
            bool bMeasured,
            int32 IterationIndex,
            const FString& ActionKind = TEXT("request_cook"),
            bool bUseAutomationScratchBakeRoot = false);

        bool StartStaticBakeAction(
            UBlenderBridgeComponent* Component,
            bool bMeasured,
            int32 IterationIndex,
            const FString& ActionKind = TEXT("static_bake"),
            bool bUseAutomationScratchBakeRoot = false);

        bool StartInstancedBakeAction(
            UBlenderBridgeComponent* Component,
            bool bMeasured,
            int32 IterationIndex,
            const FString& ActionKind = TEXT("instanced_bake"),
            bool bUseAutomationScratchBakeRoot = false);

        void ClearAutomationContext(UBlenderBridgeComponent* Component) const;
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
        TWeakObjectPtr<UBlenderBridgeSource> PrimarySource;
        TWeakObjectPtr<ABlenderBridgeActor> PrimaryActor;
        TWeakObjectPtr<UBlenderBridgeComponent> PrimaryComponent;
        EBaseStep Step = EBaseStep::CreateWorld;
        double StepStartTimeSeconds = 0.0;
        bool bFinished = false;

    private:
        TArray<TStrongObjectPtr<UBlenderBridgeSource>> ManagedSources;
        TArray<TWeakObjectPtr<ABlenderBridgeActor>> ManagedActors;
        TArray<TWeakObjectPtr<UBlenderBridgeComponent>> ManagedComponents;
        double TimeoutSeconds = DefaultScenarioTimeoutSeconds;
    };
}

#endif
