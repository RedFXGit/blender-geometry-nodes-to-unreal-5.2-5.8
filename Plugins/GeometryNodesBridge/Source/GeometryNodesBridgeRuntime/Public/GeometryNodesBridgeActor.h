#pragma once

#include "GameFramework/Actor.h"

#include "GeometryNodesBridgeActor.generated.h"

class UGeometryNodesBridgeComponent;
class USplineComponent;

UCLASS(ClassGroup = (Blender), ComponentWrapperClass, ConversionRoot)
class GEOMETRYNODESBRIDGERUNTIME_API AGeometryNodesBridgeActor : public AActor
{
    GENERATED_BODY()

public:
    AGeometryNodesBridgeActor();

    UFUNCTION(BlueprintCallable, Category = "Geometry Nodes Bridge")
    void RefreshInputSchema();

    UFUNCTION(BlueprintCallable, Category = "Geometry Nodes Bridge", meta = (DisplayName = "Request Cook"))
    void RequestCook();

    UFUNCTION(BlueprintCallable, Category = "Geometry Nodes Bridge|Bake", meta = (DisplayName = "Bake Output"))
    void BakeToStaticMesh();

    UFUNCTION(BlueprintCallable, Category = "Geometry Nodes Bridge|Bake", meta = (DisplayName = "Select Baked Output"))
    void SelectBakedOutput();

    UFUNCTION(BlueprintCallable, Category = "Geometry Nodes Bridge|Bake", meta = (DisplayName = "Delete Baked Output"))
    void DeleteBakedOutput();

    UFUNCTION(BlueprintPure, Category = "Geometry Nodes Bridge")
    UGeometryNodesBridgeComponent* GetBridgeComponent() const;

    UFUNCTION(BlueprintPure, Category = "Geometry Nodes Bridge")
    USplineComponent* GetBridgeSplineComponent() const;

    USplineComponent* EnsureBridgeSplineComponent();
    void RemoveBridgeSplineComponent();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UGeometryNodesBridgeComponent> BridgeComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USplineComponent> BridgeSplineComponent;
};
