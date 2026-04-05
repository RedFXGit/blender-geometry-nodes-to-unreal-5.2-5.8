#include "BlenderBridgeComponent.h"

#include "BlenderBridgeMessages.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSource.h"
#include "BlenderBridgeWorkerSession.h"
#include "Async/Async.h"
#include "Dom/JsonObject.h"
#include "Engine/UserDefinedEnum.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ProceduralMeshComponent.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"

#ifndef BLENDER_BRIDGE_BUILD_VERSION
#define BLENDER_BRIDGE_BUILD_VERSION TEXT("Unknown build")
#endif

namespace
{
    constexpr TCHAR BridgeEnableCategoriesMetaKey[] = TEXT("EnableCategories");
    constexpr TCHAR BridgeCategoryMetaKey[] = TEXT("Category");
    constexpr TCHAR BridgeToolTipMetaKey[] = TEXT("ToolTip");

    const TCHAR* LexToString(const EBlenderBridgeInputType Type)
    {
        switch (Type)
        {
        case EBlenderBridgeInputType::Float:
            return TEXT("Float");
        case EBlenderBridgeInputType::Integer:
            return TEXT("Integer");
        case EBlenderBridgeInputType::Boolean:
            return TEXT("Boolean");
        case EBlenderBridgeInputType::Vector3:
            return TEXT("Vector3");
        case EBlenderBridgeInputType::Color:
            return TEXT("Color");
        case EBlenderBridgeInputType::String:
            return TEXT("String");
        case EBlenderBridgeInputType::Enum:
            return TEXT("Enum");
        case EBlenderBridgeInputType::Unsupported:
        default:
            return TEXT("Unsupported");
        }
    }

    FName GetBridgeInputBagPropertyName(const FBridgeInputSchema& Entry)
    {
        return FInstancedPropertyBag::SanitizePropertyName(Entry.Id);
    }

    bool IsBridgeEnumValueRepresentable(const int32 Value)
    {
        return Value >= 0 && Value <= MAX_uint8;
    }

    bool HasBridgeEnumValue(const FBridgeInputSchema& Entry, const int32 Value)
    {
        for (const FBridgeEnumOption& Option : Entry.EnumOptions)
        {
            if (Option.Value == Value)
            {
                return true;
            }
        }

        return false;
    }

    int32 GetPreferredBridgeEnumValue(const FBridgeInputSchema& Entry)
    {
        if (IsBridgeEnumValueRepresentable(Entry.DefaultEnumValue) && HasBridgeEnumValue(Entry, Entry.DefaultEnumValue))
        {
            return Entry.DefaultEnumValue;
        }

        for (const FBridgeEnumOption& Option : Entry.EnumOptions)
        {
            if (IsBridgeEnumValueRepresentable(Option.Value))
            {
                return Option.Value;
            }
        }

        return 0;
    }

    uint32 ComputeBridgeEnumSignature(const FBridgeInputSchema& Entry)
    {
        uint32 Hash = GetTypeHash(Entry.Id);
        Hash = HashCombine(Hash, GetTypeHash(Entry.DefaultEnumValue));

        for (const FBridgeEnumOption& Option : Entry.EnumOptions)
        {
            Hash = HashCombine(Hash, GetTypeHash(Option.Id));
            Hash = HashCombine(Hash, GetTypeHash(Option.Label));
            Hash = HashCombine(Hash, GetTypeHash(Option.Value));
        }

        return Hash;
    }

    FString MakeBridgeEnumObjectBaseName(const FBridgeInputSchema& Entry)
    {
        const FString BagPropertyName = GetBridgeInputBagPropertyName(Entry).ToString();
        return BagPropertyName.IsEmpty() ? TEXT("BridgeInputEnum") : FString::Printf(TEXT("BridgeInputEnum_%s"), *BagPropertyName);
    }

    FString MakeBridgeEnumEntryShortName(const FBridgeEnumOption& Option, const int32 Index)
    {
        FString CandidateName = !Option.Id.IsEmpty() ? Option.Id : Option.Label;
        if (CandidateName.IsEmpty())
        {
            CandidateName = FString::Printf(TEXT("Option_%d"), Index);
        }

        FName SanitizedName = FInstancedPropertyBag::SanitizePropertyName(CandidateName);
        if (SanitizedName.IsNone())
        {
            SanitizedName = FName(*FString::Printf(TEXT("Option_%d"), Index));
        }

        return SanitizedName.ToString();
    }

    int32 GetStoredBridgeEnumValue(const FInstancedPropertyBag& Bag, const FName PropertyName)
    {
        if (const FPropertyBagPropertyDesc* Desc = Bag.FindPropertyDescByName(PropertyName))
        {
            if (const UEnum* EnumDefinition = Cast<UEnum>(Desc->ValueTypeObject))
            {
                if (const auto EnumValue = Bag.GetValueEnum(PropertyName, EnumDefinition); EnumValue.IsValid())
                {
                    return static_cast<int32>(EnumValue.GetValue());
                }
            }
        }

        if (const auto IntegerValue = Bag.GetValueInt32(PropertyName); IntegerValue.IsValid())
        {
            return IntegerValue.GetValue();
        }

        return INDEX_NONE;
    }

    uint64 ComputeExpandedSharedMemoryBytes(const uint64 CurrentTotalBytes, const uint64 RequiredSlotBytes)
    {
        const uint64 MinRequiredTotalBytes = sizeof(FBridgeSharedHeader) + (RequiredSlotBytes * BlenderBridge::SharedSlotCount);
        uint64 ExpandedBytes = FMath::Max(CurrentTotalBytes, BlenderBridge::DefaultSharedMemoryBytes);

        while (ExpandedBytes < MinRequiredTotalBytes)
        {
            ExpandedBytes *= 2ull;
        }

        return ExpandedBytes;
    }
}

UBlenderBridgeComponent::UBlenderBridgeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    BridgeBuildVersion = BLENDER_BRIDGE_BUILD_VERSION;
}

void UBlenderBridgeComponent::StartBridge()
{
    StartBridgeInternal(true, false);
}

void UBlenderBridgeComponent::StartBridgeInternal(const bool bResetInputValuesFromSchema, const bool bRequestCookAfterConnect)
{
    StopBridge();
    LastStatusMessage.Reset();

    EnsurePreviewMeshComponent();

    const FString WorkerScriptPath = ResolveWorkerScriptPath();
    if (WorkerScriptPath.IsEmpty())
    {
        HandleLogMessage(TEXT("Could not resolve Blender bridge worker script path."));
        ConnectionState = EBlenderBridgeConnectionState::Error;
        return;
    }

    FBlenderBridgeWorkerConfig Config;
    Config.BlenderExecutablePath = BlenderExecutablePath.FilePath;
    Config.WorkerScriptPath = WorkerScriptPath;
    Config.SharedMemoryBytes = static_cast<uint64>(FMath::Max<int64>(SharedMemoryBytes, static_cast<int64>(BlenderBridge::DefaultSharedMemoryBytes)));

    if (Source == nullptr)
    {
        HandleLogMessage(TEXT("A BlenderBridgeSource asset is required."));
        ConnectionState = EBlenderBridgeConnectionState::Error;
        return;
    }

    Config.BlendFilePath = Source->BlendFile.FilePath;
    if (Config.BlendFilePath.IsEmpty())
    {
        HandleLogMessage(TEXT("Blend File is required."));
        ConnectionState = EBlenderBridgeConnectionState::Error;
        return;
    }

    HandleLogMessage(
        FString::Printf(
            TEXT("Starting bridge for Blend='%s'. Object and modifier selection will auto-resolve from the file."),
            *Config.BlendFilePath));
    bResetInputValuesFromSchemaOnNextRefresh = bResetInputValuesFromSchema;
    bRequestCookAfterReconnect = bRequestCookAfterConnect;

    Session = MakeShared<FBlenderBridgeWorkerSession>();
    Session->OnStateChanged.AddUObject(this, &UBlenderBridgeComponent::HandleStateChanged);
    Session->OnSchemaReceived.AddUObject(this, &UBlenderBridgeComponent::HandleSchemaReceived);
    Session->OnCookComplete.AddUObject(this, &UBlenderBridgeComponent::HandleCookComplete);
    Session->OnResizeRequired.AddUObject(this, &UBlenderBridgeComponent::HandleResizeRequired);
    Session->OnLogMessage.AddUObject(this, &UBlenderBridgeComponent::HandleLogMessage);

    if (!Session->Start(Config))
    {
        if (LastStatusMessage.IsEmpty())
        {
            HandleLogMessage(TEXT("Failed to start Blender bridge session."));
        }
    }
}

void UBlenderBridgeComponent::StopBridge()
{
    if (Session.IsValid())
    {
        TSharedPtr<FBlenderBridgeWorkerSession> SessionToStop = MoveTemp(Session);
        SessionToStop->OnStateChanged.RemoveAll(this);
        SessionToStop->OnSchemaReceived.RemoveAll(this);
        SessionToStop->OnCookComplete.RemoveAll(this);
        SessionToStop->OnResizeRequired.RemoveAll(this);
        SessionToStop->OnLogMessage.RemoveAll(this);

        Async(EAsyncExecution::Thread, [SessionToStop = MoveTemp(SessionToStop)]() mutable
        {
            SessionToStop->Stop();
        });
    }

    if (PreviewMeshComponent != nullptr)
    {
        PreviewMeshComponent->ClearAllMeshSections();
    }

    ConnectionState = EBlenderBridgeConnectionState::Disconnected;
    LastStatusMessage = TEXT("Stopping bridge session.");
}

void UBlenderBridgeComponent::RefreshInputSchema()
{
    if (Session.IsValid())
    {
        Session->RequestSchema();
    }
}

void UBlenderBridgeComponent::RequestCook()
{
    QueueCurrentInputMessages(true, TEXT("Queueing cook request."));
}

void UBlenderBridgeComponent::PushCurrentInputs()
{
    QueueCurrentInputMessages(false, TEXT("Queueing input push to Blender worker."));
}

void UBlenderBridgeComponent::OnComponentDestroyed(const bool bDestroyingHierarchy)
{
    StopBridge();
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

#if WITH_EDITOR
void UBlenderBridgeComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName ChangedPropertyName = PropertyChangedEvent.GetMemberPropertyName();
    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, InputValues) && Session.IsValid() && Session->IsConnected())
    {
        QueueCurrentInputMessages(true, TEXT("Queueing cook after input edit."));
    }
}
#endif

bool UBlenderBridgeComponent::QueueCurrentInputMessages(const bool bRequestCook, const TCHAR* ContextMessage)
{
    if (!Session.IsValid())
    {
        return false;
    }

    if (!Session->IsConnected())
    {
        HandleLogMessage(TEXT("Bridge is still initializing. Wait for the input schema before cooking."));
        return false;
    }

    TArray<FBlenderBridgePendingMessage> PendingMessages;

    TSharedRef<FJsonObject> SetInputsPayload = MakeShared<FJsonObject>();
    BlenderBridge::AddInputValuesToPayload(InputSchema, InputValues, SetInputsPayload);

    FString SerializedSetInputs;
    if (!BlenderBridge::SerializeBridgeMessage(EBlenderBridgeMessageType::SetInputs, SetInputsPayload, SerializedSetInputs))
    {
        HandleLogMessage(TEXT("Failed to build SetInputs payload."));
        return false;
    }

    FBlenderBridgePendingMessage PendingSetInputs;
    PendingSetInputs.Type = EBlenderBridgeMessageType::SetInputs;
    PendingSetInputs.SerializedLine = MoveTemp(SerializedSetInputs);
    PendingMessages.Add(MoveTemp(PendingSetInputs));

    if (bRequestCook)
    {
        TSharedRef<FJsonObject> CookPayload = MakeShared<FJsonObject>();
        FString SerializedCook;
        if (!BlenderBridge::SerializeBridgeMessage(EBlenderBridgeMessageType::Cook, CookPayload, SerializedCook))
        {
            HandleLogMessage(TEXT("Failed to build Cook payload."));
            return false;
        }

        FBlenderBridgePendingMessage PendingCook;
        PendingCook.Type = EBlenderBridgeMessageType::Cook;
        PendingCook.SerializedLine = MoveTemp(SerializedCook);
        PendingMessages.Add(MoveTemp(PendingCook));
    }

    HandleLogMessage(ContextMessage);
    Session->SendSerializedMessagesAsync(MoveTemp(PendingMessages));
    return true;
}

void UBlenderBridgeComponent::EnsurePreviewMeshComponent()
{
    if (PreviewMeshComponent != nullptr)
    {
        return;
    }

    AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return;
    }

    PreviewMeshComponent = NewObject<UProceduralMeshComponent>(Owner, TEXT("BlenderBridgePreviewMesh"));
    Owner->AddInstanceComponent(PreviewMeshComponent);
    PreviewMeshComponent->SetupAttachment(this);
    PreviewMeshComponent->SetMobility(EComponentMobility::Movable);
    PreviewMeshComponent->RegisterComponent();
}

void UBlenderBridgeComponent::ApplyMeshSlot(const uint32 SlotIndex)
{
    if (!Session.IsValid() || PreviewMeshComponent == nullptr)
    {
        return;
    }

    UE_LOG(LogBlenderBridge, Log, TEXT("Applying bridge mesh from slot %u"), SlotIndex);

    FBridgeDecodedMesh DecodedMesh;
    if (!Session->GetSharedMemory().ReadMesh(SlotIndex, DecodedMesh))
    {
        HandleLogMessage(FString::Printf(TEXT("Failed to read mesh slot %u"), SlotIndex));
        return;
    }

    UE_LOG(
        LogBlenderBridge,
        Log,
        TEXT("Decoded bridge mesh from slot %u with %d vertices and %d indices"),
        SlotIndex,
        DecodedMesh.Positions.Num(),
        DecodedMesh.Triangles.Num());

    TArray<FColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;
    PreviewMeshComponent->ClearAllMeshSections();
    PreviewMeshComponent->CreateMeshSection(
        0,
        DecodedMesh.Positions,
        DecodedMesh.Triangles,
        DecodedMesh.Normals,
        DecodedMesh.UV0,
        VertexColors,
        Tangents,
        false);
    UE_LOG(LogBlenderBridge, Log, TEXT("Created procedural mesh section for slot %u"), SlotIndex);
}

UUserDefinedEnum* UBlenderBridgeComponent::GetOrCreateRuntimeEnum(const FBridgeInputSchema& Entry)
{
    if (Entry.Type != EBlenderBridgeInputType::Enum)
    {
        return nullptr;
    }

    if (Entry.EnumOptions.IsEmpty())
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Bridge enum input '%s' has no options to expose."), *Entry.Id.ToString());
        RuntimeInputEnums.Remove(Entry.Id);
        RuntimeInputEnumHashes.Remove(Entry.Id);
        return nullptr;
    }

    for (const FBridgeEnumOption& Option : Entry.EnumOptions)
    {
        if (!IsBridgeEnumValueRepresentable(Option.Value))
        {
            UE_LOG(
                LogBlenderBridge,
                Warning,
                TEXT("Bridge enum input '%s' uses unsupported value '%d'. Falling back to integer editing."),
                *Entry.Id.ToString(),
                Option.Value);
            RuntimeInputEnums.Remove(Entry.Id);
            RuntimeInputEnumHashes.Remove(Entry.Id);
            return nullptr;
        }
    }

    const uint32 Signature = ComputeBridgeEnumSignature(Entry);
    if (const uint32* ExistingSignature = RuntimeInputEnumHashes.Find(Entry.Id))
    {
        if (*ExistingSignature == Signature)
        {
            if (const TObjectPtr<UUserDefinedEnum>* ExistingEnum = RuntimeInputEnums.Find(Entry.Id))
            {
                return ExistingEnum->Get();
            }
        }
    }

    UUserDefinedEnum* RuntimeEnum = NewObject<UUserDefinedEnum>(
        this,
        MakeUniqueObjectName(this, UUserDefinedEnum::StaticClass(), *MakeBridgeEnumObjectBaseName(Entry)),
        RF_Transient);
    if (RuntimeEnum == nullptr)
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Failed to allocate runtime enum for bridge input '%s'."), *Entry.Id.ToString());
        return nullptr;
    }

    TArray<TPair<FName, int64>> EmptyNames;
    RuntimeEnum->SetEnums(EmptyNames, UEnum::ECppForm::Namespaced);

    TArray<TPair<FName, int64>> Names;
    Names.Reserve(Entry.EnumOptions.Num());

    TSet<FName> UsedNames;
    for (int32 Index = 0; Index < Entry.EnumOptions.Num(); ++Index)
    {
        const FString BaseShortName = MakeBridgeEnumEntryShortName(Entry.EnumOptions[Index], Index);
        int32 Suffix = 0;

        while (true)
        {
            const FString CandidateShortName = Suffix == 0
                ? BaseShortName
                : FString::Printf(TEXT("%s_%d"), *BaseShortName, Suffix);
            const FName FullName(*RuntimeEnum->GenerateFullEnumName(*CandidateShortName));
            if (!UsedNames.Contains(FullName))
            {
                UsedNames.Add(FullName);
                Names.Emplace(FullName, Entry.EnumOptions[Index].Value);
                break;
            }

            ++Suffix;
        }
    }

    if (!RuntimeEnum->SetEnums(Names, UEnum::ECppForm::Namespaced))
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Failed to configure runtime enum for bridge input '%s'."), *Entry.Id.ToString());
        return nullptr;
    }

    RuntimeEnum->DisplayNameMap.Reset();
    for (int32 Index = 0; Index < Entry.EnumOptions.Num(); ++Index)
    {
        const FString EnumEntryName = RuntimeEnum->GetNameStringByIndex(Index);
        const FString DisplayLabel = Entry.EnumOptions[Index].Label.IsEmpty()
            ? (Entry.EnumOptions[Index].Id.IsEmpty() ? EnumEntryName : Entry.EnumOptions[Index].Id)
            : Entry.EnumOptions[Index].Label;
        RuntimeEnum->DisplayNameMap.Add(*EnumEntryName, FText::FromString(DisplayLabel));
    }

    RuntimeInputEnums.Add(Entry.Id, RuntimeEnum);
    RuntimeInputEnumHashes.Add(Entry.Id, Signature);
    return RuntimeEnum;
}

void UBlenderBridgeComponent::RebuildInputValues(const bool bPreserveExistingValues)
{
    FInstancedPropertyBag NewBag;
    TArray<FPropertyBagPropertyDesc> Descriptors;
    Descriptors.Reserve(InputSchema.Num());
    TSet<FName> ActiveEnumIds;

    for (const FBridgeInputSchema& Entry : InputSchema)
    {
        EPropertyBagPropertyType ValueType = EPropertyBagPropertyType::Float;
        const UObject* ValueTypeObject = nullptr;
        switch (Entry.Type)
        {
        case EBlenderBridgeInputType::Integer:
            ValueType = EPropertyBagPropertyType::Int32;
            break;
        case EBlenderBridgeInputType::Boolean:
            ValueType = EPropertyBagPropertyType::Bool;
            break;
        case EBlenderBridgeInputType::Vector3:
            ValueType = EPropertyBagPropertyType::Struct;
            ValueTypeObject = TBaseStructure<FVector>::Get();
            break;
        case EBlenderBridgeInputType::Color:
            ValueType = EPropertyBagPropertyType::Struct;
            ValueTypeObject = TBaseStructure<FLinearColor>::Get();
            break;
        case EBlenderBridgeInputType::String:
            ValueType = EPropertyBagPropertyType::String;
            break;
        case EBlenderBridgeInputType::Enum:
            ActiveEnumIds.Add(Entry.Id);
            if (UUserDefinedEnum* RuntimeEnum = GetOrCreateRuntimeEnum(Entry))
            {
                ValueType = EPropertyBagPropertyType::Enum;
                ValueTypeObject = RuntimeEnum;
            }
            else
            {
                ValueType = EPropertyBagPropertyType::Int32;
            }
            break;
        case EBlenderBridgeInputType::Float:
        default:
            ValueType = EPropertyBagPropertyType::Float;
            break;
        }

        FPropertyBagPropertyDesc Descriptor(GetBridgeInputBagPropertyName(Entry), ValueType, ValueTypeObject);
#if WITH_EDITOR
        Descriptor.SetMetaData(TEXT("DisplayName"), Entry.Label);
        if (!Entry.CategoryPath.IsEmpty())
        {
            Descriptor.SetMetaData(BridgeEnableCategoriesMetaKey, TEXT(""));
            Descriptor.SetMetaData(BridgeCategoryMetaKey, Entry.CategoryPath);
        }

        if (!Entry.Description.IsEmpty())
        {
            Descriptor.SetMetaData(BridgeToolTipMetaKey, Entry.Description);
        }

        if (Entry.Type == EBlenderBridgeInputType::Float || Entry.Type == EBlenderBridgeInputType::Integer)
        {
            Descriptor.SetMetaData(TEXT("ClampMin"), LexToString(Entry.MinNumericValue));
            Descriptor.SetMetaData(TEXT("ClampMax"), LexToString(Entry.MaxNumericValue));
        }
#endif
        Descriptors.Add(MoveTemp(Descriptor));
    }

    for (auto It = RuntimeInputEnums.CreateIterator(); It; ++It)
    {
        if (!ActiveEnumIds.Contains(It->Key))
        {
            RuntimeInputEnumHashes.Remove(It->Key);
            It.RemoveCurrent();
        }
    }

    for (auto It = RuntimeInputEnumHashes.CreateIterator(); It; ++It)
    {
        if (!ActiveEnumIds.Contains(It->Key))
        {
            It.RemoveCurrent();
        }
    }

    NewBag.AddProperties(Descriptors);

    for (const FBridgeInputSchema& Entry : InputSchema)
    {
        const FName BagPropertyName = GetBridgeInputBagPropertyName(Entry);

        switch (Entry.Type)
        {
        case EBlenderBridgeInputType::Integer:
            NewBag.SetValueInt32(BagPropertyName, static_cast<int32>(Entry.DefaultNumericValue));
            break;
        case EBlenderBridgeInputType::Boolean:
            NewBag.SetValueBool(BagPropertyName, Entry.bDefaultBoolValue);
            break;
        case EBlenderBridgeInputType::Vector3:
            NewBag.SetValueStruct(BagPropertyName, Entry.DefaultVectorValue);
            break;
        case EBlenderBridgeInputType::Color:
            NewBag.SetValueStruct(BagPropertyName, Entry.DefaultColorValue);
            break;
        case EBlenderBridgeInputType::String:
            NewBag.SetValueString(BagPropertyName, Entry.DefaultStringValue);
            break;
        case EBlenderBridgeInputType::Enum:
            if (const TObjectPtr<UUserDefinedEnum>* RuntimeEnum = RuntimeInputEnums.Find(Entry.Id); RuntimeEnum != nullptr && RuntimeEnum->Get() != nullptr)
            {
                NewBag.SetValueEnum(BagPropertyName, static_cast<uint8>(GetPreferredBridgeEnumValue(Entry)), RuntimeEnum->Get());
            }
            else
            {
                NewBag.SetValueInt32(BagPropertyName, GetPreferredBridgeEnumValue(Entry));
            }
            break;
        case EBlenderBridgeInputType::Float:
        default:
            NewBag.SetValueFloat(BagPropertyName, static_cast<float>(Entry.DefaultNumericValue));
            break;
        }
    }

    if (bPreserveExistingValues)
    {
        NewBag.CopyMatchingValuesByName(InputValues);
    }

    for (const FBridgeInputSchema& Entry : InputSchema)
    {
        if (!bPreserveExistingValues || Entry.Type != EBlenderBridgeInputType::Enum)
        {
            continue;
        }

        const FName BagPropertyName = GetBridgeInputBagPropertyName(Entry);
        const int32 PreviousValue = GetStoredBridgeEnumValue(InputValues, BagPropertyName);
        if (!IsBridgeEnumValueRepresentable(PreviousValue) || !HasBridgeEnumValue(Entry, PreviousValue))
        {
            continue;
        }

        if (const TObjectPtr<UUserDefinedEnum>* RuntimeEnum = RuntimeInputEnums.Find(Entry.Id); RuntimeEnum != nullptr && RuntimeEnum->Get() != nullptr)
        {
            NewBag.SetValueEnum(BagPropertyName, static_cast<uint8>(PreviousValue), RuntimeEnum->Get());
        }
        else
        {
            NewBag.SetValueInt32(BagPropertyName, PreviousValue);
        }
    }

    InputValues = MoveTemp(NewBag);
}

void UBlenderBridgeComponent::HandleStateChanged(const EBlenderBridgeConnectionState NewState)
{
    ConnectionState = NewState;
}

void UBlenderBridgeComponent::HandleSchemaReceived(const TArray<FBridgeInputSchema>& NewSchema)
{
    InputSchema = NewSchema;
    RebuildInputValues(!bResetInputValuesFromSchemaOnNextRefresh);
    bResetInputValuesFromSchemaOnNextRefresh = false;
    HandleLogMessage(FString::Printf(TEXT("Bridge ready. Received %d bridge inputs."), InputSchema.Num()));

    if (bRequestCookAfterReconnect)
    {
        bRequestCookAfterReconnect = false;
        QueueCurrentInputMessages(true, TEXT("Shared-memory resize complete. Retrying cook request."));
    }
}

void UBlenderBridgeComponent::HandleCookComplete(const uint32 SlotIndex)
{
    ApplyMeshSlot(SlotIndex);
    HandleLogMessage(FString::Printf(TEXT("Applied bridge mesh from slot %u"), SlotIndex));
}

void UBlenderBridgeComponent::HandleResizeRequired(const uint64 RequiredSlotBytes)
{
    const uint64 CurrentTotalBytes = static_cast<uint64>(FMath::Max<int64>(SharedMemoryBytes, static_cast<int64>(BlenderBridge::DefaultSharedMemoryBytes)));
    const uint64 ExpandedSharedMemoryBytes = ComputeExpandedSharedMemoryBytes(CurrentTotalBytes, RequiredSlotBytes);
    if (ExpandedSharedMemoryBytes <= CurrentTotalBytes)
    {
        HandleLogMessage(
            FString::Printf(
                TEXT("ResizeRequired requested %llu bytes per slot, but the current shared-memory budget of %llu bytes is already large enough."),
                RequiredSlotBytes,
                CurrentTotalBytes));
        return;
    }

    SharedMemoryBytes = static_cast<int64>(ExpandedSharedMemoryBytes);
    HandleLogMessage(
        FString::Printf(
            TEXT("Growing shared memory from %llu to %llu bytes to satisfy a %llu-byte slot request. Restarting bridge and retrying cook."),
            CurrentTotalBytes,
            ExpandedSharedMemoryBytes,
            RequiredSlotBytes));
    StartBridgeInternal(false, true);
}

void UBlenderBridgeComponent::HandleLogMessage(const FString& Message)
{
    LastStatusMessage = Message;
}

FString UBlenderBridgeComponent::ResolveWorkerScriptPath() const
{
    if (const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("BlenderBridge")))
    {
        return FPaths::Combine(Plugin->GetBaseDir(), TEXT("Scripts/bridge_server.py"));
    }

    return FString();
}
