// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GeometryNodesBridgeSettings.h"
#include "UObject/SoftObjectPath.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGeometryNodesBridgeSettings() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FDirectoryPath(ETypeConstructPhase);
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FFilePath(ETypeConstructPhase);
DEVELOPERSETTINGS_API UClass* Z_Construct_UClass_UDeveloperSettings(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UClass* Z_Construct_UClass_UGeometryNodesBridgeSettings(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UClass* Z_Construct_UClass_UGeometryNodesBridgeSettings(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Class UGeometryNodesBridgeSettings *********************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UClass_UGeometryNodesBridgeSettings_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "DisplayName", "Geometry Nodes Bridge" },
		{ "IncludePath", "GeometryNodesBridgeSettings.h" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bAutoStartSessionOnEditorLaunch_MetaData[] = {
		{ "Category", "Session" },
		{ "DisplayName", "Automatically Start Session On Editor Launch" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "When enabled, Geometry Nodes Bridge will attempt to launch the global Blender session after the Unreal Editor finishes starting." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlenderExecutablePath_MetaData[] = {
		{ "Category", "Session" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WorkerScriptPath_MetaData[] = {
		{ "Category", "Session" },
		{ "DisplayName", "Bridge Worker Script Path" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Path to geometry_nodes_bridge_server.py, or to a directory that contains it. This setting is required." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SharedMemoryBytes_MetaData[] = {
		{ "Category", "Session" },
		{ "ClampMin", "1048576" },
		{ "DisplayName", "Shared Memory Bytes (Total)" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Total shared-memory budget across both bridge slots. The default is 250 MiB per slot, 500 MiB total." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BakeRootContentPath_MetaData[] = {
		{ "Category", "Bake" },
		{ "DisplayName", "Bake Root Content Path" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Content root where baked Geometry Nodes Bridge outputs, shared prototype meshes, and generated materials will be written." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AutomationFixtureRootDirectory_MetaData[] = {
		{ "Category", "Automation" },
		{ "DisplayName", "Automation Fixture Root Directory" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Root folder for local Blender fixture files consumed by the automated performance loop." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AutomationPerfScratchBakeRoot_MetaData[] = {
		{ "Category", "Automation" },
		{ "DisplayName", "Automation Perf Scratch Bake Root" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Content root reserved for automated performance bake runs so the loop does not pollute normal baked output content." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TimingHistoryOutputDirectory_MetaData[] = {
		{ "Category", "Diagnostics" },
		{ "DisplayName", "Timing History Output Directory" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Directory where Geometry Nodes Bridge appends the timing history CSV when blenderbridge.LogCookTimings is enabled. Leave empty to write to the Unreal project root." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UGeometryNodesBridgeSettings constinit property declarations *************
	static void NewProp_bAutoStartSessionOnEditorLaunch_SetBit(void* Obj)
	{
		((UGeometryNodesBridgeSettings*)Obj)->bAutoStartSessionOnEditorLaunch = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bAutoStartSessionOnEditorLaunch;
	static const UECodeGen_Private::FStructPropertyParams NewProp_BlenderExecutablePath;
	static const UECodeGen_Private::FStructPropertyParams NewProp_WorkerScriptPath;
	static const UECodeGen_Private::FInt64PropertyParams NewProp_SharedMemoryBytes;
	static const UECodeGen_Private::FStrPropertyParams NewProp_BakeRootContentPath;
	static const UECodeGen_Private::FStructPropertyParams NewProp_AutomationFixtureRootDirectory;
	static const UECodeGen_Private::FStrPropertyParams NewProp_AutomationPerfScratchBakeRoot;
	static const UECodeGen_Private::FStructPropertyParams NewProp_TimingHistoryOutputDirectory;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UGeometryNodesBridgeSettings constinit property declarations ***************
	static FTypeConstructFunc* DependentSingletons[];
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGeometryNodesBridgeSettings>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct UHT_STATICS

// ********** Begin Class UGeometryNodesBridgeSettings Property Definitions ************************
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bAutoStartSessionOnEditorLaunch = { "bAutoStartSessionOnEditorLaunch", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGeometryNodesBridgeSettings), &UHT_STATICS::NewProp_bAutoStartSessionOnEditorLaunch_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bAutoStartSessionOnEditorLaunch_MetaData), NewProp_bAutoStartSessionOnEditorLaunch_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_BlenderExecutablePath = { "BlenderExecutablePath", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeSettings, BlenderExecutablePath), Z_Construct_UScriptStruct_FFilePath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlenderExecutablePath_MetaData), NewProp_BlenderExecutablePath_MetaData) }; // 0b92f70c4aaca653170512ccb353f3aa009da14a
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_WorkerScriptPath = { "WorkerScriptPath", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeSettings, WorkerScriptPath), Z_Construct_UScriptStruct_FFilePath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WorkerScriptPath_MetaData), NewProp_WorkerScriptPath_MetaData) }; // 0b92f70c4aaca653170512ccb353f3aa009da14a
const UECodeGen_Private::FInt64PropertyParams UHT_STATICS::NewProp_SharedMemoryBytes = { "SharedMemoryBytes", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Int64, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeSettings, SharedMemoryBytes), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SharedMemoryBytes_MetaData), NewProp_SharedMemoryBytes_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_BakeRootContentPath = { "BakeRootContentPath", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeSettings, BakeRootContentPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BakeRootContentPath_MetaData), NewProp_BakeRootContentPath_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_AutomationFixtureRootDirectory = { "AutomationFixtureRootDirectory", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeSettings, AutomationFixtureRootDirectory), Z_Construct_UScriptStruct_FDirectoryPath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AutomationFixtureRootDirectory_MetaData), NewProp_AutomationFixtureRootDirectory_MetaData) }; // a9ea974572ee54d1b93cb49a6434fdbad40b6bf5
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_AutomationPerfScratchBakeRoot = { "AutomationPerfScratchBakeRoot", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeSettings, AutomationPerfScratchBakeRoot), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AutomationPerfScratchBakeRoot_MetaData), NewProp_AutomationPerfScratchBakeRoot_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_TimingHistoryOutputDirectory = { "TimingHistoryOutputDirectory", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(UGeometryNodesBridgeSettings, TimingHistoryOutputDirectory), Z_Construct_UScriptStruct_FDirectoryPath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TimingHistoryOutputDirectory_MetaData), NewProp_TimingHistoryOutputDirectory_MetaData) }; // a9ea974572ee54d1b93cb49a6434fdbad40b6bf5
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bAutoStartSessionOnEditorLaunch,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BlenderExecutablePath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_WorkerScriptPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SharedMemoryBytes,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BakeRootContentPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_AutomationFixtureRootDirectory,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_AutomationPerfScratchBakeRoot,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_TimingHistoryOutputDirectory,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Class UGeometryNodesBridgeSettings Property Definitions **************************
FTypeConstructFunc* UHT_STATICS::DependentSingletons[] = {
	(FTypeConstructFunc*)Z_Construct_UClass_UDeveloperSettings,
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams UHT_STATICS::ClassParams = {
	&Z_Construct_UClass_UGeometryNodesBridgeSettings,
	"EditorPerProjectUserSettings",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	UHT_STATICS::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	0,
	0x001000A6u,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
FClassRegistrationInfo Z_Registration_Info_UClass_UGeometryNodesBridgeSettings;
UClass* Z_Construct_UClass_UGeometryNodesBridgeSettings(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Inner)
	{
		using TClass = UGeometryNodesBridgeSettings;
		if (!Z_Registration_Info_UClass_UGeometryNodesBridgeSettings.InnerSingleton)
		{
			GetPrivateStaticClassBody(
				TClass::StaticPackage(),
				TEXT("GeometryNodesBridgeSettings"),
				Z_Registration_Info_UClass_UGeometryNodesBridgeSettings.InnerSingleton,
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
		return Z_Registration_Info_UClass_UGeometryNodesBridgeSettings.InnerSingleton;
	}
	if (!Z_Registration_Info_UClass_UGeometryNodesBridgeSettings.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGeometryNodesBridgeSettings.OuterSingleton, UHT_STATICS::ClassParams);
	}
	return Z_Registration_Info_UClass_UGeometryNodesBridgeSettings.OuterSingleton;
}
#undef UHT_STATICS
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UGeometryNodesBridgeSettings);
UGeometryNodesBridgeSettings::~UGeometryNodesBridgeSettings() {}
// ********** End Class UGeometryNodesBridgeSettings ***********************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeSettings_h__Script_GeometryNodesBridgeRuntime_Statics
struct UHT_STATICS
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGeometryNodesBridgeSettings, TEXT("UGeometryNodesBridgeSettings"), &Z_Registration_Info_UClass_UGeometryNodesBridgeSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGeometryNodesBridgeSettings), 3305241053U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeSettings_h__Script_GeometryNodesBridgeRuntime_b72cfaadfc07e9f90012feae29ac7c687fca565e{
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
