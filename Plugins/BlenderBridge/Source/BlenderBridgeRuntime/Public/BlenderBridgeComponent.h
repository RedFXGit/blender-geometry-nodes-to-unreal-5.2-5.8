#pragma once

#include "BlenderBridgeProtocol.h"
#include "Containers/Ticker.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "StructUtils/PropertyBag.h"
#include "UObject/NoExportTypes.h"

#include "BlenderBridgeComponent.generated.h"

class AStaticMeshActor;
class UMaterialInterface;
class UBlenderBridgeSource;
class UProceduralMeshComponent;
class USplineComponent;
class UStaticMesh;
class UUserDefinedEnum;
class FBlenderBridgeSessionManager;
class FBlenderBridgeBakeService;
class ABlenderBridgeActor;

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

USTRUCT(BlueprintType)
struct FBlenderBridgeBakedPrototypeMeshReference
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    FString SourceScopeKey;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    FString PrototypeKey;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TSoftObjectPtr<UStaticMesh> Mesh;
};

UCLASS(ClassGroup = (Blender), meta = (BlueprintSpawnableComponent))
class BLENDERBRIDGERUNTIME_API UBlenderBridgeComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UBlenderBridgeComponent();

    void AssignSourceFromActorFactory(UBlenderBridgeSource* InSource);
    void SetAutomationRequestContext(
        const FString& InRunLabel,
        const FString& InScenarioId,
        int32 InIterationIndex,
        const FString& InActionKind,
        bool bInBypassCookCache = true,
        bool bInUseAutomationScratchBakeRoot = false);
    void ClearAutomationRequestContext();
    bool HasAutomationTimingContext() const;
    bool ShouldBypassCookCacheForAutomation() const;
    bool ShouldUseAutomationScratchBakeRoot() const;
    const FString& GetAutomationRunLabel() const { return AutomationRunLabel; }
    const FString& GetAutomationScenarioId() const { return AutomationScenarioId; }
    const FString& GetAutomationActionKind() const { return AutomationActionKind; }
    int32 GetAutomationIterationIndex() const { return AutomationIterationIndex; }

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge")
    void RefreshInputSchema();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge", meta = (DisplayName = "Request Cook"))
    void RequestCook();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge|Bake", meta = (DisplayName = "Bake Output"))
    void BakeToStaticMesh();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge|Bake", meta = (DisplayName = "Select Baked Output"))
    void SelectBakedOutput();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge|Bake", meta = (DisplayName = "Delete Baked Output"))
    void DeleteBakedOutput();

    UFUNCTION()
    TArray<FString> GetAvailableTargetOptions() const;

    const FBridgeDecodedMesh& GetLastCookedMesh() const { return LastCookedMesh; }
    const FString& GetComponentHandle() const { return ComponentHandle; }

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

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    FString ComponentHandle;

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

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Spline")
    FBridgeTargetCapabilities TargetCapabilities;

    UPROPERTY(EditAnywhere, Category = "Blender Bridge", meta = (FixedLayout, BlenderBridgeGroupedLayout))
    FInstancedPropertyBag InputValues;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake", meta = (DisplayName = "Cooked Material Slots"))
    TArray<FBridgeMaterialSlotDescriptor> LastCookedMaterialSlots;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TArray<FBlenderBridgeMaterialOverride> MaterialOverrides;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake", meta = (DisplayName = "Bake Output Mode"))
    EBlenderBridgeBakeOutputMode BakeOutputMode = EBlenderBridgeBakeOutputMode::StaticMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TSoftObjectPtr<UStaticMesh> BakedStaticMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TSoftObjectPtr<AActor> BakedActor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge|Bake")
    TArray<FBlenderBridgeBakedPrototypeMeshReference> BakedPrototypeMeshes;

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
    void RefreshComponentHandle();
    void BindSplineComponentDelegates();
    void UnbindSplineComponentDelegates();
    USplineComponent* GetBridgeSplineComponent() const;
    void HandleBridgeSplineChanged();
    void QueueCookFromSplineEdit();
    void CancelPendingSplineCook();
    void RebuildInputValues(bool bPreserveExistingValues);
    UUserDefinedEnum* GetOrCreateRuntimeEnum(const FBridgeInputSchema& Entry);
    void ResetBridgeData(bool bClearTargetSelection);
    bool BuildCurveOwnerData(FBridgeCurveOwnerData& OutCurveData, FString& OutCurveHash) const;
    FString ApplyCurveOwnerDescription(const FString& InTargetId, const FBridgeTargetCapabilities& InCapabilities, bool bPreserveExistingSplineState);
    bool ImportCurveOwnerData(const FBridgeCurveOwnerData& InCurveData, FString& OutCurveHash);
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
    USplineComponent* EnsureBridgeSplineComponent();
    void RemoveOwnedBridgeSplineComponent();
    UMaterialInterface* FindOverrideMaterialBySlotId(const FString& SlotId) const;
    bool HasCookedMesh() const;
    bool HasBakedOutput() const;

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
    FTSTicker::FDelegateHandle PendingSplineCookTickerHandle;
    TMap<FName, uint32> RuntimeInputEnumHashes;
    FBridgeDecodedMesh LastCookedMesh;
    FString ImportedCurveTargetId;
    FString ImportedCurveHash;
    FString AutomationRunLabel;
    FString AutomationScenarioId;
    FString AutomationActionKind;
    int32 AutomationIterationIndex = INDEX_NONE;
    bool bAutomationBypassCookCache = false;
    bool bUseAutomationScratchBakeRoot = false;
    bool bHasValidCookedMesh = false;
    bool bPendingBakeAfterCook = false;
    bool bPendingInstancedBake = false;
    bool bSplineEditedSinceImport = false;
    bool bSuppressSplineNotifications = false;
    bool bSuppressPostEditNotifications = false;
};
