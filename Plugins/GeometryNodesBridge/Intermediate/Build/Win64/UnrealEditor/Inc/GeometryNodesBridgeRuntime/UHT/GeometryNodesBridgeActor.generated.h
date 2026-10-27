// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GeometryNodesBridgeActor.h"

#ifdef GEOMETRYNODESBRIDGERUNTIME_GeometryNodesBridgeActor_generated_h
#error "GeometryNodesBridgeActor.generated.h already included, missing '#pragma once' in GeometryNodesBridgeActor.h"
#endif
#define GEOMETRYNODESBRIDGERUNTIME_GeometryNodesBridgeActor_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGeometryNodesBridgeComponent;
class USplineComponent;

// ********** Begin Class AGeometryNodesBridgeActor ************************************************
#define FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execGetBridgeSplineComponent); \
	DECLARE_FUNCTION(execGetBridgeComponent); \
	DECLARE_FUNCTION(execDeleteBakedOutput); \
	DECLARE_FUNCTION(execSelectBakedOutput); \
	DECLARE_FUNCTION(execBakeToStaticMesh); \
	DECLARE_FUNCTION(execRequestCook); \
	DECLARE_FUNCTION(execRefreshInputSchema);


struct Z_Construct_UClass_AGeometryNodesBridgeActor_Statics;
GEOMETRYNODESBRIDGERUNTIME_API UClass* Z_Construct_UClass_AGeometryNodesBridgeActor(ETypeConstructPhase);

#define FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h_13_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_AGeometryNodesBridgeActor_Statics; \
	friend GEOMETRYNODESBRIDGERUNTIME_API UClass* ::Z_Construct_UClass_AGeometryNodesBridgeActor(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(AGeometryNodesBridgeActor, AActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GeometryNodesBridgeRuntime"), Z_Construct_UClass_AGeometryNodesBridgeActor) \
	DECLARE_SERIALIZER(AGeometryNodesBridgeActor)


#define FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h_13_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	AGeometryNodesBridgeActor(AGeometryNodesBridgeActor&&) = delete; \
	AGeometryNodesBridgeActor(const AGeometryNodesBridgeActor&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AGeometryNodesBridgeActor); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AGeometryNodesBridgeActor); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AGeometryNodesBridgeActor) \
	NO_API virtual ~AGeometryNodesBridgeActor();


#define FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h_10_PROLOG
#define FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h_13_INCLASS_NO_PURE_DECLS \
	FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class AGeometryNodesBridgeActor;

// ********** End Class AGeometryNodesBridgeActor **************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Growth2_Plugins_GeometryNodesBridge_Source_GeometryNodesBridgeRuntime_Public_GeometryNodesBridgeActor_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
