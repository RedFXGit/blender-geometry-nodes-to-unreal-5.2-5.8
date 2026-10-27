// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GeometryNodesBridgeProtocol.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGeometryNodesBridgeProtocol() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FLinearColor(ETypeConstructPhase);
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeCurveOwnerData(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeCurveOwnerPoint(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeEnumOption(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeGeometryInputDescriptor(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeInputSchema(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeMeshSectionDescriptor(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeTargetCapabilities(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeTargetDescriptor(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeBakeOutputMode(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeComponentStatus(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeConnectionState(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeCurveType(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeGeometrySourceKind(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeInputType(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeSplinePointType(ETypeConstructPhase);
GEOMETRYNODESBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeTargetKind(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Enum EGeometryNodesBridgeConnectionState ***************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeConnectionState_Statics
template<> GEOMETRYNODESBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EGeometryNodesBridgeConnectionState>()
{
	return Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeConnectionState(ETypeConstructPhase::Outer);
}
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "Connected.Name", "EGeometryNodesBridgeConnectionState::Connected" },
		{ "Disconnected.Name", "EGeometryNodesBridgeConnectionState::Disconnected" },
		{ "Error.Name", "EGeometryNodesBridgeConnectionState::Error" },
		{ "Launching.Name", "EGeometryNodesBridgeConnectionState::Launching" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EGeometryNodesBridgeConnectionState::Disconnected", (int64)EGeometryNodesBridgeConnectionState::Disconnected },
		{ "EGeometryNodesBridgeConnectionState::Launching", (int64)EGeometryNodesBridgeConnectionState::Launching },
		{ "EGeometryNodesBridgeConnectionState::Connected", (int64)EGeometryNodesBridgeConnectionState::Connected },
		{ "EGeometryNodesBridgeConnectionState::Error", (int64)EGeometryNodesBridgeConnectionState::Error },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct UHT_STATICS 
const UECodeGen_Private::FEnumParams UHT_STATICS::EnumParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	"EGeometryNodesBridgeConnectionState",
	"EGeometryNodesBridgeConnectionState",
	UHT_STATICS::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(UHT_STATICS::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	(uint8)UEnum::EUnderlyingType::uint8,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FEnumRegistrationInfo ZRIE_EGeometryNodesBridgeConnectionState;
UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeConnectionState(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!ZRIE_EGeometryNodesBridgeConnectionState.OuterSingleton)
		{
			ZRIE_EGeometryNodesBridgeConnectionState.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeConnectionState, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("EGeometryNodesBridgeConnectionState"));
		}
		return ZRIE_EGeometryNodesBridgeConnectionState.OuterSingleton;
	}
	if (!ZRIE_EGeometryNodesBridgeConnectionState.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(ZRIE_EGeometryNodesBridgeConnectionState.InnerSingleton, UHT_STATICS::EnumParams);
	}
	return ZRIE_EGeometryNodesBridgeConnectionState.InnerSingleton;
}
#undef UHT_STATICS
// ********** End Enum EGeometryNodesBridgeConnectionState *****************************************

// ********** Begin Enum EGeometryNodesBridgeComponentStatus ***************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeComponentStatus_Statics
template<> GEOMETRYNODESBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EGeometryNodesBridgeComponentStatus>()
{
	return Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeComponentStatus(ETypeConstructPhase::Outer);
}
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "Cooked.Name", "EGeometryNodesBridgeComponentStatus::Cooked" },
		{ "Cooking.Name", "EGeometryNodesBridgeComponentStatus::Cooking" },
		{ "CookQueued.Name", "EGeometryNodesBridgeComponentStatus::CookQueued" },
		{ "Error.Name", "EGeometryNodesBridgeComponentStatus::Error" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
		{ "NoSession.Name", "EGeometryNodesBridgeComponentStatus::NoSession" },
		{ "Ready.Name", "EGeometryNodesBridgeComponentStatus::Ready" },
		{ "SchemaPending.Name", "EGeometryNodesBridgeComponentStatus::SchemaPending" },
		{ "SourceNotLoaded.Name", "EGeometryNodesBridgeComponentStatus::SourceNotLoaded" },
		{ "TargetUnresolved.Name", "EGeometryNodesBridgeComponentStatus::TargetUnresolved" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EGeometryNodesBridgeComponentStatus::NoSession", (int64)EGeometryNodesBridgeComponentStatus::NoSession },
		{ "EGeometryNodesBridgeComponentStatus::SourceNotLoaded", (int64)EGeometryNodesBridgeComponentStatus::SourceNotLoaded },
		{ "EGeometryNodesBridgeComponentStatus::TargetUnresolved", (int64)EGeometryNodesBridgeComponentStatus::TargetUnresolved },
		{ "EGeometryNodesBridgeComponentStatus::SchemaPending", (int64)EGeometryNodesBridgeComponentStatus::SchemaPending },
		{ "EGeometryNodesBridgeComponentStatus::Ready", (int64)EGeometryNodesBridgeComponentStatus::Ready },
		{ "EGeometryNodesBridgeComponentStatus::CookQueued", (int64)EGeometryNodesBridgeComponentStatus::CookQueued },
		{ "EGeometryNodesBridgeComponentStatus::Cooking", (int64)EGeometryNodesBridgeComponentStatus::Cooking },
		{ "EGeometryNodesBridgeComponentStatus::Cooked", (int64)EGeometryNodesBridgeComponentStatus::Cooked },
		{ "EGeometryNodesBridgeComponentStatus::Error", (int64)EGeometryNodesBridgeComponentStatus::Error },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct UHT_STATICS 
const UECodeGen_Private::FEnumParams UHT_STATICS::EnumParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	"EGeometryNodesBridgeComponentStatus",
	"EGeometryNodesBridgeComponentStatus",
	UHT_STATICS::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(UHT_STATICS::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	(uint8)UEnum::EUnderlyingType::uint8,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FEnumRegistrationInfo ZRIE_EGeometryNodesBridgeComponentStatus;
UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeComponentStatus(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!ZRIE_EGeometryNodesBridgeComponentStatus.OuterSingleton)
		{
			ZRIE_EGeometryNodesBridgeComponentStatus.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeComponentStatus, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("EGeometryNodesBridgeComponentStatus"));
		}
		return ZRIE_EGeometryNodesBridgeComponentStatus.OuterSingleton;
	}
	if (!ZRIE_EGeometryNodesBridgeComponentStatus.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(ZRIE_EGeometryNodesBridgeComponentStatus.InnerSingleton, UHT_STATICS::EnumParams);
	}
	return ZRIE_EGeometryNodesBridgeComponentStatus.InnerSingleton;
}
#undef UHT_STATICS
// ********** End Enum EGeometryNodesBridgeComponentStatus *****************************************

// ********** Begin Enum EGeometryNodesBridgeTargetKind ********************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeTargetKind_Statics
template<> GEOMETRYNODESBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EGeometryNodesBridgeTargetKind>()
{
	return Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeTargetKind(ETypeConstructPhase::Outer);
}
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "CurveOwner.Name", "EGeometryNodesBridgeTargetKind::CurveOwner" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
		{ "Standard.Name", "EGeometryNodesBridgeTargetKind::Standard" },
		{ "Unknown.Name", "EGeometryNodesBridgeTargetKind::Unknown" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EGeometryNodesBridgeTargetKind::Unknown", (int64)EGeometryNodesBridgeTargetKind::Unknown },
		{ "EGeometryNodesBridgeTargetKind::Standard", (int64)EGeometryNodesBridgeTargetKind::Standard },
		{ "EGeometryNodesBridgeTargetKind::CurveOwner", (int64)EGeometryNodesBridgeTargetKind::CurveOwner },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct UHT_STATICS 
const UECodeGen_Private::FEnumParams UHT_STATICS::EnumParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	"EGeometryNodesBridgeTargetKind",
	"EGeometryNodesBridgeTargetKind",
	UHT_STATICS::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(UHT_STATICS::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	(uint8)UEnum::EUnderlyingType::uint8,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FEnumRegistrationInfo ZRIE_EGeometryNodesBridgeTargetKind;
UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeTargetKind(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!ZRIE_EGeometryNodesBridgeTargetKind.OuterSingleton)
		{
			ZRIE_EGeometryNodesBridgeTargetKind.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeTargetKind, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("EGeometryNodesBridgeTargetKind"));
		}
		return ZRIE_EGeometryNodesBridgeTargetKind.OuterSingleton;
	}
	if (!ZRIE_EGeometryNodesBridgeTargetKind.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(ZRIE_EGeometryNodesBridgeTargetKind.InnerSingleton, UHT_STATICS::EnumParams);
	}
	return ZRIE_EGeometryNodesBridgeTargetKind.InnerSingleton;
}
#undef UHT_STATICS
// ********** End Enum EGeometryNodesBridgeTargetKind **********************************************

// ********** Begin Enum EGeometryNodesBridgeCurveType *********************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeCurveType_Statics
template<> GEOMETRYNODESBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EGeometryNodesBridgeCurveType>()
{
	return Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeCurveType(ETypeConstructPhase::Outer);
}
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Bezier.Name", "EGeometryNodesBridgeCurveType::Bezier" },
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
		{ "None.Name", "EGeometryNodesBridgeCurveType::None" },
		{ "Unsupported.Name", "EGeometryNodesBridgeCurveType::Unsupported" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EGeometryNodesBridgeCurveType::None", (int64)EGeometryNodesBridgeCurveType::None },
		{ "EGeometryNodesBridgeCurveType::Bezier", (int64)EGeometryNodesBridgeCurveType::Bezier },
		{ "EGeometryNodesBridgeCurveType::Unsupported", (int64)EGeometryNodesBridgeCurveType::Unsupported },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct UHT_STATICS 
const UECodeGen_Private::FEnumParams UHT_STATICS::EnumParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	"EGeometryNodesBridgeCurveType",
	"EGeometryNodesBridgeCurveType",
	UHT_STATICS::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(UHT_STATICS::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	(uint8)UEnum::EUnderlyingType::uint8,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FEnumRegistrationInfo ZRIE_EGeometryNodesBridgeCurveType;
UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeCurveType(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!ZRIE_EGeometryNodesBridgeCurveType.OuterSingleton)
		{
			ZRIE_EGeometryNodesBridgeCurveType.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeCurveType, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("EGeometryNodesBridgeCurveType"));
		}
		return ZRIE_EGeometryNodesBridgeCurveType.OuterSingleton;
	}
	if (!ZRIE_EGeometryNodesBridgeCurveType.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(ZRIE_EGeometryNodesBridgeCurveType.InnerSingleton, UHT_STATICS::EnumParams);
	}
	return ZRIE_EGeometryNodesBridgeCurveType.InnerSingleton;
}
#undef UHT_STATICS
// ********** End Enum EGeometryNodesBridgeCurveType ***********************************************

// ********** Begin Enum EGeometryNodesBridgeSplinePointType ***************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeSplinePointType_Statics
template<> GEOMETRYNODESBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EGeometryNodesBridgeSplinePointType>()
{
	return Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeSplinePointType(ETypeConstructPhase::Outer);
}
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "Constant.Name", "EGeometryNodesBridgeSplinePointType::Constant" },
		{ "Curve.Name", "EGeometryNodesBridgeSplinePointType::Curve" },
		{ "CurveClamped.Name", "EGeometryNodesBridgeSplinePointType::CurveClamped" },
		{ "CurveCustomTangent.Name", "EGeometryNodesBridgeSplinePointType::CurveCustomTangent" },
		{ "Linear.Name", "EGeometryNodesBridgeSplinePointType::Linear" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EGeometryNodesBridgeSplinePointType::Linear", (int64)EGeometryNodesBridgeSplinePointType::Linear },
		{ "EGeometryNodesBridgeSplinePointType::Curve", (int64)EGeometryNodesBridgeSplinePointType::Curve },
		{ "EGeometryNodesBridgeSplinePointType::Constant", (int64)EGeometryNodesBridgeSplinePointType::Constant },
		{ "EGeometryNodesBridgeSplinePointType::CurveClamped", (int64)EGeometryNodesBridgeSplinePointType::CurveClamped },
		{ "EGeometryNodesBridgeSplinePointType::CurveCustomTangent", (int64)EGeometryNodesBridgeSplinePointType::CurveCustomTangent },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct UHT_STATICS 
const UECodeGen_Private::FEnumParams UHT_STATICS::EnumParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	"EGeometryNodesBridgeSplinePointType",
	"EGeometryNodesBridgeSplinePointType",
	UHT_STATICS::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(UHT_STATICS::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	(uint8)UEnum::EUnderlyingType::uint8,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FEnumRegistrationInfo ZRIE_EGeometryNodesBridgeSplinePointType;
UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeSplinePointType(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!ZRIE_EGeometryNodesBridgeSplinePointType.OuterSingleton)
		{
			ZRIE_EGeometryNodesBridgeSplinePointType.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeSplinePointType, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("EGeometryNodesBridgeSplinePointType"));
		}
		return ZRIE_EGeometryNodesBridgeSplinePointType.OuterSingleton;
	}
	if (!ZRIE_EGeometryNodesBridgeSplinePointType.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(ZRIE_EGeometryNodesBridgeSplinePointType.InnerSingleton, UHT_STATICS::EnumParams);
	}
	return ZRIE_EGeometryNodesBridgeSplinePointType.InnerSingleton;
}
#undef UHT_STATICS
// ********** End Enum EGeometryNodesBridgeSplinePointType *****************************************

// ********** Begin Enum EGeometryNodesBridgeInputType *********************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeInputType_Statics
template<> GEOMETRYNODESBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EGeometryNodesBridgeInputType>()
{
	return Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeInputType(ETypeConstructPhase::Outer);
}
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "Boolean.Name", "EGeometryNodesBridgeInputType::Boolean" },
		{ "Color.Name", "EGeometryNodesBridgeInputType::Color" },
		{ "Enum.Name", "EGeometryNodesBridgeInputType::Enum" },
		{ "Float.Name", "EGeometryNodesBridgeInputType::Float" },
		{ "Integer.Name", "EGeometryNodesBridgeInputType::Integer" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
		{ "String.Name", "EGeometryNodesBridgeInputType::String" },
		{ "Unsupported.Name", "EGeometryNodesBridgeInputType::Unsupported" },
		{ "Vector3.Name", "EGeometryNodesBridgeInputType::Vector3" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EGeometryNodesBridgeInputType::Float", (int64)EGeometryNodesBridgeInputType::Float },
		{ "EGeometryNodesBridgeInputType::Integer", (int64)EGeometryNodesBridgeInputType::Integer },
		{ "EGeometryNodesBridgeInputType::Boolean", (int64)EGeometryNodesBridgeInputType::Boolean },
		{ "EGeometryNodesBridgeInputType::Vector3", (int64)EGeometryNodesBridgeInputType::Vector3 },
		{ "EGeometryNodesBridgeInputType::Color", (int64)EGeometryNodesBridgeInputType::Color },
		{ "EGeometryNodesBridgeInputType::String", (int64)EGeometryNodesBridgeInputType::String },
		{ "EGeometryNodesBridgeInputType::Enum", (int64)EGeometryNodesBridgeInputType::Enum },
		{ "EGeometryNodesBridgeInputType::Unsupported", (int64)EGeometryNodesBridgeInputType::Unsupported },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct UHT_STATICS 
const UECodeGen_Private::FEnumParams UHT_STATICS::EnumParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	"EGeometryNodesBridgeInputType",
	"EGeometryNodesBridgeInputType",
	UHT_STATICS::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(UHT_STATICS::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	(uint8)UEnum::EUnderlyingType::uint8,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FEnumRegistrationInfo ZRIE_EGeometryNodesBridgeInputType;
UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeInputType(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!ZRIE_EGeometryNodesBridgeInputType.OuterSingleton)
		{
			ZRIE_EGeometryNodesBridgeInputType.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeInputType, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("EGeometryNodesBridgeInputType"));
		}
		return ZRIE_EGeometryNodesBridgeInputType.OuterSingleton;
	}
	if (!ZRIE_EGeometryNodesBridgeInputType.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(ZRIE_EGeometryNodesBridgeInputType.InnerSingleton, UHT_STATICS::EnumParams);
	}
	return ZRIE_EGeometryNodesBridgeInputType.InnerSingleton;
}
#undef UHT_STATICS
// ********** End Enum EGeometryNodesBridgeInputType ***********************************************

// ********** Begin Enum EGeometryNodesBridgeBakeOutputMode ****************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeBakeOutputMode_Statics
template<> GEOMETRYNODESBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EGeometryNodesBridgeBakeOutputMode>()
{
	return Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeBakeOutputMode(ETypeConstructPhase::Outer);
}
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "InstancedHierarchy.Name", "EGeometryNodesBridgeBakeOutputMode::InstancedHierarchy" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
		{ "StaticMesh.Name", "EGeometryNodesBridgeBakeOutputMode::StaticMesh" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EGeometryNodesBridgeBakeOutputMode::StaticMesh", (int64)EGeometryNodesBridgeBakeOutputMode::StaticMesh },
		{ "EGeometryNodesBridgeBakeOutputMode::InstancedHierarchy", (int64)EGeometryNodesBridgeBakeOutputMode::InstancedHierarchy },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct UHT_STATICS 
const UECodeGen_Private::FEnumParams UHT_STATICS::EnumParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	"EGeometryNodesBridgeBakeOutputMode",
	"EGeometryNodesBridgeBakeOutputMode",
	UHT_STATICS::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(UHT_STATICS::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	(uint8)UEnum::EUnderlyingType::uint8,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FEnumRegistrationInfo ZRIE_EGeometryNodesBridgeBakeOutputMode;
UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeBakeOutputMode(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!ZRIE_EGeometryNodesBridgeBakeOutputMode.OuterSingleton)
		{
			ZRIE_EGeometryNodesBridgeBakeOutputMode.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeBakeOutputMode, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("EGeometryNodesBridgeBakeOutputMode"));
		}
		return ZRIE_EGeometryNodesBridgeBakeOutputMode.OuterSingleton;
	}
	if (!ZRIE_EGeometryNodesBridgeBakeOutputMode.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(ZRIE_EGeometryNodesBridgeBakeOutputMode.InnerSingleton, UHT_STATICS::EnumParams);
	}
	return ZRIE_EGeometryNodesBridgeBakeOutputMode.InnerSingleton;
}
#undef UHT_STATICS
// ********** End Enum EGeometryNodesBridgeBakeOutputMode ******************************************

// ********** Begin Enum EGeometryNodesBridgeGeometrySourceKind ************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeGeometrySourceKind_Statics
template<> GEOMETRYNODESBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EGeometryNodesBridgeGeometrySourceKind>()
{
	return Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeGeometrySourceKind(ETypeConstructPhase::Outer);
}
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "EmbeddedSpline.Name", "EGeometryNodesBridgeGeometrySourceKind::EmbeddedSpline" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
		{ "None.Name", "EGeometryNodesBridgeGeometrySourceKind::None" },
		{ "SplineComponent.Name", "EGeometryNodesBridgeGeometrySourceKind::SplineComponent" },
		{ "StaticMeshComponent.Name", "EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EGeometryNodesBridgeGeometrySourceKind::None", (int64)EGeometryNodesBridgeGeometrySourceKind::None },
		{ "EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent", (int64)EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent },
		{ "EGeometryNodesBridgeGeometrySourceKind::SplineComponent", (int64)EGeometryNodesBridgeGeometrySourceKind::SplineComponent },
		{ "EGeometryNodesBridgeGeometrySourceKind::EmbeddedSpline", (int64)EGeometryNodesBridgeGeometrySourceKind::EmbeddedSpline },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct UHT_STATICS 
const UECodeGen_Private::FEnumParams UHT_STATICS::EnumParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	"EGeometryNodesBridgeGeometrySourceKind",
	"EGeometryNodesBridgeGeometrySourceKind",
	UHT_STATICS::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(UHT_STATICS::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	(uint8)UEnum::EUnderlyingType::uint8,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FEnumRegistrationInfo ZRIE_EGeometryNodesBridgeGeometrySourceKind;
UEnum* Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeGeometrySourceKind(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!ZRIE_EGeometryNodesBridgeGeometrySourceKind.OuterSingleton)
		{
			ZRIE_EGeometryNodesBridgeGeometrySourceKind.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeGeometrySourceKind, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("EGeometryNodesBridgeGeometrySourceKind"));
		}
		return ZRIE_EGeometryNodesBridgeGeometrySourceKind.OuterSingleton;
	}
	if (!ZRIE_EGeometryNodesBridgeGeometrySourceKind.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(ZRIE_EGeometryNodesBridgeGeometrySourceKind.InnerSingleton, UHT_STATICS::EnumParams);
	}
	return ZRIE_EGeometryNodesBridgeGeometrySourceKind.InnerSingleton;
}
#undef UHT_STATICS
// ********** End Enum EGeometryNodesBridgeGeometrySourceKind **************************************

// ********** Begin ScriptStruct FBridgeEnumOption *************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FBridgeEnumOption_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FBridgeEnumOption>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeEnumOption); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Id_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Label_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Value_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeEnumOption constinit property declarations *****************
	static const UECodeGen_Private::FStrPropertyParams NewProp_Id;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Label;
	static const UECodeGen_Private::FIntPropertyParams NewProp_Value;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeEnumOption constinit property declarations *******************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeEnumOption>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FBridgeEnumOption Property Definitions ****************************
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_Id = { "Id", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeEnumOption, Id), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Id_MetaData), NewProp_Id_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_Label = { "Label", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeEnumOption, Label), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Label_MetaData), NewProp_Label_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_Value = { "Value", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeEnumOption, Value), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Value_MetaData), NewProp_Value_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Id,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Label,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Value,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeEnumOption Property Definitions ******************************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeEnumOption",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FBridgeEnumOption>(),
	alignof(FBridgeEnumOption),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeEnumOption;
UScriptStruct* Z_Construct_UScriptStruct_FBridgeEnumOption(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FBridgeEnumOption.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FBridgeEnumOption.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeEnumOption, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("BridgeEnumOption"));
		}
		return Z_Registration_Info_UScriptStruct_FBridgeEnumOption.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FBridgeEnumOption.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeEnumOption.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeEnumOption.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FBridgeEnumOption ***************************************************

// ********** Begin ScriptStruct FBridgeCurveOwnerPoint ********************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FBridgeCurveOwnerPoint_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FBridgeCurveOwnerPoint>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeCurveOwnerPoint); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Position_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ArriveTangent_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LeaveTangent_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RollDegrees_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Scale_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Type_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeCurveOwnerPoint constinit property declarations ************
	static const UECodeGen_Private::FStructPropertyParams NewProp_Position;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ArriveTangent;
	static const UECodeGen_Private::FStructPropertyParams NewProp_LeaveTangent;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_RollDegrees;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Scale;
	static const UECodeGen_Private::FBytePropertyParams NewProp_Type_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_Type;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeCurveOwnerPoint constinit property declarations **************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeCurveOwnerPoint>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FBridgeCurveOwnerPoint Property Definitions ***********************
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_Position = { "Position", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeCurveOwnerPoint, Position), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Position_MetaData), NewProp_Position_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_ArriveTangent = { "ArriveTangent", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeCurveOwnerPoint, ArriveTangent), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ArriveTangent_MetaData), NewProp_ArriveTangent_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_LeaveTangent = { "LeaveTangent", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeCurveOwnerPoint, LeaveTangent), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LeaveTangent_MetaData), NewProp_LeaveTangent_MetaData) };
const UECodeGen_Private::FFloatPropertyParams UHT_STATICS::NewProp_RollDegrees = { "RollDegrees", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Float, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeCurveOwnerPoint, RollDegrees), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RollDegrees_MetaData), NewProp_RollDegrees_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_Scale = { "Scale", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeCurveOwnerPoint, Scale), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Scale_MetaData), NewProp_Scale_MetaData) };
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_Type_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_Type = { "Type", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeCurveOwnerPoint, Type), Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeSplinePointType, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Type_MetaData), NewProp_Type_MetaData) }; // 5cda8e66742b4ddb6f8a34df4733f9af59a1dc1a
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Position,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ArriveTangent,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_LeaveTangent,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_RollDegrees,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Scale,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Type_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Type,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeCurveOwnerPoint Property Definitions *************************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeCurveOwnerPoint",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FBridgeCurveOwnerPoint>(),
	alignof(FBridgeCurveOwnerPoint),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerPoint;
UScriptStruct* Z_Construct_UScriptStruct_FBridgeCurveOwnerPoint(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerPoint.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerPoint.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeCurveOwnerPoint, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("BridgeCurveOwnerPoint"));
		}
		return Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerPoint.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerPoint.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerPoint.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerPoint.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FBridgeCurveOwnerPoint **********************************************

// ********** Begin ScriptStruct FBridgeCurveOwnerData *********************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FBridgeCurveOwnerData_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FBridgeCurveOwnerData>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeCurveOwnerData); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CurveType_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bClosedLoop_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Points_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeCurveOwnerData constinit property declarations *************
	static const UECodeGen_Private::FBytePropertyParams NewProp_CurveType_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_CurveType;
	static void NewProp_bClosedLoop_SetBit(void* Obj)
	{
		((FBridgeCurveOwnerData*)Obj)->bClosedLoop = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bClosedLoop;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Points_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_Points;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeCurveOwnerData constinit property declarations ***************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeCurveOwnerData>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FBridgeCurveOwnerData Property Definitions ************************
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_CurveType_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_CurveType = { "CurveType", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeCurveOwnerData, CurveType), Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeCurveType, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CurveType_MetaData), NewProp_CurveType_MetaData) }; // 8f2e7ba414aa353baa88f95675e45631558fe282
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bClosedLoop = { "bClosedLoop", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(FBridgeCurveOwnerData), &UHT_STATICS::NewProp_bClosedLoop_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bClosedLoop_MetaData), NewProp_bClosedLoop_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_Points_Inner = { "Points", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FBridgeCurveOwnerPoint, METADATA_PARAMS(0, nullptr) }; // f3e173b096691c27f5ffdb5a8ea92d772e57ad62
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_Points = { "Points", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeCurveOwnerData, Points), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Points_MetaData), NewProp_Points_MetaData) }; // f3e173b096691c27f5ffdb5a8ea92d772e57ad62
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_CurveType_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_CurveType,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bClosedLoop,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Points_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Points,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeCurveOwnerData Property Definitions **************************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeCurveOwnerData",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FBridgeCurveOwnerData>(),
	alignof(FBridgeCurveOwnerData),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerData;
UScriptStruct* Z_Construct_UScriptStruct_FBridgeCurveOwnerData(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerData.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerData.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeCurveOwnerData, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("BridgeCurveOwnerData"));
		}
		return Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerData.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerData.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerData.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerData.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FBridgeCurveOwnerData ***********************************************

// ********** Begin ScriptStruct FBridgeTargetCapabilities *****************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FBridgeTargetCapabilities_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FBridgeTargetCapabilities>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeTargetCapabilities); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetKind_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bSupportsCurveOwner_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CompatibilityMessage_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OwnerCurve_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeTargetCapabilities constinit property declarations *********
	static const UECodeGen_Private::FBytePropertyParams NewProp_TargetKind_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_TargetKind;
	static void NewProp_bSupportsCurveOwner_SetBit(void* Obj)
	{
		((FBridgeTargetCapabilities*)Obj)->bSupportsCurveOwner = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bSupportsCurveOwner;
	static const UECodeGen_Private::FStrPropertyParams NewProp_CompatibilityMessage;
	static const UECodeGen_Private::FStructPropertyParams NewProp_OwnerCurve;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeTargetCapabilities constinit property declarations ***********
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeTargetCapabilities>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FBridgeTargetCapabilities Property Definitions ********************
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_TargetKind_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_TargetKind = { "TargetKind", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetCapabilities, TargetKind), Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeTargetKind, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetKind_MetaData), NewProp_TargetKind_MetaData) }; // 6f76c5bb948c4e6a71e05461320f860d2ea0ead5
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bSupportsCurveOwner = { "bSupportsCurveOwner", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(FBridgeTargetCapabilities), &UHT_STATICS::NewProp_bSupportsCurveOwner_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bSupportsCurveOwner_MetaData), NewProp_bSupportsCurveOwner_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_CompatibilityMessage = { "CompatibilityMessage", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetCapabilities, CompatibilityMessage), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CompatibilityMessage_MetaData), NewProp_CompatibilityMessage_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_OwnerCurve = { "OwnerCurve", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetCapabilities, OwnerCurve), Z_Construct_UScriptStruct_FBridgeCurveOwnerData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OwnerCurve_MetaData), NewProp_OwnerCurve_MetaData) }; // 6ccb6e4f9865b5029498aa9c0d40df09b26641a2
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_TargetKind_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_TargetKind,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bSupportsCurveOwner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_CompatibilityMessage,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_OwnerCurve,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeTargetCapabilities Property Definitions **********************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeTargetCapabilities",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FBridgeTargetCapabilities>(),
	alignof(FBridgeTargetCapabilities),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeTargetCapabilities;
UScriptStruct* Z_Construct_UScriptStruct_FBridgeTargetCapabilities(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FBridgeTargetCapabilities.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FBridgeTargetCapabilities.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeTargetCapabilities, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("BridgeTargetCapabilities"));
		}
		return Z_Registration_Info_UScriptStruct_FBridgeTargetCapabilities.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FBridgeTargetCapabilities.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeTargetCapabilities.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeTargetCapabilities.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FBridgeTargetCapabilities *******************************************

// ********** Begin ScriptStruct FBridgeInputSchema ************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FBridgeInputSchema_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FBridgeInputSchema>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeInputSchema); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Id_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Label_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CategoryPath_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Description_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Type_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultNumericValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MinNumericValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxNumericValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bDefaultBoolValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultVectorValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultColorValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultStringValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultEnumValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EnumOptions_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeInputSchema constinit property declarations ****************
	static const UECodeGen_Private::FNamePropertyParams NewProp_Id;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Label;
	static const UECodeGen_Private::FStrPropertyParams NewProp_CategoryPath;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Description;
	static const UECodeGen_Private::FBytePropertyParams NewProp_Type_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_Type;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_DefaultNumericValue;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_MinNumericValue;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_MaxNumericValue;
	static void NewProp_bDefaultBoolValue_SetBit(void* Obj)
	{
		((FBridgeInputSchema*)Obj)->bDefaultBoolValue = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDefaultBoolValue;
	static const UECodeGen_Private::FStructPropertyParams NewProp_DefaultVectorValue;
	static const UECodeGen_Private::FStructPropertyParams NewProp_DefaultColorValue;
	static const UECodeGen_Private::FStrPropertyParams NewProp_DefaultStringValue;
	static const UECodeGen_Private::FIntPropertyParams NewProp_DefaultEnumValue;
	static const UECodeGen_Private::FStructPropertyParams NewProp_EnumOptions_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_EnumOptions;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeInputSchema constinit property declarations ******************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeInputSchema>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FBridgeInputSchema Property Definitions ***************************
const UECodeGen_Private::FNamePropertyParams UHT_STATICS::NewProp_Id = { "Id", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Name, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, Id), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Id_MetaData), NewProp_Id_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_Label = { "Label", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, Label), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Label_MetaData), NewProp_Label_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_CategoryPath = { "CategoryPath", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, CategoryPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CategoryPath_MetaData), NewProp_CategoryPath_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_Description = { "Description", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, Description), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Description_MetaData), NewProp_Description_MetaData) };
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_Type_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_Type = { "Type", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, Type), Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeInputType, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Type_MetaData), NewProp_Type_MetaData) }; // 23cf5fd3458e08578fcbf3a45fb8c862d5fc6f7a
const UECodeGen_Private::FDoublePropertyParams UHT_STATICS::NewProp_DefaultNumericValue = { "DefaultNumericValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Double, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultNumericValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultNumericValue_MetaData), NewProp_DefaultNumericValue_MetaData) };
const UECodeGen_Private::FDoublePropertyParams UHT_STATICS::NewProp_MinNumericValue = { "MinNumericValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Double, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, MinNumericValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MinNumericValue_MetaData), NewProp_MinNumericValue_MetaData) };
const UECodeGen_Private::FDoublePropertyParams UHT_STATICS::NewProp_MaxNumericValue = { "MaxNumericValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Double, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, MaxNumericValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxNumericValue_MetaData), NewProp_MaxNumericValue_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bDefaultBoolValue = { "bDefaultBoolValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(FBridgeInputSchema), &UHT_STATICS::NewProp_bDefaultBoolValue_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bDefaultBoolValue_MetaData), NewProp_bDefaultBoolValue_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_DefaultVectorValue = { "DefaultVectorValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultVectorValue), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultVectorValue_MetaData), NewProp_DefaultVectorValue_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_DefaultColorValue = { "DefaultColorValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultColorValue), Z_Construct_UScriptStruct_FLinearColor, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultColorValue_MetaData), NewProp_DefaultColorValue_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_DefaultStringValue = { "DefaultStringValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultStringValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultStringValue_MetaData), NewProp_DefaultStringValue_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_DefaultEnumValue = { "DefaultEnumValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultEnumValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultEnumValue_MetaData), NewProp_DefaultEnumValue_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_EnumOptions_Inner = { "EnumOptions", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FBridgeEnumOption, METADATA_PARAMS(0, nullptr) }; // 5c955714fd091f8cb5e7ac69aae2c557da9337dc
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_EnumOptions = { "EnumOptions", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, EnumOptions), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EnumOptions_MetaData), NewProp_EnumOptions_MetaData) }; // 5c955714fd091f8cb5e7ac69aae2c557da9337dc
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Id,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Label,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_CategoryPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Description,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Type_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Type,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_DefaultNumericValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_MinNumericValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_MaxNumericValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bDefaultBoolValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_DefaultVectorValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_DefaultColorValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_DefaultStringValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_DefaultEnumValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_EnumOptions_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_EnumOptions,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeInputSchema Property Definitions *****************************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeInputSchema",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FBridgeInputSchema>(),
	alignof(FBridgeInputSchema),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeInputSchema;
UScriptStruct* Z_Construct_UScriptStruct_FBridgeInputSchema(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FBridgeInputSchema.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FBridgeInputSchema.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeInputSchema, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("BridgeInputSchema"));
		}
		return Z_Registration_Info_UScriptStruct_FBridgeInputSchema.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FBridgeInputSchema.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeInputSchema.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeInputSchema.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FBridgeInputSchema **************************************************

// ********** Begin ScriptStruct FBridgeTargetDescriptor *******************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FBridgeTargetDescriptor>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeTargetDescriptor); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetId_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DisplayLabel_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ObjectName_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ModifierName_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ObjectType_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeTargetDescriptor constinit property declarations ***********
	static const UECodeGen_Private::FStrPropertyParams NewProp_TargetId;
	static const UECodeGen_Private::FStrPropertyParams NewProp_DisplayLabel;
	static const UECodeGen_Private::FStrPropertyParams NewProp_ObjectName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_ModifierName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_ObjectType;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeTargetDescriptor constinit property declarations *************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeTargetDescriptor>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FBridgeTargetDescriptor Property Definitions **********************
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_TargetId = { "TargetId", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetDescriptor, TargetId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetId_MetaData), NewProp_TargetId_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_DisplayLabel = { "DisplayLabel", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetDescriptor, DisplayLabel), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DisplayLabel_MetaData), NewProp_DisplayLabel_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_ObjectName = { "ObjectName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetDescriptor, ObjectName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ObjectName_MetaData), NewProp_ObjectName_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_ModifierName = { "ModifierName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetDescriptor, ModifierName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ModifierName_MetaData), NewProp_ModifierName_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_ObjectType = { "ObjectType", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetDescriptor, ObjectType), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ObjectType_MetaData), NewProp_ObjectType_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_TargetId,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_DisplayLabel,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ObjectName,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ModifierName,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ObjectType,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeTargetDescriptor Property Definitions ************************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeTargetDescriptor",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FBridgeTargetDescriptor>(),
	alignof(FBridgeTargetDescriptor),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor;
UScriptStruct* Z_Construct_UScriptStruct_FBridgeTargetDescriptor(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeTargetDescriptor, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("BridgeTargetDescriptor"));
		}
		return Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FBridgeTargetDescriptor *********************************************

// ********** Begin ScriptStruct FBridgeGeometryInputDescriptor ************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FBridgeGeometryInputDescriptor_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FBridgeGeometryInputDescriptor>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeGeometryInputDescriptor); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InputId_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Label_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bUsesOwnerGeometry_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AllowedSourceKinds_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeGeometryInputDescriptor constinit property declarations ****
	static const UECodeGen_Private::FStrPropertyParams NewProp_InputId;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Label;
	static void NewProp_bUsesOwnerGeometry_SetBit(void* Obj)
	{
		((FBridgeGeometryInputDescriptor*)Obj)->bUsesOwnerGeometry = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bUsesOwnerGeometry;
	static const UECodeGen_Private::FBytePropertyParams NewProp_AllowedSourceKinds_Inner_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_AllowedSourceKinds_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_AllowedSourceKinds;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeGeometryInputDescriptor constinit property declarations ******
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeGeometryInputDescriptor>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FBridgeGeometryInputDescriptor Property Definitions ***************
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_InputId = { "InputId", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeGeometryInputDescriptor, InputId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InputId_MetaData), NewProp_InputId_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_Label = { "Label", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeGeometryInputDescriptor, Label), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Label_MetaData), NewProp_Label_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bUsesOwnerGeometry = { "bUsesOwnerGeometry", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(FBridgeGeometryInputDescriptor), &UHT_STATICS::NewProp_bUsesOwnerGeometry_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bUsesOwnerGeometry_MetaData), NewProp_bUsesOwnerGeometry_MetaData) };
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_AllowedSourceKinds_Inner_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_AllowedSourceKinds_Inner = { "AllowedSourceKinds", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, 0, Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeGeometrySourceKind, METADATA_PARAMS(0, nullptr) }; // 982d8dce3d359296593172c0bef4ace48e3c7fd7
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_AllowedSourceKinds = { "AllowedSourceKinds", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeGeometryInputDescriptor, AllowedSourceKinds), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AllowedSourceKinds_MetaData), NewProp_AllowedSourceKinds_MetaData) }; // 982d8dce3d359296593172c0bef4ace48e3c7fd7
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_InputId,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Label,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bUsesOwnerGeometry,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_AllowedSourceKinds_Inner_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_AllowedSourceKinds_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_AllowedSourceKinds,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeGeometryInputDescriptor Property Definitions *****************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeGeometryInputDescriptor",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FBridgeGeometryInputDescriptor>(),
	alignof(FBridgeGeometryInputDescriptor),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeGeometryInputDescriptor;
UScriptStruct* Z_Construct_UScriptStruct_FBridgeGeometryInputDescriptor(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FBridgeGeometryInputDescriptor.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FBridgeGeometryInputDescriptor.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeGeometryInputDescriptor, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("BridgeGeometryInputDescriptor"));
		}
		return Z_Registration_Info_UScriptStruct_FBridgeGeometryInputDescriptor.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FBridgeGeometryInputDescriptor.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeGeometryInputDescriptor.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeGeometryInputDescriptor.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FBridgeGeometryInputDescriptor **************************************

// ********** Begin ScriptStruct FBridgeMaterialSlotDescriptor *************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FBridgeMaterialSlotDescriptor>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeMaterialSlotDescriptor); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SlotId_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SlotName_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlenderMaterialName_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SectionIndex_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeMaterialSlotDescriptor constinit property declarations *****
	static const UECodeGen_Private::FStrPropertyParams NewProp_SlotId;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SlotName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_BlenderMaterialName;
	static const UECodeGen_Private::FIntPropertyParams NewProp_SectionIndex;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeMaterialSlotDescriptor constinit property declarations *******
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeMaterialSlotDescriptor>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FBridgeMaterialSlotDescriptor Property Definitions ****************
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SlotId = { "SlotId", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMaterialSlotDescriptor, SlotId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SlotId_MetaData), NewProp_SlotId_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SlotName = { "SlotName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMaterialSlotDescriptor, SlotName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SlotName_MetaData), NewProp_SlotName_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_BlenderMaterialName = { "BlenderMaterialName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMaterialSlotDescriptor, BlenderMaterialName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlenderMaterialName_MetaData), NewProp_BlenderMaterialName_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_SectionIndex = { "SectionIndex", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMaterialSlotDescriptor, SectionIndex), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SectionIndex_MetaData), NewProp_SectionIndex_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SlotId,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SlotName,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BlenderMaterialName,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SectionIndex,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeMaterialSlotDescriptor Property Definitions ******************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeMaterialSlotDescriptor",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FBridgeMaterialSlotDescriptor>(),
	alignof(FBridgeMaterialSlotDescriptor),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeMaterialSlotDescriptor;
UScriptStruct* Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FBridgeMaterialSlotDescriptor.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FBridgeMaterialSlotDescriptor.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("BridgeMaterialSlotDescriptor"));
		}
		return Z_Registration_Info_UScriptStruct_FBridgeMaterialSlotDescriptor.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FBridgeMaterialSlotDescriptor.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeMaterialSlotDescriptor.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeMaterialSlotDescriptor.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FBridgeMaterialSlotDescriptor ***************************************

// ********** Begin ScriptStruct FBridgeMeshSectionDescriptor **************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FBridgeMeshSectionDescriptor_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FBridgeMeshSectionDescriptor>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeMeshSectionDescriptor); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SectionIndex_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FirstVertex_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_VertexCount_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FirstIndex_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_IndexCount_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SlotId_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SlotName_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlenderMaterialName_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/GeometryNodesBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeMeshSectionDescriptor constinit property declarations ******
	static const UECodeGen_Private::FIntPropertyParams NewProp_SectionIndex;
	static const UECodeGen_Private::FIntPropertyParams NewProp_FirstVertex;
	static const UECodeGen_Private::FIntPropertyParams NewProp_VertexCount;
	static const UECodeGen_Private::FIntPropertyParams NewProp_FirstIndex;
	static const UECodeGen_Private::FIntPropertyParams NewProp_IndexCount;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SlotId;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SlotName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_BlenderMaterialName;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeMeshSectionDescriptor constinit property declarations ********
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeMeshSectionDescriptor>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FBridgeMeshSectionDescriptor Property Definitions *****************
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_SectionIndex = { "SectionIndex", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMeshSectionDescriptor, SectionIndex), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SectionIndex_MetaData), NewProp_SectionIndex_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_FirstVertex = { "FirstVertex", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMeshSectionDescriptor, FirstVertex), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FirstVertex_MetaData), NewProp_FirstVertex_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_VertexCount = { "VertexCount", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMeshSectionDescriptor, VertexCount), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_VertexCount_MetaData), NewProp_VertexCount_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_FirstIndex = { "FirstIndex", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMeshSectionDescriptor, FirstIndex), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FirstIndex_MetaData), NewProp_FirstIndex_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_IndexCount = { "IndexCount", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMeshSectionDescriptor, IndexCount), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_IndexCount_MetaData), NewProp_IndexCount_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SlotId = { "SlotId", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMeshSectionDescriptor, SlotId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SlotId_MetaData), NewProp_SlotId_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SlotName = { "SlotName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMeshSectionDescriptor, SlotName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SlotName_MetaData), NewProp_SlotName_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_BlenderMaterialName = { "BlenderMaterialName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeMeshSectionDescriptor, BlenderMaterialName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlenderMaterialName_MetaData), NewProp_BlenderMaterialName_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SectionIndex,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_FirstVertex,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_VertexCount,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_FirstIndex,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_IndexCount,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SlotId,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SlotName,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_BlenderMaterialName,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeMeshSectionDescriptor Property Definitions *******************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeMeshSectionDescriptor",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FBridgeMeshSectionDescriptor>(),
	alignof(FBridgeMeshSectionDescriptor),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeMeshSectionDescriptor;
UScriptStruct* Z_Construct_UScriptStruct_FBridgeMeshSectionDescriptor(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FBridgeMeshSectionDescriptor.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FBridgeMeshSectionDescriptor.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeMeshSectionDescriptor, (UObject*)Z_Construct_UPackage__Script_GeometryNodesBridgeRuntime(ETypeConstructPhase::Outer), TEXT("BridgeMeshSectionDescriptor"));
		}
		return Z_Registration_Info_UScriptStruct_FBridgeMeshSectionDescriptor.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FBridgeMeshSectionDescriptor.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeMeshSectionDescriptor.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeMeshSectionDescriptor.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FBridgeMeshSectionDescriptor ****************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeProtocol_h__Script_GeometryNodesBridgeRuntime_Statics
struct UHT_STATICS
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeConnectionState, TEXT("EGeometryNodesBridgeConnectionState"), &ZRIE_EGeometryNodesBridgeConnectionState, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1925998413U) },
		{ Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeComponentStatus, TEXT("EGeometryNodesBridgeComponentStatus"), &ZRIE_EGeometryNodesBridgeComponentStatus, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3944396677U) },
		{ Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeTargetKind, TEXT("EGeometryNodesBridgeTargetKind"), &ZRIE_EGeometryNodesBridgeTargetKind, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1870054843U) },
		{ Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeCurveType, TEXT("EGeometryNodesBridgeCurveType"), &ZRIE_EGeometryNodesBridgeCurveType, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 2402188196U) },
		{ Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeSplinePointType, TEXT("EGeometryNodesBridgeSplinePointType"), &ZRIE_EGeometryNodesBridgeSplinePointType, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1557827174U) },
		{ Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeInputType, TEXT("EGeometryNodesBridgeInputType"), &ZRIE_EGeometryNodesBridgeInputType, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 600793043U) },
		{ Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeBakeOutputMode, TEXT("EGeometryNodesBridgeBakeOutputMode"), &ZRIE_EGeometryNodesBridgeBakeOutputMode, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3021653095U) },
		{ Z_Construct_UEnum_GeometryNodesBridgeRuntime_EGeometryNodesBridgeGeometrySourceKind, TEXT("EGeometryNodesBridgeGeometrySourceKind"), &ZRIE_EGeometryNodesBridgeGeometrySourceKind, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 2553122254U) },
	};
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ Z_Construct_UScriptStruct_FBridgeEnumOption, Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::NewStructOps, TEXT("BridgeEnumOption"),&Z_Registration_Info_UScriptStruct_FBridgeEnumOption, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeEnumOption), 1553291028U) },
		{ Z_Construct_UScriptStruct_FBridgeCurveOwnerPoint, Z_Construct_UScriptStruct_FBridgeCurveOwnerPoint_Statics::NewStructOps, TEXT("BridgeCurveOwnerPoint"),&Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerPoint, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeCurveOwnerPoint), 4091638704U) },
		{ Z_Construct_UScriptStruct_FBridgeCurveOwnerData, Z_Construct_UScriptStruct_FBridgeCurveOwnerData_Statics::NewStructOps, TEXT("BridgeCurveOwnerData"),&Z_Registration_Info_UScriptStruct_FBridgeCurveOwnerData, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeCurveOwnerData), 1825271375U) },
		{ Z_Construct_UScriptStruct_FBridgeTargetCapabilities, Z_Construct_UScriptStruct_FBridgeTargetCapabilities_Statics::NewStructOps, TEXT("BridgeTargetCapabilities"),&Z_Registration_Info_UScriptStruct_FBridgeTargetCapabilities, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeTargetCapabilities), 634554122U) },
		{ Z_Construct_UScriptStruct_FBridgeInputSchema, Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewStructOps, TEXT("BridgeInputSchema"),&Z_Registration_Info_UScriptStruct_FBridgeInputSchema, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeInputSchema), 4166331717U) },
		{ Z_Construct_UScriptStruct_FBridgeTargetDescriptor, Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewStructOps, TEXT("BridgeTargetDescriptor"),&Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeTargetDescriptor), 3200179553U) },
		{ Z_Construct_UScriptStruct_FBridgeGeometryInputDescriptor, Z_Construct_UScriptStruct_FBridgeGeometryInputDescriptor_Statics::NewStructOps, TEXT("BridgeGeometryInputDescriptor"),&Z_Registration_Info_UScriptStruct_FBridgeGeometryInputDescriptor, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeGeometryInputDescriptor), 348914796U) },
		{ Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor, Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor_Statics::NewStructOps, TEXT("BridgeMaterialSlotDescriptor"),&Z_Registration_Info_UScriptStruct_FBridgeMaterialSlotDescriptor, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeMaterialSlotDescriptor), 2875040978U) },
		{ Z_Construct_UScriptStruct_FBridgeMeshSectionDescriptor, Z_Construct_UScriptStruct_FBridgeMeshSectionDescriptor_Statics::NewStructOps, TEXT("BridgeMeshSectionDescriptor"),&Z_Registration_Info_UScriptStruct_FBridgeMeshSectionDescriptor, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeMeshSectionDescriptor), 1454218802U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeProtocol_h__Script_GeometryNodesBridgeRuntime_8beae4cee85518dabf0f3cfc7606a18a58b9f375{
	TEXT("/Script/GeometryNodesBridgeRuntime"),
	nullptr, 0,
	UHT_STATICS::ScriptStructInfo, UE_ARRAY_COUNT(UHT_STATICS::ScriptStructInfo),
	UHT_STATICS::EnumInfo, UE_ARRAY_COUNT(UHT_STATICS::EnumInfo),
	nullptr, 0,
};
#undef UHT_STATICS
// ********** End Registration *********************************************************************
#undef UHT_STRUCT_BASE

PRAGMA_ENABLE_DEPRECATION_WARNINGS
