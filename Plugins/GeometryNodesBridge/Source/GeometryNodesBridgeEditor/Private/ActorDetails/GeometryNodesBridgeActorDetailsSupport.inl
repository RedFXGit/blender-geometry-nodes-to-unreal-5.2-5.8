void FGeometryNodesBridgeActorDetails::AddInfoMessageRow(
    IDetailCategoryBuilder& CategoryBuilder,
    const FText& FilterText,
    TAttribute<FText> MessageText,
    const FLinearColor& BackgroundColor,
    TAttribute<EVisibility> Visibility) const
{
    CategoryBuilder.AddCustomRow(FilterText)
    .WholeRowContent()
    [
        SNew(SBorder)
        .Visibility(Visibility)
        .BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush")))
        .BorderBackgroundColor(BackgroundColor)
        .Padding(8.f)
        [
            SNew(STextBlock)
            .Font(IDetailLayoutBuilder::GetDetailFont())
            .Text(MessageText)
            .AutoWrapText(true)
        ]
    ];
}

void FGeometryNodesBridgeActorDetails::AddReadOnlyTextRow(
    IDetailCategoryBuilder& CategoryBuilder,
    const FText& Label,
    TAttribute<FText> ValueText,
    TAttribute<EVisibility> Visibility) const
{
    CategoryBuilder.AddCustomRow(Label)
    .Visibility(Visibility)
    .NameContent()
    [
        SNew(STextBlock)
        .Font(IDetailLayoutBuilder::GetDetailFont())
        .Text(Label)
    ]
    .ValueContent()
    .MinDesiredWidth(260.f)
    .MaxDesiredWidth(600.f)
    [
        SNew(STextBlock)
        .Font(IDetailLayoutBuilder::GetDetailFont())
        .Text(ValueText)
        .AutoWrapText(true)
    ];
}

IDetailPropertyRow* FGeometryNodesBridgeActorDetails::AddComponentProperty(
    IDetailCategoryBuilder& CategoryBuilder,
    const FName PropertyName,
    const bool bForceShow,
    const bool bAutoExpand) const
{
    TArray<UObject*> ExternalObjects = GetSelectedBridgeComponentObjects();
    if (ExternalObjects.IsEmpty())
    {
        return nullptr;
    }

    FAddPropertyParams Params;
    if (bForceShow)
    {
        Params.ForceShowProperty();
    }

    IDetailPropertyRow* PropertyRow = CategoryBuilder.AddExternalObjectProperty(ExternalObjects, PropertyName, EPropertyLocation::Default, Params);
    if (PropertyRow != nullptr && bAutoExpand)
    {
        PropertyRow->ShouldAutoExpand(true);
    }

    return PropertyRow;
}

IDetailPropertyRow* FGeometryNodesBridgeActorDetails::AddActorProperty(
    IDetailLayoutBuilder& DetailBuilder,
    IDetailCategoryBuilder& CategoryBuilder,
    const FName PropertyName,
    const bool bAutoExpand) const
{
    const TSharedRef<IPropertyHandle> PropertyHandle = DetailBuilder.GetProperty(PropertyName, AGeometryNodesBridgeActor::StaticClass());
    if (!PropertyHandle->IsValidHandle())
    {
        return nullptr;
    }

    IDetailPropertyRow& PropertyRow = CategoryBuilder.AddProperty(PropertyHandle);
    if (bAutoExpand)
    {
        PropertyRow.ShouldAutoExpand(true);
    }

    return &PropertyRow;
}

void FGeometryNodesBridgeActorDetails::ForceRefreshDetails() const
{
    if (const TSharedPtr<IPropertyUtilities> PinnedUtilities = PropertyUtilities.Pin())
    {
        PinnedUtilities->ForceRefresh();
    }
}

void FGeometryNodesBridgeActorDetails::HandleRefreshDependentPropertyChanged()
{
    ForceRefreshDetails();
}

