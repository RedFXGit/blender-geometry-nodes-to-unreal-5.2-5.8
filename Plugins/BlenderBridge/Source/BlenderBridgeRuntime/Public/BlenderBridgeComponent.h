#pragma once

#include "BlenderBridgeProtocol.h"
#include "Components/SceneComponent.h"
#include "StructUtils/PropertyBag.h"
#include "UObject/NoExportTypes.h"

#include "BlenderBridgeComponent.generated.h"

class UBlenderBridgeSource;
class UProceduralMeshComponent;
class UUserDefinedEnum;
class FBlenderBridgeSessionManager;

UCLASS(ClassGroup = (Blender), meta = (BlueprintSpawnableComponent))
class BLENDERBRIDGERUNTIME_API UBlenderBridgeComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UBlenderBridgeComponent();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge")
    void RefreshInputSchema();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge", meta = (DisplayName = "Request Cook"))
    void RequestCook();

    UFUNCTION()
    TArray<FString> GetAvailableTargetOptions() const;

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

protected:
    virtual void OnRegister() override;
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    void EnsurePreviewMeshComponent();
    void ApplyDecodedMesh(const FBridgeDecodedMesh& DecodedMesh);
    void RebuildInputValues(bool bPreserveExistingValues);
    UUserDefinedEnum* GetOrCreateRuntimeEnum(const FBridgeInputSchema& Entry);
    void ResetBridgeData(bool bClearTargetSelection);
    FString GetSourceHandle() const;
    const FBridgeTargetDescriptor* FindTargetDescriptor(const FString& InTargetId) const;
    void UpdateTargetDisplayLabel();
    void SetBridgeStatus(EBlenderBridgeComponentStatus NewStatus, const FString& Message);

private:
    friend class FBlenderBridgeSessionManager;

    UPROPERTY(Transient)
    TObjectPtr<UProceduralMeshComponent> PreviewMeshComponent;

    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UUserDefinedEnum>> RuntimeInputEnums;

    TMap<FName, uint32> RuntimeInputEnumHashes;
};
