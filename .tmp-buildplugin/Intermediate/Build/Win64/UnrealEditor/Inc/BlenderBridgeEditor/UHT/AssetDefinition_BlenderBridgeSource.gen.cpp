// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AssetDefinition_BlenderBridgeSource.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeAssetDefinition_BlenderBridgeSource() {}

// ********** Begin Cross Module References ********************************************************
BLENDERBRIDGEEDITOR_API UClass* Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource();
BLENDERBRIDGEEDITOR_API UClass* Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_NoRegister();
UNREALED_API UClass* Z_Construct_UClass_UAssetDefinitionDefault();
UPackage* Z_Construct_UPackage__Script_BlenderBridgeEditor();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UAssetDefinition_BlenderBridgeSource *************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UAssetDefinition_BlenderBridgeSource;
UClass* UAssetDefinition_BlenderBridgeSource::GetPrivateStaticClass()
{
	using TClass = UAssetDefinition_BlenderBridgeSource;
	if (!Z_Registration_Info_UClass_UAssetDefinition_BlenderBridgeSource.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("AssetDefinition_BlenderBridgeSource"),
			Z_Registration_Info_UClass_UAssetDefinition_BlenderBridgeSource.InnerSingleton,
			StaticRegisterNativesUAssetDefinition_BlenderBridgeSource,
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
	return Z_Registration_Info_UClass_UAssetDefinition_BlenderBridgeSource.InnerSingleton;
}
UClass* Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_NoRegister()
{
	return UAssetDefinition_BlenderBridgeSource::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "AssetDefinition_BlenderBridgeSource.h" },
		{ "ModuleRelativePath", "Private/AssetDefinition_BlenderBridgeSource.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UAssetDefinition_BlenderBridgeSource constinit property declarations *****
// ********** End Class UAssetDefinition_BlenderBridgeSource constinit property declarations *******
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAssetDefinition_BlenderBridgeSource>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_Statics
UObject* (*const Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAssetDefinitionDefault,
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeEditor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_Statics::ClassParams = {
	&UAssetDefinition_BlenderBridgeSource::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x000000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_Statics::Class_MetaDataParams), Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_Statics::Class_MetaDataParams)
};
void UAssetDefinition_BlenderBridgeSource::StaticRegisterNativesUAssetDefinition_BlenderBridgeSource()
{
}
UClass* Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource()
{
	if (!Z_Registration_Info_UClass_UAssetDefinition_BlenderBridgeSource.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAssetDefinition_BlenderBridgeSource.OuterSingleton, Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAssetDefinition_BlenderBridgeSource.OuterSingleton;
}
UAssetDefinition_BlenderBridgeSource::UAssetDefinition_BlenderBridgeSource() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UAssetDefinition_BlenderBridgeSource);
UAssetDefinition_BlenderBridgeSource::~UAssetDefinition_BlenderBridgeSource() {}
// ********** End Class UAssetDefinition_BlenderBridgeSource ***************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Private_AssetDefinition_BlenderBridgeSource_h__Script_BlenderBridgeEditor_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAssetDefinition_BlenderBridgeSource, UAssetDefinition_BlenderBridgeSource::StaticClass, TEXT("UAssetDefinition_BlenderBridgeSource"), &Z_Registration_Info_UClass_UAssetDefinition_BlenderBridgeSource, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAssetDefinition_BlenderBridgeSource), 557308981U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Private_AssetDefinition_BlenderBridgeSource_h__Script_BlenderBridgeEditor_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Private_AssetDefinition_BlenderBridgeSource_h__Script_BlenderBridgeEditor_3116304419{
	TEXT("/Script/BlenderBridgeEditor"),
	Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Private_AssetDefinition_BlenderBridgeSource_h__Script_BlenderBridgeEditor_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Private_AssetDefinition_BlenderBridgeSource_h__Script_BlenderBridgeEditor_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
