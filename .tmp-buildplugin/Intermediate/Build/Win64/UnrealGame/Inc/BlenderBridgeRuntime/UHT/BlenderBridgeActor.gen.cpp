// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BlenderBridgeActor.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeBlenderBridgeActor() {}

// ********** Begin Cross Module References ********************************************************
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_ABlenderBridgeActor();
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_ABlenderBridgeActor_NoRegister();
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_UBlenderBridgeComponent_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor();
UPackage* Z_Construct_UPackage__Script_BlenderBridgeRuntime();
// ********** End Cross Module References **********************************************************

// ********** Begin Class ABlenderBridgeActor Function BakeToStaticMesh ****************************
struct Z_Construct_UFunction_ABlenderBridgeActor_BakeToStaticMesh_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Blender Bridge|Bake" },
		{ "DisplayName", "Bake To Static Mesh" },
		{ "ModuleRelativePath", "Public/BlenderBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function BakeToStaticMesh constinit property declarations **********************
// ********** End Function BakeToStaticMesh constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ABlenderBridgeActor_BakeToStaticMesh_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ABlenderBridgeActor, nullptr, "BakeToStaticMesh", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ABlenderBridgeActor_BakeToStaticMesh_Statics::Function_MetaDataParams), Z_Construct_UFunction_ABlenderBridgeActor_BakeToStaticMesh_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_ABlenderBridgeActor_BakeToStaticMesh()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ABlenderBridgeActor_BakeToStaticMesh_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ABlenderBridgeActor::execBakeToStaticMesh)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->BakeToStaticMesh();
	P_NATIVE_END;
}
// ********** End Class ABlenderBridgeActor Function BakeToStaticMesh ******************************

// ********** Begin Class ABlenderBridgeActor Function DeleteBakedOutput ***************************
struct Z_Construct_UFunction_ABlenderBridgeActor_DeleteBakedOutput_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Blender Bridge|Bake" },
		{ "DisplayName", "Delete Baked Output" },
		{ "ModuleRelativePath", "Public/BlenderBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function DeleteBakedOutput constinit property declarations *********************
// ********** End Function DeleteBakedOutput constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ABlenderBridgeActor_DeleteBakedOutput_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ABlenderBridgeActor, nullptr, "DeleteBakedOutput", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ABlenderBridgeActor_DeleteBakedOutput_Statics::Function_MetaDataParams), Z_Construct_UFunction_ABlenderBridgeActor_DeleteBakedOutput_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_ABlenderBridgeActor_DeleteBakedOutput()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ABlenderBridgeActor_DeleteBakedOutput_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ABlenderBridgeActor::execDeleteBakedOutput)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->DeleteBakedOutput();
	P_NATIVE_END;
}
// ********** End Class ABlenderBridgeActor Function DeleteBakedOutput *****************************

// ********** Begin Class ABlenderBridgeActor Function GetBridgeComponent **************************
struct Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics
{
	struct BlenderBridgeActor_eventGetBridgeComponent_Parms
	{
		UBlenderBridgeComponent* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeActor.h" },
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
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(BlenderBridgeActor_eventGetBridgeComponent_Parms, ReturnValue), Z_Construct_UClass_UBlenderBridgeComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ReturnValue_MetaData), NewProp_ReturnValue_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::PropPointers) < 2048);
// ********** End Function GetBridgeComponent Property Definitions *********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ABlenderBridgeActor, nullptr, "GetBridgeComponent", 	Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::BlenderBridgeActor_eventGetBridgeComponent_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::Function_MetaDataParams), Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::BlenderBridgeActor_eventGetBridgeComponent_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ABlenderBridgeActor::execGetBridgeComponent)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UBlenderBridgeComponent**)Z_Param__Result=P_THIS->GetBridgeComponent();
	P_NATIVE_END;
}
// ********** End Class ABlenderBridgeActor Function GetBridgeComponent ****************************

// ********** Begin Class ABlenderBridgeActor Function RefreshInputSchema **************************
struct Z_Construct_UFunction_ABlenderBridgeActor_RefreshInputSchema_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function RefreshInputSchema constinit property declarations ********************
// ********** End Function RefreshInputSchema constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ABlenderBridgeActor_RefreshInputSchema_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ABlenderBridgeActor, nullptr, "RefreshInputSchema", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ABlenderBridgeActor_RefreshInputSchema_Statics::Function_MetaDataParams), Z_Construct_UFunction_ABlenderBridgeActor_RefreshInputSchema_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_ABlenderBridgeActor_RefreshInputSchema()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ABlenderBridgeActor_RefreshInputSchema_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ABlenderBridgeActor::execRefreshInputSchema)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->RefreshInputSchema();
	P_NATIVE_END;
}
// ********** End Class ABlenderBridgeActor Function RefreshInputSchema ****************************

// ********** Begin Class ABlenderBridgeActor Function RequestCook *********************************
struct Z_Construct_UFunction_ABlenderBridgeActor_RequestCook_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Blender Bridge" },
		{ "DisplayName", "Request Cook" },
		{ "ModuleRelativePath", "Public/BlenderBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function RequestCook constinit property declarations ***************************
// ********** End Function RequestCook constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ABlenderBridgeActor_RequestCook_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ABlenderBridgeActor, nullptr, "RequestCook", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ABlenderBridgeActor_RequestCook_Statics::Function_MetaDataParams), Z_Construct_UFunction_ABlenderBridgeActor_RequestCook_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_ABlenderBridgeActor_RequestCook()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ABlenderBridgeActor_RequestCook_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ABlenderBridgeActor::execRequestCook)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->RequestCook();
	P_NATIVE_END;
}
// ********** End Class ABlenderBridgeActor Function RequestCook ***********************************

// ********** Begin Class ABlenderBridgeActor Function SelectBakedOutput ***************************
struct Z_Construct_UFunction_ABlenderBridgeActor_SelectBakedOutput_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Blender Bridge|Bake" },
		{ "DisplayName", "Select Baked Output" },
		{ "ModuleRelativePath", "Public/BlenderBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function SelectBakedOutput constinit property declarations *********************
// ********** End Function SelectBakedOutput constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ABlenderBridgeActor_SelectBakedOutput_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ABlenderBridgeActor, nullptr, "SelectBakedOutput", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ABlenderBridgeActor_SelectBakedOutput_Statics::Function_MetaDataParams), Z_Construct_UFunction_ABlenderBridgeActor_SelectBakedOutput_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_ABlenderBridgeActor_SelectBakedOutput()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ABlenderBridgeActor_SelectBakedOutput_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ABlenderBridgeActor::execSelectBakedOutput)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SelectBakedOutput();
	P_NATIVE_END;
}
// ********** End Class ABlenderBridgeActor Function SelectBakedOutput *****************************

// ********** Begin Class ABlenderBridgeActor ******************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_ABlenderBridgeActor;
UClass* ABlenderBridgeActor::GetPrivateStaticClass()
{
	using TClass = ABlenderBridgeActor;
	if (!Z_Registration_Info_UClass_ABlenderBridgeActor.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("BlenderBridgeActor"),
			Z_Registration_Info_UClass_ABlenderBridgeActor.InnerSingleton,
			StaticRegisterNativesABlenderBridgeActor,
			sizeof(TClass),
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
	return Z_Registration_Info_UClass_ABlenderBridgeActor.InnerSingleton;
}
UClass* Z_Construct_UClass_ABlenderBridgeActor_NoRegister()
{
	return ABlenderBridgeActor::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ABlenderBridgeActor_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "ClassGroupNames", "Blender" },
		{ "IgnoreCategoryKeywordsInSubclasses", "true" },
		{ "IncludePath", "BlenderBridgeActor.h" },
		{ "IsConversionRoot", "true" },
		{ "ModuleRelativePath", "Public/BlenderBridgeActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BridgeComponent_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "Blender Bridge" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/BlenderBridgeActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class ABlenderBridgeActor constinit property declarations **********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_BridgeComponent;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class ABlenderBridgeActor constinit property declarations ************************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("BakeToStaticMesh"), .Pointer = &ABlenderBridgeActor::execBakeToStaticMesh },
		{ .NameUTF8 = UTF8TEXT("DeleteBakedOutput"), .Pointer = &ABlenderBridgeActor::execDeleteBakedOutput },
		{ .NameUTF8 = UTF8TEXT("GetBridgeComponent"), .Pointer = &ABlenderBridgeActor::execGetBridgeComponent },
		{ .NameUTF8 = UTF8TEXT("RefreshInputSchema"), .Pointer = &ABlenderBridgeActor::execRefreshInputSchema },
		{ .NameUTF8 = UTF8TEXT("RequestCook"), .Pointer = &ABlenderBridgeActor::execRequestCook },
		{ .NameUTF8 = UTF8TEXT("SelectBakedOutput"), .Pointer = &ABlenderBridgeActor::execSelectBakedOutput },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_ABlenderBridgeActor_BakeToStaticMesh, "BakeToStaticMesh" }, // 2791912346
		{ &Z_Construct_UFunction_ABlenderBridgeActor_DeleteBakedOutput, "DeleteBakedOutput" }, // 3877535728
		{ &Z_Construct_UFunction_ABlenderBridgeActor_GetBridgeComponent, "GetBridgeComponent" }, // 3176303624
		{ &Z_Construct_UFunction_ABlenderBridgeActor_RefreshInputSchema, "RefreshInputSchema" }, // 2763533673
		{ &Z_Construct_UFunction_ABlenderBridgeActor_RequestCook, "RequestCook" }, // 910574510
		{ &Z_Construct_UFunction_ABlenderBridgeActor_SelectBakedOutput, "SelectBakedOutput" }, // 3790015174
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ABlenderBridgeActor>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_ABlenderBridgeActor_Statics

// ********** Begin Class ABlenderBridgeActor Property Definitions *********************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ABlenderBridgeActor_Statics::NewProp_BridgeComponent = { "BridgeComponent", nullptr, (EPropertyFlags)0x01440000000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ABlenderBridgeActor, BridgeComponent), Z_Construct_UClass_UBlenderBridgeComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BridgeComponent_MetaData), NewProp_BridgeComponent_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ABlenderBridgeActor_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ABlenderBridgeActor_Statics::NewProp_BridgeComponent,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ABlenderBridgeActor_Statics::PropPointers) < 2048);
// ********** End Class ABlenderBridgeActor Property Definitions ***********************************
UObject* (*const Z_Construct_UClass_ABlenderBridgeActor_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ABlenderBridgeActor_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ABlenderBridgeActor_Statics::ClassParams = {
	&ABlenderBridgeActor::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_ABlenderBridgeActor_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_ABlenderBridgeActor_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ABlenderBridgeActor_Statics::Class_MetaDataParams), Z_Construct_UClass_ABlenderBridgeActor_Statics::Class_MetaDataParams)
};
void ABlenderBridgeActor::StaticRegisterNativesABlenderBridgeActor()
{
	UClass* Class = ABlenderBridgeActor::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_ABlenderBridgeActor_Statics::Funcs));
}
UClass* Z_Construct_UClass_ABlenderBridgeActor()
{
	if (!Z_Registration_Info_UClass_ABlenderBridgeActor.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ABlenderBridgeActor.OuterSingleton, Z_Construct_UClass_ABlenderBridgeActor_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ABlenderBridgeActor.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, ABlenderBridgeActor);
ABlenderBridgeActor::~ABlenderBridgeActor() {}
// ********** End Class ABlenderBridgeActor ********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h__Script_BlenderBridgeRuntime_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ABlenderBridgeActor, ABlenderBridgeActor::StaticClass, TEXT("ABlenderBridgeActor"), &Z_Registration_Info_UClass_ABlenderBridgeActor, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ABlenderBridgeActor), 362926804U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h__Script_BlenderBridgeRuntime_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h__Script_BlenderBridgeRuntime_581651800{
	TEXT("/Script/BlenderBridgeRuntime"),
	Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h__Script_BlenderBridgeRuntime_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h__Script_BlenderBridgeRuntime_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
