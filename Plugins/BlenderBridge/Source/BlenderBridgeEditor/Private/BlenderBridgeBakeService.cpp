#include "BlenderBridgeBakeService.h"

#include "BlenderBridgeComponent.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSessionManager.h"
#include "BlenderBridgeSettings.h"
#include "BlenderBridgeSource.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Editor.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "GameFramework/Actor.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"
#include "Modules/ModuleManager.h"
#include "ObjectTools.h"
#include "PhysicsEngine/BodySetup.h"
#include "RenderingThread.h"
#include "StaticMeshAttributes.h"
#include "StaticMeshResources.h"
#include "UObject/UObjectIterator.h"
#include "UObject/Package.h"

DEFINE_LOG_CATEGORY_STATIC(LogBlenderBridgeBake, Log, All);

namespace
{
    constexpr TCHAR PlaceholderParentMaterialName[] = TEXT("M_BlenderBridgePlaceholderParent");
    constexpr TCHAR PlaceholderSupportFolderName[] = TEXT("_BridgeSupport");
    constexpr TCHAR SharedPrototypeFolderName[] = TEXT("_SharedPrototypes");
    constexpr TCHAR PlaceholderParameterName[] = TEXT("BaseColor");
    constexpr TCHAR BakeBuildPathTag[] = TEXT("[BlenderBridgeBakePath:EditorBuildSync_v2]");
    constexpr TCHAR BakedHismComponentNamePrefix[] = TEXT("BlenderBridgeBakedHISM");

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

    FString BuildSourceScopeKey(const UBlenderBridgeComponent* Component)
    {
        if (Component == nullptr || Component->Source == nullptr || Component->Source->BlendFile.FilePath.IsEmpty())
        {
            return FString();
        }

        FString SourceHandle = FPaths::ConvertRelativePathToFull(Component->Source->BlendFile.FilePath);
        FPaths::NormalizeFilename(SourceHandle);
        return SourceHandle;
    }

    FString BuildSharedPrototypeFolderPath(const UBlenderBridgeComponent* Component)
    {
        const UBlenderBridgeSettings* Settings = GetDefault<UBlenderBridgeSettings>();
        const FString RootPath = NormalizeBakeRootPath(Settings != nullptr ? Settings->BakeRootContentPath : FString());
        const FString SourceScopeKey = BuildSourceScopeKey(Component);
        if (SourceScopeKey.IsEmpty())
        {
            return FString::Printf(TEXT("%s/%s"), *RootPath, SharedPrototypeFolderName);
        }

        const FString ScopeHash = FMD5::HashAnsiString(*SourceScopeKey).Left(16);
        return FString::Printf(TEXT("%s/Blend_%s/%s"), *RootPath, *ScopeHash, SharedPrototypeFolderName);
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

    struct FResolvedBakeMaterials
    {
        TMap<FString, TObjectPtr<UMaterialInterface>> MaterialsBySlotId;
        TArray<FBlenderBridgeGeneratedMaterialReference> GeneratedMaterials;
    };

    struct FBakedOutputSnapshot
    {
        TWeakObjectPtr<AActor> Actor;
        TSoftObjectPtr<UStaticMesh> StaticMesh;
        TArray<FBlenderBridgeGeneratedMaterialReference> GeneratedMaterials;
        TArray<FBlenderBridgeBakedPrototypeMeshReference> PrototypeMeshes;
    };

    int32 GetRenderTexCoordCount(const UStaticMesh* StaticMesh, int32 LODIndex)
    {
        if (StaticMesh == nullptr || StaticMesh->GetRenderData() == nullptr || !StaticMesh->GetRenderData()->LODResources.IsValidIndex(LODIndex))
        {
            return 0;
        }

        return StaticMesh->GetRenderData()->LODResources[LODIndex].VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords();
    }

    bool ShrinkRenderTexCoordsToUV0(UStaticMesh* StaticMesh, int32 LODIndex)
    {
        if (StaticMesh == nullptr || StaticMesh->GetRenderData() == nullptr || !StaticMesh->GetRenderData()->LODResources.IsValidIndex(LODIndex))
        {
            return false;
        }

        FStaticMeshLODResources& LODResources = StaticMesh->GetRenderData()->LODResources[LODIndex];
        FStaticMeshVertexBuffer& VertexBuffer = LODResources.VertexBuffers.StaticMeshVertexBuffer;
        const uint32 SourceTexCoordCount = VertexBuffer.GetNumTexCoords();
        if (SourceTexCoordCount <= 1)
        {
            return false;
        }

        const uint32 NumVertices = VertexBuffer.GetNumVertices();
        if (NumVertices == 0)
        {
            return false;
        }

        const bool bNeedsCPUAccess = VertexBuffer.GetAllowCPUAccess();
        const bool bUseFullPrecisionUVs = VertexBuffer.GetUseFullPrecisionUVs();
        const bool bUseHighPrecisionTangentBasis = VertexBuffer.GetUseHighPrecisionTangentBasis();

        TArray<FVector3f> TangentX;
        TArray<FVector3f> TangentY;
        TArray<FVector3f> TangentZ;
        TArray<FVector2f> UV0;
        TangentX.Reserve(NumVertices);
        TangentY.Reserve(NumVertices);
        TangentZ.Reserve(NumVertices);
        UV0.Reserve(NumVertices);

        for (uint32 VertexIndex = 0; VertexIndex < NumVertices; ++VertexIndex)
        {
            const FVector4f TangentXValue = VertexBuffer.VertexTangentX(VertexIndex);
            const FVector4f TangentZValue = VertexBuffer.VertexTangentZ(VertexIndex);
            TangentX.Add(FVector3f(TangentXValue.X, TangentXValue.Y, TangentXValue.Z));
            TangentY.Add(VertexBuffer.VertexTangentY(VertexIndex));
            TangentZ.Add(FVector3f(TangentZValue.X, TangentZValue.Y, TangentZValue.Z));
            UV0.Add(VertexBuffer.GetVertexUV(VertexIndex, 0));
        }

        VertexBuffer.CleanUp();
        VertexBuffer.SetUseHighPrecisionTangentBasis(bUseHighPrecisionTangentBasis);
        VertexBuffer.SetUseFullPrecisionUVs(bUseFullPrecisionUVs);
        VertexBuffer.Init(NumVertices, 1, bNeedsCPUAccess);

        for (uint32 VertexIndex = 0; VertexIndex < NumVertices; ++VertexIndex)
        {
            VertexBuffer.SetVertexTangents(VertexIndex, TangentX[VertexIndex], TangentY[VertexIndex], TangentZ[VertexIndex]);
            VertexBuffer.SetVertexUV(VertexIndex, 0, UV0[VertexIndex]);
        }

        return true;
    }

    bool ShrinkRenderTexCoordsIfPadded(
        UStaticMesh* StaticMesh,
        int32 LODIndex,
        const FString& MeshAssetName,
        int32 SourceUVChannelCount,
        const TCHAR* PhaseTag)
    {
        if (StaticMesh == nullptr || SourceUVChannelCount != 1)
        {
            return false;
        }

        if (StaticMesh->GetRenderData() == nullptr || !StaticMesh->GetRenderData()->LODResources.IsValidIndex(LODIndex))
        {
            return false;
        }

        const int32 RenderTexCoordsBeforeFix = GetRenderTexCoordCount(StaticMesh, LODIndex);
        if (RenderTexCoordsBeforeFix <= 1)
        {
            return false;
        }

        UE_LOG(
            LogBlenderBridgeBake,
            Warning,
            TEXT("%s Shrinking padded render tex coords for '%s' %s (render tex coords before=%d)."),
            BakeBuildPathTag,
            *MeshAssetName,
            PhaseTag,
            RenderTexCoordsBeforeFix);

        TOptional<FStaticMeshComponentRecreateRenderStateContext> RecreateRenderStateContext;
        if (StaticMesh->AreRenderingResourcesInitialized())
        {
            const bool bInvalidateLighting = false;
            const bool bRefreshBounds = false;
            RecreateRenderStateContext.Emplace(StaticMesh, bInvalidateLighting, bRefreshBounds);
            StaticMesh->ReleaseResources();
            FlushRenderingCommands();
        }

        const bool bShrinkApplied = ShrinkRenderTexCoordsToUV0(StaticMesh, LODIndex);
        if (bShrinkApplied)
        {
            StaticMesh->UpdateUVChannelData(true);

            if (StaticMesh->AreRenderingResourcesInitialized() == false)
            {
                StaticMesh->InitResources();
            }
        }

        UE_LOG(
            LogBlenderBridgeBake,
            Log,
            TEXT("%s UV shrink for '%s' %s: applied=%d, render tex coords before=%d, after=%d"),
            BakeBuildPathTag,
            *MeshAssetName,
            PhaseTag,
            bShrinkApplied ? 1 : 0,
            RenderTexCoordsBeforeFix,
            GetRenderTexCoordCount(StaticMesh, LODIndex));

        return bShrinkApplied;
    }

    UStaticMesh* BuildOrUpdateStaticMeshAsset(
        const FString& MeshesFolderPath,
        const FString& MeshAssetName,
        const FBridgeDecodedMesh& CookedMesh,
        const TArray<FBridgeMaterialSlotDescriptor>& MaterialSlots,
        const TArray<UMaterialInterface*>& ResolvedMaterials)
    {
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

        StaticMesh->bAllowCPUAccess = true;
        StaticMesh->SetImportVersion(EImportStaticMeshVersion::LastVersion);
        StaticMesh->SetNumSourceModels(1);
        StaticMesh->SetLightMapCoordinateIndex(0);

        FMeshNaniteSettings NaniteSettings = StaticMesh->GetNaniteSettings();
        NaniteSettings.bEnabled = false;
        StaticMesh->SetNaniteSettings(NaniteSettings);

        if (StaticMesh->IsNaniteForceEnabled())
        {
            UE_LOG(
                LogBlenderBridgeBake,
                Warning,
                TEXT("%s Nanite is force-enabled by the engine/project for '%s'; baked meshes may still build through the Nanite fallback path."),
                BakeBuildPathTag,
                *MeshAssetName);
        }

        FStaticMeshSourceModel& SourceModel = StaticMesh->GetSourceModel(0);
        SourceModel.BuildSettings = FMeshBuildSettings();
        SourceModel.BuildSettings.bRecomputeNormals = false;
        SourceModel.BuildSettings.bRecomputeTangents = true;
        SourceModel.BuildSettings.bGenerateLightmapUVs = false;
        SourceModel.BuildSettings.SrcLightmapIndex = 0;
        SourceModel.BuildSettings.DstLightmapIndex = 0;
        SourceModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
        SourceModel.BuildSettings.bUseFullPrecisionUVs = false;

        TArray<FStaticMaterial> StaticMaterials;
        StaticMaterials.Reserve(MaterialSlots.Num());
        for (int32 SlotIndex = 0; SlotIndex < MaterialSlots.Num(); ++SlotIndex)
        {
            const FBridgeMaterialSlotDescriptor& Slot = MaterialSlots[SlotIndex];
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
        PolygonGroups.Reserve(MaterialSlots.Num());
        for (const FBridgeMaterialSlotDescriptor& Slot : MaterialSlots)
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

        FMeshDescription* StaticMeshDescription = StaticMesh->CreateMeshDescription(0);
        if (StaticMeshDescription == nullptr)
        {
            return nullptr;
        }

        *StaticMeshDescription = MeshDescription;
        StaticMesh->CommitMeshDescription(0);

        UE_LOG(
            LogBlenderBridgeBake,
            Log,
            TEXT("%s Building '%s' via committed MeshDescription + UStaticMesh::Build(false, &BuildErrors)."),
            BakeBuildPathTag,
            *MeshAssetName);

        TArray<FText> BuildErrors;
        StaticMesh->Build(false, &BuildErrors);
        for (const FText& BuildError : BuildErrors)
        {
            UE_LOG(LogBlenderBridgeBake, Warning, TEXT("Static mesh build warning for '%s': %s"), *MeshAssetName, *BuildError.ToString());
        }

        if (StaticMesh->GetRenderData() == nullptr || StaticMesh->GetRenderData()->LODResources.IsEmpty())
        {
            return nullptr;
        }

        ShrinkRenderTexCoordsIfPadded(StaticMesh, 0, MeshAssetName, MeshDescription.GetNumUVElementChannels(), TEXT("after editor build"));

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
        ShrinkRenderTexCoordsIfPadded(StaticMesh, 0, MeshAssetName, MeshDescription.GetNumUVElementChannels(), TEXT("after PostEditChange"));

        UE_LOG(
            LogBlenderBridgeBake,
            Log,
            TEXT("Built static mesh '%s': source UV channels=%d, built UV channels=%d, built tex coords=%d"),
            *StaticMesh->GetPathName(),
            MeshDescription.GetNumUVElementChannels(),
            StaticMesh->GetNumUVChannels(0),
            StaticMesh->GetNumTexCoords(0));

        StaticMesh->MarkPackageDirty();
        return StaticMesh;
    }

    void GatherInstanceBakeMaterialSlots(const FBridgeDecodedInstanceBake& InstanceBake, TArray<FBridgeMaterialSlotDescriptor>& OutSlots)
    {
        OutSlots.Reset();

        TSet<FString> SeenSlotIds;
        for (const FBridgeInstanceBakePrototype& Prototype : InstanceBake.Prototypes)
        {
            for (const FBridgeMaterialSlotDescriptor& Slot : Prototype.Mesh.MaterialSlots)
            {
                if (Slot.SlotId.IsEmpty() || SeenSlotIds.Contains(Slot.SlotId))
                {
                    continue;
                }

                SeenSlotIds.Add(Slot.SlotId);
                OutSlots.Add(Slot);
            }
        }
    }

    UMaterialInterface* FindMaterialOverrideBySlotId(const UBlenderBridgeComponent* Component, const FString& SlotId)
    {
        if (Component == nullptr)
        {
            return nullptr;
        }

        for (const FBlenderBridgeMaterialOverride& Entry : Component->MaterialOverrides)
        {
            if (Entry.SlotId == SlotId)
            {
                return Entry.Material;
            }
        }

        return nullptr;
    }

    void ResolveMaterialsForSlots(
        UBlenderBridgeComponent* Component,
        const TArray<FBridgeMaterialSlotDescriptor>& Slots,
        const FString& MaterialsFolderPath,
        UMaterialInterface* PlaceholderParent,
        FResolvedBakeMaterials& OutResolution)
    {
        OutResolution.MaterialsBySlotId.Reset();
        OutResolution.GeneratedMaterials.Reset();

        TSet<FString> AddedGeneratedSlotIds;
        for (const FBridgeMaterialSlotDescriptor& Slot : Slots)
        {
            if (Slot.SlotId.IsEmpty() || OutResolution.MaterialsBySlotId.Contains(Slot.SlotId))
            {
                continue;
            }

            if (UMaterialInterface* OverrideMaterial = FindMaterialOverrideBySlotId(Component, Slot.SlotId))
            {
                OutResolution.MaterialsBySlotId.Add(Slot.SlotId, OverrideMaterial);
                continue;
            }

            UMaterialInstanceConstant* PlaceholderInstance = EnsurePlaceholderMaterialInstance(MaterialsFolderPath, Slot, PlaceholderParent);
            OutResolution.MaterialsBySlotId.Add(Slot.SlotId, PlaceholderInstance);
            if (PlaceholderInstance == nullptr || AddedGeneratedSlotIds.Contains(Slot.SlotId))
            {
                continue;
            }

            AddedGeneratedSlotIds.Add(Slot.SlotId);
            FBlenderBridgeGeneratedMaterialReference GeneratedReference;
            GeneratedReference.SlotId = Slot.SlotId;
            GeneratedReference.SlotName = Slot.SlotName;
            GeneratedReference.BlenderMaterialName = Slot.BlenderMaterialName;
            GeneratedReference.Material = PlaceholderInstance;
            OutResolution.GeneratedMaterials.Add(MoveTemp(GeneratedReference));
        }
    }

    TArray<UMaterialInterface*> BuildResolvedMaterialsForSlotList(
        const TArray<FBridgeMaterialSlotDescriptor>& Slots,
        const FResolvedBakeMaterials& ResolvedMaterials)
    {
        TArray<UMaterialInterface*> Materials;
        Materials.Reserve(Slots.Num());
        for (const FBridgeMaterialSlotDescriptor& Slot : Slots)
        {
            if (const TObjectPtr<UMaterialInterface>* Material = ResolvedMaterials.MaterialsBySlotId.Find(Slot.SlotId))
            {
                Materials.Add(Material->Get());
            }
            else
            {
                Materials.Add(nullptr);
            }
        }

        return Materials;
    }

    EComponentMobility::Type GetSourceMobility(const UBlenderBridgeComponent* Component)
    {
        if (Component == nullptr || Component->GetOwner() == nullptr || Component->GetOwner()->GetRootComponent() == nullptr)
        {
            return EComponentMobility::Movable;
        }

        return static_cast<EComponentMobility::Type>(Component->GetOwner()->GetRootComponent()->Mobility.GetValue());
    }

    void AttachBakedActorToSource(UBlenderBridgeComponent* Component, AActor* BakedActor)
    {
        if (Component == nullptr || Component->GetOwner() == nullptr || BakedActor == nullptr)
        {
            return;
        }

        AActor* SourceActor = Component->GetOwner();
        BakedActor->SetActorTransform(SourceActor->GetActorTransform());
        BakedActor->SetActorLabel(FString::Printf(TEXT("%s Baked"), *GetOwnerSegment(Component)));

        if (USceneComponent* SourceRootComponent = SourceActor->GetRootComponent())
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
    }

    USceneComponent* EnsureActorRootComponent(AActor* Actor, const EComponentMobility::Type Mobility)
    {
        if (Actor == nullptr)
        {
            return nullptr;
        }

        USceneComponent* RootComponent = Actor->GetRootComponent();
        if (RootComponent == nullptr)
        {
            RootComponent = NewObject<USceneComponent>(Actor, TEXT("BlenderBridgeBakedRoot"), RF_Transactional);
            if (RootComponent == nullptr)
            {
                return nullptr;
            }

            Actor->AddInstanceComponent(RootComponent);
            Actor->SetRootComponent(RootComponent);
            RootComponent->RegisterComponent();
        }

        RootComponent->SetMobility(Mobility);
        return RootComponent;
    }

    AActor* SpawnAttachedActor(UBlenderBridgeComponent* Component, UClass* ActorClass)
    {
        if (Component == nullptr || Component->GetOwner() == nullptr || ActorClass == nullptr)
        {
            return nullptr;
        }

        AActor* SourceActor = Component->GetOwner();
        UWorld* World = SourceActor->GetWorld();
        if (World == nullptr)
        {
            return nullptr;
        }

        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Owner = SourceActor;
        SpawnParameters.OverrideLevel = SourceActor->GetLevel();
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        AActor* BakedActor = World->SpawnActor<AActor>(ActorClass, SourceActor->GetActorTransform(), SpawnParameters);
        if (BakedActor == nullptr)
        {
            return nullptr;
        }

        if (EnsureActorRootComponent(BakedActor, GetSourceMobility(Component)) == nullptr)
        {
            World->EditorDestroyActor(BakedActor, true);
            return nullptr;
        }

        AttachBakedActorToSource(Component, BakedActor);
        return BakedActor;
    }

    FBakedOutputSnapshot CaptureBakedOutputSnapshot(const UBlenderBridgeComponent* Component)
    {
        FBakedOutputSnapshot Snapshot;
        if (Component == nullptr)
        {
            return Snapshot;
        }

        Snapshot.Actor = Component->BakedActor.Get();
        Snapshot.StaticMesh = Component->BakedStaticMesh;
        Snapshot.GeneratedMaterials = Component->GeneratedBakeMaterials;
        Snapshot.PrototypeMeshes = Component->BakedPrototypeMeshes;
        return Snapshot;
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

        AStaticMeshActor* BakedActor = Cast<AStaticMeshActor>(Component->BakedActor.Get());
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

        AttachBakedActorToSource(Component, BakedActor);
        if (UStaticMeshComponent* StaticMeshComponent = BakedActor->GetStaticMeshComponent())
        {
            StaticMeshComponent->SetMobility(GetSourceMobility(Component));
            StaticMeshComponent->SetStaticMesh(StaticMesh);
        }

        return BakedActor;
    }

    void CollectReferencedPrototypePaths(
        const UBlenderBridgeComponent* ExcludedComponent,
        const FString& SourceScopeKey,
        TSet<FSoftObjectPath>& OutReferencedPaths)
    {
        OutReferencedPaths.Reset();

        if (SourceScopeKey.IsEmpty())
        {
            return;
        }

        for (TObjectIterator<UBlenderBridgeComponent> It; It; ++It)
        {
            const UBlenderBridgeComponent* Component = *It;
            if (!IsValid(Component) || Component == ExcludedComponent || Component->HasAnyFlags(RF_ClassDefaultObject))
            {
                continue;
            }

            for (const FBlenderBridgeBakedPrototypeMeshReference& PrototypeReference : Component->BakedPrototypeMeshes)
            {
                if (PrototypeReference.SourceScopeKey != SourceScopeKey)
                {
                    continue;
                }

                const FSoftObjectPath MeshPath = PrototypeReference.Mesh.ToSoftObjectPath();
                if (MeshPath.IsValid())
                {
                    OutReferencedPaths.Add(MeshPath);
                }
            }
        }
    }

    void CleanupReplacedBakedOutput(UBlenderBridgeComponent* Component, const FBakedOutputSnapshot& PreviousOutput)
    {
        if (Component == nullptr)
        {
            return;
        }

        if (AActor* PreviousActor = PreviousOutput.Actor.Get())
        {
            if (PreviousActor != Component->BakedActor.Get())
            {
                if (UWorld* World = PreviousActor->GetWorld())
                {
                    World->EditorDestroyActor(PreviousActor, true);
                }
            }
        }

        TArray<UObject*> AssetsToDelete;
        if (UStaticMesh* PreviousStaticMesh = PreviousOutput.StaticMesh.LoadSynchronous())
        {
            if (PreviousStaticMesh != Component->BakedStaticMesh.LoadSynchronous())
            {
                AssetsToDelete.AddUnique(PreviousStaticMesh);
            }
        }

        TSet<FSoftObjectPath> CurrentGeneratedMaterialPaths;
        for (const FBlenderBridgeGeneratedMaterialReference& MaterialReference : Component->GeneratedBakeMaterials)
        {
            const FSoftObjectPath MaterialPath = MaterialReference.Material.ToSoftObjectPath();
            if (MaterialPath.IsValid())
            {
                CurrentGeneratedMaterialPaths.Add(MaterialPath);
            }
        }

        for (const FBlenderBridgeGeneratedMaterialReference& MaterialReference : PreviousOutput.GeneratedMaterials)
        {
            const FSoftObjectPath MaterialPath = MaterialReference.Material.ToSoftObjectPath();
            if (!MaterialPath.IsValid() || CurrentGeneratedMaterialPaths.Contains(MaterialPath))
            {
                continue;
            }

            if (UMaterialInterface* Material = MaterialReference.Material.LoadSynchronous())
            {
                AssetsToDelete.AddUnique(Material);
            }
        }

        TSet<FSoftObjectPath> CurrentPrototypeMeshPaths;
        for (const FBlenderBridgeBakedPrototypeMeshReference& PrototypeReference : Component->BakedPrototypeMeshes)
        {
            const FSoftObjectPath MeshPath = PrototypeReference.Mesh.ToSoftObjectPath();
            if (MeshPath.IsValid())
            {
                CurrentPrototypeMeshPaths.Add(MeshPath);
            }
        }

        TMap<FString, TSet<FSoftObjectPath>> ReferencedPrototypePathsByScope;
        for (const FBlenderBridgeBakedPrototypeMeshReference& PrototypeReference : PreviousOutput.PrototypeMeshes)
        {
            const FSoftObjectPath MeshPath = PrototypeReference.Mesh.ToSoftObjectPath();
            if (!MeshPath.IsValid() || CurrentPrototypeMeshPaths.Contains(MeshPath))
            {
                continue;
            }

            TSet<FSoftObjectPath>* ReferencedPaths = ReferencedPrototypePathsByScope.Find(PrototypeReference.SourceScopeKey);
            if (ReferencedPaths == nullptr)
            {
                TSet<FSoftObjectPath> GatheredPaths;
                CollectReferencedPrototypePaths(Component, PrototypeReference.SourceScopeKey, GatheredPaths);
                ReferencedPaths = &ReferencedPrototypePathsByScope.Add(PrototypeReference.SourceScopeKey, MoveTemp(GatheredPaths));
            }

            if (ReferencedPaths->Contains(MeshPath))
            {
                continue;
            }

            if (UStaticMesh* Mesh = PrototypeReference.Mesh.LoadSynchronous())
            {
                AssetsToDelete.AddUnique(Mesh);
            }
        }

        if (!AssetsToDelete.IsEmpty())
        {
            ObjectTools::DeleteObjects(AssetsToDelete, false);
        }
    }
}


bool FBlenderBridgeBakeService::BakeComponent(UBlenderBridgeComponent* Component)
{
    if (Component == nullptr || Component->GetOwner() == nullptr)
    {
        return false;
    }

    const FBakedOutputSnapshot PreviousOutput = CaptureBakedOutputSnapshot(Component);
    const bool bWasShowingBakedOutput = Component->bShowingBakedOutput;
    bool bBakedSuccessfully = false;

    if (Component->BakeOutputMode == EBlenderBridgeBakeOutputMode::StaticMesh)
    {
        if (!Component->HasCookedMesh())
        {
            return false;
        }

        const FString BakeFolderPath = BuildBakeFolderPath(Component);
        const FString MaterialsFolderPath = FString::Printf(TEXT("%s/Materials"), *BakeFolderPath);

        FResolvedBakeMaterials ResolvedMaterialState;
        ResolveMaterialsForSlots(
            Component,
            Component->LastCookedMaterialSlots,
            MaterialsFolderPath,
            EnsurePlaceholderParentMaterial(BakeFolderPath),
            ResolvedMaterialState);

        const TArray<UMaterialInterface*> ResolvedMaterials =
            BuildResolvedMaterialsForSlotList(Component->LastCookedMaterialSlots, ResolvedMaterialState);
        UStaticMesh* StaticMesh = BuildOrUpdateStaticMeshAsset(
            FString::Printf(TEXT("%s/Meshes"), *BakeFolderPath),
            FString::Printf(TEXT("SM_%s"), *GetOwnerSegment(Component)),
            Component->GetLastCookedMesh(),
            Component->LastCookedMaterialSlots,
            ResolvedMaterials);
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
        Component->BakedPrototypeMeshes.Reset();
        Component->GeneratedBakeMaterials = MoveTemp(ResolvedMaterialState.GeneratedMaterials);
        Component->SetBridgeStatus(
            EBlenderBridgeComponentStatus::Cooked,
            FString::Printf(TEXT("Baked static mesh output to %s."), *StaticMesh->GetPathName()));
        bBakedSuccessfully = true;
    }
    else
    {
        const FBridgeDecodedInstanceBake* InstanceBake =
            FBlenderBridgeRuntimeModule::Get().GetSessionManager().FindCachedInstanceBake(Component);
        if (InstanceBake == nullptr || InstanceBake->Prototypes.IsEmpty() || InstanceBake->Placements.IsEmpty())
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("No instanced bake payload is available yet."));
            return false;
        }

        const FString ActorBakeFolderPath = BuildBakeFolderPath(Component);
        const FString MaterialsFolderPath = FString::Printf(TEXT("%s/Materials"), *ActorBakeFolderPath);
        const FString SharedPrototypeMeshesFolderPath = FString::Printf(TEXT("%s/Meshes"), *BuildSharedPrototypeFolderPath(Component));

        TArray<FBridgeMaterialSlotDescriptor> MaterialSlots;
        GatherInstanceBakeMaterialSlots(*InstanceBake, MaterialSlots);

        FResolvedBakeMaterials ResolvedMaterialState;
        ResolveMaterialsForSlots(
            Component,
            MaterialSlots,
            MaterialsFolderPath,
            EnsurePlaceholderParentMaterial(ActorBakeFolderPath),
            ResolvedMaterialState);

        TMap<int32, TArray<FTransform>> PlacementsByPrototypeIndex;
        for (const FBridgeInstanceBakePlacement& Placement : InstanceBake->Placements)
        {
            if (Placement.PrototypeIndex >= 0 && Placement.PrototypeIndex < InstanceBake->Prototypes.Num())
            {
                PlacementsByPrototypeIndex.FindOrAdd(Placement.PrototypeIndex).Add(Placement.Transform);
            }
        }

        AActor* BakedActor = SpawnAttachedActor(Component, AActor::StaticClass());
        if (BakedActor == nullptr)
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to spawn the baked instanced actor."));
            return false;
        }

        USceneComponent* RootComponent = EnsureActorRootComponent(BakedActor, GetSourceMobility(Component));
        if (RootComponent == nullptr)
        {
            if (UWorld* World = BakedActor->GetWorld())
            {
                World->EditorDestroyActor(BakedActor, true);
            }
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to initialize the baked instanced actor root."));
            return false;
        }

        const FString SourceScopeKey = BuildSourceScopeKey(Component);
        TArray<FBlenderBridgeBakedPrototypeMeshReference> PrototypeReferences;
        PrototypeReferences.Reserve(InstanceBake->Prototypes.Num());

        for (int32 PrototypeIndex = 0; PrototypeIndex < InstanceBake->Prototypes.Num(); ++PrototypeIndex)
        {
            const FBridgeInstanceBakePrototype& Prototype = InstanceBake->Prototypes[PrototypeIndex];
            if (Prototype.PrototypeKey.IsEmpty())
            {
                continue;
            }

            TArray<UMaterialInterface*> NullMaterials;
            NullMaterials.SetNum(Prototype.Mesh.MaterialSlots.Num());
            UStaticMesh* SharedMesh = BuildOrUpdateStaticMeshAsset(
                SharedPrototypeMeshesFolderPath,
                FString::Printf(TEXT("SMI_%s"), *SanitizePackageSegment(Prototype.PrototypeKey)),
                Prototype.Mesh,
                Prototype.Mesh.MaterialSlots,
                NullMaterials);
            if (SharedMesh == nullptr)
            {
                if (UWorld* World = BakedActor->GetWorld())
                {
                    World->EditorDestroyActor(BakedActor, true);
                }
                Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to build a shared prototype mesh asset."));
                return false;
            }

            FBlenderBridgeBakedPrototypeMeshReference PrototypeReference;
            PrototypeReference.SourceScopeKey = SourceScopeKey;
            PrototypeReference.PrototypeKey = Prototype.PrototypeKey;
            PrototypeReference.Mesh = SharedMesh;
            PrototypeReferences.Add(MoveTemp(PrototypeReference));

            const TArray<FTransform>* InstanceTransforms = PlacementsByPrototypeIndex.Find(PrototypeIndex);
            if (InstanceTransforms == nullptr || InstanceTransforms->IsEmpty())
            {
                continue;
            }

            UHierarchicalInstancedStaticMeshComponent* HismComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(
                BakedActor,
                *FString::Printf(TEXT("%s_%d"), BakedHismComponentNamePrefix, PrototypeIndex),
                RF_Transactional);
            if (HismComponent == nullptr)
            {
                if (UWorld* World = BakedActor->GetWorld())
                {
                    World->EditorDestroyActor(BakedActor, true);
                }
                Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to allocate a baked HISM component."));
                return false;
            }

            BakedActor->AddInstanceComponent(HismComponent);
            HismComponent->SetupAttachment(RootComponent);
            HismComponent->SetMobility(GetSourceMobility(Component));
            HismComponent->SetCanEverAffectNavigation(false);
            HismComponent->SetStaticMesh(SharedMesh);
            for (int32 SlotIndex = 0; SlotIndex < Prototype.Mesh.MaterialSlots.Num(); ++SlotIndex)
            {
                const FBridgeMaterialSlotDescriptor& Slot = Prototype.Mesh.MaterialSlots[SlotIndex];
                if (const TObjectPtr<UMaterialInterface>* Material = ResolvedMaterialState.MaterialsBySlotId.Find(Slot.SlotId))
                {
                    HismComponent->SetMaterial(SlotIndex, Material->Get());
                }
            }

            HismComponent->RegisterComponent();
            for (const FTransform& Transform : *InstanceTransforms)
            {
                HismComponent->AddInstance(Transform);
            }
        }

        Component->BakedActor = BakedActor;
        Component->BakedStaticMesh = nullptr;
        Component->BakedPrototypeMeshes = MoveTemp(PrototypeReferences);
        Component->GeneratedBakeMaterials = MoveTemp(ResolvedMaterialState.GeneratedMaterials);
        Component->SetBridgeStatus(
            EBlenderBridgeComponentStatus::Cooked,
            FString::Printf(
                TEXT("Baked %d shared prototype meshes across %d instances."),
                Component->BakedPrototypeMeshes.Num(),
                InstanceBake->Placements.Num()));
        bBakedSuccessfully = true;
    }

    if (!bBakedSuccessfully)
    {
        return false;
    }

    if (bWasShowingBakedOutput)
    {
        Component->SetShowingBakedOutput(true);
    }
    else
    {
        Component->SetShowingBakedOutput(false);
        Component->QueueShowBakedOutputOnNextTick();
    }

    CleanupReplacedBakedOutput(Component, PreviousOutput);
    return true;
}

bool FBlenderBridgeBakeService::SelectBakedOutput(UBlenderBridgeComponent* Component)
{
    if (Component == nullptr)
    {
        return false;
    }

    if (AActor* BakedActor = Component->BakedActor.Get())
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

    TArray<UObject*> ObjectsToSync;
    for (const FBlenderBridgeBakedPrototypeMeshReference& PrototypeReference : Component->BakedPrototypeMeshes)
    {
        if (UStaticMesh* Mesh = PrototypeReference.Mesh.LoadSynchronous())
        {
            ObjectsToSync.AddUnique(Mesh);
        }
    }

    if (!ObjectsToSync.IsEmpty())
    {
        GEditor->SyncBrowserToObjects(ObjectsToSync);
        return true;
    }

    return false;
}

bool FBlenderBridgeBakeService::DeleteBakedOutput(UBlenderBridgeComponent* Component)
{
    if (Component == nullptr || !Component->HasBakedOutput())
    {
        return false;
    }

    const FBakedOutputSnapshot PreviousOutput = CaptureBakedOutputSnapshot(Component);
    Component->BakedActor = nullptr;
    Component->BakedStaticMesh = nullptr;
    Component->BakedPrototypeMeshes.Reset();
    Component->GeneratedBakeMaterials.Reset();
    Component->SetShowingBakedOutput(false);
    CleanupReplacedBakedOutput(Component, PreviousOutput);
    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Cooked, TEXT("Deleted baked bridge output."));
    return true;
}
