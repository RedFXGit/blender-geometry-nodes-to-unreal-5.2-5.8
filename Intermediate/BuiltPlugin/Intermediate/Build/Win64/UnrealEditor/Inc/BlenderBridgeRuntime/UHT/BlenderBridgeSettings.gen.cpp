// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BlenderBridgeSettings.h"
#include "UObject/SoftObjectPath.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeBlenderBridgeSettings() {}

// ********** Begin Cross Module References ********************************************************
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_UBlenderBridgeSettings();
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_UBlenderBridgeSettings_NoRegister();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FFilePath();
DEVELOPERSETTINGS_API UClass* Z_Construct_UClass_UDeveloperSettings();
UPackage* Z_Construct_UPackage__Script_BlenderBridgeRuntime();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UBlenderBridgeSettings ***************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UBlenderBridgeSettings;
UClass* UBlenderBridgeSettings::GetPrivateStaticClass()
{
	using TClass = UBlenderBridgeSettings;
	if (!Z_Registration_Info_UClass_UBlenderBridgeSettings.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("BlenderBridgeSettings"),
			Z_Registration_Info_UClass_UBlenderBridgeSettings.InnerSingleton,
			StaticRegisterNativesUBlenderBridgeSettings,
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
	return Z_Registration_Info_UClass_UBlenderBridgeSettings.InnerSingleton;
}
UClass* Z_Construct_UClass_UBlenderBridgeSettings_NoRegister()
{
	return UBlenderBridgeSettings::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UBlenderBridgeSettings_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "DisplayName", "Blender Bridge" },
		{ "IncludePath", "BlenderBridgeSettings.h" },
		{ "ModuleRelativePath", "Public/BlenderBridgeSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlenderExecutablePath_MetaData[] = {
		{ "Category", "Session" },
		{ "ModuleRelativePath", "Public/BlenderBridgeSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SharedMemoryBytes_MetaData[] = {
		{ "Category", "Session" },
		{ "ClampMin", "1048576" },
		{ "DisplayName", "Shared Memory Bytes (Total)" },
		{ "ModuleRelativePath", "Public/BlenderBridgeSettings.h" },
		{ "ToolTip", "Total shared-memory budget across both bridge slots. The default is 250 MiB per slot, 500 MiB total." },
	};
#endif // WITH_METADATA

// ********** Begin Class UBlenderBridgeSettings constinit property declarations *******************
	static const UECodeGen_Private::FStructPropertyParams NewProp_BlenderExecutablePath;
	static const UECodeGen_Private::FInt64PropertyParams NewProp_SharedMemoryBytes;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UBlenderBridgeSettings constinit property declarations *********************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBlenderBridgeSettings>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UBlenderBridgeSettings_Statics

// ********** Begin Class UBlenderBridgeSettings Property Definitions ******************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBlenderBridgeSettings_Statics::NewProp_BlenderExecutablePath = { "BlenderExecutablePath", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeSettings, BlenderExecutablePath), Z_Construct_UScriptStruct_FFilePath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlenderExecutablePath_MetaData), NewProp_BlenderExecutablePath_MetaData) }; // 1592925316
const UECodeGen_Private::FInt64PropertyParams Z_Construct_UClass_UBlenderBridgeSettings_Statics::NewProp_SharedMemoryBytes = { "SharedMemoryBytes", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Int64, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeSettings, SharedMemoryBytes), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SharedMemoryBytes_MetaData), NewProp_SharedMemoryBytes_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UBlenderBridgeSettings_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeSettings_Statics::NewProp_BlenderExecutablePath,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeSettings_Statics::NewProp_SharedMemoryBytes,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSettings_Statics::PropPointers) < 2048);
// ********** End Class UBlenderBridgeSettings Property Definitions ********************************
UObject* (*const Z_Construct_UClass_UBlenderBridgeSettings_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UDeveloperSettings,
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSettings_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UBlenderBridgeSettings_Statics::ClassParams = {
	&UBlenderBridgeSettings::StaticClass,
	"EditorPerProjectUserSettings",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UBlenderBridgeSettings_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSettings_Statics::PropPointers),
	0,
	0x001000A6u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UBlenderBridgeSettings_Statics::Class_MetaDataParams)
};
void UBlenderBridgeSettings::StaticRegisterNativesUBlenderBridgeSettings()
{
}
UClass* Z_Construct_UClass_UBlenderBridgeSettings()
{
	if (!Z_Registration_Info_UClass_UBlenderBridgeSettings.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UBlenderBridgeSettings.OuterSingleton, Z_Construct_UClass_UBlenderBridgeSettings_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UBlenderBridgeSettings.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UBlenderBridgeSettings);
UBlenderBridgeSettings::~UBlenderBridgeSettings() {}
// ********** End Class UBlenderBridgeSettings *****************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSettings_h__Script_BlenderBridgeRuntime_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UBlenderBridgeSettings, UBlenderBridgeSettings::StaticClass, TEXT("UBlenderBridgeSettings"), &Z_Registration_Info_UClass_UBlenderBridgeSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UBlenderBridgeSettings), 3326151783U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSettings_h__Script_BlenderBridgeRuntime_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSettings_h__Script_BlenderBridgeRuntime_1589416179{
	TEXT("/Script/BlenderBridgeRuntime"),
	Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSettings_h__Script_BlenderBridgeRuntime_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeSettings_h__Script_BlenderBridgeRuntime_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
