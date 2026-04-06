#include "BlenderBridgeBakeService.h"

#include "BlenderBridgeComponent.h"
#include "BlenderBridgeSettings.h"
#include "BlenderBridgeSource.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Components/StaticMeshComponent.h"
#include "Editor.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "ObjectTools.h"
#include "PhysicsEngine/BodySetup.h"
#include "StaticMeshAttributes.h"
#include "UObject/Package.h"

DEFINE_LOG_CATEGORY_STATIC(LogBlenderBridgeBake, Log, All);

namespace
{
    constexpr TCHAR PlaceholderParentMaterialName[] = TEXT("M_BlenderBridgePlaceholderParent");
    constexpr TCHAR PlaceholderSupportFolderName[] = TEXT("_BridgeSupport");
    constexpr TCHAR PlaceholderParameterName[] = TEXT("BaseColor");

    FString NormalizeBakeRootPath(const FString& InPath)
    {
        FString Normalized = InPath.TrimStartAndEnd();
        if (Normalized.IsEmpty())
        {
            return TEXT("/Game/BlenderBridge/Baked");
        }

        if (!Normalized.StartsWith(TEXT("/Game")))
        {
            Normalized = TEXT("/Game/BlenderBridge/Baked");
        }

        while (Normalized.EndsWith(TEXT("/")))
        {
            Normalized.LeftChopInline(1);
        }

        return Normalized;
    }

    FString SanitizePackageSegment(const FString& InValue)
    {
        FString Sanitized;
        Sanitized.Reserve(InValue.Len());

        for (const TCHAR Character : InValue)
        {
            if (FChar::IsAlnum(Character))
            {
                Sanitized.AppendChar(Character);
            }
            else
            {
                Sanitized.AppendChar(TEXT('_'));
            }
        }

        while (Sanitized.Contains(TEXT("__")))
        {
            Sanitized.ReplaceInline(TEXT("__"), TEXT("_"));
        }

        Sanitized.TrimStartAndEndInline();
        bool bCharRemoved = false;
        Sanitized.TrimCharInline(TEXT('_'), &bCharRemoved);
        return Sanitized.IsEmpty() ? TEXT("BridgeAsset") : Sanitized;
    }

    FString GetSourceSegment(const UBlenderBridgeComponent* Component)
    {
        return Component != nullptr && Component->Source != nullptr
            ? SanitizePackageSegment(Component->Source->GetName())
            : TEXT("BridgeSource");
    }

    FString GetOwnerSegment(const UBlenderBridgeComponent* Component)
    {
        if (Component == nullptr || Component->GetOwner() == nullptr)
        {
            return TEXT("BridgeActor");
        }

        const AActor* Owner = Component->GetOwner();
#if WITH_EDITOR
        return SanitizePackageSegment(Owner->GetActorLabel(false));
#else
        return SanitizePackageSegment(Owner->GetName());
#endif
    }

    FString GetTargetSegment(const UBlenderBridgeComponent* Component)
    {
        return Component != nullptr && !Component->TargetId.IsEmpty()
            ? SanitizePackageSegment(Component->TargetId)
            : TEXT("Target");
    }

    FString BuildBakeFolderPath(const UBlenderBridgeComponent* Component)
    {
        const UBlenderBridgeSettings* Settings = GetDefault<UBlenderBridgeSettings>();
        const FString RootPath = NormalizeBakeRootPath(Settings != nullptr ? Settings->BakeRootContentPath : FString());
        return FString::Printf(
            TEXT("%s/%s/%s/%s"),
            *RootPath,
            *GetSourceSegment(Component),
            *GetOwnerSegment(Component),
            *GetTargetSegment(Component));
    }

    FString BuildAssetObjectPath(const FString& PackagePath, const FString& AssetName)
    {
        return FString::Printf(TEXT("%s/%s.%s"), *PackagePath, *AssetName, *AssetName);
    }

    FString BuildAssetPackageName(const FString& PackagePath, const FString& AssetName)
    {
        return FString::Printf(TEXT("%s/%s"), *PackagePath, *AssetName);
    }

    template <typename AssetType>
    AssetType* LoadAssetByPath(const FString& PackagePath, const FString& AssetName)
    {
        const FString ObjectPath = BuildAssetObjectPath(PackagePath, AssetName);
        if (AssetType* LoadedAsset = FindObject<AssetType>(nullptr, *ObjectPath))
        {
            return LoadedAsset;
        }

        FString PackageFilename;
        const FString PackageName = BuildAssetPackageName(PackagePath, AssetName);
        if (!FPackageName::DoesPackageExist(PackageName, &PackageFilename))
        {
            return nullptr;
        }

        return LoadObject<AssetType>(nullptr, *ObjectPath);
    }

    FLinearColor MakeDeterministicPlaceholderColor(const FString& SlotId)
    {
        const uint32 Hash = GetTypeHash(SlotId);
        const uint8 Hue = static_cast<uint8>(Hash & 0xFFu);
        const uint8 Saturation = static_cast<uint8>(160u + ((Hash >> 8u) & 0x3Fu));
        const uint8 Value = static_cast<uint8>(200u + ((Hash >> 16u) & 0x2Fu));
        return FLinearColor::MakeFromHSV8(Hue, Saturation, Value);
    }

    UMaterial* EnsurePlaceholderParentMaterial(const FString& BakeFolderPath)
    {
        const FString SupportFolderPath = FString::Printf(TEXT("%s/%s"), *NormalizeBakeRootPath(BakeFolderPath), PlaceholderSupportFolderName);
        if (UMaterial* ExistingMaterial = LoadAssetByPath<UMaterial>(SupportFolderPath, PlaceholderParentMaterialName))
        {
            return ExistingMaterial;
        }

        FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
        UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
        UMaterial* Material = Cast<UMaterial>(
            AssetToolsModule.Get().CreateAsset(PlaceholderParentMaterialName, SupportFolderPath, UMaterial::StaticClass(), Factory));
        if (Material == nullptr)
        {
            return nullptr;
        }

        Material->Modify();
        UMaterialEditingLibrary::DeleteAllMaterialExpressions(Material);

        UMaterialExpressionVectorParameter* ColorParameter = Cast<UMaterialExpressionVectorParameter>(
            UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionVectorParameter::StaticClass(), -320, 0));
        if (ColorParameter == nullptr)
        {
            return Material;
        }

        ColorParameter->ParameterName = PlaceholderParameterName;
        ColorParameter->DefaultValue = FLinearColor::White;
        UMaterialEditingLibrary::ConnectMaterialProperty(ColorParameter, FString(), MP_BaseColor);
        UMaterialEditingLibrary::LayoutMaterialExpressions(Material);
        UMaterialEditingLibrary::RecompileMaterial(Material);
        Material->PostEditChange();
        Material->MarkPackageDirty();
        return Material;
    }

    UMaterialInstanceConstant* EnsurePlaceholderMaterialInstance(
        const FString& MaterialsFolderPath,
        const FBridgeMaterialSlotDescriptor& Slot,
        UMaterialInterface* ParentMaterial)
    {
        const FString AssetName = FString::Printf(TEXT("MI_%s"), *SanitizePackageSegment(Slot.SlotId));
        UMaterialInstanceConstant* Instance = LoadAssetByPath<UMaterialInstanceConstant>(MaterialsFolderPath, AssetName);
        if (Instance == nullptr)
        {
            FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
            UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
            Factory->InitialParent = ParentMaterial;
            Instance = Cast<UMaterialInstanceConstant>(
                AssetToolsModule.Get().CreateAsset(AssetName, MaterialsFolderPath, UMaterialInstanceConstant::StaticClass(), Factory));
        }

        if (Instance == nullptr)
        {
            return nullptr;
        }

        if (ParentMaterial != nullptr)
        {
            Instance->SetParentEditorOnly(ParentMaterial);
        }

        UMaterialEditingLibrary::SetMaterialInstanceVectorParameterValue(
            Instance,
            PlaceholderParameterName,
            MakeDeterministicPlaceholderColor(Slot.SlotId));
        Instance->PostEditChange();
        Instance->MarkPackageDirty();
        return Instance;
    }

    bool IsSectionRangeValid(const FBridgeDecodedMesh& Mesh, const FBridgeMeshSectionDescriptor& Section)
    {
        return Section.FirstVertex >= 0 &&
            Section.VertexCount > 0 &&
            Section.FirstVertex + Section.VertexCount <= Mesh.Positions.Num() &&
            Section.FirstIndex >= 0 &&
            Section.IndexCount >= 3 &&
            Section.FirstIndex + Section.IndexCount <= Mesh.Triangles.Num();
    }

    UStaticMesh* BuildOrUpdateStaticMesh(
        UBlenderBridgeComponent* Component,
        const FString& BakeFolderPath,
        const TArray<UMaterialInterface*>& ResolvedMaterials)
    {
        const FString MeshesFolderPath = FString::Printf(TEXT("%s/Meshes"), *BakeFolderPath);
        const FString MeshAssetName = FString::Printf(TEXT("SM_%s"), *GetOwnerSegment(Component));

        UStaticMesh* StaticMesh = LoadAssetByPath<UStaticMesh>(MeshesFolderPath, MeshAssetName);
        if (StaticMesh == nullptr)
        {
            UPackage* Package = CreatePackage(*FString::Printf(TEXT("%s/%s"), *MeshesFolderPath, *MeshAssetName));
            if (Package == nullptr)
            {
                return nullptr;
            }

            StaticMesh = NewObject<UStaticMesh>(Package, *MeshAssetName, RF_Public | RF_Standalone | RF_Transactional);
            FAssetRegistryModule::AssetCreated(StaticMesh);
        }

        if (StaticMesh == nullptr)
        {
            return nullptr;
        }

        StaticMesh->Modify();

        TArray<FStaticMaterial> StaticMaterials;
        StaticMaterials.Reserve(Component->LastCookedMaterialSlots.Num());
        for (int32 SlotIndex = 0; SlotIndex < Component->LastCookedMaterialSlots.Num(); ++SlotIndex)
        {
            const FBridgeMaterialSlotDescriptor& Slot = Component->LastCookedMaterialSlots[SlotIndex];
            UMaterialInterface* MaterialInterface = ResolvedMaterials.IsValidIndex(SlotIndex) ? ResolvedMaterials[SlotIndex] : nullptr;
            StaticMaterials.Add(FStaticMaterial(
                MaterialInterface,
                FName(*Slot.SlotId),
                FName(*(Slot.BlenderMaterialName.IsEmpty() ? Slot.SlotId : Slot.BlenderMaterialName))));
        }
        StaticMesh->SetStaticMaterials(StaticMaterials);

        FMeshDescription MeshDescription;
        FStaticMeshAttributes Attributes(MeshDescription);
        Attributes.Register();

        FMeshDescriptionBuilder Builder;
        Builder.SetMeshDescription(&MeshDescription);
        Builder.SetNumUVLayers(1);
        const FBridgeDecodedMesh& CookedMesh = Component->GetLastCookedMesh();
        Builder.ReserveNewVertices(CookedMesh.Positions.Num());

        UE_LOG(
            LogBlenderBridgeBake,
            Log,
            TEXT("Bake mesh description '%s': requested UV layers=1, mesh UV element channels=%d, vertex-instance UV channels=%d, vertices=%d, triangles=%d"),
            *MeshAssetName,
            MeshDescription.GetNumUVElementChannels(),
            Attributes.GetVertexInstanceUVs().GetNumChannels(),
            CookedMesh.Positions.Num(),
            CookedMesh.Triangles.Num() / 3);

        TArray<FPolygonGroupID> PolygonGroups;
        PolygonGroups.Reserve(Component->LastCookedMaterialSlots.Num());
        for (const FBridgeMaterialSlotDescriptor& Slot : Component->LastCookedMaterialSlots)
        {
            PolygonGroups.Add(Builder.AppendPolygonGroup(FName(*Slot.SlotId)));
        }

        TArray<FVertexID> VertexIds;
        TArray<FVertexInstanceID> VertexInstanceIds;
        VertexIds.SetNum(CookedMesh.Positions.Num());
        VertexInstanceIds.SetNum(CookedMesh.Positions.Num());

        for (int32 VertexIndex = 0; VertexIndex < CookedMesh.Positions.Num(); ++VertexIndex)
        {
            VertexIds[VertexIndex] = Builder.AppendVertex(CookedMesh.Positions[VertexIndex]);
            VertexInstanceIds[VertexIndex] = Builder.AppendInstance(VertexIds[VertexIndex]);
            if (CookedMesh.Normals.IsValidIndex(VertexIndex))
            {
                Builder.SetInstanceNormal(VertexInstanceIds[VertexIndex], CookedMesh.Normals[VertexIndex]);
            }
            if (CookedMesh.UV0.IsValidIndex(VertexIndex))
            {
                Builder.SetInstanceUV(VertexInstanceIds[VertexIndex], CookedMesh.UV0[VertexIndex], 0);
            }
        }

        for (const FBridgeMeshSectionDescriptor& Section : CookedMesh.Sections)
        {
            if (!PolygonGroups.IsValidIndex(Section.SectionIndex) || !IsSectionRangeValid(CookedMesh, Section))
            {
                continue;
            }

            for (int32 TriangleIndex = Section.FirstIndex; TriangleIndex + 2 < Section.FirstIndex + Section.IndexCount; TriangleIndex += 3)
            {
                const int32 Index0 = CookedMesh.Triangles[TriangleIndex + 0];
                const int32 Index1 = CookedMesh.Triangles[TriangleIndex + 1];
                const int32 Index2 = CookedMesh.Triangles[TriangleIndex + 2];

                if (!VertexInstanceIds.IsValidIndex(Index0) || !VertexInstanceIds.IsValidIndex(Index1) || !VertexInstanceIds.IsValidIndex(Index2))
                {
                    continue;
                }

                Builder.AppendTriangle(VertexInstanceIds[Index0], VertexInstanceIds[Index1], VertexInstanceIds[Index2], PolygonGroups[Section.SectionIndex]);
            }
        }

        UStaticMesh::FBuildMeshDescriptionsParams BuildParams;
        BuildParams.bFastBuild = true;
        BuildParams.bBuildSimpleCollision = false;
        BuildParams.bAllowCpuAccess = true;
        BuildParams.bMarkPackageDirty = true;

        const TArray<const FMeshDescription*> MeshDescriptions = { &MeshDescription };
        if (!StaticMesh->BuildFromMeshDescriptions(MeshDescriptions, BuildParams))
        {
            return nullptr;
        }

        UE_LOG(
            LogBlenderBridgeBake,
            Log,
            TEXT("Built static mesh '%s': source UV channels=%d, built UV channels=%d, built tex coords=%d"),
            *StaticMesh->GetPathName(),
            MeshDescription.GetNumUVElementChannels(),
            StaticMesh->GetNumUVChannels(0),
            StaticMesh->GetNumTexCoords(0));

        StaticMesh->CreateBodySetup();
        if (UBodySetup* BodySetup = StaticMesh->GetBodySetup())
        {
            BodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
            BodySetup->MarkPackageDirty();
        }

        FMeshSectionInfoMap& SectionInfoMap = StaticMesh->GetSectionInfoMap();
        FMeshSectionInfoMap& OriginalSectionInfoMap = StaticMesh->GetOriginalSectionInfoMap();
        for (int32 SectionIndex = 0; SectionIndex < StaticMaterials.Num(); ++SectionIndex)
        {
            const FMeshSectionInfo SectionInfo(SectionIndex);
            SectionInfoMap.Set(0, SectionIndex, SectionInfo);
            OriginalSectionInfoMap.Set(0, SectionIndex, SectionInfo);
        }

        StaticMesh->PostEditChange();
        StaticMesh->MarkPackageDirty();
        return StaticMesh;
    }

    AStaticMeshActor* SpawnOrUpdateBakedActor(UBlenderBridgeComponent* Component, UStaticMesh* StaticMesh)
    {
        if (Component == nullptr || Component->GetOwner() == nullptr || StaticMesh == nullptr)
        {
            return nullptr;
        }

        AActor* SourceActor = Component->GetOwner();
        UWorld* World = SourceActor->GetWorld();
        if (World == nullptr)
        {
            return nullptr;
        }

        AStaticMeshActor* BakedActor = Component->BakedActor.Get();
        if (!IsValid(BakedActor) || BakedActor->GetWorld() != World)
        {
            FActorSpawnParameters SpawnParameters;
            SpawnParameters.Owner = SourceActor;
            SpawnParameters.OverrideLevel = SourceActor->GetLevel();
            SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            BakedActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SourceActor->GetActorTransform(), SpawnParameters);
            if (BakedActor == nullptr)
            {
                return nullptr;
            }
        }

        BakedActor->SetActorTransform(SourceActor->GetActorTransform());
        BakedActor->SetActorLabel(FString::Printf(TEXT("%s Baked"), *GetOwnerSegment(Component)));

        USceneComponent* SourceRootComponent = SourceActor->GetRootComponent();
        if (UStaticMeshComponent* StaticMeshComponent = BakedActor->GetStaticMeshComponent())
        {
            const EComponentMobility::Type TargetMobility = SourceRootComponent != nullptr
                ? static_cast<EComponentMobility::Type>(SourceRootComponent->Mobility.GetValue())
                : EComponentMobility::Movable;
            StaticMeshComponent->SetMobility(TargetMobility);
        }

        if (SourceRootComponent != nullptr)
        {
            BakedActor->AttachToComponent(SourceRootComponent, FAttachmentTransformRules::KeepWorldTransform);
        }
        else
        {
            BakedActor->AttachToActor(SourceActor, FAttachmentTransformRules::KeepWorldTransform);
        }
#if WITH_EDITOR
        BakedActor->SetFolderPath(SourceActor->GetFolderPath());
#endif

        if (UStaticMeshComponent* StaticMeshComponent = BakedActor->GetStaticMeshComponent())
        {
            StaticMeshComponent->SetStaticMesh(StaticMesh);
        }

        return BakedActor;
    }
}


bool FBlenderBridgeBakeService::BakeComponent(UBlenderBridgeComponent* Component)
{
    if (Component == nullptr || Component->GetOwner() == nullptr || !Component->HasCookedMesh())
    {
        return false;
    }

    const bool bWasShowingBakedOutput = Component->bShowingBakedOutput;

    const FString BakeFolderPath = BuildBakeFolderPath(Component);
    UMaterial* PlaceholderParent = EnsurePlaceholderParentMaterial(BakeFolderPath);

    const FString MaterialsFolderPath = FString::Printf(TEXT("%s/Materials"), *BakeFolderPath);
    TArray<UMaterialInterface*> ResolvedMaterials;
    TArray<FBlenderBridgeGeneratedMaterialReference> GeneratedMaterials;
    ResolvedMaterials.Reserve(Component->LastCookedMaterialSlots.Num());
    GeneratedMaterials.Reserve(Component->LastCookedMaterialSlots.Num());

    for (const FBridgeMaterialSlotDescriptor& Slot : Component->LastCookedMaterialSlots)
    {
        if (UMaterialInterface* OverrideMaterial = Component->FindOverrideMaterialBySlotId(Slot.SlotId))
        {
            ResolvedMaterials.Add(OverrideMaterial);
            continue;
        }

        UMaterialInstanceConstant* PlaceholderInstance = EnsurePlaceholderMaterialInstance(MaterialsFolderPath, Slot, PlaceholderParent);
        if (PlaceholderInstance == nullptr)
        {
            ResolvedMaterials.Add(nullptr);
            continue;
        }

        ResolvedMaterials.Add(PlaceholderInstance);

        FBlenderBridgeGeneratedMaterialReference GeneratedReference;
        GeneratedReference.SlotId = Slot.SlotId;
        GeneratedReference.SlotName = Slot.SlotName;
        GeneratedReference.BlenderMaterialName = Slot.BlenderMaterialName;
        GeneratedReference.Material = PlaceholderInstance;
        GeneratedMaterials.Add(MoveTemp(GeneratedReference));
    }

    UStaticMesh* StaticMesh = BuildOrUpdateStaticMesh(Component, BakeFolderPath, ResolvedMaterials);
    if (StaticMesh == nullptr)
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to build a baked static mesh asset."));
        return false;
    }

    AStaticMeshActor* BakedActor = SpawnOrUpdateBakedActor(Component, StaticMesh);
    if (BakedActor == nullptr)
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to spawn or update the baked static mesh actor."));
        return false;
    }

    Component->BakedStaticMesh = StaticMesh;
    Component->BakedActor = BakedActor;
    Component->GeneratedBakeMaterials = MoveTemp(GeneratedMaterials);
    if (bWasShowingBakedOutput)
    {
        Component->SetShowingBakedOutput(true);
    }
    else
    {
        Component->SetShowingBakedOutput(false);
        Component->QueueShowBakedOutputOnNextTick();
    }
    Component->SetBridgeStatus(
        EBlenderBridgeComponentStatus::Cooked,
        FString::Printf(TEXT("Baked static mesh output to %s."), *StaticMesh->GetPathName()));
    return true;
}

bool FBlenderBridgeBakeService::SelectBakedOutput(UBlenderBridgeComponent* Component)
{
    if (Component == nullptr)
    {
        return false;
    }

    if (AStaticMeshActor* BakedActor = Component->BakedActor.Get())
    {
        GEditor->SelectNone(false, true, false);
        GEditor->SelectActor(BakedActor, true, true, true, true);
        return true;
    }

    if (UStaticMesh* BakedMesh = Component->BakedStaticMesh.LoadSynchronous())
    {
        TArray<UObject*> ObjectsToSync;
        ObjectsToSync.Add(BakedMesh);
        GEditor->SyncBrowserToObjects(ObjectsToSync);
        return true;
    }

    return false;
}

bool FBlenderBridgeBakeService::DeleteBakedOutput(UBlenderBridgeComponent* Component)
{
    if (Component == nullptr)
    {
        return false;
    }

    bool bDeletedAnything = false;
    if (AStaticMeshActor* BakedActor = Component->BakedActor.Get())
    {
        if (UWorld* World = BakedActor->GetWorld())
        {
            World->EditorDestroyActor(BakedActor, true);
            bDeletedAnything = true;
        }
    }

    TArray<UObject*> AssetsToDelete;
    if (UStaticMesh* BakedMesh = Component->BakedStaticMesh.LoadSynchronous())
    {
        AssetsToDelete.AddUnique(BakedMesh);
    }

    for (const FBlenderBridgeGeneratedMaterialReference& GeneratedMaterial : Component->GeneratedBakeMaterials)
    {
        if (UMaterialInterface* Material = GeneratedMaterial.Material.LoadSynchronous())
        {
            AssetsToDelete.AddUnique(Material);
        }
    }

    if (!AssetsToDelete.IsEmpty())
    {
        ObjectTools::DeleteObjects(AssetsToDelete, false);
        bDeletedAnything = true;
    }

    if (!bDeletedAnything)
    {
        return false;
    }

    Component->BakedActor = nullptr;
    Component->BakedStaticMesh = nullptr;
    Component->GeneratedBakeMaterials.Reset();
    Component->SetShowingBakedOutput(false);
    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Cooked, TEXT("Deleted baked bridge output."));
    return true;
}
