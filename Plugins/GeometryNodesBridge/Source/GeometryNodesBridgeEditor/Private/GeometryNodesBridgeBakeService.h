#pragma once

class UGeometryNodesBridgeComponent;

class FGeometryNodesBridgeBakeService final
{
public:
    static bool BakeComponent(UGeometryNodesBridgeComponent* Component);
    static bool SelectBakedOutput(UGeometryNodesBridgeComponent* Component);
    static bool DeleteBakedOutput(UGeometryNodesBridgeComponent* Component);
};
