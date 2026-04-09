#include "BlenderBridgeProtocol.h"

const TCHAR* BlenderBridge::LexToString(const EBlenderBridgeMessageType Type)
{
    switch (Type)
    {
    case EBlenderBridgeMessageType::Hello:
        return TEXT("Hello");
    case EBlenderBridgeMessageType::EnsureSourceLoaded:
        return TEXT("EnsureSourceLoaded");
    case EBlenderBridgeMessageType::SourceLoaded:
        return TEXT("SourceLoaded");
    case EBlenderBridgeMessageType::DescribeTarget:
        return TEXT("DescribeTarget");
    case EBlenderBridgeMessageType::TargetDescription:
        return TEXT("TargetDescription");
    case EBlenderBridgeMessageType::CookTarget:
        return TEXT("CookTarget");
    case EBlenderBridgeMessageType::RequestInstanceBake:
        return TEXT("RequestInstanceBake");
    case EBlenderBridgeMessageType::ReleaseComponentState:
        return TEXT("ReleaseComponentState");
    case EBlenderBridgeMessageType::CookComplete:
        return TEXT("CookComplete");
    case EBlenderBridgeMessageType::InstanceBakeComplete:
        return TEXT("InstanceBakeComplete");
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
        {TEXT("EnsureSourceLoaded"), EBlenderBridgeMessageType::EnsureSourceLoaded},
        {TEXT("SourceLoaded"), EBlenderBridgeMessageType::SourceLoaded},
        {TEXT("DescribeTarget"), EBlenderBridgeMessageType::DescribeTarget},
        {TEXT("TargetDescription"), EBlenderBridgeMessageType::TargetDescription},
        {TEXT("CookTarget"), EBlenderBridgeMessageType::CookTarget},
        {TEXT("RequestInstanceBake"), EBlenderBridgeMessageType::RequestInstanceBake},
        {TEXT("ReleaseComponentState"), EBlenderBridgeMessageType::ReleaseComponentState},
        {TEXT("CookComplete"), EBlenderBridgeMessageType::CookComplete},
        {TEXT("InstanceBakeComplete"), EBlenderBridgeMessageType::InstanceBakeComplete},
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
