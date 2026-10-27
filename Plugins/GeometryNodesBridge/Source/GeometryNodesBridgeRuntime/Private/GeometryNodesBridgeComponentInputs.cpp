#include "GeometryNodesBridgeComponent.h"

#include "GeometryNodesBridgeRuntimeModule.h"
#include "Engine/UserDefinedEnum.h"
#include "Misc/SecureHash.h"
#include "Types/AttributeStorage.h"
#include "UObject/UObjectGlobals.h"

namespace
{
    constexpr TCHAR BridgeEnableCategoriesMetaKey[] = TEXT("EnableCategories");
    constexpr TCHAR BridgeCategoryMetaKey[] = TEXT("Category");
    constexpr TCHAR BridgeToolTipMetaKey[] = TEXT("ToolTip");

    const TCHAR* LexToString(const EGeometryNodesBridgeInputType Type)
    {
        switch (Type)
        {
        case EGeometryNodesBridgeInputType::Float:
            return TEXT("Float");
        case EGeometryNodesBridgeInputType::Integer:
            return TEXT("Integer");
        case EGeometryNodesBridgeInputType::Boolean:
            return TEXT("Boolean");
        case EGeometryNodesBridgeInputType::Vector3:
            return TEXT("Vector3");
        case EGeometryNodesBridgeInputType::Color:
            return TEXT("Color");
        case EGeometryNodesBridgeInputType::String:
            return TEXT("String");
        case EGeometryNodesBridgeInputType::Enum:
            return TEXT("Enum");
        case EGeometryNodesBridgeInputType::Unsupported:
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

    FString ComputeBridgeEnumObjectKey(const FBridgeInputSchema& Entry)
    {
        FString Serialized = Entry.Id.ToString();
        Serialized += TEXT("|");
        Serialized += FString::FromInt(Entry.DefaultEnumValue);

        for (const FBridgeEnumOption& Option : Entry.EnumOptions)
        {
            Serialized += TEXT("|");
            Serialized += Option.Id;
            Serialized += TEXT("|");
            Serialized += Option.Label;
            Serialized += TEXT("|");
            Serialized += FString::FromInt(Option.Value);
        }

        return FMD5::HashAnsiString(*Serialized);
    }

    FString MakeBridgeEnumObjectName(const FBridgeInputSchema& Entry)
    {
        return FString::Printf(TEXT("%s_%s"), *MakeBridgeEnumObjectBaseName(Entry), *ComputeBridgeEnumObjectKey(Entry));
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

UUserDefinedEnum* UGeometryNodesBridgeComponent::GetOrCreateRuntimeEnum(const FBridgeInputSchema& Entry)
{
    if (Entry.Type != EGeometryNodesBridgeInputType::Enum)
    {
        return nullptr;
    }

    if (Entry.EnumOptions.IsEmpty())
    {
        UE_LOG(LogGeometryNodesBridge, Warning, TEXT("Bridge enum input '%s' has no options to expose."), *Entry.Id.ToString());
        RuntimeInputEnums.Remove(Entry.Id);
        RuntimeInputEnumHashes.Remove(Entry.Id);
        return nullptr;
    }

    for (const FBridgeEnumOption& Option : Entry.EnumOptions)
    {
        if (!IsBridgeEnumValueRepresentable(Option.Value))
        {
            UE_LOG(
                LogGeometryNodesBridge,
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

    const FString RuntimeEnumObjectName = MakeBridgeEnumObjectName(Entry);
    if (UUserDefinedEnum* ExistingGlobalEnum = FindObject<UUserDefinedEnum>(GetTransientPackage(), *RuntimeEnumObjectName))
    {
        RuntimeInputEnums.Add(Entry.Id, ExistingGlobalEnum);
        RuntimeInputEnumHashes.Add(Entry.Id, Signature);
        return ExistingGlobalEnum;
    }

    UUserDefinedEnum* RuntimeEnum = NewObject<UUserDefinedEnum>(
        GetTransientPackage(),
        *RuntimeEnumObjectName,
        RF_Transient);
    if (RuntimeEnum == nullptr)
    {
        UE_LOG(LogGeometryNodesBridge, Warning, TEXT("Failed to allocate runtime enum for bridge input '%s'."), *Entry.Id.ToString());
        return nullptr;
    }

    TArray<TPair<FName, int64>> EmptyNames;
    RuntimeEnum->SetEnums(EmptyNames, UEnum::ECppForm::Namespaced, UEnum::EUnderlyingType::int32, EEnumFlags::None, UEnum::EAddMaxKeyIfMissing::No);

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

    if (!RuntimeEnum->SetEnums(EmptyNames, UEnum::ECppForm::Namespaced, UEnum::EUnderlyingType::int32, EEnumFlags::None, UEnum::EAddMaxKeyIfMissing::No))
    {
        UE_LOG(LogGeometryNodesBridge, Warning, TEXT("Failed to configure runtime enum for bridge input '%s'."), *Entry.Id.ToString());
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

void UGeometryNodesBridgeComponent::RebuildInputValues(const bool bPreserveExistingValues)
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
        case EGeometryNodesBridgeInputType::Integer:
            ValueType = EPropertyBagPropertyType::Int32;
            break;
        case EGeometryNodesBridgeInputType::Boolean:
            ValueType = EPropertyBagPropertyType::Bool;
            break;
        case EGeometryNodesBridgeInputType::Vector3:
            ValueType = EPropertyBagPropertyType::Struct;
            ValueTypeObject = TBaseStructure<FVector>::Get();
            break;
        case EGeometryNodesBridgeInputType::Color:
            ValueType = EPropertyBagPropertyType::Struct;
            ValueTypeObject = TBaseStructure<FLinearColor>::Get();
            break;
        case EGeometryNodesBridgeInputType::String:
            ValueType = EPropertyBagPropertyType::String;
            break;
        case EGeometryNodesBridgeInputType::Enum:
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
        case EGeometryNodesBridgeInputType::Float:
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

        if (Entry.Type == EGeometryNodesBridgeInputType::Float || Entry.Type == EGeometryNodesBridgeInputType::Integer)
        {
            Descriptor.SetMetaData(TEXT("ClampMin"), ::LexToString(Entry.MinNumericValue));
            Descriptor.SetMetaData(TEXT("ClampMax"), ::LexToString(Entry.MaxNumericValue));
            Descriptor.SetMetaData(TEXT("Delta"), Entry.Type == EGeometryNodesBridgeInputType::Integer ? TEXT("1") : TEXT("0.01"));
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
        case EGeometryNodesBridgeInputType::Integer:
            NewBag.SetValueInt32(BagPropertyName, static_cast<int32>(Entry.DefaultNumericValue));
            break;
        case EGeometryNodesBridgeInputType::Boolean:
            NewBag.SetValueBool(BagPropertyName, Entry.bDefaultBoolValue);
            break;
        case EGeometryNodesBridgeInputType::Vector3:
            NewBag.SetValueStruct(BagPropertyName, Entry.DefaultVectorValue);
            break;
        case EGeometryNodesBridgeInputType::Color:
            NewBag.SetValueStruct(BagPropertyName, Entry.DefaultColorValue);
            break;
        case EGeometryNodesBridgeInputType::String:
            NewBag.SetValueString(BagPropertyName, Entry.DefaultStringValue);
            break;
        case EGeometryNodesBridgeInputType::Enum:
            if (const TObjectPtr<UUserDefinedEnum>* RuntimeEnum = RuntimeInputEnums.Find(Entry.Id); RuntimeEnum != nullptr && RuntimeEnum->Get() != nullptr)
            {
                NewBag.SetValueEnum(BagPropertyName, static_cast<uint8>(GetPreferredBridgeEnumValue(Entry)), RuntimeEnum->Get());
            }
            else
            {
                NewBag.SetValueInt32(BagPropertyName, GetPreferredBridgeEnumValue(Entry));
            }
            break;
        case EGeometryNodesBridgeInputType::Float:
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
        if (!bPreserveExistingValues || Entry.Type != EGeometryNodesBridgeInputType::Enum)
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
