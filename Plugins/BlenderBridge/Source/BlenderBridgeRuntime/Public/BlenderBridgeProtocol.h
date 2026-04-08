#pragma once

#include "CoreMinimal.h"
#include "StructUtils/PropertyBag.h"

#include "BlenderBridgeProtocol.generated.h"

UENUM(BlueprintType)
enum class EBlenderBridgeConnectionState : uint8
{
    Disconnected,
    Launching,
    Connected,
    Error
};

UENUM(BlueprintType)
enum class EBlenderBridgeComponentStatus : uint8
{
    NoSession,
    SourceNotLoaded,
    TargetUnresolved,
    SchemaPending,
    Ready,
    CookQueued,
    Cooking,
    Cooked,
    Error
};

UENUM(BlueprintType)
enum class EBlenderBridgeTargetKind : uint8
{
    Unknown,
    Standard,
    CurveOwner
};

UENUM(BlueprintType)
enum class EBlenderBridgeCurveType : uint8
{
    None,
    Bezier,
    Unsupported
};

UENUM(BlueprintType)
enum class EBlenderBridgeSplinePointType : uint8
{
    Linear,
    Curve,
    Constant,
    CurveClamped,
    CurveCustomTangent
};

UENUM(BlueprintType)
enum class EBlenderBridgeInputType : uint8
{
    Float,
    Integer,
    Boolean,
    Vector3,
    Color,
    String,
    Enum,
    Unsupported
};

USTRUCT(BlueprintType)
struct FBridgeEnumOption
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString Id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString Label;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    int32 Value = 0;
};

USTRUCT(BlueprintType)
struct FBridgeCurveOwnerPoint
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FVector Position = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FVector ArriveTangent = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FVector LeaveTangent = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    float RollDegrees = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FVector Scale = FVector::OneVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    EBlenderBridgeSplinePointType Type = EBlenderBridgeSplinePointType::Curve;
};

USTRUCT(BlueprintType)
struct FBridgeCurveOwnerData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    EBlenderBridgeCurveType CurveType = EBlenderBridgeCurveType::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    bool bClosedLoop = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    TArray<FBridgeCurveOwnerPoint> Points;
};

USTRUCT(BlueprintType)
struct FBridgeTargetCapabilities
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    EBlenderBridgeTargetKind TargetKind = EBlenderBridgeTargetKind::Unknown;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    bool bSupportsCurveOwner = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString CompatibilityMessage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FBridgeCurveOwnerData OwnerCurve;
};

USTRUCT(BlueprintType)
struct FBridgeInputSchema
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FName Id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString Label;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString CategoryPath;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString Description;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    EBlenderBridgeInputType Type = EBlenderBridgeInputType::Unsupported;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    double DefaultNumericValue = 0.0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    double MinNumericValue = 0.0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    double MaxNumericValue = 1.0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    bool bDefaultBoolValue = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FVector DefaultVectorValue = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FLinearColor DefaultColorValue = FLinearColor::Black;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString DefaultStringValue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    int32 DefaultEnumValue = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    TArray<FBridgeEnumOption> EnumOptions;
};

USTRUCT(BlueprintType)
struct FBridgeTargetDescriptor
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString TargetId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString DisplayLabel;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString ObjectName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString ModifierName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString ObjectType;
};

USTRUCT(BlueprintType)
struct FBridgeMaterialSlotDescriptor
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString SlotId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString SlotName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString BlenderMaterialName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    int32 SectionIndex = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct FBridgeMeshSectionDescriptor
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    int32 SectionIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    int32 FirstVertex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    int32 VertexCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    int32 FirstIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    int32 IndexCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString SlotId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString SlotName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bridge")
    FString BlenderMaterialName;
};

struct FBridgeDecodedMesh
{
    TArray<FVector> Positions;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    TArray<FBridgeMaterialSlotDescriptor> MaterialSlots;
    TArray<FBridgeMeshSectionDescriptor> Sections;
    FBox Bounds = FBox(FVector::ZeroVector, FVector::ZeroVector);

    void Reset()
    {
        Positions.Reset();
        Triangles.Reset();
        Normals.Reset();
        UV0.Reset();
        MaterialSlots.Reset();
        Sections.Reset();
        Bounds = FBox(FVector::ZeroVector, FVector::ZeroVector);
    }
};

enum class EBlenderBridgeMessageType : uint8
{
    Unknown,
    Hello,
    EnsureSourceLoaded,
    SourceLoaded,
    DescribeTarget,
    TargetDescription,
    CookTarget,
    ReleaseComponentState,
    CookComplete,
    ResizeRequired,
    Error,
    Shutdown
};

namespace BlenderBridge
{
    inline constexpr uint32 SharedMemoryMagic = 0x42424745u;
    inline constexpr uint32 SharedMemoryVersion = 1u;
    inline constexpr uint32 SharedSlotCount = 2u;
    inline constexpr uint32 SlotStatusEmpty = 0u;
    inline constexpr uint32 SlotStatusReady = 1u;
    inline constexpr uint64 DefaultSharedMemoryBytesPerSlot = 250ull * 1024ull * 1024ull;
    inline constexpr uint64 DefaultSharedMemoryBytes = DefaultSharedMemoryBytesPerSlot * SharedSlotCount;
    inline constexpr uint64 MinimumSlotStride = 256ull;

    BLENDERBRIDGERUNTIME_API const TCHAR* LexToString(EBlenderBridgeMessageType Type);
    BLENDERBRIDGERUNTIME_API bool TryParseMessageType(const FString& Text, EBlenderBridgeMessageType& OutType);
}

#pragma pack(push, 1)

struct FBridgeMeshSlotHeader
{
    uint64 Sequence = 0;
    uint32 SlotIndex = 0;
    uint32 Status = BlenderBridge::SlotStatusEmpty;
    uint32 VertexCount = 0;
    uint32 IndexCount = 0;
    uint32 PositionsOffset = 0;
    uint32 PositionsBytes = 0;
    uint32 NormalsOffset = 0;
    uint32 NormalsBytes = 0;
    uint32 UV0Offset = 0;
    uint32 UV0Bytes = 0;
    uint32 IndicesOffset = 0;
    uint32 IndicesBytes = 0;
    float BoundsMin[3] = {0.0f, 0.0f, 0.0f};
    float BoundsMax[3] = {0.0f, 0.0f, 0.0f};
    uint32 Reserved[8] = {};
};

struct FBridgeSharedHeader
{
    uint32 Magic = BlenderBridge::SharedMemoryMagic;
    uint32 Version = BlenderBridge::SharedMemoryVersion;
    uint64 HeaderBytes = sizeof(FBridgeSharedHeader);
    uint64 TotalBytes = 0;
    uint64 SlotStride = 0;
    uint32 SlotCount = BlenderBridge::SharedSlotCount;
    uint32 ActiveSlot = 0;
    uint64 Reserved[4] = {};
    FBridgeMeshSlotHeader Slots[BlenderBridge::SharedSlotCount];
};

#pragma pack(pop)

static_assert(sizeof(FBridgeMeshSlotHeader) == 112, "Unexpected bridge mesh slot header size");
static_assert(sizeof(FBridgeSharedHeader) == 296, "Unexpected bridge shared header size");
