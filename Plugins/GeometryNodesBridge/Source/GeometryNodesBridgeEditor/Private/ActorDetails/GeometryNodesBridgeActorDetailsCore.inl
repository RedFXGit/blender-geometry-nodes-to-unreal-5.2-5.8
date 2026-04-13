TSharedRef<IDetailCustomization> FGeometryNodesBridgeActorDetails::MakeInstance()
{
    return MakeShared<FGeometryNodesBridgeActorDetails>();
}

void FGeometryNodesBridgeActorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
    DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

    SelectedActors.Reset();
    for (const TWeakObjectPtr<UObject>& CustomizedObject : CustomizedObjects)
    {
        if (AGeometryNodesBridgeActor* Actor = Cast<AGeometryNodesBridgeActor>(CustomizedObject.Get()))
        {
            SelectedActors.Add(Actor);
        }
    }

    PropertyUtilities = DetailBuilder.GetPropertyUtilities();
    HideDefaultBridgeCategories(DetailBuilder);

    if (UGeometryNodesBridgeComponent* SingleBridgeComponent = GetSingleSelectedBridgeComponent())
    {
        RegisterRefreshOnPropertyChange(DetailBuilder, SingleBridgeComponent, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, Source));
        RegisterRefreshOnPropertyChange(DetailBuilder, SingleBridgeComponent, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, TargetId));
        RegisterRefreshOnPropertyChange(DetailBuilder, SingleBridgeComponent, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, GeometryBindings));
        RegisterRefreshOnPropertyChange(DetailBuilder, SingleBridgeComponent, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, BakeOutputMode));
    }

    BuildOverviewCategory(DetailBuilder);
    BuildGenerateCategory(DetailBuilder);

    if (IsSingleSelection())
    {
        BuildSetupCategory(DetailBuilder);
        BuildParametersCategory(DetailBuilder);

        if (ShouldShowCurveCategory())
        {
            BuildCurveCategory(DetailBuilder);
        }

        BuildMaterialsCategory(DetailBuilder);
        BuildBakeCategory(DetailBuilder);
        BuildOutputsCategory(DetailBuilder);
        BuildAdvancedCategory(DetailBuilder);
    }
}

void FGeometryNodesBridgeActorDetails::HideDefaultBridgeCategories(IDetailLayoutBuilder& DetailBuilder) const
{
    DetailBuilder.HideCategory(TEXT("Geometry Nodes Bridge"));
    DetailBuilder.HideCategory(TEXT("Geometry Nodes Bridge|Bake"));
    DetailBuilder.HideCategory(TEXT("Geometry Nodes Bridge|Spline"));
}

void FGeometryNodesBridgeActorDetails::RegisterRefreshOnPropertyChange(
    IDetailLayoutBuilder& DetailBuilder,
    UGeometryNodesBridgeComponent* BridgeComponent,
    const FName PropertyName)
{
    if (BridgeComponent == nullptr)
    {
        return;
    }

    TArray<UObject*> ExternalObjects;
    ExternalObjects.Add(BridgeComponent);

    const TSharedPtr<IPropertyHandle> PropertyHandle = DetailBuilder.AddObjectPropertyData(ExternalObjects, PropertyName);
    if (PropertyHandle.IsValid())
    {
        PropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FGeometryNodesBridgeActorDetails::HandleRefreshDependentPropertyChanged));
    }
}

