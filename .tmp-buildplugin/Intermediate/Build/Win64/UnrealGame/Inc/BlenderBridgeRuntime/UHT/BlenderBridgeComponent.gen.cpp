// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "BlenderBridgeComponent.h"
#include "BlenderBridgeProtocol.h"
#include "StructUtils/PropertyBag.h"
#include "UObject/SoftObjectPath.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeBlenderBridgeComponent() {}

// ********** Begin Cross Module References ********************************************************
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_UBlenderBridgeComponent();
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_UBlenderBridgeComponent_NoRegister();
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_UBlenderBridgeSource_NoRegister();
BLENDERBRIDGERUNTIME_API UEnum* Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState();
BLENDERBRIDGERUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FBridgeInputSchema();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FFilePath();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FInstancedPropertyBag();
ENGINE_API UClass* Z_Construct_UClass_USceneComponent();
ENGINE_API UClass* Z_Construct_UClass_UUserDefinedEnum_NoRegister();
PROCEDURALMESHCOMPONENT_API UClass* Z_Construct_UClass_UProceduralMeshComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_BlenderBridgeRuntime();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UBlenderBridgeComponent Function PushCurrentInputs ***********************
struct Z_Construct_UFunction_UBlenderBridgeComponent_PushCurrentInputs_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function PushCurrentInputs constinit property declarations *********************
// ********** End Function PushCurrentInputs constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBlenderBridgeComponent_PushCurrentInputs_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBlenderBridgeComponent, nullptr, "PushCurrentInputs", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBlenderBridgeComponent_PushCurrentInputs_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBlenderBridgeComponent_PushCurrentInputs_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UBlenderBridgeComponent_PushCurrentInputs()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBlenderBridgeComponent_PushCurrentInputs_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBlenderBridgeComponent::execPushCurrentInputs)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->PushCurrentInputs();
	P_NATIVE_END;
}
// ********** End Class UBlenderBridgeComponent Function PushCurrentInputs *************************

// ********** Begin Class UBlenderBridgeComponent Function RefreshInputSchema **********************
struct Z_Construct_UFunction_UBlenderBridgeComponent_RefreshInputSchema_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function RefreshInputSchema constinit property declarations ********************
// ********** End Function RefreshInputSchema constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBlenderBridgeComponent_RefreshInputSchema_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBlenderBridgeComponent, nullptr, "RefreshInputSchema", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBlenderBridgeComponent_RefreshInputSchema_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBlenderBridgeComponent_RefreshInputSchema_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UBlenderBridgeComponent_RefreshInputSchema()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBlenderBridgeComponent_RefreshInputSchema_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBlenderBridgeComponent::execRefreshInputSchema)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->RefreshInputSchema();
	P_NATIVE_END;
}
// ********** End Class UBlenderBridgeComponent Function RefreshInputSchema ************************

// ********** Begin Class UBlenderBridgeComponent Function RequestCook *****************************
struct Z_Construct_UFunction_UBlenderBridgeComponent_RequestCook_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Blender Bridge" },
		{ "DisplayName", "Request Cook" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function RequestCook constinit property declarations ***************************
// ********** End Function RequestCook constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBlenderBridgeComponent_RequestCook_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBlenderBridgeComponent, nullptr, "RequestCook", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBlenderBridgeComponent_RequestCook_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBlenderBridgeComponent_RequestCook_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UBlenderBridgeComponent_RequestCook()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBlenderBridgeComponent_RequestCook_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBlenderBridgeComponent::execRequestCook)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->RequestCook();
	P_NATIVE_END;
}
// ********** End Class UBlenderBridgeComponent Function RequestCook *******************************

// ********** Begin Class UBlenderBridgeComponent Function StartBridge *****************************
struct Z_Construct_UFunction_UBlenderBridgeComponent_StartBridge_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function StartBridge constinit property declarations ***************************
// ********** End Function StartBridge constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBlenderBridgeComponent_StartBridge_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBlenderBridgeComponent, nullptr, "StartBridge", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBlenderBridgeComponent_StartBridge_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBlenderBridgeComponent_StartBridge_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UBlenderBridgeComponent_StartBridge()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBlenderBridgeComponent_StartBridge_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBlenderBridgeComponent::execStartBridge)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StartBridge();
	P_NATIVE_END;
}
// ********** End Class UBlenderBridgeComponent Function StartBridge *******************************

// ********** Begin Class UBlenderBridgeComponent Function StopBridge ******************************
struct Z_Construct_UFunction_UBlenderBridgeComponent_StopBridge_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "CallInEditor", "true" },
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function StopBridge constinit property declarations ****************************
// ********** End Function StopBridge constinit property declarations ******************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBlenderBridgeComponent_StopBridge_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBlenderBridgeComponent, nullptr, "StopBridge", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBlenderBridgeComponent_StopBridge_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBlenderBridgeComponent_StopBridge_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UBlenderBridgeComponent_StopBridge()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBlenderBridgeComponent_StopBridge_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBlenderBridgeComponent::execStopBridge)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StopBridge();
	P_NATIVE_END;
}
// ********** End Class UBlenderBridgeComponent Function StopBridge ********************************

// ********** Begin Class UBlenderBridgeComponent **************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UBlenderBridgeComponent;
UClass* UBlenderBridgeComponent::GetPrivateStaticClass()
{
	using TClass = UBlenderBridgeComponent;
	if (!Z_Registration_Info_UClass_UBlenderBridgeComponent.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("BlenderBridgeComponent"),
			Z_Registration_Info_UClass_UBlenderBridgeComponent.InnerSingleton,
			StaticRegisterNativesUBlenderBridgeComponent,
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
	return Z_Registration_Info_UClass_UBlenderBridgeComponent.InnerSingleton;
}
UClass* Z_Construct_UClass_UBlenderBridgeComponent_NoRegister()
{
	return UBlenderBridgeComponent::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UBlenderBridgeComponent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Blender" },
		{ "HideCategories", "Trigger PhysicsVolume" },
		{ "IncludePath", "BlenderBridgeComponent.h" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Source_MetaData[] = {
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BlenderExecutablePath_MetaData[] = {
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SharedMemoryBytes_MetaData[] = {
		{ "Category", "Blender Bridge" },
		{ "ClampMin", "1048576" },
		{ "DisplayName", "Shared Memory Bytes (Total)" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
		{ "ToolTip", "Total shared-memory budget across both bridge slots. The default is 250 MiB per slot, 500 MiB total." },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ConnectionState_MetaData[] = {
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LastStatusMessage_MetaData[] = {
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BridgeBuildVersion_MetaData[] = {
		{ "Category", "Blender Bridge" },
		{ "DisplayName", "Bridge Build" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InputSchema_MetaData[] = {
		{ "Category", "Blender Bridge" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InputValues_MetaData[] = {
		{ "BlenderBridgeGroupedLayout", "" },
		{ "Category", "Blender Bridge" },
		{ "FixedLayout", "" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PreviewMeshComponent_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RuntimeInputEnums_MetaData[] = {
		{ "ModuleRelativePath", "Public/BlenderBridgeComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UBlenderBridgeComponent constinit property declarations ******************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Source;
	static const UECodeGen_Private::FStructPropertyParams NewProp_BlenderExecutablePath;
	static const UECodeGen_Private::FInt64PropertyParams NewProp_SharedMemoryBytes;
	static const UECodeGen_Private::FBytePropertyParams NewProp_ConnectionState_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_ConnectionState;
	static const UECodeGen_Private::FStrPropertyParams NewProp_LastStatusMessage;
	static const UECodeGen_Private::FStrPropertyParams NewProp_BridgeBuildVersion;
	static const UECodeGen_Private::FStructPropertyParams NewProp_InputSchema_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_InputSchema;
	static const UECodeGen_Private::FStructPropertyParams NewProp_InputValues;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_PreviewMeshComponent;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_RuntimeInputEnums_ValueProp;
	static const UECodeGen_Private::FNamePropertyParams NewProp_RuntimeInputEnums_Key_KeyProp;
	static const UECodeGen_Private::FMapPropertyParams NewProp_RuntimeInputEnums;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UBlenderBridgeComponent constinit property declarations ********************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("PushCurrentInputs"), .Pointer = &UBlenderBridgeComponent::execPushCurrentInputs },
		{ .NameUTF8 = UTF8TEXT("RefreshInputSchema"), .Pointer = &UBlenderBridgeComponent::execRefreshInputSchema },
		{ .NameUTF8 = UTF8TEXT("RequestCook"), .Pointer = &UBlenderBridgeComponent::execRequestCook },
		{ .NameUTF8 = UTF8TEXT("StartBridge"), .Pointer = &UBlenderBridgeComponent::execStartBridge },
		{ .NameUTF8 = UTF8TEXT("StopBridge"), .Pointer = &UBlenderBridgeComponent::execStopBridge },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UBlenderBridgeComponent_PushCurrentInputs, "PushCurrentInputs" }, // 449137774
		{ &Z_Construct_UFunction_UBlenderBridgeComponent_RefreshInputSchema, "RefreshInputSchema" }, // 892428668
		{ &Z_Construct_UFunction_UBlenderBridgeComponent_RequestCook, "RequestCook" }, // 2731516328
		{ &Z_Construct_UFunction_UBlenderBridgeComponent_StartBridge, "StartBridge" }, // 1598543793
		{ &Z_Construct_UFunction_UBlenderBridgeComponent_StopBridge, "StopBridge" }, // 2473744594
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBlenderBridgeComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UBlenderBridgeComponent_Statics

// ********** Begin Class UBlenderBridgeComponent Property Definitions *****************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_Source = { "Source", nullptr, (EPropertyFlags)0x0114000000000015, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, Source), Z_Construct_UClass_UBlenderBridgeSource_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Source_MetaData), NewProp_Source_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_BlenderExecutablePath = { "BlenderExecutablePath", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, BlenderExecutablePath), Z_Construct_UScriptStruct_FFilePath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BlenderExecutablePath_MetaData), NewProp_BlenderExecutablePath_MetaData) }; // 1592925316
const UECodeGen_Private::FInt64PropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_SharedMemoryBytes = { "SharedMemoryBytes", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Int64, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, SharedMemoryBytes), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SharedMemoryBytes_MetaData), NewProp_SharedMemoryBytes_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_ConnectionState_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_ConnectionState = { "ConnectionState", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, ConnectionState), Z_Construct_UEnum_BlenderBridgeRuntime_EBlenderBridgeConnectionState, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ConnectionState_MetaData), NewProp_ConnectionState_MetaData) }; // 329663175
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_LastStatusMessage = { "LastStatusMessage", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, LastStatusMessage), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LastStatusMessage_MetaData), NewProp_LastStatusMessage_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_BridgeBuildVersion = { "BridgeBuildVersion", nullptr, (EPropertyFlags)0x0010000000022015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, BridgeBuildVersion), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BridgeBuildVersion_MetaData), NewProp_BridgeBuildVersion_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_InputSchema_Inner = { "InputSchema", nullptr, (EPropertyFlags)0x0000000000020000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FBridgeInputSchema, METADATA_PARAMS(0, nullptr) }; // 3680569157
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_InputSchema = { "InputSchema", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, InputSchema), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InputSchema_MetaData), NewProp_InputSchema_MetaData) }; // 3680569157
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_InputValues = { "InputValues", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, InputValues), Z_Construct_UScriptStruct_FInstancedPropertyBag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InputValues_MetaData), NewProp_InputValues_MetaData) }; // 1261298821
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_PreviewMeshComponent = { "PreviewMeshComponent", nullptr, (EPropertyFlags)0x0144000000082008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, PreviewMeshComponent), Z_Construct_UClass_UProceduralMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PreviewMeshComponent_MetaData), NewProp_PreviewMeshComponent_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_RuntimeInputEnums_ValueProp = { "RuntimeInputEnums", nullptr, (EPropertyFlags)0x0104000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 1, Z_Construct_UClass_UUserDefinedEnum_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_RuntimeInputEnums_Key_KeyProp = { "RuntimeInputEnums_Key", nullptr, (EPropertyFlags)0x0100000000000000, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FMapPropertyParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_RuntimeInputEnums = { "RuntimeInputEnums", nullptr, (EPropertyFlags)0x0144000000002000, UECodeGen_Private::EPropertyGenFlags::Map, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBlenderBridgeComponent, RuntimeInputEnums), EMapPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RuntimeInputEnums_MetaData), NewProp_RuntimeInputEnums_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UBlenderBridgeComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_Source,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_BlenderExecutablePath,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_SharedMemoryBytes,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_ConnectionState_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_ConnectionState,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_LastStatusMessage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_BridgeBuildVersion,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_InputSchema_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_InputSchema,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_InputValues,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_PreviewMeshComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_RuntimeInputEnums_ValueProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_RuntimeInputEnums_Key_KeyProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBlenderBridgeComponent_Statics::NewProp_RuntimeInputEnums,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeComponent_Statics::PropPointers) < 2048);
// ********** End Class UBlenderBridgeComponent Property Definitions *******************************
UObject* (*const Z_Construct_UClass_UBlenderBridgeComponent_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USceneComponent,
	(UObject* (*)())Z_Construct_UPackage__Script_BlenderBridgeRuntime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeComponent_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UBlenderBridgeComponent_Statics::ClassParams = {
	&UBlenderBridgeComponent::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UBlenderBridgeComponent_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeComponent_Statics::PropPointers),
	0,
	0x00B000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UBlenderBridgeComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_UBlenderBridgeComponent_Statics::Class_MetaDataParams)
};
void UBlenderBridgeComponent::StaticRegisterNativesUBlenderBridgeComponent()
{
	UClass* Class = UBlenderBridgeComponent::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UBlenderBridgeComponent_Statics::Funcs));
}
UClass* Z_Construct_UClass_UBlenderBridgeComponent()
{
	if (!Z_Registration_Info_UClass_UBlenderBridgeComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UBlenderBridgeComponent.OuterSingleton, Z_Construct_UClass_UBlenderBridgeComponent_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UBlenderBridgeComponent.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UBlenderBridgeComponent);
UBlenderBridgeComponent::~UBlenderBridgeComponent() {}
// ********** End Class UBlenderBridgeComponent ****************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeComponent_h__Script_BlenderBridgeRuntime_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UBlenderBridgeComponent, UBlenderBridgeComponent::StaticClass, TEXT("UBlenderBridgeComponent"), &Z_Registration_Info_UClass_UBlenderBridgeComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UBlenderBridgeComponent), 2457211173U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeComponent_h__Script_BlenderBridgeRuntime_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeComponent_h__Script_BlenderBridgeRuntime_3377950077{
	TEXT("/Script/BlenderBridgeRuntime"),
	Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeComponent_h__Script_BlenderBridgeRuntime_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeComponent_h__Script_BlenderBridgeRuntime_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
