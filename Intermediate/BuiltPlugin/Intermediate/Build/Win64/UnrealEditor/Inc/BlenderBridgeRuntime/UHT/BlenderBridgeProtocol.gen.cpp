// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BlenderBridgeProtocol.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeBlenderBridgeProtocol() {}

// ********** Begin Cross Module References ********************************************************
BLENDERBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus();
BLENDERBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState();
BLENDERBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType();
BLENDERBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeEnumOption();
BLENDERBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeInputSchema();
BLENDERBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeTargetDescriptor();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FLinearColor();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
UPackage* Z_Construct_UPackage__Script_BlenderBridgeRuntime();
// ********** End Cross Module References **********************************************************

// ********** Begin Enum EBlenderBridgeConnectionState *********************************************
static FEnumRegistrationInfo Z_Registration_Info_UEnum_EBlenderBridgeConnectionState;
static UEnum* EBlenderBridgeConnectionState_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_EBlenderBridgeConnectionState.OuterSingleton)
	{
		Z_Registration_Info_UEnum_EBlenderBridgeConnectionState.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState, (UObject*)Z_Construct_UPackage__Script_BlenderBridgeRuntime(), TEXT("EBlenderBridgeConnectionState"));
	}
	return Z_Registration_Info_UEnum_EBlenderBridgeConnectionState.OuterSingleton;
}
template<> BLENDERBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EBlenderBridgeConnectionState>()
{
	return EBlenderBridgeConnectionState_StaticEnum();
}
struct Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Connected.Name", "EBlenderBridgeConnectionState::Connected" },
		{ "Disconnected.Name", "EBlenderBridgeConnectionState::Disconnected" },
		{ "Error.Name", "EBlenderBridgeConnectionState::Error" },
		{ "Launching.Name", "EBlenderBridgeConnectionState::Launching" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EBlenderBridgeConnectionState::Disconnected", (int64)EBlenderBridgeConnectionState::Disconnected },
		{ "EBlenderBridgeConnectionState::Launching", (int64)EBlenderBridgeConnectionState::Launching },
		{ "EBlenderBridgeConnectionState::Connected", (int64)EBlenderBridgeConnectionState::Connected },
		{ "EBlenderBridgeConnectionState::Error", (int64)EBlenderBridgeConnectionState::Error },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState_Statics 
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
	nullptr,
	"EBlenderBridgeConnectionState",
	"EBlenderBridgeConnectionState",
	Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState_Statics::Enum_MetaDataParams), Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState()
{
	if (!Z_Registration_Info_UEnum_EBlenderBridgeConnectionState.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EBlenderBridgeConnectionState.InnerSingleton, Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_EBlenderBridgeConnectionState.InnerSingleton;
}
// ********** End Enum EBlenderBridgeConnectionState ***********************************************

// ********** Begin Enum EBlenderBridgeComponentStatus *********************************************
static FEnumRegistrationInfo Z_Registration_Info_UEnum_EBlenderBridgeComponentStatus;
static UEnum* EBlenderBridgeComponentStatus_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_EBlenderBridgeComponentStatus.OuterSingleton)
	{
		Z_Registration_Info_UEnum_EBlenderBridgeComponentStatus.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus, (UObject*)Z_Construct_UPackage__Script_BlenderBridgeRuntime(), TEXT("EBlenderBridgeComponentStatus"));
	}
	return Z_Registration_Info_UEnum_EBlenderBridgeComponentStatus.OuterSingleton;
}
template<> BLENDERBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EBlenderBridgeComponentStatus>()
{
	return EBlenderBridgeComponentStatus_StaticEnum();
}
struct Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Cooked.Name", "EBlenderBridgeComponentStatus::Cooked" },
		{ "Cooking.Name", "EBlenderBridgeComponentStatus::Cooking" },
		{ "CookQueued.Name", "EBlenderBridgeComponentStatus::CookQueued" },
		{ "Error.Name", "EBlenderBridgeComponentStatus::Error" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
		{ "NoSession.Name", "EBlenderBridgeComponentStatus::NoSession" },
		{ "Ready.Name", "EBlenderBridgeComponentStatus::Ready" },
		{ "SchemaPending.Name", "EBlenderBridgeComponentStatus::SchemaPending" },
		{ "SourceNotLoaded.Name", "EBlenderBridgeComponentStatus::SourceNotLoaded" },
		{ "TargetUnresolved.Name", "EBlenderBridgeComponentStatus::TargetUnresolved" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EBlenderBridgeComponentStatus::NoSession", (int64)EBlenderBridgeComponentStatus::NoSession },
		{ "EBlenderBridgeComponentStatus::SourceNotLoaded", (int64)EBlenderBridgeComponentStatus::SourceNotLoaded },
		{ "EBlenderBridgeComponentStatus::TargetUnresolved", (int64)EBlenderBridgeComponentStatus::TargetUnresolved },
		{ "EBlenderBridgeComponentStatus::SchemaPending", (int64)EBlenderBridgeComponentStatus::SchemaPending },
		{ "EBlenderBridgeComponentStatus::Ready", (int64)EBlenderBridgeComponentStatus::Ready },
		{ "EBlenderBridgeComponentStatus::CookQueued", (int64)EBlenderBridgeComponentStatus::CookQueued },
		{ "EBlenderBridgeComponentStatus::Cooking", (int64)EBlenderBridgeComponentStatus::Cooking },
		{ "EBlenderBridgeComponentStatus::Cooked", (int64)EBlenderBridgeComponentStatus::Cooked },
		{ "EBlenderBridgeComponentStatus::Error", (int64)EBlenderBridgeComponentStatus::Error },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus_Statics 
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
	nullptr,
	"EBlenderBridgeComponentStatus",
	"EBlenderBridgeComponentStatus",
	Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus_Statics::Enum_MetaDataParams), Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus()
{
	if (!Z_Registration_Info_UEnum_EBlenderBridgeComponentStatus.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EBlenderBridgeComponentStatus.InnerSingleton, Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeComponentStatus_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_EBlenderBridgeComponentStatus.InnerSingleton;
}
// ********** End Enum EBlenderBridgeComponentStatus ***********************************************

// ********** Begin Enum EBlenderBridgeInputType ***************************************************
static FEnumRegistrationInfo Z_Registration_Info_UEnum_EBlenderBridgeInputType;
static UEnum* EBlenderBridgeInputType_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_EBlenderBridgeInputType.OuterSingleton)
	{
		Z_Registration_Info_UEnum_EBlenderBridgeInputType.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType, (UObject*)Z_Construct_UPackage__Script_BlenderBridgeRuntime(), TEXT("EBlenderBridgeInputType"));
	}
	return Z_Registration_Info_UEnum_EBlenderBridgeInputType.OuterSingleton;
}
template<> BLENDERBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EBlenderBridgeInputType>()
{
	return EBlenderBridgeInputType_StaticEnum();
}
struct Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Boolean.Name", "EBlenderBridgeInputType::Boolean" },
		{ "Color.Name", "EBlenderBridgeInputType::Color" },
		{ "Enum.Name", "EBlenderBridgeInputType::Enum" },
		{ "Float.Name", "EBlenderBridgeInputType::Float" },
		{ "Integer.Name", "EBlenderBridgeInputType::Integer" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
		{ "String.Name", "EBlenderBridgeInputType::String" },
		{ "Unsupported.Name", "EBlenderBridgeInputType::Unsupported" },
		{ "Vector3.Name", "EBlenderBridgeInputType::Vector3" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EBlenderBridgeInputType::Float", (int64)EBlenderBridgeInputType::Float },
		{ "EBlenderBridgeInputType::Integer", (int64)EBlenderBridgeInputType::Integer },
		{ "EBlenderBridgeInputType::Boolean", (int64)EBlenderBridgeInputType::Boolean },
		{ "EBlenderBridgeInputType::Vector3", (int64)EBlenderBridgeInputType::Vector3 },
		{ "EBlenderBridgeInputType::Color", (int64)EBlenderBridgeInputType::Color },
		{ "EBlenderBridgeInputType::String", (int64)EBlenderBridgeInputType::String },
		{ "EBlenderBridgeInputType::Enum", (int64)EBlenderBridgeInputType::Enum },
		{ "EBlenderBridgeInputType::Unsupported", (int64)EBlenderBridgeInputType::Unsupported },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType_Statics 
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
	nullptr,
	"EBlenderBridgeInputType",
	"EBlenderBridgeInputType",
	Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType_Statics::Enum_MetaDataParams), Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType()
{
	if (!Z_Registration_Info_UEnum_EBlenderBridgeInputType.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EBlenderBridgeInputType.InnerSingleton, Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_EBlenderBridgeInputType.InnerSingleton;
}
// ********** End Enum EBlenderBridgeInputType *****************************************************

// ********** Begin ScriptStruct FBridgeEnumOption *************************************************
struct Z_Construct_UScriptStruct_FBridgeEnumOption_Statics
{
	static inline consteval int32 GetStructSize() { return sizeof(FBridgeEnumOption); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeEnumOption); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Id_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Label_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Value_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
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
}; // struct Z_Construct_UScriptStruct_FBridgeEnumOption_Statics
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeEnumOption;
class UScriptStruct* FBridgeEnumOption::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FBridgeEnumOption.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FBridgeEnumOption.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeEnumOption, (UObject*)Z_Construct_UPackage__Script_BlenderBridgeRuntime(), TEXT("BridgeEnumOption"));
	}
	return Z_Registration_Info_UScriptStruct_FBridgeEnumOption.OuterSingleton;
	}

// ********** Begin ScriptStruct FBridgeEnumOption Property Definitions ****************************
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::NewProp_Id = { "Id", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeEnumOption, Id), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Id_MetaData), NewProp_Id_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::NewProp_Label = { "Label", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeEnumOption, Label), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Label_MetaData), NewProp_Label_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::NewProp_Value = { "Value", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeEnumOption, Value), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Value_MetaData), NewProp_Value_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::NewProp_Id,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::NewProp_Label,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::NewProp_Value,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeEnumOption Property Definitions ******************************
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeEnumOption",
	Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::PropPointers),
	sizeof(FBridgeEnumOption),
	alignof(FBridgeEnumOption),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FBridgeEnumOption()
{
	if (!Z_Registration_Info_UScriptStruct_FBridgeEnumOption.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeEnumOption.InnerSingleton, Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeEnumOption.InnerSingleton);
}
// ********** End ScriptStruct FBridgeEnumOption ***************************************************

// ********** Begin ScriptStruct FBridgeInputSchema ************************************************
struct Z_Construct_UScriptStruct_FBridgeInputSchema_Statics
{
	static inline consteval int32 GetStructSize() { return sizeof(FBridgeInputSchema); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeInputSchema); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Id_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Label_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CategoryPath_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Description_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Type_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultNumericValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MinNumericValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxNumericValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bDefaultBoolValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultVectorValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultColorValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultStringValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultEnumValue_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EnumOptions_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
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
	static void NewProp_bDefaultBoolValue_SetBit(void* Obj);
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
}; // struct Z_Construct_UScriptStruct_FBridgeInputSchema_Statics
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeInputSchema;
class UScriptStruct* FBridgeInputSchema::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FBridgeInputSchema.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FBridgeInputSchema.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeInputSchema, (UObject*)Z_Construct_UPackage__Script_BlenderBridgeRuntime(), TEXT("BridgeInputSchema"));
	}
	return Z_Registration_Info_UScriptStruct_FBridgeInputSchema.OuterSingleton;
	}

// ********** Begin ScriptStruct FBridgeInputSchema Property Definitions ***************************
const UECodeGen_Private::FNamePropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Id = { "Id", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, Id), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Id_MetaData), NewProp_Id_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Label = { "Label", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, Label), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Label_MetaData), NewProp_Label_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_CategoryPath = { "CategoryPath", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, CategoryPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CategoryPath_MetaData), NewProp_CategoryPath_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Description = { "Description", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, Description), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Description_MetaData), NewProp_Description_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Type_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Type = { "Type", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, Type), Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeInputType, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Type_MetaData), NewProp_Type_MetaData) }; // 1526252165
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultNumericValue = { "DefaultNumericValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultNumericValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultNumericValue_MetaData), NewProp_DefaultNumericValue_MetaData) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_MinNumericValue = { "MinNumericValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, MinNumericValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MinNumericValue_MetaData), NewProp_MinNumericValue_MetaData) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_MaxNumericValue = { "MaxNumericValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, MaxNumericValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxNumericValue_MetaData), NewProp_MaxNumericValue_MetaData) };
void Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_bDefaultBoolValue_SetBit(void* Obj)
{
	((FBridgeInputSchema*)Obj)->bDefaultBoolValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_bDefaultBoolValue = { "bDefaultBoolValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(FBridgeInputSchema), &Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_bDefaultBoolValue_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bDefaultBoolValue_MetaData), NewProp_bDefaultBoolValue_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultVectorValue = { "DefaultVectorValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultVectorValue), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultVectorValue_MetaData), NewProp_DefaultVectorValue_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultColorValue = { "DefaultColorValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultColorValue), Z_Construct_UScriptStruct_FLinearColor, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultColorValue_MetaData), NewProp_DefaultColorValue_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultStringValue = { "DefaultStringValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultStringValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultStringValue_MetaData), NewProp_DefaultStringValue_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultEnumValue = { "DefaultEnumValue", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, DefaultEnumValue), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultEnumValue_MetaData), NewProp_DefaultEnumValue_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_EnumOptions_Inner = { "EnumOptions", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FBridgeEnumOption, METADATA_PARAMS(0, nullptr) }; // 2094061569
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_EnumOptions = { "EnumOptions", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeInputSchema, EnumOptions), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EnumOptions_MetaData), NewProp_EnumOptions_MetaData) }; // 2094061569
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Id,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Label,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_CategoryPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Description,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Type_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_Type,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultNumericValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_MinNumericValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_MaxNumericValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_bDefaultBoolValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultVectorValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultColorValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultStringValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_DefaultEnumValue,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_EnumOptions_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewProp_EnumOptions,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeInputSchema Property Definitions *****************************
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeInputSchema",
	Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::PropPointers),
	sizeof(FBridgeInputSchema),
	alignof(FBridgeInputSchema),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FBridgeInputSchema()
{
	if (!Z_Registration_Info_UScriptStruct_FBridgeInputSchema.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeInputSchema.InnerSingleton, Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeInputSchema.InnerSingleton);
}
// ********** End ScriptStruct FBridgeInputSchema **************************************************

// ********** Begin ScriptStruct FBridgeTargetDescriptor *******************************************
struct Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics
{
	static inline consteval int32 GetStructSize() { return sizeof(FBridgeTargetDescriptor); }
	static inline consteval int16 GetStructAlignment() { return alignof(FBridgeTargetDescriptor); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetId_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DisplayLabel_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ObjectName_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ModifierName_MetaData[] = {
		{ "Category", "Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeProtocol.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FBridgeTargetDescriptor constinit property declarations ***********
	static const UECodeGen_Private::FStrPropertyParams NewProp_TargetId;
	static const UECodeGen_Private::FStrPropertyParams NewProp_DisplayLabel;
	static const UECodeGen_Private::FStrPropertyParams NewProp_ObjectName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_ModifierName;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FBridgeTargetDescriptor constinit property declarations *************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FBridgeTargetDescriptor>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor;
class UScriptStruct* FBridgeTargetDescriptor::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FBridgeTargetDescriptor, (UObject*)Z_Construct_UPackage__Script_BlenderBridgeRuntime(), TEXT("BridgeTargetDescriptor"));
	}
	return Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.OuterSingleton;
	}

// ********** Begin ScriptStruct FBridgeTargetDescriptor Property Definitions **********************
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewProp_TargetId = { "TargetId", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetDescriptor, TargetId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetId_MetaData), NewProp_TargetId_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewProp_DisplayLabel = { "DisplayLabel", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetDescriptor, DisplayLabel), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DisplayLabel_MetaData), NewProp_DisplayLabel_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewProp_ObjectName = { "ObjectName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetDescriptor, ObjectName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ObjectName_MetaData), NewProp_ObjectName_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewProp_ModifierName = { "ModifierName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FBridgeTargetDescriptor, ModifierName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ModifierName_MetaData), NewProp_ModifierName_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewProp_TargetId,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewProp_DisplayLabel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewProp_ObjectName,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewProp_ModifierName,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::PropPointers) < 2048);
// ********** End ScriptStruct FBridgeTargetDescriptor Property Definitions ************************
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
	nullptr,
	&NewStructOps,
	"BridgeTargetDescriptor",
	Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::PropPointers),
	sizeof(FBridgeTargetDescriptor),
	alignof(FBridgeTargetDescriptor),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FBridgeTargetDescriptor()
{
	if (!Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.InnerSingleton, Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor.InnerSingleton);
}
// ********** End ScriptStruct FBridgeTargetDescriptor *********************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h__Script_BlenderBridgeRuntime_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ EBlenderBridgeConnectionState_StaticEnum, TEXT("EBlenderBridgeConnectionState"), &Z_Registration_Info_UEnum_EBlenderBridgeConnectionState, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 329663175U) },
		{ EBlenderBridgeComponentStatus_StaticEnum, TEXT("EBlenderBridgeComponentStatus"), &Z_Registration_Info_UEnum_EBlenderBridgeComponentStatus, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1326850390U) },
		{ EBlenderBridgeInputType_StaticEnum, TEXT("EBlenderBridgeInputType"), &Z_Registration_Info_UEnum_EBlenderBridgeInputType, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1526252165U) },
	};
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FBridgeEnumOption::StaticStruct, Z_Construct_UScriptStruct_FBridgeEnumOption_Statics::NewStructOps, TEXT("BridgeEnumOption"),&Z_Registration_Info_UScriptStruct_FBridgeEnumOption, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeEnumOption), 2094061569U) },
		{ FBridgeInputSchema::StaticStruct, Z_Construct_UScriptStruct_FBridgeInputSchema_Statics::NewStructOps, TEXT("BridgeInputSchema"),&Z_Registration_Info_UScriptStruct_FBridgeInputSchema, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeInputSchema), 3680569157U) },
		{ FBridgeTargetDescriptor::StaticStruct, Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics::NewStructOps, TEXT("BridgeTargetDescriptor"),&Z_Registration_Info_UScriptStruct_FBridgeTargetDescriptor, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FBridgeTargetDescriptor), 3240295580U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h__Script_BlenderBridgeRuntime_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h__Script_BlenderBridgeRuntime_1659580743{
	TEXT("/Script/BlenderBridgeRuntime"),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h__Script_BlenderBridgeRuntime_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h__Script_BlenderBridgeRuntime_Statics::ScriptStructInfo),
	Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h__Script_BlenderBridgeRuntime_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge_Intermediate_BuiltPlugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h__Script_BlenderBridgeRuntime_Statics::EnumInfo),
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
