#include "BlenderBridgeMessages.h"

#include "BlenderBridgeRuntimeModule.h"
#include "Dom/JsonObject.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
    constexpr TCHAR CurveOwnerFieldName[] = TEXT("owner_curve");

    FName GetBagPropertyName(const FBridgeInputSchema& Entry)
    {
        return FInstancedPropertyBag::SanitizePropertyName(Entry.Id);
    }

    EBlenderBridgeTargetKind ParseTargetKind(const FString& Value)
    {
        if (Value.Equals(TEXT("curve_owner"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeTargetKind::CurveOwner;
        }

        if (Value.Equals(TEXT("standard"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeTargetKind::Standard;
        }

        return EBlenderBridgeTargetKind::Unknown;
    }

    EBlenderBridgeCurveType ParseCurveType(const FString& Value)
    {
        if (Value.Equals(TEXT("bezier"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeCurveType::Bezier;
        }

        if (Value.IsEmpty() || Value.Equals(TEXT("none"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeCurveType::None;
        }

        return EBlenderBridgeCurveType::Unsupported;
    }

    EBlenderBridgeSplinePointType ParseSplinePointType(const FString& Value)
    {
        if (Value.Equals(TEXT("linear"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeSplinePointType::Linear;
        }

        if (Value.Equals(TEXT("constant"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeSplinePointType::Constant;
        }

        if (Value.Equals(TEXT("curveclamped"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeSplinePointType::CurveClamped;
        }

        if (Value.Equals(TEXT("curvecustomtangent"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeSplinePointType::CurveCustomTangent;
        }

        return EBlenderBridgeSplinePointType::Curve;
    }

    EBlenderBridgeGeometrySourceKind ParseGeometrySourceKind(const FString& Value)
    {
        if (Value.Equals(TEXT("static_mesh_component"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeGeometrySourceKind::StaticMeshComponent;
        }

        if (Value.Equals(TEXT("spline_component"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeGeometrySourceKind::SplineComponent;
        }

        if (Value.Equals(TEXT("embedded_spline"), ESearchCase::IgnoreCase))
        {
            return EBlenderBridgeGeometrySourceKind::EmbeddedSpline;
        }

        return EBlenderBridgeGeometrySourceKind::None;
    }

    FString SplinePointTypeToString(const EBlenderBridgeSplinePointType Value)
    {
        switch (Value)
        {
        case EBlenderBridgeSplinePointType::Linear:
            return TEXT("Linear");
        case EBlenderBridgeSplinePointType::Constant:
            return TEXT("Constant");
        case EBlenderBridgeSplinePointType::CurveClamped:
            return TEXT("CurveClamped");
        case EBlenderBridgeSplinePointType::CurveCustomTangent:
            return TEXT("CurveCustomTangent");
        case EBlenderBridgeSplinePointType::Curve:
        default:
            return TEXT("Curve");
        }
    }

    FString CurveTypeToString(const EBlenderBridgeCurveType Value)
    {
        switch (Value)
        {
        case EBlenderBridgeCurveType::Bezier:
            return TEXT("Bezier");
        case EBlenderBridgeCurveType::Unsupported:
            return TEXT("Unsupported");
        case EBlenderBridgeCurveType::None:
        default:
            return TEXT("None");
        }
    }

    FString GeometrySourceKindToString(const EBlenderBridgeGeometrySourceKind Value)
    {
        switch (Value)
        {
        case EBlenderBridgeGeometrySourceKind::StaticMeshComponent:
            return TEXT("static_mesh_component");
        case EBlenderBridgeGeometrySourceKind::SplineComponent:
            return TEXT("spline_component");
        case EBlenderBridgeGeometrySourceKind::EmbeddedSpline:
            return TEXT("embedded_spline");
        case EBlenderBridgeGeometrySourceKind::None:
        default:
            return TEXT("none");
        }
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

    bool TryReadScaleField(const TSharedPtr<FJsonObject>& Payload, const TCHAR* FieldName, FVector& OutScale)
    {
        return TryReadVectorField(Payload, FieldName, OutScale);
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

    void SetScaleField(const TSharedRef<FJsonObject>& Payload, const FString& FieldName, const FVector& Value)
    {
        SetVectorField(Payload, FieldName, Value);
    }

    void SetTransformField(const TSharedRef<FJsonObject>& Payload, const FString& FieldName, const FTransform& Value)
    {
        TSharedRef<FJsonObject> TransformObject = MakeShared<FJsonObject>();
        SetVectorField(TransformObject, TEXT("translation"), Value.GetTranslation());
        TArray<TSharedPtr<FJsonValue>> RotationValues;
        RotationValues.Reserve(4);
        RotationValues.Add(MakeShared<FJsonValueNumber>(Value.GetRotation().X));
        RotationValues.Add(MakeShared<FJsonValueNumber>(Value.GetRotation().Y));
        RotationValues.Add(MakeShared<FJsonValueNumber>(Value.GetRotation().Z));
        RotationValues.Add(MakeShared<FJsonValueNumber>(Value.GetRotation().W));
        TransformObject->SetArrayField(TEXT("rotation"), RotationValues);
        SetScaleField(TransformObject, TEXT("scale3d"), Value.GetScale3D());
        Payload->SetObjectField(FieldName, TransformObject);
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

bool BlenderBridge::ParseTargetCapabilities(const TSharedPtr<FJsonObject>& Payload, FBridgeTargetCapabilities& OutCapabilities)
{
    OutCapabilities = FBridgeTargetCapabilities();

    if (!Payload.IsValid())
    {
        return false;
    }

    FString TargetKindText;
    Payload->TryGetStringField(TEXT("target_kind"), TargetKindText);
    OutCapabilities.TargetKind = ParseTargetKind(TargetKindText);
    Payload->TryGetBoolField(TEXT("supports_curve_owner"), OutCapabilities.bSupportsCurveOwner);
    Payload->TryGetStringField(TEXT("curve_owner_message"), OutCapabilities.CompatibilityMessage);

    const TSharedPtr<FJsonObject>* CurveOwnerObject = nullptr;
    if (!Payload->TryGetObjectField(CurveOwnerFieldName, CurveOwnerObject) || CurveOwnerObject == nullptr || !CurveOwnerObject->IsValid())
    {
        return true;
    }

    FString CurveTypeText;
    CurveOwnerObject->Get()->TryGetStringField(TEXT("curve_type"), CurveTypeText);
    OutCapabilities.OwnerCurve.CurveType = ParseCurveType(CurveTypeText);
    CurveOwnerObject->Get()->TryGetBoolField(TEXT("closed"), OutCapabilities.OwnerCurve.bClosedLoop);

    const TArray<TSharedPtr<FJsonValue>>* PointArray = nullptr;
    if (!CurveOwnerObject->Get()->TryGetArrayField(TEXT("points"), PointArray) || PointArray == nullptr)
    {
        return true;
    }

    OutCapabilities.OwnerCurve.Points.Reserve(PointArray->Num());
    for (const TSharedPtr<FJsonValue>& PointValue : *PointArray)
    {
        const TSharedPtr<FJsonObject>* PointObject = nullptr;
        if (!PointValue.IsValid() || !PointValue->TryGetObject(PointObject) || PointObject == nullptr || !PointObject->IsValid())
        {
            continue;
        }

        FBridgeCurveOwnerPoint Point;
        FString PointTypeText;
        double RollDegrees = 0.0;
        TryReadVectorField(*PointObject, TEXT("position"), Point.Position);
        TryReadVectorField(*PointObject, TEXT("arrive_tangent"), Point.ArriveTangent);
        TryReadVectorField(*PointObject, TEXT("leave_tangent"), Point.LeaveTangent);
        PointObject->Get()->TryGetNumberField(TEXT("roll_degrees"), RollDegrees);
        Point.RollDegrees = static_cast<float>(RollDegrees);
        TryReadScaleField(*PointObject, TEXT("scale"), Point.Scale);
        PointObject->Get()->TryGetStringField(TEXT("point_type"), PointTypeText);
        Point.Type = ParseSplinePointType(PointTypeText);
        OutCapabilities.OwnerCurve.Points.Add(MoveTemp(Point));
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
        EntryObject->Get()->TryGetStringField(TEXT("object_type"), Target.ObjectType);

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

bool BlenderBridge::ParseGeometryInputDescriptors(
    const TSharedPtr<FJsonObject>& Payload,
    TArray<FBridgeGeometryInputDescriptor>& OutDescriptors)
{
    OutDescriptors.Reset();

    const TArray<TSharedPtr<FJsonValue>>* DescriptorArray = nullptr;
    if (!Payload.IsValid() || !Payload->TryGetArrayField(TEXT("geometry_inputs"), DescriptorArray) || DescriptorArray == nullptr)
    {
        return false;
    }

    for (const TSharedPtr<FJsonValue>& EntryValue : *DescriptorArray)
    {
        const TSharedPtr<FJsonObject>* EntryObject = nullptr;
        if (!EntryValue.IsValid() || !EntryValue->TryGetObject(EntryObject) || !EntryObject || !EntryObject->IsValid())
        {
            continue;
        }

        FBridgeGeometryInputDescriptor Descriptor;
        EntryObject->Get()->TryGetStringField(TEXT("input_id"), Descriptor.InputId);
        EntryObject->Get()->TryGetStringField(TEXT("label"), Descriptor.Label);
        EntryObject->Get()->TryGetBoolField(TEXT("uses_owner_geometry"), Descriptor.bUsesOwnerGeometry);

        const TArray<TSharedPtr<FJsonValue>>* AllowedKindsArray = nullptr;
        if (EntryObject->Get()->TryGetArrayField(TEXT("allowed_source_kinds"), AllowedKindsArray) && AllowedKindsArray != nullptr)
        {
            for (const TSharedPtr<FJsonValue>& AllowedKindValue : *AllowedKindsArray)
            {
                FString AllowedKindText;
                if (!AllowedKindValue.IsValid() || !AllowedKindValue->TryGetString(AllowedKindText))
                {
                    continue;
                }

                Descriptor.AllowedSourceKinds.Add(ParseGeometrySourceKind(AllowedKindText));
            }
        }

        if (Descriptor.Label.IsEmpty())
        {
            Descriptor.Label = Descriptor.InputId;
        }

        if (!Descriptor.InputId.IsEmpty())
        {
            OutDescriptors.Add(MoveTemp(Descriptor));
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

void BlenderBridge::AddCurveOwnerDataToPayload(const FBridgeCurveOwnerData& CurveOwnerData, const TSharedRef<FJsonObject>& Payload)
{
    TSharedRef<FJsonObject> CurveOwnerObject = MakeShared<FJsonObject>();
    CurveOwnerObject->SetStringField(TEXT("curve_type"), CurveTypeToString(CurveOwnerData.CurveType));
    CurveOwnerObject->SetBoolField(TEXT("closed"), CurveOwnerData.bClosedLoop);

    TArray<TSharedPtr<FJsonValue>> PointArray;
    PointArray.Reserve(CurveOwnerData.Points.Num());
    for (const FBridgeCurveOwnerPoint& Point : CurveOwnerData.Points)
    {
        TSharedRef<FJsonObject> PointObject = MakeShared<FJsonObject>();
        SetVectorField(PointObject, TEXT("position"), Point.Position);
        SetVectorField(PointObject, TEXT("arrive_tangent"), Point.ArriveTangent);
        SetVectorField(PointObject, TEXT("leave_tangent"), Point.LeaveTangent);
        PointObject->SetNumberField(TEXT("roll_degrees"), Point.RollDegrees);
        SetScaleField(PointObject, TEXT("scale"), Point.Scale);
        PointObject->SetStringField(TEXT("point_type"), SplinePointTypeToString(Point.Type));
        PointArray.Add(MakeShared<FJsonValueObject>(PointObject));
    }

    CurveOwnerObject->SetArrayField(TEXT("points"), PointArray);
    Payload->SetObjectField(CurveOwnerFieldName, CurveOwnerObject);
}

void BlenderBridge::AddGeometryBindingPayloadsToPayload(
    const TArray<FBridgeGeometryBindingPayload>& GeometryBindingPayloads,
    const TSharedRef<FJsonObject>& Payload)
{
    TArray<TSharedPtr<FJsonValue>> BindingArray;
    BindingArray.Reserve(GeometryBindingPayloads.Num());

    for (const FBridgeGeometryBindingPayload& BindingPayload : GeometryBindingPayloads)
    {
        TSharedRef<FJsonObject> BindingObject = MakeShared<FJsonObject>();
        BindingObject->SetStringField(TEXT("input_id"), BindingPayload.InputId);
        BindingObject->SetStringField(TEXT("source_kind"), GeometrySourceKindToString(BindingPayload.SourceKind));

        if (BindingPayload.SourceKind == EBlenderBridgeGeometrySourceKind::StaticMeshComponent)
        {
            TSharedRef<FJsonObject> MeshObject = MakeShared<FJsonObject>();

            TArray<TSharedPtr<FJsonValue>> PositionArray;
            PositionArray.Reserve(BindingPayload.MeshData.Positions.Num() * 3);
            for (const FVector& Position : BindingPayload.MeshData.Positions)
            {
                PositionArray.Add(MakeShared<FJsonValueNumber>(Position.X));
                PositionArray.Add(MakeShared<FJsonValueNumber>(Position.Y));
                PositionArray.Add(MakeShared<FJsonValueNumber>(Position.Z));
            }
            MeshObject->SetArrayField(TEXT("positions"), PositionArray);

            TArray<TSharedPtr<FJsonValue>> TriangleArray;
            TriangleArray.Reserve(BindingPayload.MeshData.Triangles.Num());
            for (const int32 TriangleIndex : BindingPayload.MeshData.Triangles)
            {
                TriangleArray.Add(MakeShared<FJsonValueNumber>(TriangleIndex));
            }
            MeshObject->SetArrayField(TEXT("triangles"), TriangleArray);
            SetTransformField(MeshObject, TEXT("transform"), BindingPayload.MeshData.RelativeTransform);
            BindingObject->SetObjectField(TEXT("mesh"), MeshObject);
        }
        else if (
            BindingPayload.SourceKind == EBlenderBridgeGeometrySourceKind::SplineComponent
            || BindingPayload.SourceKind == EBlenderBridgeGeometrySourceKind::EmbeddedSpline)
        {
            TSharedRef<FJsonObject> CurveObject = MakeShared<FJsonObject>();
            AddCurveOwnerDataToPayload(BindingPayload.CurveData, CurveObject);

            const TSharedPtr<FJsonObject>* NestedCurveObject = nullptr;
            if (CurveObject->TryGetObjectField(CurveOwnerFieldName, NestedCurveObject) && NestedCurveObject != nullptr && NestedCurveObject->IsValid())
            {
                BindingObject->SetObjectField(TEXT("curve"), *NestedCurveObject);
            }
        }

        BindingArray.Add(MakeShared<FJsonValueObject>(BindingObject));
    }

    Payload->SetArrayField(TEXT("geometry_bindings"), BindingArray);
}

bool BlenderBridge::SerializeCurveOwnerData(const FBridgeCurveOwnerData& CurveOwnerData, FString& OutSerializedJson)
{
    TSharedRef<FJsonObject> CurveOwnerObject = MakeShared<FJsonObject>();
    AddCurveOwnerDataToPayload(CurveOwnerData, CurveOwnerObject);

    const TSharedPtr<FJsonObject>* NestedCurveOwnerObject = nullptr;
    if (!CurveOwnerObject->TryGetObjectField(CurveOwnerFieldName, NestedCurveOwnerObject) || NestedCurveOwnerObject == nullptr || !NestedCurveOwnerObject->IsValid())
    {
        OutSerializedJson.Reset();
        return false;
    }

    OutSerializedJson.Reset();
    using FCurveOwnerJsonWriter = TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>;
    TSharedRef<FCurveOwnerJsonWriter> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutSerializedJson);
    return FJsonSerializer::Serialize(*NestedCurveOwnerObject, Writer);
}

bool BlenderBridge::SerializeGeometryBindingPayloads(
    const TArray<FBridgeGeometryBindingPayload>& GeometryBindingPayloads,
    FString& OutSerializedJson)
{
    TSharedRef<FJsonObject> GeometryBindingsObject = MakeShared<FJsonObject>();
    AddGeometryBindingPayloadsToPayload(GeometryBindingPayloads, GeometryBindingsObject);

    OutSerializedJson.Reset();
    using FGeometryBindingsJsonWriter = TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>;
    TSharedRef<FGeometryBindingsJsonWriter> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutSerializedJson);
    return FJsonSerializer::Serialize(GeometryBindingsObject, Writer);
}
