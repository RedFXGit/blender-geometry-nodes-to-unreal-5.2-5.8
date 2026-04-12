#pragma once

#include "GeometryNodesBridgeProtocol.h"
#include "HAL/PlatformMemory.h"

class FGeometryNodesBridgeSharedMemoryRegion final
{
public:
    ~FGeometryNodesBridgeSharedMemoryRegion();

    bool Create(const FString& InName, uint64 InTotalBytes);
    bool Open(const FString& InName, uint64 InTotalBytes);
    bool Remap(uint64 InTotalBytes);
    void Reset();

    bool IsValid() const { return Region != nullptr; }
    const FString& GetName() const { return Name; }
    uint64 GetSize() const { return SizeInBytes; }

    FBridgeSharedHeader* GetHeader();
    const FBridgeSharedHeader* GetHeader() const;

    bool ReadMesh(uint32 SlotIndex, FBridgeDecodedMesh& OutMesh) const;

private:
    bool Map(const FString& InName, bool bCreate, uint64 InTotalBytes);
    void InitializeHeader();
    const uint8* GetSlotPayloadBase(uint32 SlotIndex) const;

private:
    FString Name;
    uint64 SizeInBytes = 0;
    FPlatformMemory::FSharedMemoryRegion* Region = nullptr;
};

class FGeometryNodesBridgeInstanceBakeSharedMemoryRegion final
{
public:
    ~FGeometryNodesBridgeInstanceBakeSharedMemoryRegion();

    bool Create(const FString& InName, uint64 InTotalBytes);
    bool Open(const FString& InName, uint64 InTotalBytes);
    bool Remap(uint64 InTotalBytes);
    void Reset();

    bool IsValid() const { return Region != nullptr; }
    const FString& GetName() const { return Name; }
    uint64 GetSize() const { return SizeInBytes; }

    FBridgeInstanceBakeSharedHeader* GetHeader();
    const FBridgeInstanceBakeSharedHeader* GetHeader() const;

    bool ReadInstanceBake(uint32 SlotIndex, FBridgeDecodedInstanceBake& OutInstanceBake) const;

private:
    bool Map(const FString& InName, bool bCreate, uint64 InTotalBytes);
    void InitializeHeader();
    const uint8* GetSlotPayloadBase(uint32 SlotIndex) const;

private:
    FString Name;
    uint64 SizeInBytes = 0;
    FPlatformMemory::FSharedMemoryRegion* Region = nullptr;
};
