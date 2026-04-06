#pragma once

class UBlenderBridgeComponent;

class FBlenderBridgeBakeService final
{
public:
    static bool BakeComponent(UBlenderBridgeComponent* Component);
    static bool SelectBakedOutput(UBlenderBridgeComponent* Component);
    static bool DeleteBakedOutput(UBlenderBridgeComponent* Component);
};
