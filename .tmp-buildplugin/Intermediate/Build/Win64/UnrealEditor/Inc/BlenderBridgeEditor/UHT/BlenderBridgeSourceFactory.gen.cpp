// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BlenderBridgeSourceFactory.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeBlenderBridgeSourceFactory() {}

// ********** Begin Cross Module References ********************************************************
BLENDERBRIDGEEDITOR_API UClass* Z_Construct_UClass_UBlenderBridgeSourceFactory();
BLENDERBRIDGEEDITOR_API UClass* Z_Construct_UClass_UBlenderBridgeSourceFactory_NoRegister();
UNREALED_API UClass* Z_Construct_UClass_UFactory();
UPackage* Z_Construct_UPackage__Script_BlenderBridgeEditor();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UBlenderBridgeSourceFactory **********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UBlenderBridgeSourceFactory;
UClass* UBlenderBridgeSourceFactory::GetPrivateStaticClass()
{
	using TClass = UBlenderBridgeSourceFactory;
	if (!Z_Registration_Info_UClass_UBlenderBridgeSourceFactory.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("BlenderBridgeSourceFactory"),
			Z_Registration_Info_UClass_UBlenderBridgeSourceFactory.InnerSingleton,
			StaticRegisterNativesUBlenderBridgeSourceFactory,
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
	return Z_Registration_Info_UClass_UBlenderBridgeSourceFactory.InnerSingleton;
}
UClass* Z_Construct_UClass_UBlenderBridgeSourceFactory_NoRegister()
{
	return UBlenderBridgeSourceFactory::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UBlenderBridgeSourceFactory_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Object" },
		{ "IncludePath", "BlenderBridgeSourceFactory.h" },
		{ "ModuleRelativePath", "Public/BlenderBridgeSourceFactory.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif // WITH_METADATA

// ********** Begin Class UBlenderBridgeSourceFactory constinit property declarations **************
// ********** End Class UBlenderBridgeSourceFactory constinit property declarations ****************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBlenderBridgeSourceFactory>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UBlenderBridgeSourceFactory_Statics
UObject* (*const Z_Construct_UClass_UBlenderBridgeSourceFactory_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UFactory,
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeEditor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSourceFactory_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UBlenderBridgeSourceFactory_Statics::ClassParams = {
	&UBlenderBridgeSourceFactory::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSourceFactory_Statics::Class_MetaDataParams), Z_Construct_UClass_UBlenderBridgeSourceFactory_Statics::Class_MetaDataParams)
};
void UBlenderBridgeSourceFactory::StaticRegisterNativesUBlenderBridgeSourceFactory()
{
}
UClass* Z_Construct_UClass_UBlenderBridgeSourceFactory()
{
	if (!Z_Registration_Info_UClass_UBlenderBridgeSourceFactory.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UBlenderBridgeSourceFactory.OuterSingleton, Z_Construct_UClass_UBlenderBridgeSourceFactory_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UBlenderBridgeSourceFactory.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UBlenderBridgeSourceFactory);
UBlenderBridgeSourceFactory::~UBlenderBridgeSourceFactory() {}
// ********** End Class UBlenderBridgeSourceFactory ************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_BlenderBridgeSourceFactory_h__Script_BlenderBridgeEditor_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UBlenderBridgeSourceFactory, UBlenderBridgeSourceFactory::StaticClass, TEXT("UBlenderBridgeSourceFactory"), &Z_Registration_Info_UClass_UBlenderBridgeSourceFactory, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UBlenderBridgeSourceFactory), 2666120233U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_BlenderBridgeSourceFactory_h__Script_BlenderBridgeEditor_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_BlenderBridgeSourceFactory_h__Script_BlenderBridgeEditor_1663698810{
	TEXT("/Script/BlenderBridgeEditor"),
	Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_BlenderBridgeSourceFactory_h__Script_BlenderBridgeEditor_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_BlenderBridgeSourceFactory_h__Script_BlenderBridgeEditor_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
