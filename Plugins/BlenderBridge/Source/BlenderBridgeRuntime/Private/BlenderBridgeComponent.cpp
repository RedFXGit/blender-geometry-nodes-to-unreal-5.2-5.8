#include "BlenderBridgeComponent.h"

#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSessionManager.h"
#include "BlenderBridgeSource.h"
#include "Engine/UserDefinedEnum.h"
#include "GameFramework/Actor.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
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
}

UBlenderBridgeComponent::UBlenderBridgeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    BridgeBuildVersion = BLENDER_BRIDGE_BUILD_VERSION;
}

void UBlenderBridgeComponent::OnRegister()
{
    Super::OnRegister();

    if (FModuleManager::Get().IsModuleLoaded(TEXT("BlenderBridgeRuntime")))
    {
        ConnectionState = FBlenderBridgeRuntimeModule::Get().GetSessionManager().GetConnectionState();
    }
}

void UBlenderBridgeComponent::RefreshInputSchema()
{
    FBlenderBridgeRuntimeModule::Get().GetSessionManager().RefreshComponentSchema(this, true);
}

void UBlenderBridgeComponent::RequestCook()
{
    FBlenderBridgeRuntimeModule::Get().GetSessionManager().RequestCook(this);
}

TArray<FString> UBlenderBridgeComponent::GetAvailableTargetOptions() const
{
    TArray<FString> Options;
    Options.Reserve(AvailableTargets.Num() + (TargetId.IsEmpty() ? 0 : 1));

    for (const FBridgeTargetDescriptor& Target : AvailableTargets)
    {
        Options.Add(Target.TargetId);
    }

    if (!TargetId.IsEmpty() && !Options.Contains(TargetId))
    {
        Options.Add(TargetId);
    }

    return Options;
}

void UBlenderBridgeComponent::OnComponentDestroyed(const bool bDestroyingHierarchy)
{
    ResetBridgeData(false);
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

#if WITH_EDITOR
void UBlenderBridgeComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName ChangedPropertyName = PropertyChangedEvent.GetMemberPropertyName();
    FBlenderBridgeSessionManager& SessionManager = FBlenderBridgeRuntimeModule::Get().GetSessionManager();

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, Source))
    {
        SessionManager.NotifySourceChanged(this);
        return;
    }

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, TargetId))
    {
        SessionManager.NotifyTargetChanged(this);
        return;
    }

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, InputValues)
        && ConnectionState == EBlenderBridgeConnectionState::Connected)
    {
        SessionManager.RequestCook(this);
    }
}
#endif

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

void UBlenderBridgeComponent::ApplyDecodedMesh(const FBridgeDecodedMesh& DecodedMesh)
{
    EnsurePreviewMeshComponent();
    if (PreviewMeshComponent == nullptr)
    {
        SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to create the preview mesh component."));
        return;
    }

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
            Descriptor.SetMetaData(TEXT("ClampMin"), ::LexToString(Entry.MinNumericValue));
            Descriptor.SetMetaData(TEXT("ClampMax"), ::LexToString(Entry.MaxNumericValue));
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

void UBlenderBridgeComponent::ResetBridgeData(const bool bClearTargetSelection)
{
    if (PreviewMeshComponent != nullptr)
    {
        PreviewMeshComponent->ClearAllMeshSections();
    }

    AvailableTargets.Reset();
    SchemaTargetId.Reset();
    InputSchema.Reset();
    TargetDisplayLabel.Reset();
    RuntimeInputEnums.Reset();
    RuntimeInputEnumHashes.Reset();
    InputValues = FInstancedPropertyBag();

    if (bClearTargetSelection)
    {
        TargetId.Reset();
    }
}

FString UBlenderBridgeComponent::GetSourceHandle() const
{
    if (Source == nullptr || Source->BlendFile.FilePath.IsEmpty())
    {
        return FString();
    }

    FString SourceHandle = FPaths::ConvertRelativePathToFull(Source->BlendFile.FilePath);
    FPaths::NormalizeFilename(SourceHandle);
    return SourceHandle;
}

const FBridgeTargetDescriptor* UBlenderBridgeComponent::FindTargetDescriptor(const FString& InTargetId) const
{
    return AvailableTargets.FindByPredicate([&InTargetId](const FBridgeTargetDescriptor& Target)
    {
        return Target.TargetId == InTargetId;
    });
}

void UBlenderBridgeComponent::UpdateTargetDisplayLabel()
{
    if (const FBridgeTargetDescriptor* TargetDescriptor = FindTargetDescriptor(TargetId))
    {
        TargetDisplayLabel = TargetDescriptor->DisplayLabel;
        return;
    }

    TargetDisplayLabel = TargetId;
}

void UBlenderBridgeComponent::SetBridgeStatus(const EBlenderBridgeComponentStatus NewStatus, const FString& Message)
{
    ComponentStatus = NewStatus;
    LastStatusMessage = Message;
}
