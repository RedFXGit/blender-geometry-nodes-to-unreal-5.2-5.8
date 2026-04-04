#pragma once

#include "BlenderBridgeProtocol.h"
#include "Components/SceneComponent.h"
#include "StructUtils/PropertyBag.h"
#include "UObject/NoExportTypes.h"

#include "BlenderBridgeComponent.generated.h"

class FBlenderBridgeWorkerSession;
class UBlenderBridgeSource;
class UProceduralMeshComponent;

UCLASS(ClassGroup = (Blender), meta = (BlueprintSpawnableComponent))
class BLENDERBRIDGERUNTIME_API UBlenderBridgeComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UBlenderBridgeComponent();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge")
    void StartBridge();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge")
    void StopBridge();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge")
    void RefreshInputSchema();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge", meta = (DisplayName = "Request Cook"))
    void RequestHelloWorldCook();

    UFUNCTION(CallInEditor, BlueprintCallable, Category = "Blender Bridge")
    void PushCurrentInputs();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    TObjectPtr<UBlenderBridgeSource> Source;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    FFilePath BlenderExecutablePath;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender Bridge", meta = (ClampMin = "1048576"))
    int64 SharedMemoryBytes = static_cast<int64>(BlenderBridge::DefaultSharedMemoryBytes);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    EBlenderBridgeConnectionState ConnectionState = EBlenderBridgeConnectionState::Disconnected;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    TArray<FBridgeInputSchema> InputSchema;

    UPROPERTY(EditAnywhere, Category = "Blender Bridge", meta = (FixedLayout))
    FInstancedPropertyBag InputValues;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge")
    FString LastStatusMessage;

protected:
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
    bool QueueCurrentInputMessages(bool bRequestCook, const TCHAR* ContextMessage);
    void EnsurePreviewMeshComponent();
    void ApplyMeshSlot(uint32 SlotIndex);
    void RebuildInputValues();
    void HandleStateChanged(EBlenderBridgeConnectionState NewState);
    void HandleSchemaReceived(const TArray<FBridgeInputSchema>& NewSchema);
    void HandleCookComplete(uint32 SlotIndex);
    void HandleLogMessage(const FString& Message);
    FString ResolveWorkerScriptPath() const;

private:
    TSharedPtr<FBlenderBridgeWorkerSession> Session;

    UPROPERTY(Transient)
    TObjectPtr<UProceduralMeshComponent> PreviewMeshComponent;
};
