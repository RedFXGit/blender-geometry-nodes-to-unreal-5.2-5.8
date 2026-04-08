#pragma once

#include "GameFramework/Actor.h"

#include "BlenderBridgeActor.generated.h"

class UBlenderBridgeComponent;
class USplineComponent;

UCLASS(ClassGroup = (Blender), ComponentWrapperClass, ConversionRoot)
class BLENDERBRIDGERUNTIME_API ABlenderBridgeActor : public AActor
{
    GENERATED_BODY()

public:
    ABlenderBridgeActor();

    UFUNCTION(BlueprintCallable, Category = "Blender Bridge")
    void RefreshInputSchema();

    UFUNCTION(BlueprintCallable, Category = "Blender Bridge", meta = (DisplayName = "Request Cook"))
    void RequestCook();

    UFUNCTION(BlueprintCallable, Category = "Blender Bridge|Bake", meta = (DisplayName = "Bake To Static Mesh"))
    void BakeToStaticMesh();

    UFUNCTION(BlueprintCallable, Category = "Blender Bridge|Bake", meta = (DisplayName = "Select Baked Output"))
    void SelectBakedOutput();

    UFUNCTION(BlueprintCallable, Category = "Blender Bridge|Bake", meta = (DisplayName = "Delete Baked Output"))
    void DeleteBakedOutput();

    UFUNCTION(BlueprintPure, Category = "Blender Bridge")
    UBlenderBridgeComponent* GetBridgeComponent() const;

    UFUNCTION(BlueprintPure, Category = "Blender Bridge")
    USplineComponent* GetBridgeSplineComponent() const;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBlenderBridgeComponent> BridgeComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blender Bridge", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USplineComponent> BridgeSplineComponent;
};
