// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GeometryNodesBridgeComponent.h"
#include "GeometryNodesBridgeComponentTypes.h"
#include "GeometryNodesBridgeProtocol.h"
#include "StructUtils/PropertyBag.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGeometryNodesBridgeComponent() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FInstancedPropertyBag(ETypeConstructPhase);
ENGINE_API UClass* Z_Construct_UClass_USceneComponent(ETypeConstructPhase);
ENGINE_API UClass* Z_Construct_UClass_AActor(ETypeConstructPhase);
ENGINE_API UClass* Z_Construct_UClass_UStaticMesh(ETypeConstructPhase);
ENGINE_API UClass* Z_Construct_UClass_UUserDefinedEnum(ETypeConstructPhase);
PROCEDURALMESHCOMPONENT_API UClass* Z_Construct_UClass_UProceduralMeshComponent(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeGeometryInputDescriptor(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeInputSchema(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeTargetCapabilities(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeTargetDescriptor(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeBakeOutputMode(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeComponentStatus(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeConnectionState(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UClass* Z_Construct_UClass_UGeometryNodesBridgeComponent(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeGeometryBinding(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FGeometryNodesBridgeMaterialOverride(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UClass* Z_Construct_UClass_UGeometryNodesBridgeComponent(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UClass* Z_Construct_UClass_UGeometryNodesBridgeSource(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Class UGeometryNodesBridgeComponent Function BakeToStaticMesh ******************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGeometryNodesBridgeComponent_BakeToStaticMesh_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Bake Output" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function BakeToStaticMesh constinit property declarations **********************
// ********** End Function BakeToStaticMesh constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGeometryNodesBridgeComponent, nullptr, "BakeToStaticMesh", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_UGeometryNodesBridgeComponent_BakeToStaticMesh(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGeometryNodesBridgeComponent::execBakeToStaticMesh)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->BakeToStaticMesh();
	P_NATIVE_END;
}
// ********** End Class UGeometryNodesBridgeComponent Function BakeToStaticMesh ********************

// ********** Begin Class UGeometryNodesBridgeComponent Function DeleteBakedOutput *****************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGeometryNodesBridgeComponent_DeleteBakedOutput_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Delete Baked Output" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function DeleteBakedOutput constinit property declarations *********************
// ********** End Function DeleteBakedOutput constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGeometryNodesBridgeComponent, nullptr, "DeleteBakedOutput", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_UGeometryNodesBridgeComponent_DeleteBakedOutput(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGeometryNodesBridgeComponent::execDeleteBakedOutput)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->DeleteBakedOutput();
	P_NATIVE_END;
}
// ********** End Class UGeometryNodesBridgeComponent Function DeleteBakedOutput *******************

// ********** Begin Class UGeometryNodesBridgeComponent Function GetAvailableTargetOptions *********
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGeometryNodesBridgeComponent_GetAvailableTargetOptions_Statics
struct UHT_STATICS
{
	struct GeometryNodesBridgeComponent_eventGetAvailableTargetOptions_Parms
	{
		TArray<FString> ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function GetAvailableTargetOptions constinit property declarations *************
	static const UECodeGen_Private::FStrPropertyParams NewProp_ReturnValue_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetAvailableTargetOptions constinit property declarations ***************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetAvailableTargetOptions Property Definitions ************************
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_ReturnValue_Inner = { "ReturnValue", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(GeometryNodesBridgeComponent_eventGetAvailableTargetOptions_Parms, ReturnValue), EArrayPropertyFlags::None, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ReturnValue_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Function GetAvailableTargetOptions Property Definitions **************************
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGeometryNodesBridgeComponent, nullptr, "GetAvailableTargetOptions", UHT_STATICS::PropPointers, UE_ARRAY_COUNT(UHT_STATICS::PropPointers), DataSizeOf<UHT_STATICS::GeometryNodesBridgeComponent_eventGetAvailableTargetOptions_Parms>(), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x40020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
static_assert(sizeof(UHT_STATICS::GeometryNodesBridgeComponent_eventGetAvailableTargetOptions_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGeometryNodesBridgeComponent_GetAvailableTargetOptions(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGeometryNodesBridgeComponent::execGetAvailableTargetOptions)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(TArray<FString>*)Z_Param__Result=P_THIS->GetAvailableTargetOptions();
	P_NATIVE_END;
}
// ********** End Class UGeometryNodesBridgeComponent Function GetAvailableTargetOptions ***********

// ********** Begin Class UGeometryNodesBridgeComponent Function RefreshInputSchema ****************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGeometryNodesBridgeComponent_RefreshInputSchema_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function RefreshInputSchema constinit property declarations ********************
// ********** End Function RefreshInputSchema constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGeometryNodesBridgeComponent, nullptr, "RefreshInputSchema", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_UGeometryNodesBridgeComponent_RefreshInputSchema(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGeometryNodesBridgeComponent::execRefreshInputSchema)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->RefreshInputSchema();
	P_NATIVE_END;
}
// ********** End Class UGeometryNodesBridgeComponent Function RefreshInputSchema ******************

// ********** Begin Class UGeometryNodesBridgeComponent Function RequestCook ***********************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGeometryNodesBridgeComponent_RequestCook_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Geometry Nodes Bridge" },
		{ "DisplayName", "Request Cook" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function RequestCook constinit property declarations ***************************
// ********** End Function RequestCook constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGeometryNodesBridgeComponent, nullptr, "RequestCook", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_UGeometryNodesBridgeComponent_RequestCook(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGeometryNodesBridgeComponent::execRequestCook)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->RequestCook();
	P_NATIVE_END;
}
// ********** End Class UGeometryNodesBridgeComponent Function RequestCook *************************

// ********** Begin Class UGeometryNodesBridgeComponent Function SelectBakedOutput *****************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGeometryNodesBridgeComponent_SelectBakedOutput_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Select Baked Output" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function SelectBakedOutput constinit property declarations *********************
// ********** End Function SelectBakedOutput constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGeometryNodesBridgeComponent, nullptr, "SelectBakedOutput", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_UGeometryNodesBridgeComponent_SelectBakedOutput(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGeometryNodesBridgeComponent::execSelectBakedOutput)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SelectBakedOutput();
	P_NATIVE_END;
}
// ********** End Class UGeometryNodesBridgeComponent Function SelectBakedOutput *******************

// ********** Begin Class UGeometryNodesBridgeComponent ********************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UClass_UGeometryNodesBridgeComponent_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Blender" },
		{ "HideCategories", "Trigger PhysicsVolume" },
		{ "IncludePath", "GeometryNodesBridgeComponent.h" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Source_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetId_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "DisplayName", "Target" },
		{ "GetOptions", "GetAvailableTargetOptions" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The Geometry Nodes target within the selected blend source." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetDisplayLabel_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AvailableTargets_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SchemaTargetId_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ComponentHandle_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ConnectionState_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ComponentStatus_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LastStatusMessage_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BridgeBuildVersion_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "DisplayName", "Bridge Build" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ObservedGeometryDebugSummary_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Geometry" },
		{ "DisplayName", "Observed Reference Watch" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LastObservedGeometryEvent_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Geometry" },
		{ "DisplayName", "Last Reference Follow-Cook Event" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InputSchema_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GeometryInputDescriptors_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Geometry" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GeometryBindings_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Geometry" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetCapabilities_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Spline" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InputValues_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "FixedLayout", "" },
		{ "GeometryNodesBridgeGroupedLayout", "" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LastCookedMaterialSlots_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Cooked Material Slots" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaterialOverrides_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BakeOutputMode_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Bake Output Mode" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bShareGeneratedBakeMaterials_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Share Generated Bake Materials" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "When enabled, generated placeholder bake materials are shared per source and target. Disable this to keep generated bake materials actor-local." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BakedStaticMesh_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BakedActor_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BakedPrototypeMeshes_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GeneratedBakeMaterials_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bHidePreviewAfterBake_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Hide Preview After Bake" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PreviewMeshComponent_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RuntimeInputEnums_MetaData[] = {
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bShowingBakedOutput_MetaData[] = {
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UGeometryNodesBridgeComponent constinit property declarations ************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Source;
	static const UECodeGen_Private::FStrPropertyParams NewProp_TargetId;
	static const UECodeGen_Private::FStrPropertyParams NewProp_TargetDisplayLabel;
	static const UECodeGen_Private::FStructPropertyParams NewProp_AvailableTargets_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_AvailableTargets;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SchemaTargetId;
	static const UECodeGen_Private::FStrPropertyParams NewProp_ComponentHandle;
	static const UECodeGen_Private::FBytePropertyParams NewProp_ConnectionState_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_ConnectionState;
	static const UECodeGen_Private::FBytePropertyParams NewProp_ComponentStatus_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_ComponentStatus;
	static const UECodeGen_Private::FStrPropertyParams NewProp_LastStatusMessage;
	static const UECodeGen_Private::FStrPropertyParams NewProp_BridgeBuildVersion;
	static const UECodeGen_Private::FStrPropertyParams NewProp_ObservedGeometryDebugSummary;
	static const UECodeGen_Private::FStrPropertyParams NewProp_LastObservedGeometryEvent;
	static const UECodeGen_Private::FStructPropertyParams NewProp_InputSchema_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_InputSchema;
	static const UECodeGen_Private::FStructPropertyParams NewProp_GeometryInputDescriptors_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_GeometryInputDescriptors;
	static const UECodeGen_Private::FStructPropertyParams NewProp_GeometryBindings_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_GeometryBindings;
	static const UECodeGen_Private::FStructPropertyParams NewProp_TargetCapabilities;
	static const UECodeGen_Private::FStructPropertyParams NewProp_InputValues;
	static const UECodeGen_Private::FStructPropertyParams NewProp_LastCookedMaterialSlots_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_LastCookedMaterialSlots;
	static const UECodeGen_Private::FStructPropertyParams NewProp_MaterialOverrides_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_MaterialOverrides;
	static const UECodeGen_Private::FBytePropertyParams NewProp_BakeOutputMode_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_BakeOutputMode;
	static void NewProp_bShareGeneratedBakeMaterials_SetBit(void* Obj)
	{
		((UGeometryNodesBridgeComponent*)Obj)->bShareGeneratedBakeMaterials = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bShareGeneratedBakeMaterials;
	static const UECodeGen_Private::FSoftObjectPropertyParams NewProp_BakedStaticMesh;
	static const UECodeGen_Private::FSoftObjectPropertyParams NewProp_BakedActor;
	static const UECodeGen_Private::FStructPropertyParams NewProp_BakedPrototypeMeshes_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_BakedPrototypeMeshes;
	static const UECodeGen_Private::FStructPropertyParams NewProp_GeneratedBakeMaterials_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_GeneratedBakeMaterials;
	static void NewProp_bHidePreviewAfterBake_SetBit(void* Obj)
	{
		((UGeometryNodesBridgeComponent*)Obj)->bHidePreviewAfterBake = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bHidePreviewAfterBake;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PreviewMeshComponent;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_RuntimeInputEnums_ValueProp;
	static const UECodeGen_Private::FNamePropertyParams NewProp_RuntimeInputEnums_Key_KeyProp;
	static const UECodeGen_Private::FMapPropertyParams NewProp_RuntimeInputEnums;
	static void NewProp_bShowingBakedOutput_SetBit(void* Obj)
	{
		((UGeometryNodesBridgeComponent*)Obj)->bShowingBakedOutput = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bShowingBakedOutput;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UGeometryNodesBridgeComponent constinit property declarations **************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("BakeToStaticMesh"), .Pointer = &UGeometryNodesBridgeComponent::execBakeToStaticMesh },
		{ .NameUTF8 = UTF8TEXT("DeleteBakedOutput"), .Pointer = &UGeometryNodesBridgeComponent::execDeleteBakedOutput },
		{ .NameUTF8 = UTF8TEXT("GetAvailableTargetOptions"), .Pointer = &UGeometryNodesBridgeComponent::execGetAvailableTargetOptions },
		{ .NameUTF8 = UTF8TEXT("RefreshInputSchema"), .Pointer = &UGeometryNodesBridgeComponent::execRefreshInputSchema },
		{ .NameUTF8 = UTF8TEXT("RequestCook"), .Pointer = &UGeometryNodesBridgeComponent::execRequestCook },
		{ .NameUTF8 = UTF8TEXT("SelectBakedOutput"), .Pointer = &UGeometryNodesBridgeComponent::execSelectBakedOutput },
	};
	static FTypeConstructFunc* DependentSingletons[];
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGeometryNodesBridgeComponent_BakeToStaticMesh, "BakeToStaticMesh" }, // 4e13875cace74eda8b9b884f30a93aedcc624181
		{ &Z_Construct_UFunction_UGeometryNodesBridgeComponent_DeleteBakedOutput, "DeleteBakedOutput" }, // 29d1a11ab4f3b0502b4437cd137c21fc92ba9694
		{ &Z_Construct_UFunction_UGeometryNodesBridgeComponent_GetAvailableTargetOptions, "GetAvailableTargetOptions" }, // bcbc9a5c2e9ad164ad6b71506deab6146472f0de
		{ &Z_Construct_UFunction_UGeometryNodesBridgeComponent_RefreshInputSchema, "RefreshInputSchema" }, // 4ea5222fddb5c7940bc28bfd09e1fe3c988287d4
		{ &Z_Construct_UFunction_UGeometryNodesBridgeComponent_RequestCook, "RequestCook" }, // de7e2925a3841ca3a1d97e6c9b6b991e349adaa0
		{ &Z_Construct_UFunction_UGeometryNodesBridgeComponent_SelectBakedOutput, "SelectBakedOutput" }, // acdcd74f37120bc86a962dae6cd1c5b0370827bc
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGeometryNodesBridgeComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct UHT_STATICS

// ********** Begin Class UGeometryNodesBridgeComponent Property Definitions ***********************
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_Source = { "Source", nullptr, (EPropertyFlags)0x0114000000000015, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, Source), Z_Construct_UClass_UGeometryNodesBridgeSource, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Source_MetaData), NewProp_Source_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_TargetId = { "TargetId", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, TargetId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetId_MetaData), NewProp_TargetId_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_TargetDisplayLabel = { "TargetDisplayLabel", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, TargetDisplayLabel), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetDisplayLabel_MetaData), NewProp_TargetDisplayLabel_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_AvailableTargets_Inner = { "AvailableTargets", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FBridgeTargetDescriptor, METADATA_PARAMS(0, nullptr) }; // bebedd6195fb11ce7426180600ca7e92dd1f2bbe
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_AvailableTargets = { "AvailableTargets", nullptr, (EPropertyFlags)0x0010000000022015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, AvailableTargets), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AvailableTargets_MetaData), NewProp_AvailableTargets_MetaData) }; // bebedd6195fb11ce7426180600ca7e92dd1f2bbe
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SchemaTargetId = { "SchemaTargetId", nullptr, (EPropertyFlags)0x0010000000022015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, SchemaTargetId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SchemaTargetId_MetaData), NewProp_SchemaTargetId_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_ComponentHandle = { "ComponentHandle", nullptr, (EPropertyFlags)0x0010000000022015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, ComponentHandle), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ComponentHandle_MetaData), NewProp_ComponentHandle_MetaData) };
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_ConnectionState_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_ConnectionState = { "ConnectionState", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, ConnectionState), Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeConnectionState, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ConnectionState_MetaData), NewProp_ConnectionState_MetaData) }; // 72cc674dbd30ca998a3f033d2a728eb3ff42bd1e
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_ComponentStatus_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_ComponentStatus = { "ComponentStatus", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, ComponentStatus), Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeComponentStatus, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ComponentStatus_MetaData), NewProp_ComponentStatus_MetaData) }; // eb1ab7850637b762a7f9c42cb93f5a2994ca8fa5
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_LastStatusMessage = { "LastStatusMessage", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, LastStatusMessage), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LastStatusMessage_MetaData), NewProp_LastStatusMessage_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_BridgeBuildVersion = { "BridgeBuildVersion", nullptr, (EPropertyFlags)0x0010000000022015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, BridgeBuildVersion), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BridgeBuildVersion_MetaData), NewProp_BridgeBuildVersion_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_ObservedGeometryDebugSummary = { "ObservedGeometryDebugSummary", nullptr, (EPropertyFlags)0x0010000000022015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, ObservedGeometryDebugSummary), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ObservedGeometryDebugSummary_MetaData), NewProp_ObservedGeometryDebugSummary_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_LastObservedGeometryEvent = { "LastObservedGeometryEvent", nullptr, (EPropertyFlags)0x0010000000022015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, LastObservedGeometryEvent), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LastObservedGeometryEvent_MetaData), NewProp_LastObservedGeometryEvent_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_InputSchema_Inner = { "InputSchema", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FBridgeInputSchema, METADATA_PARAMS(0, nullptr) }; // f8552d45f384f24d81cdaa55e13a470a2c299b52
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_InputSchema = { "InputSchema", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, InputSchema), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InputSchema_MetaData), NewProp_InputSchema_MetaData) }; // f8552d45f384f24d81cdaa55e13a470a2c299b52
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_GeometryInputDescriptors_Inner = { "GeometryInputDescriptors", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FBridgeGeometryInputDescriptor, METADATA_PARAMS(0, nullptr) }; // 14cc046cf6c04bcb440715fc89455ef8a3251b5a
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_GeometryInputDescriptors = { "GeometryInputDescriptors", nullptr, (EPropertyFlags)0x0010000000022015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, GeometryInputDescriptors), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GeometryInputDescriptors_MetaData), NewProp_GeometryInputDescriptors_MetaData) }; // 14cc046cf6c04bcb440715fc89455ef8a3251b5a
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_GeometryBindings_Inner = { "GeometryBindings", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGeometryNodesBridgeGeometryBinding, METADATA_PARAMS(0, nullptr) }; // bcadd7b5232d9de4c4e89781fc10c27af324f483
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_GeometryBindings = { "GeometryBindings", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, GeometryBindings), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GeometryBindings_MetaData), NewProp_GeometryBindings_MetaData) }; // bcadd7b5232d9de4c4e89781fc10c27af324f483
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_TargetCapabilities = { "TargetCapabilities", nullptr, (EPropertyFlags)0x0010000000022015, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, TargetCapabilities), Z_Construct_UScriptStruct_FBridgeTargetCapabilities, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetCapabilities_MetaData), NewProp_TargetCapabilities_MetaData) }; // 25d2870a4fbb6d936bd1ae38e2b21b1adbaa67a1
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_InputValues = { "InputValues", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, InputValues), Z_Construct_UScriptStruct_FInstancedPropertyBag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InputValues_MetaData), NewProp_InputValues_MetaData) }; // 1a91353de49cd69029c47d4b12d799703d21bdcc
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_LastCookedMaterialSlots_Inner = { "LastCookedMaterialSlots", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor, METADATA_PARAMS(0, nullptr) }; // ab5da4d2a789fe023dd7dcf9378abcf186730d16
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_LastCookedMaterialSlots = { "LastCookedMaterialSlots", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, LastCookedMaterialSlots), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LastCookedMaterialSlots_MetaData), NewProp_LastCookedMaterialSlots_MetaData) }; // ab5da4d2a789fe023dd7dcf9378abcf186730d16
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_MaterialOverrides_Inner = { "MaterialOverrides", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGeometryNodesBridgeMaterialOverride, METADATA_PARAMS(0, nullptr) }; // fd4df6596479cd916b17187b4835bd7ebc0a0bf2
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_MaterialOverrides = { "MaterialOverrides", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, MaterialOverrides), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaterialOverrides_MetaData), NewProp_MaterialOverrides_MetaData) }; // fd4df6596479cd916b17187b4835bd7ebc0a0bf2
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_BakeOutputMode_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_BakeOutputMode = { "BakeOutputMode", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, BakeOutputMode), Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeBakeOutputMode, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BakeOutputMode_MetaData), NewProp_BakeOutputMode_MetaData) }; // b41ac467cd55d89f83ba30400d85cd8fefcd0164
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bShareGeneratedBakeMaterials = { "bShareGeneratedBakeMaterials", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGeometryNodesBridgeComponent), &UHT_STATICS::NewProp_bShareGeneratedBakeMaterials_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bShareGeneratedBakeMaterials_MetaData), NewProp_bShareGeneratedBakeMaterials_MetaData) };
const UECodeGen_Private::FSoftObjectPropertyParams UHT_STATICS::NewProp_BakedStaticMesh = { "BakedStaticMesh", nullptr, (EPropertyFlags)0x0014000000020015, UECodeGen_Private::EPropertyGenFlags::SoftObject, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, BakedStaticMesh), Z_Construct_UClass_UStaticMesh, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BakedStaticMesh_MetaData), NewProp_BakedStaticMesh_MetaData) };
const UECodeGen_Private::FSoftObjectPropertyParams UHT_STATICS::NewProp_BakedActor = { "BakedActor", nullptr, (EPropertyFlags)0x0014000000020015, UECodeGen_Private::EPropertyGenFlags::SoftObject, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, BakedActor), Z_Construct_UClass_AActor, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BakedActor_MetaData), NewProp_BakedActor_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_BakedPrototypeMeshes_Inner = { "BakedPrototypeMeshes", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGeometryNodesBridgeBakedPrototypeMeshReference, METADATA_PARAMS(0, nullptr) }; // b73a3ac619b0679a0d6fcfc3ff52e48608837e78
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_BakedPrototypeMeshes = { "BakedPrototypeMeshes", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, BakedPrototypeMeshes), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BakedPrototypeMeshes_MetaData), NewProp_BakedPrototypeMeshes_MetaData) }; // b73a3ac619b0679a0d6fcfc3ff52e48608837e78
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_GeneratedBakeMaterials_Inner = { "GeneratedBakeMaterials", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGeometryNodesBridgeGeneratedMaterialReference, METADATA_PARAMS(0, nullptr) }; // 83c89366fa97432ffa83f32eeceda4f125e74221
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_GeneratedBakeMaterials = { "GeneratedBakeMaterials", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, GeneratedBakeMaterials), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GeneratedBakeMaterials_MetaData), NewProp_GeneratedBakeMaterials_MetaData) }; // 83c89366fa97432ffa83f32eeceda4f125e74221
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bHidePreviewAfterBake = { "bHidePreviewAfterBake", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGeometryNodesBridgeComponent), &UHT_STATICS::NewProp_bHidePreviewAfterBake_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bHidePreviewAfterBake_MetaData), NewProp_bHidePreviewAfterBake_MetaData) };
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_PreviewMeshComponent = { "PreviewMeshComponent", nullptr, (EPropertyFlags)0x0144000000082008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, PreviewMeshComponent), Z_Construct_UClass_UProceduralMeshComponent, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PreviewMeshComponent_MetaData), NewProp_PreviewMeshComponent_MetaData) };
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_RuntimeInputEnums_ValueProp = { "RuntimeInputEnums", nullptr, (EPropertyFlags)0x0104000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, nullptr, nullptr, 1, 1, Z_Construct_UClass_UUserDefinedEnum, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FNamePropertyParams UHT_STATICS::NewProp_RuntimeInputEnums_Key_KeyProp = { "RuntimeInputEnums_Key", nullptr, (EPropertyFlags)0x0100000000000000, UECodeGen_Private::EPropertyGenFlags::Name, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FMapPropertyParams UHT_STATICS::NewProp_RuntimeInputEnums = { "RuntimeInputEnums", nullptr, (EPropertyFlags)0x0144000000002000, UECodeGen_Private::EPropertyGenFlags::Map, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeComponent, RuntimeInputEnums), EMapPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RuntimeInputEnums_MetaData), NewProp_RuntimeInputEnums_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bShowingBakedOutput = { "bShowingBakedOutput", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGeometryNodesBridgeComponent), &UHT_STATICS::NewProp_bShowingBakedOutput_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bShowingBakedOutput_MetaData), NewProp_bShowingBakedOutput_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Source,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_TargetId,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_TargetDisplayLabel,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_AvailableTargets_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_AvailableTargets,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SchemaTargetId,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ComponentHandle,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ConnectionState_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ConnectionState,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ComponentStatus_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ComponentStatus,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_LastStatusMessage,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BridgeBuildVersion,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ObservedGeometryDebugSummary,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_LastObservedGeometryEvent,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_InputSchema_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_InputSchema,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GeometryInputDescriptors_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GeometryInputDescriptors,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GeometryBindings_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GeometryBindings,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_TargetCapabilities,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_InputValues,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_LastCookedMaterialSlots_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_LastCookedMaterialSlots,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_MaterialOverrides_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_MaterialOverrides,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BakeOutputMode_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BakeOutputMode,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bShareGeneratedBakeMaterials,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BakedStaticMesh,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BakedActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BakedPrototypeMeshes_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BakedPrototypeMeshes,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GeneratedBakeMaterials_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GeneratedBakeMaterials,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bHidePreviewAfterBake,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_PreviewMeshComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_RuntimeInputEnums_ValueProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_RuntimeInputEnums_Key_KeyProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_RuntimeInputEnums,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bShowingBakedOutput,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Class UGeometryNodesBridgeComponent Property Definitions *************************
FTypeConstructFunc* UHT_STATICS::DependentSingletons[] = {
	(FTypeConstructFunc*)Z_Construct_UClass_USceneComponent,
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams UHT_STATICS::ClassParams = {
	&Z_Construct_UClass_UGeometryNodesBridgeComponent,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	UHT_STATICS::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	0,
	0x00B000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static void UGeometryNodesBridgeComponent_StaticRegisterNativesUGeometryNodesBridgeComponent()
{
	UClass* Class = UGeometryNodesBridgeComponent::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, 		MakeConstArrayView(UHT_STATICS::Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_UGeometryNodesBridgeComponent;
UClass* Z_Construct_UClass_UGeometryNodesBridgeComponent(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Inner)
	{
		using TClass = UGeometryNodesBridgeComponent;
		if (!Z_Registration_Info_UClass_UGeometryNodesBridgeComponent.InnerSingleton)
		{
			GetPrivateStaticClassBody(
				TClass::StaticPackage(),
				TEXT("GeometryNodesBridgeComponent"),
				Z_Registration_Info_UClass_UGeometryNodesBridgeComponent.InnerSingleton,
				UGeometryNodesBridgeComponent_StaticRegisterNativesUGeometryNodesBridgeComponent,
				DataSizeOf<TClass>(),
				alignof(TClass),
				TClass::StaticClassFlags,
				TClass::StaticClassCastFlags(),
				TClass::StaticConfigName(),
				(UClass::ClassConstructorType)InternalConstructor<TClass>,
				(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
				UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
				&TClass::Super::StaticClass,
				&TClass::WithinClass::StaticClass
			);
		}
		return Z_Registration_Info_UClass_UGeometryNodesBridgeComponent.InnerSingleton;
	}
	if (!Z_Registration_Info_UClass_UGeometryNodesBridgeComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGeometryNodesBridgeComponent.OuterSingleton, UHT_STATICS::ClassParams);
	}
	return Z_Registration_Info_UClass_UGeometryNodesBridgeComponent.OuterSingleton;
}
#undef UHT_STATICS
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UGeometryNodesBridgeComponent);
UGeometryNodesBridgeComponent::~UGeometryNodesBridgeComponent() {}
// ********** End Class UGeometryNodesBridgeComponent **********************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeComponent_h__Script_GeometryNodesBridgeRuntime_Statics
struct UHT_STATICS
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGeometryNodesBridgeComponent, TEXT("UGeometryNodesBridgeComponent"), &Z_Registration_Info_UClass_UGeometryNodesBridgeComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGeometryNodesBridgeComponent), 4034753126U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeComponent_h__Script_GeometryNodesBridgeRuntime_e6f8e45a368e5af495366540bd72a3911a5cbab1{
	TEXT("/Script/GeometryNodesBridgeRuntime"),
	UHT_STATICS::ClassInfo, UE_ARRAY_COUNT(UHT_STATICS::ClassInfo),
	nullptr, 0,
	nullptr, 0,
	nullptr, 0,
};
#undef UHT_STATICS
// ********** End Registration *********************************************************************
#undef UHT_STRUCT_BASE

PRAGMA_ENABLE_DEPRECATION_WARNINGS
