// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "BlenderBridgeProtocol.h"

#ifdef BLENDERBRIDGERUNTIME_BlenderBridgeProtocol_generated_h
#error "BlenderBridgeProtocol.generated.h already included, missing '#pragma once' in BlenderBridgeProtocol.h"
#endif
#define BLENDERBRIDGERUNTIME_BlenderBridgeProtocol_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin ScriptStruct FBridgeEnumOption *************************************************
struct Z_Construct_UScriptStruct_FBridgeEnumOption_Statics;
#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h_47_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FBridgeEnumOption_Statics; \
	BLENDERBRIDGERUNTIME_API static class UScriptStruct* StaticStruct();


struct FBridgeEnumOption;
// ********** End ScriptStruct FBridgeEnumOption ***************************************************

// ********** Begin ScriptStruct FBridgeInputSchema ************************************************
struct Z_Construct_UScriptStruct_FBridgeInputSchema_Statics;
#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h_62_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FBridgeInputSchema_Statics; \
	BLENDERBRIDGERUNTIME_API static class UScriptStruct* StaticStruct();


struct FBridgeInputSchema;
// ********** End ScriptStruct FBridgeInputSchema **************************************************

// ********** Begin ScriptStruct FBridgeTargetDescriptor *******************************************
struct Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics;
#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h_110_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FBridgeTargetDescriptor_Statics; \
	BLENDERBRIDGERUNTIME_API static class UScriptStruct* StaticStruct();


struct FBridgeTargetDescriptor;
// ********** End ScriptStruct FBridgeTargetDescriptor *********************************************

// ********** Begin ScriptStruct FBridgeMaterialSlotDescriptor *************************************
struct Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor_Statics;
#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h_128_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FBridgeMaterialSlotDescriptor_Statics; \
	BLENDERBRIDGERUNTIME_API static class UScriptStruct* StaticStruct();


struct FBridgeMaterialSlotDescriptor;
// ********** End ScriptStruct FBridgeMaterialSlotDescriptor ***************************************

// ********** Begin ScriptStruct FBridgeMeshSectionDescriptor **************************************
struct Z_Construct_UScriptStruct_FBridgeMeshSectionDescriptor_Statics;
#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h_146_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FBridgeMeshSectionDescriptor_Statics; \
	BLENDERBRIDGERUNTIME_API static class UScriptStruct* StaticStruct();


struct FBridgeMeshSectionDescriptor;
// ********** End ScriptStruct FBridgeMeshSectionDescriptor ****************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeProtocol_h

// ********** Begin Enum EBlenderBridgeConnectionState *********************************************
#define FOREACH_ENUM_EBLENDERBRIDGECONNECTIONSTATE(op) \
	op(EBlenderBridgeConnectionState::Disconnected) \
	op(EBlenderBridgeConnectionState::Launching) \
	op(EBlenderBridgeConnectionState::Connected) \
	op(EBlenderBridgeConnectionState::Error) 

enum class EBlenderBridgeConnectionState : uint8;
template<> struct TIsUEnumClass<EBlenderBridgeConnectionState> { enum { Value = true }; };
template<> BLENDERBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EBlenderBridgeConnectionState>();
// ********** End Enum EBlenderBridgeConnectionState ***********************************************

// ********** Begin Enum EBlenderBridgeComponentStatus *********************************************
#define FOREACH_ENUM_EBLENDERBRIDGECOMPONENTSTATUS(op) \
	op(EBlenderBridgeComponentStatus::NoSession) \
	op(EBlenderBridgeComponentStatus::SourceNotLoaded) \
	op(EBlenderBridgeComponentStatus::TargetUnresolved) \
	op(EBlenderBridgeComponentStatus::SchemaPending) \
	op(EBlenderBridgeComponentStatus::Ready) \
	op(EBlenderBridgeComponentStatus::CookQueued) \
	op(EBlenderBridgeComponentStatus::Cooking) \
	op(EBlenderBridgeComponentStatus::Cooked) \
	op(EBlenderBridgeComponentStatus::Error) 

enum class EBlenderBridgeComponentStatus : uint8;
template<> struct TIsUEnumClass<EBlenderBridgeComponentStatus> { enum { Value = true }; };
template<> BLENDERBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EBlenderBridgeComponentStatus>();
// ********** End Enum EBlenderBridgeComponentStatus ***********************************************

// ********** Begin Enum EBlenderBridgeInputType ***************************************************
#define FOREACH_ENUM_EBLENDERBRIDGEINPUTTYPE(op) \
	op(EBlenderBridgeInputType::Float) \
	op(EBlenderBridgeInputType::Integer) \
	op(EBlenderBridgeInputType::Boolean) \
	op(EBlenderBridgeInputType::Vector3) \
	op(EBlenderBridgeInputType::Color) \
	op(EBlenderBridgeInputType::String) \
	op(EBlenderBridgeInputType::Enum) \
	op(EBlenderBridgeInputType::Unsupported) 

enum class EBlenderBridgeInputType : uint8;
template<> struct TIsUEnumClass<EBlenderBridgeInputType> { enum { Value = true }; };
template<> BLENDERBRIDGERUNTIME_NON_ATTRIBUTED_API UEnum* StaticEnum<EBlenderBridgeInputType>();
// ********** End Enum EBlenderBridgeInputType *****************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
