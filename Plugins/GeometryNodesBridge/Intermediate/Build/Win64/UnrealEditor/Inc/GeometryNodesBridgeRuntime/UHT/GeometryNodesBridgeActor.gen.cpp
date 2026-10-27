// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GeometryNodesBridgeActor.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGeometryNodesBridgeActor() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_AActor(ETypeConstructPhase);
ENGINE_API UClass* Z_Construct_UClass_USplineComponent(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UClass* Z_Construct_UClass_AGeometryNodesBridgeActor(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UClass* Z_Construct_UClass_AGeometryNodesBridgeActor(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UClass* Z_Construct_UClass_UGeometryNodesBridgeComponent(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Class AGeometryNodesBridgeActor Function BakeToStaticMesh **********************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_AGeometryNodesBridgeActor_BakeToStaticMesh_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Bake Output" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function BakeToStaticMesh constinit property declarations **********************
// ********** End Function BakeToStaticMesh constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_AGeometryNodesBridgeActor, nullptr, "BakeToStaticMesh", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_AGeometryNodesBridgeActor_BakeToStaticMesh(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(AGeometryNodesBridgeActor::execBakeToStaticMesh)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->BakeToStaticMesh();
	P_NATIVE_END;
}
// ********** End Class AGeometryNodesBridgeActor Function BakeToStaticMesh ************************

// ********** Begin Class AGeometryNodesBridgeActor Function DeleteBakedOutput *********************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_AGeometryNodesBridgeActor_DeleteBakedOutput_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Delete Baked Output" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function DeleteBakedOutput constinit property declarations *********************
// ********** End Function DeleteBakedOutput constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_AGeometryNodesBridgeActor, nullptr, "DeleteBakedOutput", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_AGeometryNodesBridgeActor_DeleteBakedOutput(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(AGeometryNodesBridgeActor::execDeleteBakedOutput)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->DeleteBakedOutput();
	P_NATIVE_END;
}
// ********** End Class AGeometryNodesBridgeActor Function DeleteBakedOutput ***********************

// ********** Begin Class AGeometryNodesBridgeActor Function GetBridgeComponent ********************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_AGeometryNodesBridgeActor_GetBridgeComponent_Statics
struct UHT_STATICS
{
	struct GeometryNodesBridgeActor_eventGetBridgeComponent_Parms
	{
		UGeometryNodesBridgeComponent* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA

// ********** Begin Function GetBridgeComponent constinit property declarations ********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetBridgeComponent constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetBridgeComponent Property Definitions *******************************
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, nullptr, nullptr, 1, STRUCT_OFFSET(GeometryNodesBridgeActor_eventGetBridgeComponent_Parms, ReturnValue), Z_Construct_UClass_UGeometryNodesBridgeComponent, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ReturnValue_MetaData), NewProp_ReturnValue_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Function GetBridgeComponent Property Definitions *********************************
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_AGeometryNodesBridgeActor, nullptr, "GetBridgeComponent", UHT_STATICS::PropPointers, UE_ARRAY_COUNT(UHT_STATICS::PropPointers), DataSizeOf<UHT_STATICS::GeometryNodesBridgeActor_eventGetBridgeComponent_Parms>(), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
static_assert(sizeof(UHT_STATICS::GeometryNodesBridgeActor_eventGetBridgeComponent_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_AGeometryNodesBridgeActor_GetBridgeComponent(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(AGeometryNodesBridgeActor::execGetBridgeComponent)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UGeometryNodesBridgeComponent**)Z_Param__Result=P_THIS->GetBridgeComponent();
	P_NATIVE_END;
}
// ********** End Class AGeometryNodesBridgeActor Function GetBridgeComponent **********************

// ********** Begin Class AGeometryNodesBridgeActor Function GetBridgeSplineComponent **************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_AGeometryNodesBridgeActor_GetBridgeSplineComponent_Statics
struct UHT_STATICS
{
	struct GeometryNodesBridgeActor_eventGetBridgeSplineComponent_Parms
	{
		USplineComponent* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA

// ********** Begin Function GetBridgeSplineComponent constinit property declarations **************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetBridgeSplineComponent constinit property declarations ****************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetBridgeSplineComponent Property Definitions *************************
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, nullptr, nullptr, 1, STRUCT_OFFSET(GeometryNodesBridgeActor_eventGetBridgeSplineComponent_Parms, ReturnValue), Z_Construct_UClass_USplineComponent, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ReturnValue_MetaData), NewProp_ReturnValue_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Function GetBridgeSplineComponent Property Definitions ***************************
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_AGeometryNodesBridgeActor, nullptr, "GetBridgeSplineComponent", UHT_STATICS::PropPointers, UE_ARRAY_COUNT(UHT_STATICS::PropPointers), DataSizeOf<UHT_STATICS::GeometryNodesBridgeActor_eventGetBridgeSplineComponent_Parms>(), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
static_assert(sizeof(UHT_STATICS::GeometryNodesBridgeActor_eventGetBridgeSplineComponent_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_AGeometryNodesBridgeActor_GetBridgeSplineComponent(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(AGeometryNodesBridgeActor::execGetBridgeSplineComponent)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(USplineComponent**)Z_Param__Result=P_THIS->GetBridgeSplineComponent();
	P_NATIVE_END;
}
// ********** End Class AGeometryNodesBridgeActor Function GetBridgeSplineComponent ****************

// ********** Begin Class AGeometryNodesBridgeActor Function RefreshInputSchema ********************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_AGeometryNodesBridgeActor_RefreshInputSchema_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function RefreshInputSchema constinit property declarations ********************
// ********** End Function RefreshInputSchema constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_AGeometryNodesBridgeActor, nullptr, "RefreshInputSchema", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_AGeometryNodesBridgeActor_RefreshInputSchema(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(AGeometryNodesBridgeActor::execRefreshInputSchema)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->RefreshInputSchema();
	P_NATIVE_END;
}
// ********** End Class AGeometryNodesBridgeActor Function RefreshInputSchema **********************

// ********** Begin Class AGeometryNodesBridgeActor Function RequestCook ***************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_AGeometryNodesBridgeActor_RequestCook_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge" },
		{ "DisplayName", "Request Cook" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function RequestCook constinit property declarations ***************************
// ********** End Function RequestCook constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_AGeometryNodesBridgeActor, nullptr, "RequestCook", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_AGeometryNodesBridgeActor_RequestCook(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(AGeometryNodesBridgeActor::execRequestCook)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->RequestCook();
	P_NATIVE_END;
}
// ********** End Class AGeometryNodesBridgeActor Function RequestCook *****************************

// ********** Begin Class AGeometryNodesBridgeActor Function SelectBakedOutput *********************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_AGeometryNodesBridgeActor_SelectBakedOutput_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Geometry Nodes Bridge|Bake" },
		{ "DisplayName", "Select Baked Output" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function SelectBakedOutput constinit property declarations *********************
// ********** End Function SelectBakedOutput constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_AGeometryNodesBridgeActor, nullptr, "SelectBakedOutput", nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
UFunction* Z_Construct_UFunction_AGeometryNodesBridgeActor_SelectBakedOutput(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(AGeometryNodesBridgeActor::execSelectBakedOutput)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SelectBakedOutput();
	P_NATIVE_END;
}
// ********** End Class AGeometryNodesBridgeActor Function SelectBakedOutput ***********************

// ********** Begin Class AGeometryNodesBridgeActor ************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UClass_AGeometryNodesBridgeActor_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "ClassGroupNames", "Blender" },
		{ "IgnoreCategoryKeywordsInSubclasses", "true" },
		{ "IncludePath", "GeometryNodesBridgeActor.h" },
		{ "IsConversionRoot", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BridgeComponent_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Geometry Nodes Bridge" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BridgeSplineComponent_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Geometry Nodes Bridge" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class AGeometryNodesBridgeActor constinit property declarations ****************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_BridgeComponent;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_BridgeSplineComponent;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class AGeometryNodesBridgeActor constinit property declarations ******************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("BakeToStaticMesh"), .Pointer = &AGeometryNodesBridgeActor::execBakeToStaticMesh },
		{ .NameUTF8 = UTF8TEXT("DeleteBakedOutput"), .Pointer = &AGeometryNodesBridgeActor::execDeleteBakedOutput },
		{ .NameUTF8 = UTF8TEXT("GetBridgeComponent"), .Pointer = &AGeometryNodesBridgeActor::execGetBridgeComponent },
		{ .NameUTF8 = UTF8TEXT("GetBridgeSplineComponent"), .Pointer = &AGeometryNodesBridgeActor::execGetBridgeSplineComponent },
		{ .NameUTF8 = UTF8TEXT("RefreshInputSchema"), .Pointer = &AGeometryNodesBridgeActor::execRefreshInputSchema },
		{ .NameUTF8 = UTF8TEXT("RequestCook"), .Pointer = &AGeometryNodesBridgeActor::execRequestCook },
		{ .NameUTF8 = UTF8TEXT("SelectBakedOutput"), .Pointer = &AGeometryNodesBridgeActor::execSelectBakedOutput },
	};
	static FTypeConstructFunc* DependentSingletons[];
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_AGeometryNodesBridgeActor_BakeToStaticMesh, "BakeToStaticMesh" }, // fe6415c22f8c09a5c79fdc0d3d9d7868210a1711
		{ &Z_Construct_UFunction_AGeometryNodesBridgeActor_DeleteBakedOutput, "DeleteBakedOutput" }, // 0abc2fed188d74f8380747b82b8493d95e18f798
		{ &Z_Construct_UFunction_AGeometryNodesBridgeActor_GetBridgeComponent, "GetBridgeComponent" }, // 02c25f4096d9d1ed77bdb797d3821ffe9c1c67ff
		{ &Z_Construct_UFunction_AGeometryNodesBridgeActor_GetBridgeSplineComponent, "GetBridgeSplineComponent" }, // f76f2a55b8e6d0a47abad68413f56c3e20ecc2e4
		{ &Z_Construct_UFunction_AGeometryNodesBridgeActor_RefreshInputSchema, "RefreshInputSchema" }, // cd22b7b3e5f5a3f5d71b6b28816c0b6ba5acfd90
		{ &Z_Construct_UFunction_AGeometryNodesBridgeActor_RequestCook, "RequestCook" }, // 9bd5919396de3d321007c98db533791eabdffa9a
		{ &Z_Construct_UFunction_AGeometryNodesBridgeActor_SelectBakedOutput, "SelectBakedOutput" }, // 719192ec74caa7b545a3573f69441673a666b306
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGeometryNodesBridgeActor>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct UHT_STATICS

// ********** Begin Class AGeometryNodesBridgeActor Property Definitions ***************************
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_BridgeComponent = { "BridgeComponent", nullptr, (EPropertyFlags)0x01440000000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, nullptr, nullptr, 1, STRUCT_OFFSET(AGeometryNodesBridgeActor, BridgeComponent), Z_Construct_UClass_UGeometryNodesBridgeComponent, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BridgeComponent_MetaData), NewProp_BridgeComponent_MetaData) };
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_BridgeSplineComponent = { "BridgeSplineComponent", nullptr, (EPropertyFlags)0x01440000000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, nullptr, nullptr, 1, STRUCT_OFFSET(AGeometryNodesBridgeActor, BridgeSplineComponent), Z_Construct_UClass_USplineComponent, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BridgeSplineComponent_MetaData), NewProp_BridgeSplineComponent_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BridgeComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BridgeSplineComponent,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Class AGeometryNodesBridgeActor Property Definitions *****************************
FTypeConstructFunc* UHT_STATICS::DependentSingletons[] = {
	(FTypeConstructFunc*)Z_Construct_UClass_AActor,
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams UHT_STATICS::ClassParams = {
	&Z_Construct_UClass_AGeometryNodesBridgeActor,
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
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static void AGeometryNodesBridgeActor_StaticRegisterNativesAGeometryNodesBridgeActor()
{
	UClass* Class = AGeometryNodesBridgeActor::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, 		MakeConstArrayView(UHT_STATICS::Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_AGeometryNodesBridgeActor;
UClass* Z_Construct_UClass_AGeometryNodesBridgeActor(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Inner)
	{
		using TClass = AGeometryNodesBridgeActor;
		if (!Z_Registration_Info_UClass_AGeometryNodesBridgeActor.InnerSingleton)
		{
			GetPrivateStaticClassBody(
				TClass::StaticPackage(),
				TEXT("GeometryNodesBridgeActor"),
				Z_Registration_Info_UClass_AGeometryNodesBridgeActor.InnerSingleton,
				AGeometryNodesBridgeActor_StaticRegisterNativesAGeometryNodesBridgeActor,
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
		return Z_Registration_Info_UClass_AGeometryNodesBridgeActor.InnerSingleton;
	}
	if (!Z_Registration_Info_UClass_AGeometryNodesBridgeActor.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AGeometryNodesBridgeActor.OuterSingleton, UHT_STATICS::ClassParams);
	}
	return Z_Registration_Info_UClass_AGeometryNodesBridgeActor.OuterSingleton;
}
#undef UHT_STATICS
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, AGeometryNodesBridgeActor);
AGeometryNodesBridgeActor::~AGeometryNodesBridgeActor() {}
// ********** End Class AGeometryNodesBridgeActor **************************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h__Script_GeometryNodesBridgeRuntime_Statics
struct UHT_STATICS
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AGeometryNodesBridgeActor, TEXT("AGeometryNodesBridgeActor"), &Z_Registration_Info_UClass_AGeometryNodesBridgeActor, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AGeometryNodesBridgeActor), 2729859222U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h__Script_GeometryNodesBridgeRuntime_88fc9c27e6ac3a223ab9f3db446555695bcbf18c{
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
