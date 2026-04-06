// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ActorFactoryBlenderBridgeSource.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeActorFactoryBlenderBridgeSource() {}

// ********** Begin Cross Module References ********************************************************
BLENDERBRIDGEEDITOR_API UClass* Z_Construct_UClass_UActorFactoryBlenderBridgeSource();
BLENDERBRIDGEEDITOR_API UClass* Z_Construct_UClass_UActorFactoryBlenderBridgeSource_NoRegister();
UNREALED_API UClass* Z_Construct_UClass_UActorFactory();
UPackage* Z_Construct_UPackage__Script_BlenderBridgeEditor();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UActorFactoryBlenderBridgeSource *****************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UActorFactoryBlenderBridgeSource;
UClass* UActorFactoryBlenderBridgeSource::GetPrivateStaticClass()
{
	using TClass = UActorFactoryBlenderBridgeSource;
	if (!Z_Registration_Info_UClass_UActorFactoryBlenderBridgeSource.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("ActorFactoryBlenderBridgeSource"),
			Z_Registration_Info_UClass_UActorFactoryBlenderBridgeSource.InnerSingleton,
			StaticRegisterNativesUActorFactoryBlenderBridgeSource,
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
	return Z_Registration_Info_UClass_UActorFactoryBlenderBridgeSource.InnerSingleton;
}
UClass* Z_Construct_UClass_UActorFactoryBlenderBridgeSource_NoRegister()
{
	return UActorFactoryBlenderBridgeSource::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UActorFactoryBlenderBridgeSource_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Object" },
		{ "IncludePath", "ActorFactoryBlenderBridgeSource.h" },
		{ "ModuleRelativePath", "Public/ActorFactoryBlenderBridgeSource.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif // WITH_METADATA

// ********** Begin Class UActorFactoryBlenderBridgeSource constinit property declarations *********
// ********** End Class UActorFactoryBlenderBridgeSource constinit property declarations ***********
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UActorFactoryBlenderBridgeSource>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UActorFactoryBlenderBridgeSource_Statics
UObject* (*const Z_Construct_UClass_UActorFactoryBlenderBridgeSource_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UActorFactory,
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeEditor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UActorFactoryBlenderBridgeSource_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UActorFactoryBlenderBridgeSource_Statics::ClassParams = {
	&UActorFactoryBlenderBridgeSource::StaticClass,
	"Editor",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x001030ACu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UActorFactoryBlenderBridgeSource_Statics::Class_MetaDataParams), Z_Construct_UClass_UActorFactoryBlenderBridgeSource_Statics::Class_MetaDataParams)
};
void UActorFactoryBlenderBridgeSource::StaticRegisterNativesUActorFactoryBlenderBridgeSource()
{
}
UClass* Z_Construct_UClass_UActorFactoryBlenderBridgeSource()
{
	if (!Z_Registration_Info_UClass_UActorFactoryBlenderBridgeSource.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UActorFactoryBlenderBridgeSource.OuterSingleton, Z_Construct_UClass_UActorFactoryBlenderBridgeSource_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UActorFactoryBlenderBridgeSource.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UActorFactoryBlenderBridgeSource);
UActorFactoryBlenderBridgeSource::~UActorFactoryBlenderBridgeSource() {}
// ********** End Class UActorFactoryBlenderBridgeSource *******************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_ActorFactoryBlenderBridgeSource_h__Script_BlenderBridgeEditor_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UActorFactoryBlenderBridgeSource, UActorFactoryBlenderBridgeSource::StaticClass, TEXT("UActorFactoryBlenderBridgeSource"), &Z_Registration_Info_UClass_UActorFactoryBlenderBridgeSource, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UActorFactoryBlenderBridgeSource), 1705555002U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_ActorFactoryBlenderBridgeSource_h__Script_BlenderBridgeEditor_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_ActorFactoryBlenderBridgeSource_h__Script_BlenderBridgeEditor_1087909437{
	TEXT("/Script/BlenderBridgeEditor"),
	Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_ActorFactoryBlenderBridgeSource_h__Script_BlenderBridgeEditor_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeEditor_Public_ActorFactoryBlenderBridgeSource_h__Script_BlenderBridgeEditor_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
