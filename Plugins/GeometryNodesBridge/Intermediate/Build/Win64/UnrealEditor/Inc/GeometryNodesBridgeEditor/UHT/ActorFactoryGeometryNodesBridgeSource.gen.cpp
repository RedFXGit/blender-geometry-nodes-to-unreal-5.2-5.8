// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ActorFactoryGeometryNodesBridgeSource.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeActorFactoryGeometryNodesBridgeSource() {}

// ********** Begin Cross Module References ********************************************************
UNREALED_API UClass* Z_Construct_UClass_UActorFactory(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GeometryNodesBridgeEditor(ETypeConstructPhase);
GEOMETRYNODESBRIDGEEDITOR_API UClass* Z_Construct_UClass_UActorFactoryGeometryNodesBridgeSource(ETypeConstructPhase);
GEOMETRYNODESBRIDGEEDITOR_API UClass* Z_Construct_UClass_UActorFactoryGeometryNodesBridgeSource(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Class UActorFactoryGeometryNodesBridgeSource ***********************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UClass_UActorFactoryGeometryNodesBridgeSource_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "HideCategories", "Object" },
		{ "IncludePath", "ActorFactoryGeometryNodesBridgeSource.h" },
		{ "ModuleRelativePath", "Public/ActorFactoryGeometryNodesBridgeSource.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif // WITH_METADATA

// ********** Begin Class UActorFactoryGeometryNodesBridgeSource constinit property declarations ***
// ********** End Class UActorFactoryGeometryNodesBridgeSource constinit property declarations *****
	static FTypeConstructFunc* DependentSingletons[];
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UActorFactoryGeometryNodesBridgeSource>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct UHT_STATICS
FTypeConstructFunc* UHT_STATICS::DependentSingletons[] = {
	(FTypeConstructFunc*)Z_Construct_UClass_UActorFactory,
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeEditor,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams UHT_STATICS::ClassParams = {
	&Z_Construct_UClass_UActorFactoryGeometryNodesBridgeSource,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
FClassRegistrationInfo Z_Registration_Info_UClass_UActorFactoryGeometryNodesBridgeSource;
UClass* Z_Construct_UClass_UActorFactoryGeometryNodesBridgeSource(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Inner)
	{
		using TClass = UActorFactoryGeometryNodesBridgeSource;
		if (!Z_Registration_Info_UClass_UActorFactoryGeometryNodesBridgeSource.InnerSingleton)
		{
			GetPrivateStaticClassBody(
				TClass::StaticPackage(),
				TEXT("ActorFactoryGeometryNodesBridgeSource"),
				Z_Registration_Info_UClass_UActorFactoryGeometryNodesBridgeSource.InnerSingleton,
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
		return Z_Registration_Info_UClass_UActorFactoryGeometryNodesBridgeSource.InnerSingleton;
	}
	if (!Z_Registration_Info_UClass_UActorFactoryGeometryNodesBridgeSource.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UActorFactoryGeometryNodesBridgeSource.OuterSingleton, UHT_STATICS::ClassParams);
	}
	return Z_Registration_Info_UClass_UActorFactoryGeometryNodesBridgeSource.OuterSingleton;
}
#undef UHT_STATICS
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UActorFactoryGeometryNodesBridgeSource);
UActorFactoryGeometryNodesBridgeSource::~UActorFactoryGeometryNodesBridgeSource() {}
// ********** End Class UActorFactoryGeometryNodesBridgeSource *************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeEditor_Public_ActorFactoryGeometryNodesBridgeSource_h__Script_GeometryNodesBridgeEditor_Statics
struct UHT_STATICS
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UActorFactoryGeometryNodesBridgeSource, TEXT("UActorFactoryGeometryNodesBridgeSource"), &Z_Registration_Info_UClass_UActorFactoryGeometryNodesBridgeSource, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UActorFactoryGeometryNodesBridgeSource), 2626714543U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeEditor_Public_ActorFactoryGeometryNodesBridgeSource_h__Script_GeometryNodesBridgeEditor_e6df47ded13ace4749a5c2aae42e3e44ad0284b0{
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
