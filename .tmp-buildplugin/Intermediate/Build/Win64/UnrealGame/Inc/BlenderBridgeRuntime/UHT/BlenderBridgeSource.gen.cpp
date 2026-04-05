// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BlenderBridgeSource.h"
#include "UObject/SoftObjectPath.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeBlenderBridgeSource() {}

// ********** Begin Cross Module References ********************************************************
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_UBlenderBridgeSource();
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_UBlenderBridgeSource_NoRegister();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FFilePath();
ENGINE_API UClass* Z_Construct_UClass_UAssetImportData_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UDataAsset();
UPackage* Z_Construct_UPackage__Script_BlenderBridgeRuntime();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UBlenderBridgeSource *****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UBlenderBridgeSource;
UClass* UBlenderBridgeSource::GetPrivateStaticClass()
{
	using TClass = UBlenderBridgeSource;
	if (!Z_Registration_Info_UClass_UBlenderBridgeSource.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("BlenderBridgeSource"),
			Z_Registration_Info_UClass_UBlenderBridgeSource.InnerSingleton,
			StaticRegisterNativesUBlenderBridgeSource,
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
	return Z_Registration_Info_UClass_UBlenderBridgeSource.InnerSingleton;
}
UClass* Z_Construct_UClass_UBlenderBridgeSource_NoRegister()
{
	return UBlenderBridgeSource::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UBlenderBridgeSource_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "BlenderBridgeSource.h" },
		{ "ModuleRelativePath", "Public/BlenderBridgeSource.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlendFile_MetaData[] = {
		{ "Category", "Blender" },
		{ "FilePathFilter", "blend" },
		{ "ModuleRelativePath", "Public/BlenderBridgeSource.h" },
	};
#if WITH_EDITORONLY_DATA
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AssetImportData_MetaData[] = {
		{ "Category", "Import Settings" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/BlenderBridgeSource.h" },
	};
#endif // WITH_EDITORONLY_DATA
#endif // WITH_METADATA

// ********** Begin Class UBlenderBridgeSource constinit property declarations *********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_BlendFile;
#if WITH_EDITORONLY_DATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AssetImportData;
#endif // WITH_EDITORONLY_DATA
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UBlenderBridgeSource constinit property declarations ***********************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBlenderBridgeSource>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UBlenderBridgeSource_Statics

// ********** Begin Class UBlenderBridgeSource Property Definitions ********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBlenderBridgeSource_Statics::NewProp_BlendFile = { "BlendFile", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeSource, BlendFile), Z_Construct_UScriptStruct_FFilePath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlendFile_MetaData), NewProp_BlendFile_MetaData) }; // 1592925316
#if WITH_EDITORONLY_DATA
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UBlenderBridgeSource_Statics::NewProp_AssetImportData = { "AssetImportData", nullptr, (EPropertyFlags)0x01160008000a0009, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeSource, AssetImportData), Z_Construct_UClass_UAssetImportData_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AssetImportData_MetaData), NewProp_AssetImportData_MetaData) };
#endif // WITH_EDITORONLY_DATA
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UBlenderBridgeSource_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeSource_Statics::NewProp_BlendFile,
#if WITH_EDITORONLY_DATA
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeSource_Statics::NewProp_AssetImportData,
#endif // WITH_EDITORONLY_DATA
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSource_Statics::PropPointers) < 2048);
// ********** End Class UBlenderBridgeSource Property Definitions **********************************
UObject* (*const Z_Construct_UClass_UBlenderBridgeSource_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UDataAsset,
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSource_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UBlenderBridgeSource_Statics::ClassParams = {
	&UBlenderBridgeSource::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UBlenderBridgeSource_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSource_Statics::PropPointers),
	0,
	0x009000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSource_Statics::Class_MetaDataParams), Z_Construct_UClass_UBlenderBridgeSource_Statics::Class_MetaDataParams)
};
void UBlenderBridgeSource::StaticRegisterNativesUBlenderBridgeSource()
{
}
UClass* Z_Construct_UClass_UBlenderBridgeSource()
{
	if (!Z_Registration_Info_UClass_UBlenderBridgeSource.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UBlenderBridgeSource.OuterSingleton, Z_Construct_UClass_UBlenderBridgeSource_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UBlenderBridgeSource.OuterSingleton;
}
UBlenderBridgeSource::UBlenderBridgeSource(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UBlenderBridgeSource);
UBlenderBridgeSource::~UBlenderBridgeSource() {}
// ********** End Class UBlenderBridgeSource *******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSource_h__Script_BlenderBridgeRuntime_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UBlenderBridgeSource, UBlenderBridgeSource::StaticClass, TEXT("UBlenderBridgeSource"), &Z_Registration_Info_UClass_UBlenderBridgeSource, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UBlenderBridgeSource), 2701054478U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSource_h__Script_BlenderBridgeRuntime_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSource_h__Script_BlenderBridgeRuntime_2787802212{
	TEXT("/Script/BlenderBridgeRuntime"),
	Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSource_h__Script_BlenderBridgeRuntime_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSource_h__Script_BlenderBridgeRuntime_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
