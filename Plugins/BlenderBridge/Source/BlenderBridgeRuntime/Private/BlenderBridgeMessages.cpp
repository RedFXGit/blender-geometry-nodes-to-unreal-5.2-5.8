#include "BlenderBridgeMessages.h"

#include "BlenderBridgeRuntimeModule.h"
#include "Dom/JsonObject.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
    FName GetBagPropertyName(const FBridgeInputSchema& Entry)
    {
        return FInstancedPropertyBag::SanitizePropertyName(Entry.Id);
    }

    EBlenderBridgeInputType ParseInputType(const FString& Value)
    {
        if (Value.Equals(TEXT("float"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeInputType::Float;
        }

        if (Value.Equals(TEXT("int"), ESearchCase::IgnoreCase) || Value.Equals(TEXT("integer"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeInputType::Integer;
        }

        if (Value.Equals(TEXT("bool"), ESearchCase::IgnoreCase) || Value.Equals(TEXT("boolean"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeInputType::Boolean;
        }

        if (Value.Equals(TEXT("vector"), ESearchCase::IgnoreCase) || Value.Equals(TEXT("vector3"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeInputType::Vector3;
        }

        if (Value.Equals(TEXT("color"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeInputType::Color;
        }

        if (Value.Equals(TEXT("string"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeInputType::String;
        }

        if (Value.Equals(TEXT("enum"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeInputType::Enum;
        }

        return EBlenderBridgeInputType::Unsupported;
    }

    bool TryReadNumberArrayField(
        const TSharedPtr<FJsonObject>& Payload,
        const TCHAR* FieldName,
        const int32 ExpectedLength,
        TArray<double>& OutValues)
    {
        OutValues.Reset();

        const TArray<TSharedPtr<FJsonValue>>* ArrayValues = nullptr;
        if (!Payload.IsValid() || !Payload->TryGetArrayField(FieldName, ArrayValues) || ArrayValues == nullptr || ArrayValues->Num() != ExpectedLength)
        {
            return false;
        }

        OutValues.Reserve(ExpectedLength);
        for (const TSharedPtr<FJsonValue>& EntryValue : *ArrayValues)
        {
            double NumberValue = 0.0;
            if (!EntryValue.IsValid() || !EntryValue->TryGetNumber(NumberValue))
            {
                OutValues.Reset();
                return false;
            }

            OutValues.Add(NumberValue);
        }

        return true;
    }

    bool TryReadVectorField(const TSharedPtr<FJsonObject>& Payload, const TCHAR* FieldName, FVector& OutVector)
    {
        TArray<double> Values;
        if (!TryReadNumberArrayField(Payload, FieldName, 3, Values))
        {
            return false;
        }

        OutVector = FVector(Values[0], Values[1], Values[2]);
        return true;
    }

    bool TryReadColorField(const TSharedPtr<FJsonObject>& Payload, const TCHAR* FieldName, FLinearColor& OutColor)
    {
        TArray<double> Values;
        if (!TryReadNumberArrayField(Payload, FieldName, 4, Values))
        {
            return false;
        }

        OutColor = FLinearColor(
            static_cast<float>(Values[0]),
            static_cast<float>(Values[1]),
            static_cast<float>(Values[2]),
            static_cast<float>(Values[3]));
        return true;
    }

    void ParseEnumOptions(const TSharedPtr<FJsonObject>& Payload, TArray<FBridgeEnumOption>& OutOptions)
    {
        OutOptions.Reset();

        const TArray<TSharedPtr<FJsonValue>>* OptionsArray = nullptr;
        if (!Payload.IsValid() || !Payload->TryGetArrayField(TEXT("enum_options"), OptionsArray) || OptionsArray == nullptr)
        {
            return;
        }

        OutOptions.Reserve(OptionsArray->Num());
        for (int32 Index = 0; Index < OptionsArray->Num(); ++Index)
        {
            const TSharedPtr<FJsonObject>* OptionObject = nullptr;
            if (!(*OptionsArray)[Index].IsValid() || !(*OptionsArray)[Index]->TryGetObject(OptionObject) || !OptionObject || !OptionObject->IsValid())
            {
                continue;
            }

            FBridgeEnumOption Option;
            OptionObject->Get()->TryGetStringField(TEXT("id"), Option.Id);
            OptionObject->Get()->TryGetStringField(TEXT("label"), Option.Label);

            double RawValue = static_cast<double>(Index);
            OptionObject->Get()->TryGetNumberField(TEXT("value"), RawValue);
            Option.Value = static_cast<int32>(RawValue);

            if (Option.Id.IsEmpty())
            {
                Option.Id = Option.Label;
            }

            if (Option.Label.IsEmpty())
            {
                Option.Label = Option.Id.IsEmpty() ? FString::Printf(TEXT("Option %d"), Index) : Option.Id;
            }

            if (Option.Id.IsEmpty())
            {
                Option.Id = FString::Printf(TEXT("Option_%d"), Index);
            }

            OutOptions.Add(MoveTemp(Option));
        }
    }

    void SetNumberArrayField(const TSharedRef<FJsonObject>& Payload, const FString& FieldName, const TArray<double>& Values)
    {
        TArray<TSharedPtr<FJsonValue>> ArrayValues;
        ArrayValues.Reserve(Values.Num());
        for (const double Value : Values)
        {
            ArrayValues.Add(MakeShared<FJsonValueNumber>(Value));
        }

        Payload->SetArrayField(FieldName, ArrayValues);
    }

    void SetVectorField(const TSharedRef<FJsonObject>& Payload, const FString& FieldName, const FVector& Value)
    {
        TArray<double> Values;
        Values.Reserve(3);
        Values.Add(Value.X);
        Values.Add(Value.Y);
        Values.Add(Value.Z);
        SetNumberArrayField(Payload, FieldName, Values);
    }

    void SetColorField(const TSharedRef<FJsonObject>& Payload, const FString& FieldName, const FLinearColor& Value)
    {
        TArray<double> Values;
        Values.Reserve(4);
        Values.Add(Value.R);
        Values.Add(Value.G);
        Values.Add(Value.B);
        Values.Add(Value.A);
        SetNumberArrayField(Payload, FieldName, Values);
    }
}

bool BlenderBridge::SerializeBridgeMessage(const EBlenderBridgeMessageType Type, const TSharedRef<FJsonObject>& Payload, FString& OutLine)
{
    Payload->SetStringField(TEXT("message_type"), LexToString(Type));

    OutLine.Reset();

    using FBridgeMessageJsonWriter = TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>;
    TSharedRef<FBridgeMessageJsonWriter> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutLine);
    return FJsonSerializer::Serialize(Payload, Writer);
}

bool BlenderBridge::DeserializeBridgeMessage(const FString& Line, FBlenderBridgeJsonMessage& OutMessage)
{
    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Line);

    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Failed to deserialize bridge message: %s"), *Line);
        return false;
    }

    FString MessageTypeText;
    if (!RootObject->TryGetStringField(TEXT("message_type"), MessageTypeText))
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Bridge message missing message_type: %s"), *Line);
        return false;
    }

    TryParseMessageType(MessageTypeText, OutMessage.Type);
    OutMessage.Payload = RootObject;
    return true;
}

bool BlenderBridge::ParseInputSchema(const TSharedPtr<FJsonObject>& Payload, TArray<FBridgeInputSchema>& OutSchema)
{
    OutSchema.Reset();

    const TArray<TSharedPtr<FJsonValue>>* SchemaArray = nullptr;
    if (!Payload.IsValid() || !Payload->TryGetArrayField(TEXT("schema"), SchemaArray))
    {
        return false;
    }

    for (const TSharedPtr<FJsonValue>& EntryValue : *SchemaArray)
    {
        const TSharedPtr<FJsonObject>* EntryObject = nullptr;
        if (!EntryValue.IsValid() || !EntryValue->TryGetObject(EntryObject) || !EntryObject || !EntryObject->IsValid())
        {
            continue;
        }

        FBridgeInputSchema Entry;
        FString IdText;
        FString TypeText;
        EntryObject->Get()->TryGetStringField(TEXT("id"), IdText);
        EntryObject->Get()->TryGetStringField(TEXT("label"), Entry.Label);
        EntryObject->Get()->TryGetStringField(TEXT("category"), Entry.CategoryPath);
        EntryObject->Get()->TryGetStringField(TEXT("description"), Entry.Description);
        EntryObject->Get()->TryGetStringField(TEXT("type"), TypeText);
        EntryObject->Get()->TryGetNumberField(TEXT("default"), Entry.DefaultNumericValue);
        EntryObject->Get()->TryGetNumberField(TEXT("min"), Entry.MinNumericValue);
        EntryObject->Get()->TryGetNumberField(TEXT("max"), Entry.MaxNumericValue);
        EntryObject->Get()->TryGetBoolField(TEXT("default_bool"), Entry.bDefaultBoolValue);
        Entry.DefaultEnumValue = static_cast<int32>(Entry.DefaultNumericValue);

        Entry.Id = FName(*IdText);
        if (Entry.Label.IsEmpty())
        {
            Entry.Label = IdText;
        }

        Entry.Type = ParseInputType(TypeText);

        switch (Entry.Type)
        {
        case EBlenderBridgeInputType::Vector3:
            TryReadVectorField(*EntryObject, TEXT("default_vector"), Entry.DefaultVectorValue);
            break;

        case EBlenderBridgeInputType::Color:
            TryReadColorField(*EntryObject, TEXT("default_color"), Entry.DefaultColorValue);
            break;

        case EBlenderBridgeInputType::String:
            EntryObject->Get()->TryGetStringField(TEXT("default_string"), Entry.DefaultStringValue);
            break;

        case EBlenderBridgeInputType::Enum:
            ParseEnumOptions(*EntryObject, Entry.EnumOptions);
            break;

        default:
            break;
        }

        OutSchema.Add(MoveTemp(Entry));
    }

    return true;
}

bool BlenderBridge::ParseTargetDescriptors(const TSharedPtr<FJsonObject>& Payload, TArray<FBridgeTargetDescriptor>& OutTargets)
{
    OutTargets.Reset();

    const TArray<TSharedPtr<FJsonValue>>* TargetArray = nullptr;
    if (!Payload.IsValid() || !Payload->TryGetArrayField(TEXT("targets"), TargetArray))
    {
        return false;
    }

    for (const TSharedPtr<FJsonValue>& EntryValue : *TargetArray)
    {
        const TSharedPtr<FJsonObject>* EntryObject = nullptr;
        if (!EntryValue.IsValid() || !EntryValue->TryGetObject(EntryObject) || !EntryObject || !EntryObject->IsValid())
        {
            continue;
        }

        FBridgeTargetDescriptor Target;
        EntryObject->Get()->TryGetStringField(TEXT("id"), Target.TargetId);
        EntryObject->Get()->TryGetStringField(TEXT("label"), Target.DisplayLabel);
        EntryObject->Get()->TryGetStringField(TEXT("object_name"), Target.ObjectName);
        EntryObject->Get()->TryGetStringField(TEXT("modifier_name"), Target.ModifierName);

        if (Target.DisplayLabel.IsEmpty())
        {
            if (!Target.ObjectName.IsEmpty() && !Target.ModifierName.IsEmpty())
            {
                Target.DisplayLabel = FString::Printf(TEXT("%s / %s"), *Target.ObjectName, *Target.ModifierName);
            }
            else
            {
                Target.DisplayLabel = Target.TargetId;
            }
        }

        if (!Target.TargetId.IsEmpty())
        {
            OutTargets.Add(MoveTemp(Target));
        }
    }

    return true;
}

bool BlenderBridge::ParseCookedMeshMetadata(
    const TSharedPtr<FJsonObject>& Payload,
    TArray<FBridgeMaterialSlotDescriptor>& OutMaterialSlots,
    TArray<FBridgeMeshSectionDescriptor>& OutSections)
{
    OutMaterialSlots.Reset();
    OutSections.Reset();

    if (!Payload.IsValid())
    {
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* MaterialSlotArray = nullptr;
    if (Payload->TryGetArrayField(TEXT("material_slots"), MaterialSlotArray) && MaterialSlotArray != nullptr)
    {
        for (const TSharedPtr<FJsonValue>& EntryValue : *MaterialSlotArray)
        {
            const TSharedPtr<FJsonObject>* EntryObject = nullptr;
            if (!EntryValue.IsValid() || !EntryValue->TryGetObject(EntryObject) || !EntryObject || !EntryObject->IsValid())
            {
                continue;
            }

            FBridgeMaterialSlotDescriptor Slot;
            EntryObject->Get()->TryGetStringField(TEXT("slot_id"), Slot.SlotId);
            EntryObject->Get()->TryGetStringField(TEXT("slot_name"), Slot.SlotName);
            EntryObject->Get()->TryGetStringField(TEXT("material_name"), Slot.BlenderMaterialName);

            double SectionIndex = static_cast<double>(INDEX_NONE);
            EntryObject->Get()->TryGetNumberField(TEXT("section_index"), SectionIndex);
            Slot.SectionIndex = static_cast<int32>(SectionIndex);

            if (Slot.SlotId.IsEmpty())
            {
                Slot.SlotId = Slot.SlotName.IsEmpty() ? Slot.BlenderMaterialName : Slot.SlotName;
            }

            OutMaterialSlots.Add(MoveTemp(Slot));
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* SectionArray = nullptr;
    if (Payload->TryGetArrayField(TEXT("sections"), SectionArray) && SectionArray != nullptr)
    {
        for (const TSharedPtr<FJsonValue>& EntryValue : *SectionArray)
        {
            const TSharedPtr<FJsonObject>* EntryObject = nullptr;
            if (!EntryValue.IsValid() || !EntryValue->TryGetObject(EntryObject) || !EntryObject || !EntryObject->IsValid())
            {
                continue;
            }

            FBridgeMeshSectionDescriptor Section;
            EntryObject->Get()->TryGetStringField(TEXT("slot_id"), Section.SlotId);
            EntryObject->Get()->TryGetStringField(TEXT("slot_name"), Section.SlotName);
            EntryObject->Get()->TryGetStringField(TEXT("material_name"), Section.BlenderMaterialName);

            double NumericValue = 0.0;
            if (EntryObject->Get()->TryGetNumberField(TEXT("section_index"), NumericValue))
            {
                Section.SectionIndex = static_cast<int32>(NumericValue);
            }
            if (EntryObject->Get()->TryGetNumberField(TEXT("first_vertex"), NumericValue))
            {
                Section.FirstVertex = static_cast<int32>(NumericValue);
            }
            if (EntryObject->Get()->TryGetNumberField(TEXT("vertex_count"), NumericValue))
            {
                Section.VertexCount = static_cast<int32>(NumericValue);
            }
            if (EntryObject->Get()->TryGetNumberField(TEXT("first_index"), NumericValue))
            {
                Section.FirstIndex = static_cast<int32>(NumericValue);
            }
            if (EntryObject->Get()->TryGetNumberField(TEXT("index_count"), NumericValue))
            {
                Section.IndexCount = static_cast<int32>(NumericValue);
            }

            if (Section.SlotId.IsEmpty())
            {
                Section.SlotId = Section.SlotName.IsEmpty() ? Section.BlenderMaterialName : Section.SlotName;
            }

            OutSections.Add(MoveTemp(Section));
        }
    }

    if (OutSections.IsEmpty() && OutMaterialSlots.Num() == 1)
    {
        FBridgeMeshSectionDescriptor Section;
        Section.SectionIndex = 0;
        Section.SlotId = OutMaterialSlots[0].SlotId;
        Section.SlotName = OutMaterialSlots[0].SlotName;
        Section.BlenderMaterialName = OutMaterialSlots[0].BlenderMaterialName;
        OutSections.Add(MoveTemp(Section));
    }

    return true;
}

void BlenderBridge::AddInputValuesToPayload(const TArray<FBridgeInputSchema>& Schema, const FInstancedPropertyBag& InputValues, const TSharedRef<FJsonObject>& Payload)
{
    TSharedRef<FJsonObject> ValuesObject = MakeShared<FJsonObject>();

    for (const FBridgeInputSchema& Entry : Schema)
    {
        const FName BagPropertyName = GetBagPropertyName(Entry);
        const FString PayloadFieldName = Entry.Id.ToString();

        switch (Entry.Type)
        {
        case EBlenderBridgeInputType::Float:
            if (const auto Value = InputValues.GetValueDouble(BagPropertyName); Value.IsValid())
            {
                ValuesObject->SetNumberField(PayloadFieldName, Value.GetValue());
            }
            else if (const auto FloatValue = InputValues.GetValueFloat(BagPropertyName); FloatValue.IsValid())
            {
                ValuesObject->SetNumberField(PayloadFieldName, FloatValue.GetValue());
            }
            break;

        case EBlenderBridgeInputType::Integer:
            if (const auto Value = InputValues.GetValueInt32(BagPropertyName); Value.IsValid())
            {
                ValuesObject->SetNumberField(PayloadFieldName, Value.GetValue());
            }
            break;

        case EBlenderBridgeInputType::Boolean:
            if (const auto Value = InputValues.GetValueBool(BagPropertyName); Value.IsValid())
            {
                ValuesObject->SetBoolField(PayloadFieldName, Value.GetValue());
            }
            break;

        case EBlenderBridgeInputType::Vector3:
            if (const auto Value = InputValues.GetValueStruct<FVector>(BagPropertyName); Value.IsValid() && Value.GetValue() != nullptr)
            {
                SetVectorField(ValuesObject, PayloadFieldName, *Value.GetValue());
            }
            break;

        case EBlenderBridgeInputType::Color:
            if (const auto Value = InputValues.GetValueStruct<FLinearColor>(BagPropertyName); Value.IsValid() && Value.GetValue() != nullptr)
            {
                SetColorField(ValuesObject, PayloadFieldName, *Value.GetValue());
            }
            break;

        case EBlenderBridgeInputType::String:
            if (const auto Value = InputValues.GetValueString(BagPropertyName); Value.IsValid())
            {
                ValuesObject->SetStringField(PayloadFieldName, Value.GetValue());
            }
            break;

        case EBlenderBridgeInputType::Enum:
            if (const FPropertyBagPropertyDesc* Desc = InputValues.FindPropertyDescByName(BagPropertyName))
            {
                if (const UEnum* EnumDefinition = Cast<UEnum>(Desc->ValueTypeObject))
                {
                    if (const auto Value = InputValues.GetValueEnum(BagPropertyName, EnumDefinition); Value.IsValid())
                    {
                        ValuesObject->SetNumberField(PayloadFieldName, Value.GetValue());
                        break;
                    }
                }
            }

            if (const auto IntegerValue = InputValues.GetValueInt32(BagPropertyName); IntegerValue.IsValid())
            {
                ValuesObject->SetNumberField(PayloadFieldName, IntegerValue.GetValue());
            }
            break;

        default:
            break;
        }
    }

    Payload->SetObjectField(TEXT("values"), ValuesObject);
}
