#include "BlenderBridgeMessages.h"

#include "BlenderBridgeRuntimeModule.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
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

    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutLine);
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
        EntryObject->Get()->TryGetStringField(TEXT("type"), TypeText);
        EntryObject->Get()->TryGetNumberField(TEXT("default"), Entry.DefaultNumericValue);
        EntryObject->Get()->TryGetNumberField(TEXT("min"), Entry.MinNumericValue);
        EntryObject->Get()->TryGetNumberField(TEXT("max"), Entry.MaxNumericValue);
        EntryObject->Get()->TryGetBoolField(TEXT("default_bool"), Entry.bDefaultBoolValue);

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

        default:
            break;
        }

        OutSchema.Add(MoveTemp(Entry));
    }

    return true;
}

void BlenderBridge::AddInputValuesToPayload(const TArray<FBridgeInputSchema>& Schema, const FInstancedPropertyBag& InputValues, const TSharedRef<FJsonObject>& Payload)
{
    TSharedRef<FJsonObject> ValuesObject = MakeShared<FJsonObject>();

    for (const FBridgeInputSchema& Entry : Schema)
    {
        switch (Entry.Type)
        {
        case EBlenderBridgeInputType::Float:
            if (const auto Value = InputValues.GetValueDouble(Entry.Id); Value.IsValid())
            {
                ValuesObject->SetNumberField(Entry.Id.ToString(), Value.GetValue());
            }
            else if (const auto FloatValue = InputValues.GetValueFloat(Entry.Id); FloatValue.IsValid())
            {
                ValuesObject->SetNumberField(Entry.Id.ToString(), FloatValue.GetValue());
            }
            break;

        case EBlenderBridgeInputType::Integer:
            if (const auto Value = InputValues.GetValueInt32(Entry.Id); Value.IsValid())
            {
                ValuesObject->SetNumberField(Entry.Id.ToString(), Value.GetValue());
            }
            break;

        case EBlenderBridgeInputType::Boolean:
            if (const auto Value = InputValues.GetValueBool(Entry.Id); Value.IsValid())
            {
                ValuesObject->SetBoolField(Entry.Id.ToString(), Value.GetValue());
            }
            break;

        case EBlenderBridgeInputType::Vector3:
            if (const auto Value = InputValues.GetValueStruct<FVector>(Entry.Id); Value.IsValid() && Value.GetValue() != nullptr)
            {
                SetVectorField(ValuesObject, Entry.Id.ToString(), *Value.GetValue());
            }
            break;

        case EBlenderBridgeInputType::Color:
            if (const auto Value = InputValues.GetValueStruct<FLinearColor>(Entry.Id); Value.IsValid() && Value.GetValue() != nullptr)
            {
                SetColorField(ValuesObject, Entry.Id.ToString(), *Value.GetValue());
            }
            break;

        case EBlenderBridgeInputType::String:
            if (const auto Value = InputValues.GetValueString(Entry.Id); Value.IsValid())
            {
                ValuesObject->SetStringField(Entry.Id.ToString(), Value.GetValue());
            }
            break;

        default:
            break;
        }
    }

    Payload->SetObjectField(TEXT("values"), ValuesObject);
}
