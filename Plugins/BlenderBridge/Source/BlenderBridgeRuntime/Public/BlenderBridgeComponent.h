#pragma once

#include "BlenderBridgeProtocol.h"
#include "Containers/Ticker.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "StructUtils/PropertyBag.h"
#include "UObject/NoExportTypes.h"

#include "BlenderBridgeComponent.generated.h"

class AStaticMeshActor;
class UMaterialInterface;
class UBlenderBridgeSource;
class UProceduralMeshComponent;
class UStaticMesh;
class UUserDefinedEnum;
class FBlenderBridgeSessionManager;
class FBlenderBridgeBakeService;

USTRUCT(BlueprintType)
struct FBlenderBridgeMaterialOverride
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    FString SlotId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    FString SlotName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    FString BlenderMaterialName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TObjectPtr<UMaterialInterface> Material = nullptr;
};

USTRUCT(BlueprintType)
struct FBlenderBridgeGeneratedMaterialReference
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    FString SlotId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    FString SlotName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    FString BlenderMaterialName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TSoftObjectPtr<UMaterialInterface> Material;
};

UCLASS(ClassGroup = (Blender), meta = (BlueprintSpawnableComponent))
class BLENDERBRIDGERUNTIME_API UBlenderBridgeComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UBlenderBridgeComponent();

    void AssignSourceFromActorFactory(UBlenderBridgeSource* InSource);

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge")
    void RefreshInputSchema();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge", meta = (DisplayName = "Request Cook"))
    void RequestCook();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge|Bake", meta = (DisplayName = "Bake To Static Mesh"))
    void BakeToStaticMesh();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge|Bake", meta = (DisplayName = "Select Baked Output"))
    void SelectBakedOutput();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge|Bake", meta = (DisplayName = "Delete Baked Output"))
    void DeleteBakedOutput();

    UFUNCTION()
    TArray<FString> GetAvailableTargetOptions() const;

    const FBridgeDecodedMesh& GetLastCookedMesh() const { return LastCookedMesh; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    TObjectPtr<UBlenderBridgeSource> Source;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Blender Bridge",
        meta = (
            DisplayName = "Target",
            GetOptions = "GetAvailableTargetOptions",
            ToolTip = "The Geometry Nodes target within the selected blend source."
        ))
    FString TargetId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    FString TargetDisplayLabel;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    TArray<FBridgeTargetDescriptor> AvailableTargets;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    FString SchemaTargetId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    EBlenderBridgeConnectionState ConnectionState = EBlenderBridgeConnectionState::Disconnected;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    EBlenderBridgeComponentStatus ComponentStatus = EBlenderBridgeComponentStatus::NoSession;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    FString LastStatusMessage;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge", meta = (DisplayName = "Bridge Build"))
    FString BridgeBuildVersion;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    TArray<FBridgeInputSchema> InputSchema;

    UPROPERTY(EditAnywhere, Category = "Blender Bridge", meta = (FixedLayout, BlenderBridgeGroupedLayout))
    FInstancedPropertyBag InputValues;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake", meta = (DisplayName = "Cooked Material Slots"))
    TArray<FBridgeMaterialSlotDescriptor> LastCookedMaterialSlots;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TArray<FBlenderBridgeMaterialOverride> MaterialOverrides;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TSoftObjectPtr<UStaticMesh> BakedStaticMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TSoftObjectPtr<AStaticMeshActor> BakedActor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TArray<FBlenderBridgeGeneratedMaterialReference> GeneratedBakeMaterials;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake", meta = (DisplayName = "Hide Preview After Bake"))
    bool bHidePreviewAfterBake = true;

protected:
    virtual void OnRegister() override;
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void EnsurePreviewMeshComponent();
    void ApplyDecodedMesh(FBridgeDecodedMesh&& DecodedMesh);
    void RebuildInputValues(bool bPreserveExistingValues);
    UUserDefinedEnum* GetOrCreateRuntimeEnum(const FBridgeInputSchema& Entry);
    void ResetBridgeData(bool bClearTargetSelection);
    FString GetSourceHandle() const;
    const FBridgeTargetDescriptor* FindTargetDescriptor(const FString& InTargetId) const;
    void UpdateTargetDisplayLabel();
    void SetBridgeStatus(EBlenderBridgeComponentStatus NewStatus, const FString& Message);
    void SyncMaterialOverridesFromCookedSlots();
    void QueueShowBakedOutputOnNextTick();
    void CancelPendingBakedOutputVisibilitySwap();
    void SetShowingBakedOutput(bool bInShowingBakedOutput);
    void UpdatePreviewVisibility();
    void UpdateBakedOutputVisibility();
    void ApplyPreviewMaterials();
    UMaterialInterface* FindOverrideMaterialBySlotId(const FString& SlotId) const;
    bool HasCookedMesh() const;

private:
    friend class FBlenderBridgeSessionManager;
    friend class FBlenderBridgeBakeService;

    UPROPERTY(Transient)
    TObjectPtr<UProceduralMeshComponent> PreviewMeshComponent;

    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UUserDefinedEnum>> RuntimeInputEnums;

    UPROPERTY()
    bool bShowingBakedOutput = false;

    FTSTicker::FDelegateHandle PendingShowBakedOutputTickerHandle;
    TMap<FName, uint32> RuntimeInputEnumHashes;
    FBridgeDecodedMesh LastCookedMesh;
    bool bHasValidCookedMesh = false;
    bool bPendingBakeAfterCook = false;
    bool bSuppressPostEditNotifications = false;
};
