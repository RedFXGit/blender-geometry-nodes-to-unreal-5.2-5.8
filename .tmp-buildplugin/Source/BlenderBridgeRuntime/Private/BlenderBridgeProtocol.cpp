#include "BlenderBridgeProtocol.h"

const TCHAR* BlenderBridge::LexToString(const EBlenderBridgeMessageType Type)
{
    switch (Type)
    {
    case EBlenderBridgeMessageType::Hello:
        return TEXT("Hello");
    case EBlenderBridgeMessageType::LoadAsset:
        return TEXT("LoadAsset");
    case EBlenderBridgeMessageType::DescribeInputs:
        return TEXT("DescribeInputs");
    case EBlenderBridgeMessageType::SetInputs:
        return TEXT("SetInputs");
    case EBlenderBridgeMessageType::Cook:
        return TEXT("Cook");
    case EBlenderBridgeMessageType::CookComplete:
        return TEXT("CookComplete");
    case EBlenderBridgeMessageType::ResizeRequired:
        return TEXT("ResizeRequired");
    case EBlenderBridgeMessageType::Error:
        return TEXT("Error");
    case EBlenderBridgeMessageType::Shutdown:
        return TEXT("Shutdown");
    default:
        return TEXT("Unknown");
    }
}

bool BlenderBridge::TryParseMessageType(const FString& Text, EBlenderBridgeMessageType& OutType)
{
    static const TMap<FString, EBlenderBridgeMessageType> Lookup = {
        {TEXT("Hello"), EBlenderBridgeMessageType::Hello},
        {TEXT("LoadAsset"), EBlenderBridgeMessageType::LoadAsset},
        {TEXT("DescribeInputs"), EBlenderBridgeMessageType::DescribeInputs},
        {TEXT("SetInputs"), EBlenderBridgeMessageType::SetInputs},
        {TEXT("Cook"), EBlenderBridgeMessageType::Cook},
        {TEXT("CookComplete"), EBlenderBridgeMessageType::CookComplete},
        {TEXT("ResizeRequired"), EBlenderBridgeMessageType::ResizeRequired},
        {TEXT("Error"), EBlenderBridgeMessageType::Error},
        {TEXT("Shutdown"), EBlenderBridgeMessageType::Shutdown}
    };

    if (const EBlenderBridgeMessageType* Found = Lookup.Find(Text))
    {
        OutType = *Found;
        return true;
    }

    OutType = EBlenderBridgeMessageType::Unknown;
    return false;
}
