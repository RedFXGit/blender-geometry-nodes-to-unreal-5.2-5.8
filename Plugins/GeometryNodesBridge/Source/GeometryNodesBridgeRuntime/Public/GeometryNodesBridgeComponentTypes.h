#pragma once

#include "GeometryNodesBridgeProtocol.h"

#include "GeometryNodesBridgeComponentTypes.generated.h"

class UMaterialInterface;
class UStaticMesh;

USTRUCT(BlueprintType)
struct FGeometryNodesBridgeMaterialOverride
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    FString SlotId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    FString SlotName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    FString BlenderMaterialName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    TObjectPtr<UMaterialInterface> Material = nullptr;
};

USTRUCT(BlueprintType)
struct FGeometryNodesBridgeGeneratedMaterialReference
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    FString SlotId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    FString SlotName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    FString BlenderMaterialName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    TSoftObjectPtr<UMaterialInterface> Material;
};

USTRUCT(BlueprintType)
struct FGeometryNodesBridgeBakedPrototypeMeshReference
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    FString SourceScopeKey;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    FString PrototypeKey;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Bake")
    TSoftObjectPtr<UStaticMesh> Mesh;
};

USTRUCT(BlueprintType)
struct FGeometryNodesBridgeGeometryBinding
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Geometry")
    FString InputId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Geometry")
    EGeometryNodesBridgeGeometrySourceKind SourceKind = EGeometryNodesBridgeGeometrySourceKind::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Geometry Nodes Bridge|Geometry")
    FComponentReference ComponentReference;
};
