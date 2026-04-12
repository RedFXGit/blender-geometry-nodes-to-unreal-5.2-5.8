#include "GeometryNodesBridgeActor.h"
#include "GeometryNodesBridgeComponent.h"
#include "GeometryNodesBridgeMoveProbeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "UObject/Package.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    constexpr TCHAR ObservedBindingId[] = TEXT("ObservedMesh");
    constexpr TCHAR ObservedTargetId[] = TEXT("ObservedOverrideActorMove");
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FGeometryNodesBridgeObservedOverrideMoveAutomationTest,
    "GeometryNodesBridge.Editor.ObservedOverrideActorMove",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeometryNodesBridgeObservedOverrideMoveAutomationTest::RunTest(const FString& Parameters)
{
    (void)Parameters;

    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Created editor world"), World))
    {
        return false;
    }

    if (UPackage* WorldPackage = World->GetOutermost())
    {
        WorldPackage->SetDirtyFlag(false);
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.ObjectFlags |= RF_Transient;
    SpawnParameters.OverrideLevel = World->PersistentLevel;
    SpawnParameters.bTemporaryEditorActor = true;
    SpawnParameters.bHideFromSceneOutliner = true;
    SpawnParameters.bCreateActorPackage = false;

    AGeometryNodesBridgeActor* BridgeActor =
        World->SpawnActor<AGeometryNodesBridgeActor>(AGeometryNodesBridgeActor::StaticClass(), FTransform::Identity, SpawnParameters);
    if (!TestNotNull(TEXT("Spawned bridge actor"), BridgeActor))
    {
        return false;
    }

    UGeometryNodesBridgeComponent* BridgeComponent = BridgeActor->GetBridgeComponent();
    if (!TestNotNull(TEXT("Resolved bridge component"), BridgeComponent))
    {
        BridgeActor->Destroy();
        return false;
    }

    AStaticMeshActor* OverrideActor =
        World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform::Identity, SpawnParameters);
    if (!TestNotNull(TEXT("Spawned override actor"), OverrideActor))
    {
        BridgeActor->Destroy();
        return false;
    }

    UStaticMeshComponent* OverrideMeshComponent = OverrideActor->GetStaticMeshComponent();
    if (!TestNotNull(TEXT("Resolved override mesh component"), OverrideMeshComponent))
    {
        OverrideActor->Destroy();
        BridgeActor->Destroy();
        return false;
    }

    FBridgeGeometryInputDescriptor Descriptor;
    Descriptor.InputId = ObservedBindingId;
    Descriptor.Label = TEXT("Observed Mesh");
    Descriptor.AllowedSourceKinds.Add(EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent);
    BridgeComponent->GeometryInputDescriptors.Reset();
    BridgeComponent->GeometryInputDescriptors.Add(Descriptor);

    FGeometryNodesBridgeGeometryBinding Binding;
    Binding.InputId = ObservedBindingId;
    Binding.SourceKind = EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent;
    Binding.ComponentReference.OtherActor = OverrideActor;
    BridgeComponent->GeometryBindings.Reset();
    BridgeComponent->GeometryBindings.Add(Binding);
    BridgeComponent->TargetId = ObservedTargetId;
    BridgeComponent->LastObservedGeometryEvent.Reset();
    BridgeComponent->ReregisterComponent();

    TestTrue(
        TEXT("Observed summary tracks the override component"),
        BridgeComponent->ObservedGeometryDebugSummary.Contains(OverrideMeshComponent->GetName()));

    TInlineComponentArray<UGeometryNodesBridgeMoveProbeComponent*> MoveProbes(OverrideActor);
    UGeometryNodesBridgeMoveProbeComponent** MatchingProbe = MoveProbes.FindByPredicate([OverrideMeshComponent](const UGeometryNodesBridgeMoveProbeComponent* Candidate)
    {
        return Candidate != nullptr && Candidate->MatchesObservedComponent(OverrideMeshComponent);
    });
    UGeometryNodesBridgeMoveProbeComponent* MoveProbe = MatchingProbe != nullptr ? *MatchingProbe : nullptr;

    TestNotNull(TEXT("Observed override actor received a move probe"), MoveProbe);
    if (!TestTrue(
            TEXT("Move probe is attached to the watched component"),
            MoveProbe != nullptr && MoveProbe->GetAttachParent() == OverrideMeshComponent))
    {
        OverrideActor->Destroy();
        BridgeActor->Destroy();
        return false;
    }

    BridgeComponent->LastObservedGeometryEvent.Reset();
    OverrideActor->SetActorLocation(FVector(125.0f, -30.0f, 15.0f), false, nullptr, ETeleportType::TeleportPhysics);

    TestTrue(
        TEXT("Observed override move queued the follow-cook path"),
        !BridgeComponent->LastObservedGeometryEvent.IsEmpty());
    TestTrue(
        TEXT("Observed override move reached the guarded recook path"),
        BridgeComponent->LastObservedGeometryEvent.Contains(TEXT("Cook skipped because source/target/session is not ready.")));

    OverrideActor->Destroy();
    BridgeActor->Destroy();
    return true;
}

#endif
