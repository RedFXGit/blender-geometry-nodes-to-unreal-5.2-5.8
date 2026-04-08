#pragma once

#include "BlenderBridgeProtocol.h"

class FJsonObject;

struct FBlenderBridgeJsonMessage
{
    EBlenderBridgeMessageType Type = EBlenderBridgeMessageType::Unknown;
    TSharedPtr<FJsonObject> Payload;
};

namespace BlenderBridge
{
    BLENDERBRIDGERUNTIME_API bool SerializeBridgeMessage(EBlenderBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload, FString& OutLine);
    BLENDERBRIDGERUNTIME_API bool DeserializeBridgeMessage(const FString& Line, FBlenderBridgeJsonMessage& OutMessage);
    BLENDERBRIDGERUNTIME_API bool ParseInputSchema(const TSharedPtr<FJsonObject>& Payload, TArray<FBridgeInputSchema>& OutSchema);
    BLENDERBRIDGERUNTIME_API bool ParseTargetCapabilities(const TSharedPtr<FJsonObject>& Payload, FBridgeTargetCapabilities& OutCapabilities);
    BLENDERBRIDGERUNTIME_API bool ParseTargetDescriptors(const TSharedPtr<FJsonObject>& Payload, TArray<FBridgeTargetDescriptor>& OutTargets);
    BLENDERBRIDGERUNTIME_API bool ParseCookedMeshMetadata(
        const TSharedPtr<FJsonObject>& Payload,
        TArray<FBridgeMaterialSlotDescriptor>& OutMaterialSlots,
        TArray<FBridgeMeshSectionDescriptor>& OutSections);
    BLENDERBRIDGERUNTIME_API void AddInputValuesToPayload(const TArray<FBridgeInputSchema>& Schema, const FInstancedPropertyBag& InputValues, const TSharedRef<FJsonObject>& Payload);
    BLENDERBRIDGERUNTIME_API void AddCurveOwnerDataToPayload(const FBridgeCurveOwnerData& CurveOwnerData, const TSharedRef<FJsonObject>& Payload);
    BLENDERBRIDGERUNTIME_API bool SerializeCurveOwnerData(const FBridgeCurveOwnerData& CurveOwnerData, FString& OutSerializedJson);
}
