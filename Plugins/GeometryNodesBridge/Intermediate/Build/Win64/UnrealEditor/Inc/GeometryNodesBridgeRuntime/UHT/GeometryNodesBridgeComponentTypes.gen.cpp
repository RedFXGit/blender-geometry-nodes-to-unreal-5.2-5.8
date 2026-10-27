// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GeometryNodesBridgeComponentTypes.h"
#include "Engine/EngineTypes.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGeometryNodesBridgeComponentTypes() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FComponentReference(ETypeConstructPhase);
ENGINE_API UClass* Z_Construct_UClass_UMaterialInterface(ETypeConstructPhase);
ENGINE_API UClass* Z_Construct_UClass_UStaticMesh(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeGeometrySourceKind(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeGeometryBinding(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeMaterialOverride(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin ScriptStruct FGeometryNodesBridgeMaterialOverride ******************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FGeometryNodesBridgeMaterialOverride_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FGeometryNodesBridgeMaterialOverride>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FGeometryNodesBridgeMaterialOverride); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SlotId_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SlotName_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlenderMaterialName_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Material_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FGeometryNodesBridgeMaterialOverride constinit property declarations 
	static const UECodeGen_Private::FStrPropertyParams NewProp_SlotId;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SlotName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_BlenderMaterialName;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Material;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FGeometryNodesBridgeMaterialOverride constinit property declarations 
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGeometryNodesBridgeMaterialOverride>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FGeometryNodesBridgeMaterialOverride Property Definitions *********
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SlotId = { "SlotId", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeMaterialOverride, SlotId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SlotId_MetaData), NewProp_SlotId_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SlotName = { "SlotName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeMaterialOverride, SlotName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SlotName_MetaData), NewProp_SlotName_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_BlenderMaterialName = { "BlenderMaterialName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeMaterialOverride, BlenderMaterialName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlenderMaterialName_MetaData), NewProp_BlenderMaterialName_MetaData) };
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_Material = { "Material", nullptr, (EPropertyFlags)0x0114000000000015, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeMaterialOverride, Material), Z_Construct_UClass_UMaterialInterface, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Material_MetaData), NewProp_Material_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SlotId,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SlotName,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BlenderMaterialName,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Material,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FGeometryNodesBridgeMaterialOverride Property Definitions ***********
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"GeometryNodesBridgeMaterialOverride",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FGeometryNodesBridgeMaterialOverride>(),
	alignof(FGeometryNodesBridgeMaterialOverride),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeMaterialOverride;
UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeMaterialOverride(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeMaterialOverride.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeMaterialOverride.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FGeometryNodesBridgeMaterialOverride, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("GeometryNodesBridgeMaterialOverride"));
		}
		return Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeMaterialOverride.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeMaterialOverride.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeMaterialOverride.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeMaterialOverride.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FGeometryNodesBridgeMaterialOverride ********************************

// ********** Begin ScriptStruct FGeometryNodesBridgeGeneratedMaterialReference ********************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FGeometryNodesBridgeGeneratedMaterialReference>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FGeometryNodesBridgeGeneratedMaterialReference); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SlotId_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SlotName_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlenderMaterialName_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Material_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FGeometryNodesBridgeGeneratedMaterialReference constinit property declarations 
	static const UECodeGen_Private::FStrPropertyParams NewProp_SlotId;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SlotName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_BlenderMaterialName;
	static const UECodeGen_Private::FSoftObjectPropertyParams NewProp_Material;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FGeometryNodesBridgeGeneratedMaterialReference constinit property declarations 
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGeometryNodesBridgeGeneratedMaterialReference>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FGeometryNodesBridgeGeneratedMaterialReference Property Definitions 
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SlotId = { "SlotId", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeGeneratedMaterialReference, SlotId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SlotId_MetaData), NewProp_SlotId_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SlotName = { "SlotName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeGeneratedMaterialReference, SlotName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SlotName_MetaData), NewProp_SlotName_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_BlenderMaterialName = { "BlenderMaterialName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeGeneratedMaterialReference, BlenderMaterialName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlenderMaterialName_MetaData), NewProp_BlenderMaterialName_MetaData) };
const UECodeGen_Private::FSoftObjectPropertyParams UHT_STATICS::NewProp_Material = { "Material", nullptr, (EPropertyFlags)0x0014000000020015, UECodeGen_Private::EPropertyGenFlags::SoftObject, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeGeneratedMaterialReference, Material), Z_Construct_UClass_UMaterialInterface, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Material_MetaData), NewProp_Material_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SlotId,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SlotName,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BlenderMaterialName,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Material,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FGeometryNodesBridgeGeneratedMaterialReference Property Definitions *
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"GeometryNodesBridgeGeneratedMaterialReference",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FGeometryNodesBridgeGeneratedMaterialReference>(),
	alignof(FGeometryNodesBridgeGeneratedMaterialReference),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference;
UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("GeometryNodesBridgeGeneratedMaterialReference"));
		}
		return Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FGeometryNodesBridgeGeneratedMaterialReference **********************

// ********** Begin ScriptStruct FGeometryNodesBridgeBakedPrototypeMeshReference *******************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FGeometryNodesBridgeBakedPrototypeMeshReference>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FGeometryNodesBridgeBakedPrototypeMeshReference); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SourceScopeKey_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PrototypeKey_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Mesh_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FGeometryNodesBridgeBakedPrototypeMeshReference constinit property declarations 
	static const UECodeGen_Private::FStrPropertyParams NewProp_SourceScopeKey;
	static const UECodeGen_Private::FStrPropertyParams NewProp_PrototypeKey;
	static const UECodeGen_Private::FSoftObjectPropertyParams NewProp_Mesh;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FGeometryNodesBridgeBakedPrototypeMeshReference constinit property declarations 
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGeometryNodesBridgeBakedPrototypeMeshReference>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FGeometryNodesBridgeBakedPrototypeMeshReference Property Definitions 
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SourceScopeKey = { "SourceScopeKey", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeBakedPrototypeMeshReference, SourceScopeKey), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SourceScopeKey_MetaData), NewProp_SourceScopeKey_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_PrototypeKey = { "PrototypeKey", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeBakedPrototypeMeshReference, PrototypeKey), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PrototypeKey_MetaData), NewProp_PrototypeKey_MetaData) };
const UECodeGen_Private::FSoftObjectPropertyParams UHT_STATICS::NewProp_Mesh = { "Mesh", nullptr, (EPropertyFlags)0x0014000000020015, UECodeGen_Private::EPropertyGenFlags::SoftObject, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeBakedPrototypeMeshReference, Mesh), Z_Construct_UClass_UStaticMesh, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Mesh_MetaData), NewProp_Mesh_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SourceScopeKey,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_PrototypeKey,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Mesh,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FGeometryNodesBridgeBakedPrototypeMeshReference Property Definitions 
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"GeometryNodesBridgeBakedPrototypeMeshReference",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FGeometryNodesBridgeBakedPrototypeMeshReference>(),
	alignof(FGeometryNodesBridgeBakedPrototypeMeshReference),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference;
UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("GeometryNodesBridgeBakedPrototypeMeshReference"));
		}
		return Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FGeometryNodesBridgeBakedPrototypeMeshReference *********************

// ********** Begin ScriptStruct FGeometryNodesBridgeGeometryBinding *******************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FGeometryNodesBridgeGeometryBinding_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FGeometryNodesBridgeGeometryBinding>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FGeometryNodesBridgeGeometryBinding); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InputId_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Geometry" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SourceKind_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Geometry" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ComponentReference_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Geometry" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponentTypes.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FGeometryNodesBridgeGeometryBinding constinit property declarations 
	static const UECodeGen_Private::FStrPropertyParams NewProp_InputId;
	static const UECodeGen_Private::FBytePropertyParams NewProp_SourceKind_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_SourceKind;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ComponentReference;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FGeometryNodesBridgeGeometryBinding constinit property declarations *
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGeometryNodesBridgeGeometryBinding>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FGeometryNodesBridgeGeometryBinding Property Definitions **********
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_InputId = { "InputId", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeGeometryBinding, InputId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InputId_MetaData), NewProp_InputId_MetaData) };
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_SourceKind_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_SourceKind = { "SourceKind", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeGeometryBinding, SourceKind), Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeGeometrySourceKind, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SourceKind_MetaData), NewProp_SourceKind_MetaData) }; // 982d8dce3d359296593172c0bef4ace48e3c7fd7
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_ComponentReference = { "ComponentReference", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(FGeometryNodesBridgeGeometryBinding, ComponentReference), Z_Construct_UScriptStruct_FComponentReference, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ComponentReference_MetaData), NewProp_ComponentReference_MetaData) }; // 9b9ae5abd01cfb2cb2ece10137db004a3044215c
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_InputId,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SourceKind_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SourceKind,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ComponentReference,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FGeometryNodesBridgeGeometryBinding Property Definitions ************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"GeometryNodesBridgeGeometryBinding",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FGeometryNodesBridgeGeometryBinding>(),
	alignof(FGeometryNodesBridgeGeometryBinding),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeometryBinding;
UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeGeometryBinding(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeometryBinding.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeometryBinding.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FGeometryNodesBridgeGeometryBinding, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("GeometryNodesBridgeGeometryBinding"));
		}
		return Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeometryBinding.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeometryBinding.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeometryBinding.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeometryBinding.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FGeometryNodesBridgeGeometryBinding *********************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeComponentTypes_h__Script_GeometryNodesBridgeRuntime_Statics
struct UHT_STATICS
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ Z_Construct_UScriptStruct_FGeometryNodesBridgeMaterialOverride, Z_Construct_UScriptStruct_FGeometryNodesBridgeMaterialOverride_Statics::NewStructOps, TEXT("GeometryNodesBridgeMaterialOverride"),&Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeMaterialOverride, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FGeometryNodesBridgeMaterialOverride), 4249744985U) },
		{ Z_Construct_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference, Z_Construct_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference_Statics::NewStructOps, TEXT("GeometryNodesBridgeGeneratedMaterialReference"),&Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FGeometryNodesBridgeGeneratedMaterialReference), 2210960230U) },
		{ Z_Construct_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference, Z_Construct_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference_Statics::NewStructOps, TEXT("GeometryNodesBridgeBakedPrototypeMeshReference"),&Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FGeometryNodesBridgeBakedPrototypeMeshReference), 3074046662U) },
		{ Z_Construct_UScriptStruct_FGeometryNodesBridgeGeometryBinding, Z_Construct_UScriptStruct_FGeometryNodesBridgeGeometryBinding_Statics::NewStructOps, TEXT("GeometryNodesBridgeGeometryBinding"),&Z_Registration_Info_UScriptStruct_FGeometryNodesBridgeGeometryBinding, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FGeometryNodesBridgeGeometryBinding), 3165509557U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeComponentTypes_h__Script_GeometryNodesBridgeRuntime_cbc3aaff93b5768a2dc694b8c9dbbd94ca3c22d0{
	TEXT("/Script/GeometryNodesBridgeRuntime"),
	nullptr, 0,
	UHT_STATICS::ScriptStructInfo, UE_ARRAY_COUNT(UHT_STATICS::ScriptStructInfo),
	nullptr, 0,
	nullptr, 0,
};
#undef UHT_STATICS
// ********** End Registration *********************************************************************
#undef UHT_STRUCT_BASE

PRAGMA_ENABLE_DEPRECATION_WARNINGS
