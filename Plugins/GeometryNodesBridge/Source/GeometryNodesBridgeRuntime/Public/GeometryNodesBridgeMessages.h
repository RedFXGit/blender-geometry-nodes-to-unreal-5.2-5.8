#pragma once

#include "GeometryNodesBridgeProtocol.h"

class FJsonObject;

struct FGeometryNodesBridgeJsonMessage
{
    EGeometryNodesBridgeMessageType Type = EGeometryNodesBridgeMessageType::Unknown;
    TSharedPtr<FJsonObject> Payload;
};

namespace GeometryNodesBridge
{
    GEOMETRYNODESBRIDGERUNTIME_API bool SerializeBridgeMessage(EGeometryNodesBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload, FString& OutLine);
    GEOMETRYNODESBRIDGERUNTIME_API bool DeserializeBridgeMessage(const FString& Line, FGeometryNodesBridgeJsonMessage& OutMessage);
    GEOMETRYNODESBRIDGERUNTIME_API bool ParseInputSchema(const TSharedPtr<FJsonObject>& Payload, TArray<FBridgeInputSchema>& OutSchema);
    GEOMETRYNODESBRIDGERUNTIME_API bool ParseTargetCapabilities(const TSharedPtr<FJsonObject>& Payload, FBridgeTargetCapabilities& OutCapabilities);
    GEOMETRYNODESBRIDGERUNTIME_API bool ParseTargetDescriptors(const TSharedPtr<FJsonObject>& Payload, TArray<FBridgeTargetDescriptor>& OutTargets);
    GEOMETRYNODESBRIDGERUNTIME_API bool ParseGeometryInputDescriptors(
        const TSharedPtr<FJsonObject>& Payload,
        TArray<FBridgeGeometryInputDescriptor>& OutDescriptors);
    GEOMETRYNODESBRIDGERUNTIME_API bool ParseCookedMeshMetadata(
        const TSharedPtr<FJsonObject>& Payload,
        TArray<FBridgeMaterialSlotDescriptor>& OutMaterialSlots,
        TArray<FBridgeMeshSectionDescriptor>& OutSections);
    GEOMETRYNODESBRIDGERUNTIME_API void AddInputValuesToPayload(const TArray<FBridgeInputSchema>& Schema, const FInstancedPropertyBag& InputValues, const TSharedRef<FJsonObject>& Payload);
    GEOMETRYNODESBRIDGERUNTIME_API void AddCurveOwnerDataToPayload(const FBridgeCurveOwnerData& CurveOwnerData, const TSharedRef<FJsonObject>& Payload);
    GEOMETRYNODESBRIDGERUNTIME_API void AddGeometryBindingPayloadsToPayload(
        const TArray<FBridgeGeometryBindingPayload>& GeometryBindingPayloads,
        const TSharedRef<FJsonObject>& Payload);
    GEOMETRYNODESBRIDGERUNTIME_API bool SerializeCurveOwnerData(const FBridgeCurveOwnerData& CurveOwnerData, FString& OutSerializedJson);
    GEOMETRYNODESBRIDGERUNTIME_API bool SerializeGeometryBindingPayloads(
        const TArray<FBridgeGeometryBindingPayload>& GeometryBindingPayloads,
        FString& OutSerializedJson);
}
