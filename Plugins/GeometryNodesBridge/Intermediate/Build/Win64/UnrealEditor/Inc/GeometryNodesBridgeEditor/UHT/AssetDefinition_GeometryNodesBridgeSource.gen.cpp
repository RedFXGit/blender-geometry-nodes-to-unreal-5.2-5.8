// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AssetDefinition_GeometryNodesBridgeSource.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeAssetDefinition_GeometryNodesBridgeSource() {}

// ********** Begin Cross Module References ********************************************************
UNREALED_API UClass* Z_Construct_UClass_UAssetDefinitionDefault(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GeometryNodesBridgeEditor(ETypeConstructPhase);
GEOMETRYNODESBRIDGEEDITOR_API UClass* Z_Construct_UClass_UAssetDefinition_GeometryNodesBridgeSource(ETypeConstructPhase);
GEOMETRYNODESBRIDGEEDITOR_API UClass* Z_Construct_UClass_UAssetDefinition_GeometryNodesBridgeSource(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Class UAssetDefinition_GeometryNodesBridgeSource *******************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UClass_UAssetDefinition_GeometryNodesBridgeSource_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "IncludePath", "AssetDefinition_GeometryNodesBridgeSource.h" },
		{ "ModuleRelativePath", "Private/AssetDefinition_GeometryNodesBridgeSource.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UAssetDefinition_GeometryNodesBridgeSource constinit property declarations 
// ********** End Class UAssetDefinition_GeometryNodesBridgeSource constinit property declarations *
	static FTypeConstructFunc* DependentSingletons[];
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAssetDefinition_GeometryNodesBridgeSource>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct UHT_STATICS
FTypeConstructFunc* UHT_STATICS::DependentSingletons[] = {
	(FTypeConstructFunc*)Z_Construct_UClass_UAssetDefinitionDefault,
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeEditor,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams UHT_STATICS::ClassParams = {
	&Z_Construct_UClass_UAssetDefinition_GeometryNodesBridgeSource,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
FClassRegistrationInfo Z_Registration_Info_UClass_UAssetDefinition_GeometryNodesBridgeSource;
UClass* Z_Construct_UClass_UAssetDefinition_GeometryNodesBridgeSource(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Inner)
	{
		using TClass = UAssetDefinition_GeometryNodesBridgeSource;
		if (!Z_Registration_Info_UClass_UAssetDefinition_GeometryNodesBridgeSource.InnerSingleton)
		{
			GetPrivateStaticClassBody(
				TClass::StaticPackage(),
				TEXT("AssetDefinition_GeometryNodesBridgeSource"),
				Z_Registration_Info_UClass_UAssetDefinition_GeometryNodesBridgeSource.InnerSingleton,
				nullptr,
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
		return Z_Registration_Info_UClass_UAssetDefinition_GeometryNodesBridgeSource.InnerSingleton;
	}
	if (!Z_Registration_Info_UClass_UAssetDefinition_GeometryNodesBridgeSource.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAssetDefinition_GeometryNodesBridgeSource.OuterSingleton, UHT_STATICS::ClassParams);
	}
	return Z_Registration_Info_UClass_UAssetDefinition_GeometryNodesBridgeSource.OuterSingleton;
}
#undef UHT_STATICS
UAssetDefinition_GeometryNodesBridgeSource::UAssetDefinition_GeometryNodesBridgeSource() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UAssetDefinition_GeometryNodesBridgeSource);
UAssetDefinition_GeometryNodesBridgeSource::~UAssetDefinition_GeometryNodesBridgeSource() {}
// ********** End Class UAssetDefinition_GeometryNodesBridgeSource *********************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeEditor_Private_AssetDefinition_GeometryNodesBridgeSource_h__Script_GeometryNodesBridgeEditor_Statics
struct UHT_STATICS
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAssetDefinition_GeometryNodesBridgeSource, TEXT("UAssetDefinition_GeometryNodesBridgeSource"), &Z_Registration_Info_UClass_UAssetDefinition_GeometryNodesBridgeSource, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAssetDefinition_GeometryNodesBridgeSource), 2383965319U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeEditor_Private_AssetDefinition_GeometryNodesBridgeSource_h__Script_GeometryNodesBridgeEditor_1aa2857534840720fdf18803cf0b4e43c39988b1{
	TEXT("/Script/GeometryNodesBridgeEditor"),
	UHT_STATICS::ClassInfo, UE_ARRAY_COUNT(UHT_STATICS::ClassInfo),
	nullptr, 0,
	nullptr, 0,
	nullptr, 0,
};
#undef UHT_STATICS
// ********** End Registration *********************************************************************
#undef UHT_STRUCT_BASE

PRAGMA_ENABLE_DEPRECATION_WARNINGS
