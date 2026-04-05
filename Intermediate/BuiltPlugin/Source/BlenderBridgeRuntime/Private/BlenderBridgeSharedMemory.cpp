#include "BlenderBridgeSharedMemory.h"

#include "BlenderBridgeRuntimeModule.h"

FBlenderBridgeSharedMemoryRegion::~FBlenderBridgeSharedMemoryRegion()
{
    Reset();
}

bool FBlenderBridgeSharedMemoryRegion::Create(const FString& InName, const uint64 InTotalBytes)
{
    return Map(InName, true, InTotalBytes);
}

bool FBlenderBridgeSharedMemoryRegion::Open(const FString& InName, const uint64 InTotalBytes)
{
    return Map(InName, false, InTotalBytes);
}

bool FBlenderBridgeSharedMemoryRegion::Remap(const uint64 InTotalBytes)
{
    const FString ExistingName = Name;
    Reset();
    return Create(ExistingName, InTotalBytes);
}

void FBlenderBridgeSharedMemoryRegion::Reset()
{
    if (Region != nullptr)
    {
        FPlatformMemory::UnmapNamedSharedMemoryRegion(Region);
        Region = nullptr;
    }

    Name.Reset();
    SizeInBytes = 0;
}

FBridgeSharedHeader* FBlenderBridgeSharedMemoryRegion::GetHeader()
{
    return Region != nullptr ? static_cast<FBridgeSharedHeader*>(Region->GetAddress()) : nullptr;
}

const FBridgeSharedHeader* FBlenderBridgeSharedMemoryRegion::GetHeader() const
{
    return Region != nullptr ? static_cast<const FBridgeSharedHeader*>(Region->GetAddress()) : nullptr;
}

bool FBlenderBridgeSharedMemoryRegion::ReadMesh(const uint32 SlotIndex, FBridgeDecodedMesh& OutMesh) const
{
    OutMesh.Reset();

    if (SlotIndex >= BlenderBridge::SharedSlotCount || Region == nullptr)
    {
        return false;
    }

    const FBridgeSharedHeader* Header = GetHeader();
    if (Header == nullptr)
    {
        return false;
    }

    if (Header->Magic != BlenderBridge::SharedMemoryMagic || Header->Version != BlenderBridge::SharedMemoryVersion || Header->SlotCount != BlenderBridge::SharedSlotCount)
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Shared memory header mismatch. Magic=%u Version=%u SlotCount=%u"), Header->Magic, Header->Version, Header->SlotCount);
        return false;
    }

    const FBridgeMeshSlotHeader& Slot = Header->Slots[SlotIndex];
    const uint8* SlotBase = GetSlotPayloadBase(SlotIndex);
    if (SlotBase == nullptr || Slot.Status != BlenderBridge::SlotStatusReady)
    {
        return false;
    }

    const auto ValidateRange = [Header, &Slot](const uint32 Offset, const uint32 Bytes) -> bool
    {
        return Offset <= Header->SlotStride && Bytes <= Header->SlotStride && (static_cast<uint64>(Offset) + static_cast<uint64>(Bytes)) <= Header->SlotStride;
    };

    if (!ValidateRange(Slot.PositionsOffset, Slot.PositionsBytes) ||
        !ValidateRange(Slot.NormalsOffset, Slot.NormalsBytes) ||
        !ValidateRange(Slot.UV0Offset, Slot.UV0Bytes) ||
        !ValidateRange(Slot.IndicesOffset, Slot.IndicesBytes))
    {
        return false;
    }

    const uint64 ExpectedPositionBytes = static_cast<uint64>(Slot.VertexCount) * 3ull * sizeof(float);
    const uint64 ExpectedNormalBytes = static_cast<uint64>(Slot.VertexCount) * 3ull * sizeof(float);
    const uint64 ExpectedUvBytes = static_cast<uint64>(Slot.VertexCount) * 2ull * sizeof(float);
    const uint64 ExpectedIndexBytes = static_cast<uint64>(Slot.IndexCount) * sizeof(uint32);

    if (ExpectedPositionBytes > MAX_uint32 || ExpectedNormalBytes > MAX_uint32 || ExpectedUvBytes > MAX_uint32 || ExpectedIndexBytes > MAX_uint32)
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Shared memory slot reported implausible counts. Vertices=%u Indices=%u"), Slot.VertexCount, Slot.IndexCount);
        return false;
    }

    if (Slot.PositionsBytes != ExpectedPositionBytes || Slot.NormalsBytes != ExpectedNormalBytes || Slot.UV0Bytes != ExpectedUvBytes || Slot.IndicesBytes != ExpectedIndexBytes)
    {
        UE_LOG(
            LogBlenderBridge,
            Warning,
            TEXT("Shared memory slot byte mismatch. Vertices=%u Indices=%u PosBytes=%u NormalBytes=%u UVBytes=%u IndexBytes=%u"),
            Slot.VertexCount,
            Slot.IndexCount,
            Slot.PositionsBytes,
            Slot.NormalsBytes,
            Slot.UV0Bytes,
            Slot.IndicesBytes);
        return false;
    }

    if (Slot.VertexCount == 0 || Slot.IndexCount == 0)
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Shared memory slot contained no geometry."));
        return false;
    }

    const float* Positions = reinterpret_cast<const float*>(SlotBase + Slot.PositionsOffset);
    const float* Normals = reinterpret_cast<const float*>(SlotBase + Slot.NormalsOffset);
    const float* UV0 = reinterpret_cast<const float*>(SlotBase + Slot.UV0Offset);
    const uint32* Indices = reinterpret_cast<const uint32*>(SlotBase + Slot.IndicesOffset);

    OutMesh.Positions.SetNum(Slot.VertexCount);
    OutMesh.Normals.SetNum(Slot.VertexCount);
    OutMesh.UV0.SetNum(Slot.VertexCount);
    OutMesh.Triangles.SetNum(Slot.IndexCount);

    for (uint32 VertexIndex = 0; VertexIndex < Slot.VertexCount; ++VertexIndex)
    {
        const uint32 PositionOffset = VertexIndex * 3u;
        const uint32 UVOffset = VertexIndex * 2u;
        OutMesh.Positions[VertexIndex] = FVector(Positions[PositionOffset + 0u], Positions[PositionOffset + 1u], Positions[PositionOffset + 2u]);
        OutMesh.Normals[VertexIndex] = FVector(Normals[PositionOffset + 0u], Normals[PositionOffset + 1u], Normals[PositionOffset + 2u]);
        OutMesh.UV0[VertexIndex] = FVector2D(UV0[UVOffset + 0u], UV0[UVOffset + 1u]);
    }

    for (uint32 Index = 0; Index < Slot.IndexCount; ++Index)
    {
        OutMesh.Triangles[Index] = static_cast<int32>(Indices[Index]);
    }

    OutMesh.Bounds = FBox(
        FVector(Slot.BoundsMin[0], Slot.BoundsMin[1], Slot.BoundsMin[2]),
        FVector(Slot.BoundsMax[0], Slot.BoundsMax[1], Slot.BoundsMax[2]));

    return true;
}

bool FBlenderBridgeSharedMemoryRegion::Map(const FString& InName, const bool bCreate, const uint64 InTotalBytes)
{
    Reset();

    if (InTotalBytes < sizeof(FBridgeSharedHeader) + BlenderBridge::MinimumSlotStride * BlenderBridge::SharedSlotCount)
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Shared memory mapping too small: %llu"), InTotalBytes);
        return false;
    }

    Name = InName;
    SizeInBytes = InTotalBytes;
    Region = FPlatformMemory::MapNamedSharedMemoryRegion(Name, bCreate, FPlatformMemory::Read | FPlatformMemory::Write, static_cast<SIZE_T>(SizeInBytes));

    if (Region == nullptr)
    {
        UE_LOG(LogBlenderBridge, Error, TEXT("Failed to map shared memory region %s"), *Name);
        return false;
    }

    if (bCreate)
    {
        FMemory::Memzero(Region->GetAddress(), Region->GetSize());
        InitializeHeader();
    }

    return true;
}

void FBlenderBridgeSharedMemoryRegion::InitializeHeader()
{
    FBridgeSharedHeader* Header = GetHeader();
    if (Header == nullptr)
    {
        return;
    }

    const uint64 UsableBytes = SizeInBytes - sizeof(FBridgeSharedHeader);
    const uint64 SlotStride = FMath::Max(BlenderBridge::MinimumSlotStride, UsableBytes / BlenderBridge::SharedSlotCount);

    Header->Magic = BlenderBridge::SharedMemoryMagic;
    Header->Version = BlenderBridge::SharedMemoryVersion;
    Header->HeaderBytes = sizeof(FBridgeSharedHeader);
    Header->TotalBytes = SizeInBytes;
    Header->SlotStride = SlotStride;
    Header->SlotCount = BlenderBridge::SharedSlotCount;
    Header->ActiveSlot = 0;

    for (uint32 SlotIndex = 0; SlotIndex < BlenderBridge::SharedSlotCount; ++SlotIndex)
    {
        Header->Slots[SlotIndex] = FBridgeMeshSlotHeader{};
        Header->Slots[SlotIndex].SlotIndex = SlotIndex;
    }
}

const uint8* FBlenderBridgeSharedMemoryRegion::GetSlotPayloadBase(const uint32 SlotIndex) const
{
    const FBridgeSharedHeader* Header = GetHeader();
    if (Header == nullptr || Region == nullptr || SlotIndex >= Header->SlotCount)
    {
        return nullptr;
    }

    return static_cast<const uint8*>(Region->GetAddress()) + Header->HeaderBytes + (SlotIndex * Header->SlotStride);
}
