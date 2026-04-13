#pragma once

#include "GeometryNodesBridgeComponentTypes.h"
#include "GeometryNodesBridgeProtocol.h"
#include "Containers/Ticker.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "StructUtils/PropertyBag.h"
#include "UObject/ObjectKey.h"
#include "UObject/NoExportTypes.h"

#include "GeometryNodesBridgeComponent.generated.h"

class AStaticMeshActor;
class UMaterialInterface;
class UGeometryNodesBridgeSource;
class UProceduralMeshComponent;
class UGeometryNodesBridgeMoveProbeComponent;
class USplineComponent;
class UStaticMeshComponent;
class UStaticMesh;
class UUserDefinedEnum;
class FGeometryNodesBridgeSessionManager;
class FGeometryNodesBridgeBakeService;
class AGeometryNodesBridgeActor;
struct FPropertyChangedEvent;
#if WITH_DEV_AUTOMATION_TESTS
struct FGeometryNodesBridgeComponentTestAccess;
#endif

UCLASS(ClassGroup = (Blender), meta = (BlueprintSpawnableComponent))
class GEOMETRYNODESBRIDGERUNTIME_API UGeometryNodesBridgeComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UGeometryNodesBridgeComponent();

    void AssignSourceFromActorFactory(UGeometryNodesBridgeSource* InSource);
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

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Geometry Nodes Bridge")
    void RefreshInputSchema();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Geometry Nodes Bridge", meta = (DisplayName = "Request Cook"))
    void RequestCook();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Geometry Nodes Bridge|Bake", meta = (DisplayName = "Bake Output"))
    void BakeToStaticMesh();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Geometry Nodes Bridge|Bake", meta = (DisplayName = "Select Baked Output"))
    void SelectBakedOutput();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Geometry Nodes Bridge|Bake", meta = (DisplayName = "Delete Baked Output"))
    void DeleteBakedOutput();

    UFUNCTION()
    TArray<FString> GetAvailableTargetOptions() const;

    const FBridgeDecodedMesh& GetLastCookedMesh() const { return LastCookedMesh; }
    const FString& GetComponentHandle() const { return ComponentHandle; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge")
    TObjectPtr<UGeometryNodesBridgeSource> Source;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Geometry Nodes Bridge",
        meta = (
            DisplayName = "Target",
            GetOptions = "GetAvailableTargetOptions",
            ToolTip = "The Geometry Nodes target within the selected blend source."
        ))
    FString TargetId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge")
    FString TargetDisplayLabel;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge")
    TArray<FBridgeTargetDescriptor> AvailableTargets;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge")
    FString SchemaTargetId;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge")
    FString ComponentHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge")
    EGeometryNodesBridgeConnectionState ConnectionState = EGeometryNodesBridgeConnectionState::Disconnected;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge")
    EGeometryNodesBridgeComponentStatus ComponentStatus = EGeometryNodesBridgeComponentStatus::NoSession;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge")
    FString LastStatusMessage;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge", meta = (DisplayName = "Bridge Build"))
    FString BridgeBuildVersion;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Geometry", meta = (DisplayName = "Observed Reference Watch"))
    FString ObservedGeometryDebugSummary;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Geometry", meta = (DisplayName = "Last Reference Follow-Cook Event"))
    FString LastObservedGeometryEvent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge")
    TArray<FBridgeInputSchema> InputSchema;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Geometry")
    TArray<FBridgeGeometryInputDescriptor> GeometryInputDescriptors;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Geometry")
    TArray<FGeometryNodesBridgeGeometryBinding> GeometryBindings;

    UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Spline")
    FBridgeTargetCapabilities TargetCapabilities;

    UPROPERTY(EditAnywhere, Category = "Geometry Nodes Bridge", meta = (FixedLayout, GeometryNodesBridgeGroupedLayout))
    FInstancedPropertyBag InputValues;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake", meta = (DisplayName = "Cooked Material Slots"))
    TArray<FBridgeMaterialSlotDescriptor> LastCookedMaterialSlots;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    TArray<FGeometryNodesBridgeMaterialOverride> MaterialOverrides;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake", meta = (DisplayName = "Bake Output Mode"))
    EGeometryNodesBridgeBakeOutputMode BakeOutputMode = EGeometryNodesBridgeBakeOutputMode::StaticMesh;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Geometry Nodes Bridge|Bake",
        meta = (
            DisplayName = "Share Generated Bake Materials",
            ToolTip = "When enabled, generated placeholder bake materials are shared per source and target. Disable this to keep generated bake materials actor-local."
        ))
    bool bShareGeneratedBakeMaterials = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    TSoftObjectPtr<UStaticMesh> BakedStaticMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    TSoftObjectPtr<AActor> BakedActor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    TArray<FGeometryNodesBridgeBakedPrototypeMeshReference> BakedPrototypeMeshes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    TArray<FGeometryNodesBridgeGeneratedMaterialReference> GeneratedBakeMaterials;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake", meta = (DisplayName = "Hide Preview After Bake"))
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
    void HandleObservedMoveProbeTransformUpdated(
        UGeometryNodesBridgeMoveProbeComponent* MoveProbeComponent,
        const FTransform& PreviousTransform,
        const FTransform& CurrentTransform,
        ETeleportType Teleport);
    void QueueCookFromObservedGeometryChange(const TCHAR* StatusMessage);
    void QueueCookFromSplineEdit();
    void CancelPendingSplineCook();
    void RebuildInputValues(bool bPreserveExistingValues);
    void RebuildGeometryBindings(bool bPreserveExistingValues);
    UUserDefinedEnum* GetOrCreateRuntimeEnum(const FBridgeInputSchema& Entry);
    void ResetBridgeData(bool bClearTargetSelection);
    bool BuildCurveOwnerData(FBridgeCurveOwnerData& OutCurveData, FString& OutCurveHash) const;
    bool BuildGeometryBindingPayloads(
        TArray<FBridgeGeometryBindingPayload>& OutBindingPayloads,
        FString& OutBindingPayloadHash,
        FString& OutErrorMessage) const;
    FString ApplyCurveOwnerDescription(const FString& InTargetId, const FBridgeTargetCapabilities& InCapabilities, bool bPreserveExistingSplineState);
    bool ImportCurveOwnerData(const FBridgeCurveOwnerData& InCurveData, FString& OutCurveHash);
    FString GetSourceHandle() const;
    const FBridgeTargetDescriptor* FindTargetDescriptor(const FString& InTargetId) const;
    const FBridgeGeometryInputDescriptor* FindGeometryInputDescriptor(const FString& InInputId) const;
    void UpdateTargetDisplayLabel();
    void SetBridgeStatus(EGeometryNodesBridgeComponentStatus NewStatus, const FString& Message);
    void SyncMaterialOverridesFromCookedSlots();
    void QueueShowBakedOutputOnNextTick();
    void CancelPendingBakedOutputVisibilitySwap();
    void SetShowingBakedOutput(bool bInShowingBakedOutput);
    void UpdatePreviewVisibility();
    void UpdateBakedOutputVisibility();
    void ApplyPreviewMaterials();
    USplineComponent* EnsureBridgeSplineComponent();
    void RemoveOwnedBridgeSplineComponent();
    void NormalizeGeometryBindings();
    void ResetObservedGeometryTracking();
    EGeometryNodesBridgeGeometrySourceKind GetDefaultGeometryBindingSourceKind(const FBridgeGeometryInputDescriptor& Descriptor) const;
    EGeometryNodesBridgeGeometrySourceKind GetEffectiveGeometryBindingSourceKind(const FString& InputId) const;
    UStaticMeshComponent* ResolveBoundStaticMeshComponent(const FString& InputId) const;
    USplineComponent* ResolveBoundSplineComponent(const FString& InputId) const;
    USplineComponent* GetActiveCurveSourceSplineComponent() const;
    void RefreshObservedSplineComponents();
    UGeometryNodesBridgeMoveProbeComponent* FindOrCreateObservedMoveProbe(USceneComponent* SceneComponent);
    void ReleaseObservedMoveProbes();
    FString BuildObservedGeometryDebugSummary() const;
    bool BuildCurveOwnerDataFromSplineComponent(
        const USplineComponent* SplineComponent,
        FBridgeCurveOwnerData& OutCurveData,
        FString& OutCurveHash) const;
    UMaterialInterface* FindOverrideMaterialBySlotId(const FString& SlotId) const;
    bool HasCookedMesh() const;
    bool HasBakedOutput() const;

private:
    friend class FGeometryNodesBridgeSessionManager;
    friend class FGeometryNodesBridgeBakeService;
    friend class UGeometryNodesBridgeMoveProbeComponent;
#if WITH_DEV_AUTOMATION_TESTS
    friend struct FGeometryNodesBridgeComponentTestAccess;
#endif

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
    TArray<TWeakObjectPtr<USplineComponent>> ObservedSplineComponents;
    TArray<TWeakObjectPtr<USceneComponent>> ObservedGeometryComponents;
    TArray<TWeakObjectPtr<UGeometryNodesBridgeMoveProbeComponent>> ObservedMoveProbes;
};
