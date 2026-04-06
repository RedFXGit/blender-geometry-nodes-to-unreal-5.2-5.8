// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "BlenderBridgeActor.h"

#ifdef BLENDERBRIDGERUNTIME_BlenderBridgeActor_generated_h
#error "BlenderBridgeActor.generated.h already included, missing '#pragma once' in BlenderBridgeActor.h"
#endif
#define BLENDERBRIDGERUNTIME_BlenderBridgeActor_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UBlenderBridgeComponent;

// ********** Begin Class ABlenderBridgeActor ******************************************************
#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h_12_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execGetBridgeComponent); \
	DECLARE_FUNCTION(execDeleteBakedOutput); \
	DECLARE_FUNCTION(execSelectBakedOutput); \
	DECLARE_FUNCTION(execBakeToStaticMesh); \
	DECLARE_FUNCTION(execRequestCook); \
	DECLARE_FUNCTION(execRefreshInputSchema);


struct Z_Construct_UClass_ABlenderBridgeActor_Statics;
BLENDERBRIDGERUNTIME_API UClass* Z_Construct_UClass_ABlenderBridgeActor_NoRegister();

#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h_12_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesABlenderBridgeActor(); \
	friend struct ::Z_Construct_UClass_ABlenderBridgeActor_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend BLENDERBRIDGERUNTIME_API UClass* ::Z_Construct_UClass_ABlenderBridgeActor_NoRegister(); \
public: \
	DECLARE_CLASS2(ABlenderBridgeActor, AActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/BlenderBridgeRuntime"), Z_Construct_UClass_ABlenderBridgeActor_NoRegister) \
	DECLARE_SERIALIZER(ABlenderBridgeActor)


#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h_12_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	ABlenderBridgeActor(ABlenderBridgeActor&&) = delete; \
	ABlenderBridgeActor(const ABlenderBridgeActor&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ABlenderBridgeActor); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ABlenderBridgeActor); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ABlenderBridgeActor) \
	NO_API virtual ~ABlenderBridgeActor();


#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h_9_PROLOG
#define FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h_12_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h_12_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h_12_INCLASS_NO_PURE_DECLS \
	FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h_12_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class ABlenderBridgeActor;

// ********** End Class ABlenderBridgeActor ********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_Lassiter_Documents_GitHub_blender_unreal_bridge__tmp_buildplugin_HostProject_Plugins_BlenderBridge_Source_BlenderBridgeRuntime_Public_BlenderBridgeActor_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
