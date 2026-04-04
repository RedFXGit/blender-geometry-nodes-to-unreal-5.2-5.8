#include "BlenderBridgeComponent.h"

#include "BlenderBridgeMessages.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSource.h"
#include "BlenderBridgeWorkerSession.h"
#include "Async/Async.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ProceduralMeshComponent.h"
#include "UObject/Class.h"

UBlenderBridgeComponent::UBlenderBridgeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBlenderBridgeComponent::StartBridge()
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

    if (Source != nullptr)
    {
        Config.BlendFilePath = Source->BlendFile.FilePath;
        Config.ObjectName = Source->ObjectName;
        Config.ModifierName = Source->ModifierName;
        Config.bHelloWorldMode = Source->bHelloWorldMode;

        if (!Config.bHelloWorldMode && Config.BlendFilePath.IsEmpty())
        {
            HandleLogMessage(TEXT("Blend File is required when hello world mode is disabled."));
            ConnectionState = EBlenderBridgeConnectionState::Error;
            return;
        }
    }

    HandleLogMessage(
        FString::Printf(
            TEXT("Starting bridge in %s mode. Blend='%s' Object='%s' Modifier='%s'"),
            Config.bHelloWorldMode ? TEXT("hello-world") : TEXT("real-mesh"),
            *Config.BlendFilePath,
            *Config.ObjectName,
            *Config.ModifierName));

    Session = MakeShared<FBlenderBridgeWorkerSession>();
    Session->OnStateChanged.AddUObject(this, &UBlenderBridgeComponent::HandleStateChanged);
    Session->OnSchemaReceived.AddUObject(this, &UBlenderBridgeComponent::HandleSchemaReceived);
    Session->OnCookComplete.AddUObject(this, &UBlenderBridgeComponent::HandleCookComplete);
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

void UBlenderBridgeComponent::RequestHelloWorldCook()
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

void UBlenderBridgeComponent::RebuildInputValues()
{
    FInstancedPropertyBag NewBag;
    TArray<FPropertyBagPropertyDesc> Descriptors;
    Descriptors.Reserve(InputSchema.Num());

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
        case EBlenderBridgeInputType::Float:
        default:
            ValueType = EPropertyBagPropertyType::Float;
            break;
        }

        FPropertyBagPropertyDesc Descriptor(FInstancedPropertyBag::SanitizePropertyName(Entry.Id), ValueType, ValueTypeObject);
#if WITH_EDITOR
        Descriptor.SetMetaData(TEXT("DisplayName"), Entry.Label);
        if (Entry.Type == EBlenderBridgeInputType::Float || Entry.Type == EBlenderBridgeInputType::Integer)
        {
            Descriptor.SetMetaData(TEXT("ClampMin"), LexToString(Entry.MinNumericValue));
            Descriptor.SetMetaData(TEXT("ClampMax"), LexToString(Entry.MaxNumericValue));
        }
#endif
        Descriptors.Add(MoveTemp(Descriptor));
    }

    NewBag.AddProperties(Descriptors);
    NewBag.CopyMatchingValuesByName(InputValues);

    for (const FBridgeInputSchema& Entry : InputSchema)
    {
        switch (Entry.Type)
        {
        case EBlenderBridgeInputType::Integer:
            NewBag.SetValueInt32(Entry.Id, static_cast<int32>(Entry.DefaultNumericValue));
            break;
        case EBlenderBridgeInputType::Boolean:
            NewBag.SetValueBool(Entry.Id, Entry.bDefaultBoolValue);
            break;
        case EBlenderBridgeInputType::Vector3:
            NewBag.SetValueStruct(Entry.Id, Entry.DefaultVectorValue);
            break;
        case EBlenderBridgeInputType::Color:
            NewBag.SetValueStruct(Entry.Id, Entry.DefaultColorValue);
            break;
        case EBlenderBridgeInputType::String:
            NewBag.SetValueString(Entry.Id, Entry.DefaultStringValue);
            break;
        case EBlenderBridgeInputType::Float:
        default:
            NewBag.SetValueFloat(Entry.Id, static_cast<float>(Entry.DefaultNumericValue));
            break;
        }
    }

    NewBag.CopyMatchingValuesByName(InputValues);
    InputValues = MoveTemp(NewBag);
}

void UBlenderBridgeComponent::HandleStateChanged(const EBlenderBridgeConnectionState NewState)
{
    ConnectionState = NewState;
}

void UBlenderBridgeComponent::HandleSchemaReceived(const TArray<FBridgeInputSchema>& NewSchema)
{
    InputSchema = NewSchema;
    RebuildInputValues();
    HandleLogMessage(FString::Printf(TEXT("Bridge ready. Received %d bridge inputs."), InputSchema.Num()));
}

void UBlenderBridgeComponent::HandleCookComplete(const uint32 SlotIndex)
{
    ApplyMeshSlot(SlotIndex);
    HandleLogMessage(FString::Printf(TEXT("Applied bridge mesh from slot %u"), SlotIndex));
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
