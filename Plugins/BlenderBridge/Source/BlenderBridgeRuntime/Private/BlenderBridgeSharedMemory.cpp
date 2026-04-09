#include "BlenderBridgeSharedMemory.h"

#include "BlenderBridgeRuntimeModule.h"
#include "Containers/StringConv.h"

namespace
{
    template <typename SharedHeaderType>
    bool ValidateSlotRange(const SharedHeaderType* Header, const uint32 Offset, const uint32 Bytes)
    {
        return Header != nullptr &&
            Offset <= Header->SlotStride &&
            Bytes <= Header->SlotStride &&
            (static_cast<uint64>(Offset) + static_cast<uint64>(Bytes)) <= Header->SlotStride;
    }

    bool ReadUtf8String(
        const TArray<uint8>& PayloadBytes,
        const FBridgeInstanceBakePayloadHeader& Header,
        const uint32 RelativeOffset,
        const uint32 Length,
        FString& OutString)
    {
        OutString.Reset();

        if (Length == 0)
        {
            return true;
        }

        if (RelativeOffset > Header.StringTableBytes || Length > Header.StringTableBytes || RelativeOffset + Length > Header.StringTableBytes)
        {
            return false;
        }

        const uint32 AbsoluteOffset = Header.StringTableOffset + RelativeOffset;
        if (!PayloadBytes.IsValidIndex(static_cast<int32>(AbsoluteOffset)) || AbsoluteOffset + Length > static_cast<uint32>(PayloadBytes.Num()))
        {
            return false;
        }

        FUTF8ToTCHAR Converter(reinterpret_cast<const ANSICHAR*>(PayloadBytes.GetData() + AbsoluteOffset), static_cast<int32>(Length));
        OutString = FString(Converter.Length(), Converter.Get());
        return true;
    }

    bool DecodeInstanceBakePayload(const TArray<uint8>& PayloadBytes, FBridgeDecodedInstanceBake& OutInstanceBake)
    {
        OutInstanceBake.Reset();

        if (PayloadBytes.Num() < static_cast<int32>(sizeof(FBridgeInstanceBakePayloadHeader)))
        {
            return false;
        }

        const FBridgeInstanceBakePayloadHeader* Header =
            reinterpret_cast<const FBridgeInstanceBakePayloadHeader*>(PayloadBytes.GetData());
        if (Header->Magic != BlenderBridge::InstanceBakePayloadMagic || Header->Version != BlenderBridge::InstanceBakePayloadVersion)
        {
            return false;
        }

        if (Header->PayloadBytes > static_cast<uint32>(PayloadBytes.Num()))
        {
            return false;
        }

        const auto ValidatePayloadRange = [&PayloadBytes, Header](const uint32 Offset, const uint64 Bytes) -> bool
        {
            return Offset <= Header->PayloadBytes &&
                Bytes <= Header->PayloadBytes &&
                (static_cast<uint64>(Offset) + Bytes) <= Header->PayloadBytes &&
                (static_cast<uint64>(Offset) + Bytes) <= static_cast<uint64>(PayloadBytes.Num());
        };

        const uint64 PrototypeRecordsBytes = static_cast<uint64>(Header->PrototypeCount) * sizeof(FBridgeInstanceBakePrototypeRecord);
        const uint64 MaterialSlotRecordsBytes = static_cast<uint64>(Header->MaterialSlotCount) * sizeof(FBridgeInstanceBakeMaterialSlotRecord);
        const uint64 SectionRecordsBytes = static_cast<uint64>(Header->SectionCount) * sizeof(FBridgeInstanceBakeSectionRecord);
        const uint64 PlacementRecordsBytes = static_cast<uint64>(Header->PlacementCount) * sizeof(FBridgeInstanceBakePlacementRecord);

        if (!ValidatePayloadRange(Header->PrototypeRecordsOffset, PrototypeRecordsBytes) ||
            !ValidatePayloadRange(Header->MaterialSlotRecordsOffset, MaterialSlotRecordsBytes) ||
            !ValidatePayloadRange(Header->SectionRecordsOffset, SectionRecordsBytes) ||
            !ValidatePayloadRange(Header->PlacementRecordsOffset, PlacementRecordsBytes) ||
            !ValidatePayloadRange(Header->StringTableOffset, Header->StringTableBytes))
        {
            return false;
        }

        const FBridgeInstanceBakePrototypeRecord* PrototypeRecords =
            reinterpret_cast<const FBridgeInstanceBakePrototypeRecord*>(PayloadBytes.GetData() + Header->PrototypeRecordsOffset);
        const FBridgeInstanceBakeMaterialSlotRecord* MaterialSlotRecords =
            reinterpret_cast<const FBridgeInstanceBakeMaterialSlotRecord*>(PayloadBytes.GetData() + Header->MaterialSlotRecordsOffset);
        const FBridgeInstanceBakeSectionRecord* SectionRecords =
            reinterpret_cast<const FBridgeInstanceBakeSectionRecord*>(PayloadBytes.GetData() + Header->SectionRecordsOffset);
        const FBridgeInstanceBakePlacementRecord* PlacementRecords =
            reinterpret_cast<const FBridgeInstanceBakePlacementRecord*>(PayloadBytes.GetData() + Header->PlacementRecordsOffset);

        OutInstanceBake.Prototypes.Reserve(Header->PrototypeCount);
        for (uint32 PrototypeIndex = 0; PrototypeIndex < Header->PrototypeCount; ++PrototypeIndex)
        {
            const FBridgeInstanceBakePrototypeRecord& Record = PrototypeRecords[PrototypeIndex];
            if (Record.FirstMaterialSlotIndex > Header->MaterialSlotCount ||
                Record.MaterialSlotCount > Header->MaterialSlotCount ||
                Record.FirstMaterialSlotIndex + Record.MaterialSlotCount > Header->MaterialSlotCount ||
                Record.FirstSectionIndex > Header->SectionCount ||
                Record.SectionCount > Header->SectionCount ||
                Record.FirstSectionIndex + Record.SectionCount > Header->SectionCount)
            {
                return false;
            }

            const uint64 ExpectedPositionBytes = static_cast<uint64>(Record.VertexCount) * 3ull * sizeof(float);
            const uint64 ExpectedNormalBytes = static_cast<uint64>(Record.VertexCount) * 3ull * sizeof(float);
            const uint64 ExpectedUvBytes = static_cast<uint64>(Record.VertexCount) * 2ull * sizeof(float);
            const uint64 ExpectedIndexBytes = static_cast<uint64>(Record.IndexCount) * sizeof(uint32);
            if (Record.PositionsBytes != ExpectedPositionBytes ||
                Record.NormalsBytes != ExpectedNormalBytes ||
                Record.UV0Bytes != ExpectedUvBytes ||
                Record.IndicesBytes != ExpectedIndexBytes ||
                !ValidatePayloadRange(Record.PositionsOffset, Record.PositionsBytes) ||
                !ValidatePayloadRange(Record.NormalsOffset, Record.NormalsBytes) ||
                !ValidatePayloadRange(Record.UV0Offset, Record.UV0Bytes) ||
                !ValidatePayloadRange(Record.IndicesOffset, Record.IndicesBytes))
            {
                return false;
            }

            FBridgeInstanceBakePrototype Prototype;
            if (!ReadUtf8String(PayloadBytes, *Header, Record.PrototypeKeyOffset, Record.PrototypeKeyLength, Prototype.PrototypeKey))
            {
                return false;
            }

            Prototype.Mesh.Positions.SetNum(Record.VertexCount);
            Prototype.Mesh.Normals.SetNum(Record.VertexCount);
            Prototype.Mesh.UV0.SetNum(Record.VertexCount);
            Prototype.Mesh.Triangles.SetNum(Record.IndexCount);
            Prototype.Mesh.MaterialSlots.Reserve(Record.MaterialSlotCount);
            Prototype.Mesh.Sections.Reserve(Record.SectionCount);

            const float* Positions = reinterpret_cast<const float*>(PayloadBytes.GetData() + Record.PositionsOffset);
            const float* Normals = reinterpret_cast<const float*>(PayloadBytes.GetData() + Record.NormalsOffset);
            const float* UV0 = reinterpret_cast<const float*>(PayloadBytes.GetData() + Record.UV0Offset);
            const uint32* Indices = reinterpret_cast<const uint32*>(PayloadBytes.GetData() + Record.IndicesOffset);

            for (uint32 VertexIndex = 0; VertexIndex < Record.VertexCount; ++VertexIndex)
            {
                const uint32 PositionOffset = VertexIndex * 3u;
                const uint32 UVOffset = VertexIndex * 2u;
                Prototype.Mesh.Positions[VertexIndex] = FVector(
                    Positions[PositionOffset + 0u],
                    Positions[PositionOffset + 1u],
                    Positions[PositionOffset + 2u]);
                Prototype.Mesh.Normals[VertexIndex] = FVector(
                    Normals[PositionOffset + 0u],
                    Normals[PositionOffset + 1u],
                    Normals[PositionOffset + 2u]);
                Prototype.Mesh.UV0[VertexIndex] = FVector2D(UV0[UVOffset + 0u], UV0[UVOffset + 1u]);
            }

            for (uint32 Index = 0; Index < Record.IndexCount; ++Index)
            {
                Prototype.Mesh.Triangles[Index] = static_cast<int32>(Indices[Index]);
            }

            Prototype.Mesh.Bounds = FBox(
                FVector(Record.BoundsMin[0], Record.BoundsMin[1], Record.BoundsMin[2]),
                FVector(Record.BoundsMax[0], Record.BoundsMax[1], Record.BoundsMax[2]));

            for (uint32 MaterialSlotIndex = 0; MaterialSlotIndex < Record.MaterialSlotCount; ++MaterialSlotIndex)
            {
                const FBridgeInstanceBakeMaterialSlotRecord& SlotRecord =
                    MaterialSlotRecords[Record.FirstMaterialSlotIndex + MaterialSlotIndex];

                FBridgeMaterialSlotDescriptor Slot;
                if (!ReadUtf8String(PayloadBytes, *Header, SlotRecord.SlotIdOffset, SlotRecord.SlotIdLength, Slot.SlotId) ||
                    !ReadUtf8String(PayloadBytes, *Header, SlotRecord.SlotNameOffset, SlotRecord.SlotNameLength, Slot.SlotName) ||
                    !ReadUtf8String(
                        PayloadBytes,
                        *Header,
                        SlotRecord.MaterialNameOffset,
                        SlotRecord.MaterialNameLength,
                        Slot.BlenderMaterialName))
                {
                    return false;
                }

                Slot.SectionIndex = SlotRecord.SectionIndex;
                Prototype.Mesh.MaterialSlots.Add(MoveTemp(Slot));
            }

            for (uint32 SectionIndex = 0; SectionIndex < Record.SectionCount; ++SectionIndex)
            {
                const FBridgeInstanceBakeSectionRecord& SectionRecord =
                    SectionRecords[Record.FirstSectionIndex + SectionIndex];

                FBridgeMeshSectionDescriptor Section;
                Section.SectionIndex = SectionRecord.SectionIndex;
                Section.FirstVertex = SectionRecord.FirstVertex;
                Section.VertexCount = SectionRecord.VertexCount;
                Section.FirstIndex = SectionRecord.FirstIndex;
                Section.IndexCount = SectionRecord.IndexCount;
                if (!ReadUtf8String(PayloadBytes, *Header, SectionRecord.SlotIdOffset, SectionRecord.SlotIdLength, Section.SlotId) ||
                    !ReadUtf8String(PayloadBytes, *Header, SectionRecord.SlotNameOffset, SectionRecord.SlotNameLength, Section.SlotName) ||
                    !ReadUtf8String(
                        PayloadBytes,
                        *Header,
                        SectionRecord.MaterialNameOffset,
                        SectionRecord.MaterialNameLength,
                        Section.BlenderMaterialName))
                {
                    return false;
                }

                Prototype.Mesh.Sections.Add(MoveTemp(Section));
            }

            OutInstanceBake.Prototypes.Add(MoveTemp(Prototype));
        }

        OutInstanceBake.Placements.Reserve(Header->PlacementCount);
        for (uint32 PlacementIndex = 0; PlacementIndex < Header->PlacementCount; ++PlacementIndex)
        {
            const FBridgeInstanceBakePlacementRecord& Record = PlacementRecords[PlacementIndex];
            if (!OutInstanceBake.Prototypes.IsValidIndex(static_cast<int32>(Record.PrototypeIndex)))
            {
                return false;
            }

            FBridgeInstanceBakePlacement Placement;
            Placement.PrototypeIndex = static_cast<int32>(Record.PrototypeIndex);
            Placement.Transform = FTransform(
                FQuat(Record.Rotation[0], Record.Rotation[1], Record.Rotation[2], Record.Rotation[3]),
                FVector(Record.Translation[0], Record.Translation[1], Record.Translation[2]),
                FVector(Record.Scale3D[0], Record.Scale3D[1], Record.Scale3D[2]));
            OutInstanceBake.Placements.Add(MoveTemp(Placement));
        }

        return true;
    }
}

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

    if (!ValidateSlotRange(Header, Slot.PositionsOffset, Slot.PositionsBytes) ||
        !ValidateSlotRange(Header, Slot.NormalsOffset, Slot.NormalsBytes) ||
        !ValidateSlotRange(Header, Slot.UV0Offset, Slot.UV0Bytes) ||
        !ValidateSlotRange(Header, Slot.IndicesOffset, Slot.IndicesBytes))
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

FBlenderBridgeInstanceBakeSharedMemoryRegion::~FBlenderBridgeInstanceBakeSharedMemoryRegion()
{
    Reset();
}

bool FBlenderBridgeInstanceBakeSharedMemoryRegion::Create(const FString& InName, const uint64 InTotalBytes)
{
    return Map(InName, true, InTotalBytes);
}

bool FBlenderBridgeInstanceBakeSharedMemoryRegion::Open(const FString& InName, const uint64 InTotalBytes)
{
    return Map(InName, false, InTotalBytes);
}

bool FBlenderBridgeInstanceBakeSharedMemoryRegion::Remap(const uint64 InTotalBytes)
{
    const FString ExistingName = Name;
    Reset();
    return Create(ExistingName, InTotalBytes);
}

void FBlenderBridgeInstanceBakeSharedMemoryRegion::Reset()
{
    if (Region != nullptr)
    {
        FPlatformMemory::UnmapNamedSharedMemoryRegion(Region);
        Region = nullptr;
    }

    Name.Reset();
    SizeInBytes = 0;
}

FBridgeInstanceBakeSharedHeader* FBlenderBridgeInstanceBakeSharedMemoryRegion::GetHeader()
{
    return Region != nullptr ? static_cast<FBridgeInstanceBakeSharedHeader*>(Region->GetAddress()) : nullptr;
}

const FBridgeInstanceBakeSharedHeader* FBlenderBridgeInstanceBakeSharedMemoryRegion::GetHeader() const
{
    return Region != nullptr ? static_cast<const FBridgeInstanceBakeSharedHeader*>(Region->GetAddress()) : nullptr;
}

bool FBlenderBridgeInstanceBakeSharedMemoryRegion::ReadInstanceBake(
    const uint32 SlotIndex,
    FBridgeDecodedInstanceBake& OutInstanceBake) const
{
    OutInstanceBake.Reset();

    if (SlotIndex >= BlenderBridge::SharedSlotCount || Region == nullptr)
    {
        return false;
    }

    const FBridgeInstanceBakeSharedHeader* Header = GetHeader();
    if (Header == nullptr)
    {
        return false;
    }

    if (Header->Magic != BlenderBridge::InstanceBakeSharedMemoryMagic ||
        Header->Version != BlenderBridge::InstanceBakeSharedMemoryVersion ||
        Header->SlotCount != BlenderBridge::SharedSlotCount)
    {
        UE_LOG(
            LogBlenderBridge,
            Warning,
            TEXT("Instance-bake shared memory header mismatch. Magic=%u Version=%u SlotCount=%u"),
            Header->Magic,
            Header->Version,
            Header->SlotCount);
        return false;
    }

    const FBridgeInstanceBakeSlotHeader& Slot = Header->Slots[SlotIndex];
    const uint8* SlotBase = GetSlotPayloadBase(SlotIndex);
    if (SlotBase == nullptr || Slot.Status != BlenderBridge::SlotStatusReady)
    {
        return false;
    }

    if (Slot.PayloadBytes == 0 || !ValidateSlotRange(Header, 0u, Slot.PayloadBytes))
    {
        return false;
    }

    TArray<uint8> PayloadBytes;
    PayloadBytes.SetNumUninitialized(static_cast<int32>(Slot.PayloadBytes));
    FMemory::Memcpy(PayloadBytes.GetData(), SlotBase, Slot.PayloadBytes);

    if (!DecodeInstanceBakePayload(PayloadBytes, OutInstanceBake))
    {
        return false;
    }

    OutInstanceBake.TrueInstanceCount = static_cast<int32>(Slot.TrueInstanceCount);
    return true;
}

bool FBlenderBridgeInstanceBakeSharedMemoryRegion::Map(const FString& InName, const bool bCreate, const uint64 InTotalBytes)
{
    Reset();

    if (InTotalBytes < sizeof(FBridgeInstanceBakeSharedHeader) + BlenderBridge::MinimumSlotStride * BlenderBridge::SharedSlotCount)
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Instance-bake shared memory mapping too small: %llu"), InTotalBytes);
        return false;
    }

    Name = InName;
    SizeInBytes = InTotalBytes;
    Region = FPlatformMemory::MapNamedSharedMemoryRegion(Name, bCreate, FPlatformMemory::Read | FPlatformMemory::Write, static_cast<SIZE_T>(SizeInBytes));

    if (Region == nullptr)
    {
        UE_LOG(LogBlenderBridge, Error, TEXT("Failed to map instance-bake shared memory region %s"), *Name);
        return false;
    }

    if (bCreate)
    {
        FMemory::Memzero(Region->GetAddress(), Region->GetSize());
        InitializeHeader();
    }

    return true;
}

void FBlenderBridgeInstanceBakeSharedMemoryRegion::InitializeHeader()
{
    FBridgeInstanceBakeSharedHeader* Header = GetHeader();
    if (Header == nullptr)
    {
        return;
    }

    const uint64 UsableBytes = SizeInBytes - sizeof(FBridgeInstanceBakeSharedHeader);
    const uint64 SlotStride = FMath::Max(BlenderBridge::MinimumSlotStride, UsableBytes / BlenderBridge::SharedSlotCount);

    Header->Magic = BlenderBridge::InstanceBakeSharedMemoryMagic;
    Header->Version = BlenderBridge::InstanceBakeSharedMemoryVersion;
    Header->HeaderBytes = sizeof(FBridgeInstanceBakeSharedHeader);
    Header->TotalBytes = SizeInBytes;
    Header->SlotStride = SlotStride;
    Header->SlotCount = BlenderBridge::SharedSlotCount;
    Header->ActiveSlot = 0;

    for (uint32 SlotIndex = 0; SlotIndex < BlenderBridge::SharedSlotCount; ++SlotIndex)
    {
        Header->Slots[SlotIndex] = FBridgeInstanceBakeSlotHeader{};
        Header->Slots[SlotIndex].SlotIndex = SlotIndex;
    }
}

const uint8* FBlenderBridgeInstanceBakeSharedMemoryRegion::GetSlotPayloadBase(const uint32 SlotIndex) const
{
    const FBridgeInstanceBakeSharedHeader* Header = GetHeader();
    if (Header == nullptr || Region == nullptr || SlotIndex >= Header->SlotCount)
    {
        return nullptr;
    }

    return static_cast<const uint8*>(Region->GetAddress()) + Header->HeaderBytes + (SlotIndex * Header->SlotStride);
}
