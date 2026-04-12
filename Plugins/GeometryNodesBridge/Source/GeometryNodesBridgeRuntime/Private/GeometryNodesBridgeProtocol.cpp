#include "GeometryNodesBridgeProtocol.h"

const TCHAR* GeometryNodesBridge::LexToString(const EGeometryNodesBridgeMessageType Type)
{
    switch (Type)
    {
    case EGeometryNodesBridgeMessageType::Hello:
        return TEXT("Hello");
    case EGeometryNodesBridgeMessageType::EnsureSourceLoaded:
        return TEXT("EnsureSourceLoaded");
    case EGeometryNodesBridgeMessageType::SourceLoaded:
        return TEXT("SourceLoaded");
    case EGeometryNodesBridgeMessageType::DescribeTarget:
        return TEXT("DescribeTarget");
    case EGeometryNodesBridgeMessageType::TargetDescription:
        return TEXT("TargetDescription");
    case EGeometryNodesBridgeMessageType::CookTarget:
        return TEXT("CookTarget");
    case EGeometryNodesBridgeMessageType::RequestInstanceBake:
        return TEXT("RequestInstanceBake");
    case EGeometryNodesBridgeMessageType::ReleaseComponentState:
        return TEXT("ReleaseComponentState");
    case EGeometryNodesBridgeMessageType::CookComplete:
        return TEXT("CookComplete");
    case EGeometryNodesBridgeMessageType::InstanceBakeComplete:
        return TEXT("InstanceBakeComplete");
    case EGeometryNodesBridgeMessageType::ResizeRequired:
        return TEXT("ResizeRequired");
    case EGeometryNodesBridgeMessageType::Error:
        return TEXT("Error");
    case EGeometryNodesBridgeMessageType::Shutdown:
        return TEXT("Shutdown");
    default:
        return TEXT("Unknown");
    }
}

bool GeometryNodesBridge::TryParseMessageType(const FString& Text, EGeometryNodesBridgeMessageType& OutType)
{
    static const TMap<FString, EGeometryNodesBridgeMessageType> Lookup = {
        {TEXT("Hello"), EGeometryNodesBridgeMessageType::Hello},
        {TEXT("EnsureSourceLoaded"), EGeometryNodesBridgeMessageType::EnsureSourceLoaded},
        {TEXT("SourceLoaded"), EGeometryNodesBridgeMessageType::SourceLoaded},
        {TEXT("DescribeTarget"), EGeometryNodesBridgeMessageType::DescribeTarget},
        {TEXT("TargetDescription"), EGeometryNodesBridgeMessageType::TargetDescription},
        {TEXT("CookTarget"), EGeometryNodesBridgeMessageType::CookTarget},
        {TEXT("RequestInstanceBake"), EGeometryNodesBridgeMessageType::RequestInstanceBake},
        {TEXT("ReleaseComponentState"), EGeometryNodesBridgeMessageType::ReleaseComponentState},
        {TEXT("CookComplete"), EGeometryNodesBridgeMessageType::CookComplete},
        {TEXT("InstanceBakeComplete"), EGeometryNodesBridgeMessageType::InstanceBakeComplete},
        {TEXT("ResizeRequired"), EGeometryNodesBridgeMessageType::ResizeRequired},
        {TEXT("Error"), EGeometryNodesBridgeMessageType::Error},
        {TEXT("Shutdown"), EGeometryNodesBridgeMessageType::Shutdown}
    };

    if (const EGeometryNodesBridgeMessageType* Found = Lookup.Find(Text))
    {
        OutType = *Found;
        return true;
    }

    OutType = EGeometryNodesBridgeMessageType::Unknown;
    return false;
}
