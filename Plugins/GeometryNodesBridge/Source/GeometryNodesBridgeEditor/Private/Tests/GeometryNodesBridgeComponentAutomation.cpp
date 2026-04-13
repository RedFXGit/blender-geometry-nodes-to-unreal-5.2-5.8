#include "GeometryNodesBridgeActor.h"
#include "GeometryNodesBridgeComponent.h"
#include "GeometryNodesBridgeMessages.h"
#include "Components/StaticMeshComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/UserDefinedEnum.h"
#include "Engine/World.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/AutomationTest.h"
#include "ProceduralMeshComponent.h"
#include "Tests/AutomationEditorCommon.h"
#include "UObject/Package.h"

#if WITH_DEV_AUTOMATION_TESTS

struct FGeometryNodesBridgeComponentTestAccess
{
    static void RebuildInputValues(UGeometryNodesBridgeComponent* Component, const bool bPreserveExistingValues)
    {
        Component->RebuildInputValues(bPreserveExistingValues);
    }

    static void RebuildGeometryBindings(UGeometryNodesBridgeComponent* Component, const bool bPreserveExistingValues)
    {
        Component->RebuildGeometryBindings(bPreserveExistingValues);
    }

    static void NormalizeGeometryBindings(UGeometryNodesBridgeComponent* Component)
    {
        Component->NormalizeGeometryBindings();
    }

    static bool BuildGeometryBindingPayloads(
        const UGeometryNodesBridgeComponent* Component,
        TArray<FBridgeGeometryBindingPayload>& OutBindingPayloads,
        FString& OutBindingPayloadHash,
        FString& OutErrorMessage)
    {
        return Component->BuildGeometryBindingPayloads(OutBindingPayloads, OutBindingPayloadHash, OutErrorMessage);
    }

    static UProceduralMeshComponent* EnsurePreviewMeshComponent(UGeometryNodesBridgeComponent* Component)
    {
        Component->EnsurePreviewMeshComponent();
        return Component->PreviewMeshComponent;
    }

    static void SyncMaterialOverridesFromCookedSlots(UGeometryNodesBridgeComponent* Component)
    {
        Component->SyncMaterialOverridesFromCookedSlots();
    }

    static void ApplyPreviewMaterials(UGeometryNodesBridgeComponent* Component)
    {
        Component->ApplyPreviewMaterials();
    }

    static void SetCookedMeshSections(UGeometryNodesBridgeComponent* Component, TArray<FBridgeMeshSectionDescriptor> Sections)
    {
        Component->LastCookedMesh.Sections = MoveTemp(Sections);
    }

    static EGeometryNodesBridgeGeometrySourceKind GetEffectiveGeometryBindingSourceKind(
        const UGeometryNodesBridgeComponent* Component,
        const FString& InputId)
    {
        return Component->GetEffectiveGeometryBindingSourceKind(InputId);
    }
};

namespace
{
    constexpr TCHAR EnumInputId[] = TEXT("mode");
    constexpr TCHAR MeshInputId[] = TEXT("mesh_input");
    constexpr TCHAR EmbeddedInputId[] = TEXT("embedded_curve");
    constexpr TCHAR SlotAId[] = TEXT("slot_a");
    constexpr TCHAR SlotBId[] = TEXT("slot_b");
    constexpr TCHAR LegacySlotId[] = TEXT("legacy_slot");

    void MarkWorldPackageClean(UWorld* World)
    {
        if (World == nullptr)
        {
            return;
        }

        if (UPackage* WorldPackage = World->GetOutermost())
        {
            WorldPackage->SetDirtyFlag(false);
        }
    }

    FActorSpawnParameters MakeTransientSpawnParameters(UWorld* World)
    {
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.ObjectFlags |= RF_Transient;
        SpawnParameters.OverrideLevel = World != nullptr ? World->PersistentLevel : nullptr;
        SpawnParameters.bTemporaryEditorActor = true;
        SpawnParameters.bHideFromSceneOutliner = true;
        SpawnParameters.bCreateActorPackage = false;
        return SpawnParameters;
    }

    const FGeometryNodesBridgeMaterialOverride* FindOverrideBySlotId(
        const TArray<FGeometryNodesBridgeMaterialOverride>& Overrides,
        const FString& SlotId)
    {
        return Overrides.FindByPredicate([&SlotId](const FGeometryNodesBridgeMaterialOverride& Entry)
        {
            return Entry.SlotId == SlotId;
        });
    }

    FBridgeInputSchema MakeEnumSchema(
        const FString& LowLabel,
        const FString& MediumLabel,
        const FString& HighLabel)
    {
        FBridgeInputSchema Entry;
        Entry.Id = EnumInputId;
        Entry.Label = TEXT("Quality Mode");
        Entry.Type = EGeometryNodesBridgeInputType::Enum;
        Entry.DefaultEnumValue = 2;

        FBridgeEnumOption LowOption;
        LowOption.Id = TEXT("low");
        LowOption.Label = LowLabel;
        LowOption.Value = 1;
        Entry.EnumOptions.Add(LowOption);

        FBridgeEnumOption MediumOption;
        MediumOption.Id = TEXT("medium");
        MediumOption.Label = MediumLabel;
        MediumOption.Value = 2;
        Entry.EnumOptions.Add(MediumOption);

        FBridgeEnumOption HighOption;
        HighOption.Id = TEXT("high");
        HighOption.Label = HighLabel;
        HighOption.Value = 3;
        Entry.EnumOptions.Add(HighOption);

        return Entry;
    }

    FBridgeMaterialSlotDescriptor MakeMaterialSlot(const FString& SlotId, const FString& SlotName, const FString& BlenderMaterialName)
    {
        FBridgeMaterialSlotDescriptor Slot;
        Slot.SlotId = SlotId;
        Slot.SlotName = SlotName;
        Slot.BlenderMaterialName = BlenderMaterialName;
        return Slot;
    }

    FBridgeMeshSectionDescriptor MakeSection(const int32 SectionIndex, const FString& SlotId, const FString& SlotName)
    {
        FBridgeMeshSectionDescriptor Section;
        Section.SectionIndex = SectionIndex;
        Section.SlotId = SlotId;
        Section.SlotName = SlotName;
        Section.BlenderMaterialName = SlotName;
        return Section;
    }

    FBridgeGeometryInputDescriptor MakeGeometryDescriptor(
        const FString& InputId,
        const FString& Label,
        std::initializer_list<EGeometryNodesBridgeGeometrySourceKind> AllowedSourceKinds)
    {
        FBridgeGeometryInputDescriptor Descriptor;
        Descriptor.InputId = InputId;
        Descriptor.Label = Label;
        for (const EGeometryNodesBridgeGeometrySourceKind SourceKind : AllowedSourceKinds)
        {
            Descriptor.AllowedSourceKinds.Add(SourceKind);
        }

        return Descriptor;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FGeometryNodesBridgeEnumPropertyBagAutomationTest,
    "GeometryNodesBridge.Editor.Component.EnumPropertyBagPreservesValues",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeometryNodesBridgeEnumPropertyBagAutomationTest::RunTest(const FString& Parameters)
{
    (void)Parameters;

    TStrongObjectPtr<UGeometryNodesBridgeComponent> Component(NewObject<UGeometryNodesBridgeComponent>(GetTransientPackage()));
    if (!TestNotNull(TEXT("Created transient bridge component"), Component.Get()))
    {
        return false;
    }

    Component->InputSchema = {MakeEnumSchema(TEXT("Low"), TEXT("Medium"), TEXT("High"))};
    FGeometryNodesBridgeComponentTestAccess::RebuildInputValues(Component.Get(), false);

    const FName BagPropertyName = FInstancedPropertyBag::SanitizePropertyName(FName(EnumInputId));
    const FPropertyBagPropertyDesc* InitialDescriptor = Component->InputValues.FindPropertyDescByName(BagPropertyName);
    if (!TestNotNull(TEXT("Enum property descriptor was created"), InitialDescriptor))
    {
        return false;
    }

    TestEqual(TEXT("Enum schema exposes an enum property"), InitialDescriptor->ValueType, EPropertyBagPropertyType::Enum);

    const UUserDefinedEnum* InitialRuntimeEnum = Cast<UUserDefinedEnum>(InitialDescriptor->ValueTypeObject);
    if (!TestNotNull(TEXT("Runtime enum was created for enum input"), InitialRuntimeEnum))
    {
        return false;
    }

    const auto StoreResult = Component->InputValues.SetValueEnum(BagPropertyName, 3, InitialRuntimeEnum);
    if (!TestEqual(TEXT("Stored an enum selection in the property bag"), StoreResult, EPropertyBagResult::Success))
    {
        return false;
    }

    Component->InputSchema = {MakeEnumSchema(TEXT("Low Quality"), TEXT("Balanced"), TEXT("High Quality"))};
    FGeometryNodesBridgeComponentTestAccess::RebuildInputValues(Component.Get(), true);

    const FPropertyBagPropertyDesc* UpdatedDescriptor = Component->InputValues.FindPropertyDescByName(BagPropertyName);
    if (!TestNotNull(TEXT("Enum descriptor survives schema rebuild"), UpdatedDescriptor))
    {
        return false;
    }

    const UUserDefinedEnum* UpdatedRuntimeEnum = Cast<UUserDefinedEnum>(UpdatedDescriptor->ValueTypeObject);
    if (!TestNotNull(TEXT("Updated schema still resolves to a runtime enum"), UpdatedRuntimeEnum))
    {
        return false;
    }

    TestTrue(TEXT("Enum schema changes regenerate the runtime enum definition"), UpdatedRuntimeEnum != InitialRuntimeEnum);

    const auto PreservedEnumValue = Component->InputValues.GetValueEnum(BagPropertyName, UpdatedRuntimeEnum);
    if (!TestTrue(TEXT("Enum selection remains readable after schema rebuild"), PreservedEnumValue.IsValid()))
    {
        return false;
    }

    TestEqual(TEXT("Enum selection is preserved across schema rebuild"), static_cast<int32>(PreservedEnumValue.GetValue()), 3);

    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    GeometryNodesBridge::AddInputValuesToPayload(Component->InputSchema, Component->InputValues, Payload);
    if (!TestTrue(TEXT("Enum payload includes a values object"), Payload->HasTypedField<EJson::Object>(TEXT("values"))))
    {
        return false;
    }

    const TSharedPtr<FJsonObject> ValuesObject = Payload->GetObjectField(TEXT("values"));
    TestEqual(TEXT("Serialized payload uses the preserved enum numeric value"), ValuesObject->GetNumberField(EnumInputId), 3.0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FGeometryNodesBridgeMaterialOverrideAutomationTest,
    "GeometryNodesBridge.Editor.Component.MaterialOverridesStayMappedToPreview",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeometryNodesBridgeMaterialOverrideAutomationTest::RunTest(const FString& Parameters)
{
    (void)Parameters;

    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Created editor world"), World))
    {
        return false;
    }

    MarkWorldPackageClean(World);

    const FActorSpawnParameters SpawnParameters = MakeTransientSpawnParameters(World);
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

    UProceduralMeshComponent* PreviewMeshComponent = FGeometryNodesBridgeComponentTestAccess::EnsurePreviewMeshComponent(BridgeComponent);
    if (!TestNotNull(TEXT("Preview mesh component was created"), PreviewMeshComponent))
    {
        BridgeActor->Destroy();
        return false;
    }

    UMaterialInterface* BaseMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
    UMaterialInstanceDynamic* SlotAMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, GetTransientPackage());
    UMaterialInstanceDynamic* SlotBMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, GetTransientPackage());
    UMaterialInstanceDynamic* LegacyMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, GetTransientPackage());
    if (!TestNotNull(TEXT("Created slot A material override"), SlotAMaterial)
        || !TestNotNull(TEXT("Created slot B material override"), SlotBMaterial)
        || !TestNotNull(TEXT("Created legacy material override"), LegacyMaterial))
    {
        BridgeActor->Destroy();
        return false;
    }

    FGeometryNodesBridgeMaterialOverride ExistingOverride;
    ExistingOverride.SlotId = SlotAId;
    ExistingOverride.SlotName = TEXT("Slot A");
    ExistingOverride.BlenderMaterialName = TEXT("Blender Slot A");
    ExistingOverride.Material = SlotAMaterial;

    FGeometryNodesBridgeMaterialOverride LegacyOverride;
    LegacyOverride.SlotId = LegacySlotId;
    LegacyOverride.SlotName = TEXT("Legacy");
    LegacyOverride.BlenderMaterialName = TEXT("Legacy");
    LegacyOverride.Material = LegacyMaterial;

    BridgeComponent->MaterialOverrides = {ExistingOverride, LegacyOverride};
    BridgeComponent->LastCookedMaterialSlots = {
        MakeMaterialSlot(SlotAId, TEXT("Slot A"), TEXT("Blender Slot A")),
        MakeMaterialSlot(SlotBId, TEXT("Slot B"), TEXT("Blender Slot B"))};

    FGeometryNodesBridgeComponentTestAccess::SyncMaterialOverridesFromCookedSlots(BridgeComponent);

    TestEqual(TEXT("Material overrides were rebuilt from the cooked slot list"), BridgeComponent->MaterialOverrides.Num(), 2);

    const FGeometryNodesBridgeMaterialOverride* SlotAOverride = FindOverrideBySlotId(BridgeComponent->MaterialOverrides, SlotAId);
    const FGeometryNodesBridgeMaterialOverride* SlotBOverride = FindOverrideBySlotId(BridgeComponent->MaterialOverrides, SlotBId);
    TestNotNull(TEXT("Slot A override is retained"), SlotAOverride);
    TestNotNull(TEXT("Slot B override is created"), SlotBOverride);
    TestTrue(TEXT("Slot A keeps its previous material assignment"), SlotAOverride != nullptr && SlotAOverride->Material == SlotAMaterial);
    TestTrue(TEXT("New slots start without a material assignment"), SlotBOverride != nullptr && SlotBOverride->Material == nullptr);
    TestTrue(TEXT("Legacy slots are removed during rebuild"), FindOverrideBySlotId(BridgeComponent->MaterialOverrides, LegacySlotId) == nullptr);

    for (FGeometryNodesBridgeMaterialOverride& Entry : BridgeComponent->MaterialOverrides)
    {
        if (Entry.SlotId == SlotBId)
        {
            Entry.Material = SlotBMaterial;
        }
    }

    FGeometryNodesBridgeComponentTestAccess::SetCookedMeshSections(
        BridgeComponent,
        {
            MakeSection(0, SlotAId, TEXT("Slot A")),
            MakeSection(1, SlotBId, TEXT("Slot B")),
        });
    FGeometryNodesBridgeComponentTestAccess::ApplyPreviewMaterials(BridgeComponent);

    TestTrue(TEXT("Preview section 0 maps to the slot A override"), PreviewMeshComponent->GetMaterial(0) == SlotAMaterial);
    TestTrue(TEXT("Preview section 1 maps to the slot B override"), PreviewMeshComponent->GetMaterial(1) == SlotBMaterial);

    BridgeComponent->LastCookedMaterialSlots = {
        MakeMaterialSlot(SlotBId, TEXT("Slot B"), TEXT("Blender Slot B")),
        MakeMaterialSlot(SlotAId, TEXT("Slot A"), TEXT("Blender Slot A"))};
    FGeometryNodesBridgeComponentTestAccess::SyncMaterialOverridesFromCookedSlots(BridgeComponent);

    SlotAOverride = FindOverrideBySlotId(BridgeComponent->MaterialOverrides, SlotAId);
    SlotBOverride = FindOverrideBySlotId(BridgeComponent->MaterialOverrides, SlotBId);
    TestTrue(TEXT("Slot A material survives output rebuilds"), SlotAOverride != nullptr && SlotAOverride->Material == SlotAMaterial);
    TestTrue(TEXT("Slot B material survives output rebuilds"), SlotBOverride != nullptr && SlotBOverride->Material == SlotBMaterial);

    BridgeActor->Destroy();
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FGeometryNodesBridgeGeometryBindingAutomationTest,
    "GeometryNodesBridge.Editor.Component.GeometryBindingsNormalizeAndBuildPayloads",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeometryNodesBridgeGeometryBindingAutomationTest::RunTest(const FString& Parameters)
{
    (void)Parameters;

    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    if (!TestNotNull(TEXT("Created editor world"), World))
    {
        return false;
    }

    MarkWorldPackageClean(World);

    const FActorSpawnParameters SpawnParameters = MakeTransientSpawnParameters(World);
    AGeometryNodesBridgeActor* BridgeActor = World->SpawnActor<AGeometryNodesBridgeActor>(
        AGeometryNodesBridgeActor::StaticClass(),
        FTransform(FRotator::ZeroRotator, FVector(10.0f, 0.0f, 0.0f)),
        SpawnParameters);
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

    AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(
        AStaticMeshActor::StaticClass(),
        FTransform(FRotator::ZeroRotator, FVector(110.0f, 25.0f, 0.0f)),
        SpawnParameters);
    if (!TestNotNull(TEXT("Spawned geometry source actor"), MeshActor))
    {
        BridgeActor->Destroy();
        return false;
    }

    UStaticMeshComponent* MeshComponent = MeshActor->GetStaticMeshComponent();
    if (!TestNotNull(TEXT("Resolved geometry source mesh component"), MeshComponent))
    {
        MeshActor->Destroy();
        BridgeActor->Destroy();
        return false;
    }

    UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (!TestNotNull(TEXT("Loaded engine cube mesh"), CubeMesh))
    {
        MeshActor->Destroy();
        BridgeActor->Destroy();
        return false;
    }

    MeshComponent->SetStaticMesh(CubeMesh);

    FGeometryNodesBridgeGeometryBinding MeshBinding;
    MeshBinding.InputId = MeshInputId;
    MeshBinding.SourceKind = EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent;
    MeshBinding.ComponentReference.OtherActor = MeshActor;

    FGeometryNodesBridgeGeometryBinding StaleBinding;
    StaleBinding.InputId = TEXT("stale_binding");
    StaleBinding.SourceKind = EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent;
    StaleBinding.ComponentReference.OtherActor = MeshActor;

    BridgeComponent->GeometryBindings = {MeshBinding, StaleBinding};
    BridgeComponent->GeometryInputDescriptors = {
        MakeGeometryDescriptor(MeshInputId, TEXT("Mesh Input"), {EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent}),
        MakeGeometryDescriptor(EmbeddedInputId, TEXT("Embedded Curve"), {EGeometryNodesBridgeGeometrySourceKind::EmbeddedSpline}),
    };

    FGeometryNodesBridgeComponentTestAccess::RebuildGeometryBindings(BridgeComponent, true);

    TestEqual(TEXT("Schema rebuild keeps one binding per active descriptor"), BridgeComponent->GeometryBindings.Num(), 2);

    const FGeometryNodesBridgeGeometryBinding* RebuiltMeshBinding = BridgeComponent->GeometryBindings.FindByPredicate(
        [](const FGeometryNodesBridgeGeometryBinding& Binding)
        {
            return Binding.InputId == MeshInputId;
        });
    const FGeometryNodesBridgeGeometryBinding* RebuiltEmbeddedBinding = BridgeComponent->GeometryBindings.FindByPredicate(
        [](const FGeometryNodesBridgeGeometryBinding& Binding)
        {
            return Binding.InputId == EmbeddedInputId;
        });

    if (!TestNotNull(TEXT("Mesh binding survived schema rebuild"), RebuiltMeshBinding)
        || !TestNotNull(TEXT("Embedded binding was synthesized during schema rebuild"), RebuiltEmbeddedBinding))
    {
        MeshActor->Destroy();
        BridgeActor->Destroy();
        return false;
    }

    TestTrue(TEXT("Mesh binding keeps its referenced actor across schema rebuild"), RebuiltMeshBinding->ComponentReference.OtherActor == MeshActor);
    TestEqual(
        TEXT("Embedded-only binding falls back to the embedded-spline default"),
        RebuiltEmbeddedBinding->SourceKind,
        EGeometryNodesBridgeGeometrySourceKind::EmbeddedSpline);
    TestTrue(
        TEXT("Embedded-only bindings clear component references"),
        RebuiltEmbeddedBinding->ComponentReference.OtherActor == nullptr && RebuiltEmbeddedBinding->ComponentReference.PathToComponent.IsEmpty());

    FGeometryNodesBridgeComponentTestAccess::NormalizeGeometryBindings(BridgeComponent);

    RebuiltMeshBinding = BridgeComponent->GeometryBindings.FindByPredicate(
        [](const FGeometryNodesBridgeGeometryBinding& Binding)
        {
            return Binding.InputId == MeshInputId;
        });
    if (!TestNotNull(TEXT("Mesh binding still exists after normalization"), RebuiltMeshBinding))
    {
        MeshActor->Destroy();
        BridgeActor->Destroy();
        return false;
    }

    TestTrue(
        TEXT("Normalization resolves the referenced mesh component path"),
        !RebuiltMeshBinding->ComponentReference.PathToComponent.IsEmpty());
    TestEqual(
        TEXT("Effective source kind resolves from the normalized component reference"),
        FGeometryNodesBridgeComponentTestAccess::GetEffectiveGeometryBindingSourceKind(BridgeComponent, MeshInputId),
        EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent);

    TArray<FBridgeGeometryBindingPayload> BindingPayloads;
    FString BindingPayloadHash;
    FString ErrorMessage;
    const bool bBuiltPayloads = FGeometryNodesBridgeComponentTestAccess::BuildGeometryBindingPayloads(
        BridgeComponent,
        BindingPayloads,
        BindingPayloadHash,
        ErrorMessage);

    if (!TestTrue(TEXT("Geometry bindings build a payload for referenced static meshes"), bBuiltPayloads))
    {
        AddError(ErrorMessage);
        MeshActor->Destroy();
        BridgeActor->Destroy();
        return false;
    }

    TestEqual(TEXT("Only the mesh binding emits a payload"), BindingPayloads.Num(), 1);
    TestTrue(TEXT("Binding payload hash is populated"), !BindingPayloadHash.IsEmpty());
    if (!TestEqual(TEXT("Payload uses the mesh input id"), BindingPayloads[0].InputId, FString(MeshInputId)))
    {
        MeshActor->Destroy();
        BridgeActor->Destroy();
        return false;
    }

    TestEqual(
        TEXT("Payload source kind resolves to static mesh component"),
        BindingPayloads[0].SourceKind,
        EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent);
    TestTrue(TEXT("Payload captured mesh vertices"), BindingPayloads[0].MeshData.Positions.Num() > 0);
    TestTrue(TEXT("Payload captured mesh triangles"), BindingPayloads[0].MeshData.Triangles.Num() >= 3);
    TestTrue(
        TEXT("Payload stores the mesh transform relative to the bridge component"),
        BindingPayloads[0].MeshData.RelativeTransform.GetLocation().Equals(FVector(100.0f, 25.0f, 0.0f), 0.5f));

    MeshActor->Destroy();
    BridgeActor->Destroy();
    return true;
}

#endif
