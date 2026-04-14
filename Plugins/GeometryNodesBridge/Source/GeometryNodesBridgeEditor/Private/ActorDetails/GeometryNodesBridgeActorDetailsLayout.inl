void FGeometryNodesBridgeActorDetails::BuildOverviewCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& OverviewCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Overview"),
        FText::FromString(TEXT("Overview")),
        ECategoryPriority::Important);
    OverviewCategory.SetSortOrder(OverviewSortOrder);

    OverviewCategory.AddCustomRow(FText::FromString(TEXT("Bridge Status")))
    .WholeRowContent()
    [
        SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush")))
        .BorderBackgroundColor(FLinearColor(0.07f, 0.07f, 0.07f, 0.90f))
        .Padding(10.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(0.f, 0.f, 10.f, 0.f)
            [
                SNew(SBox)
                .WidthOverride(12.f)
                .HeightOverride(12.f)
                [
                    SNew(SBorder)
                    .BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush")))
                    .BorderBackgroundColor(this, &FGeometryNodesBridgeActorDetails::GetStatusStripColor)
                    .Padding(0.f)
                ]
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Font(IDetailLayoutBuilder::GetDetailFontBold())
                    .Text(this, &FGeometryNodesBridgeActorDetails::GetStatusStripText)
                    .ToolTipText(this, &FGeometryNodesBridgeActorDetails::GetStatusStripToolTipText)
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.f, 4.f, 0.f, 0.f)
                [
                    SNew(STextBlock)
                    .Font(IDetailLayoutBuilder::GetDetailFont())
                    .ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.80f, 0.80f)))
                    .Text(this, &FGeometryNodesBridgeActorDetails::GetOverviewSelectionSummaryText)
                    .AutoWrapText(true)
                ]
            ]
        ]
    ];

    AddInfoMessageRow(
        OverviewCategory,
        FText::FromString(TEXT("Multi Selection Summary")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetMultiSelectionSummaryText),
        InfoBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetMultiSelectionVisibility));

    AddReadOnlyTextRow(
        OverviewCategory,
        FText::FromString(TEXT("Source Asset")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetSourceSummaryText),
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetSingleSelectionVisibility));
    AddReadOnlyTextRow(
        OverviewCategory,
        FText::FromString(TEXT("Target")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetTargetSummaryText),
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetSingleSelectionVisibility));
    AddReadOnlyTextRow(
        OverviewCategory,
        FText::FromString(TEXT("Bridge Build")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetBridgeBuildSummaryText),
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetSingleSelectionVisibility));
    AddReadOnlyTextRow(
        OverviewCategory,
        FText::FromString(TEXT("Last Message")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetLastStatusSummaryText),
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetSingleSelectionVisibility));
}

void FGeometryNodesBridgeActorDetails::BuildGenerateCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& GenerateCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Generate"),
        FText::FromString(TEXT("Generate")),
        ECategoryPriority::Important);
    GenerateCategory.SetSortOrder(GenerateSortOrder);

    GenerateCategory.AddCustomRow(FText::FromString(TEXT("Bridge Actions")))
    .WholeRowContent()
    [
        SNew(SWrapBox)
        .UseAllottedSize(true)
        + SWrapBox::Slot()
        .Padding(0.f, 0.f, 8.f, 8.f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Refresh Input Schema")))
            .OnClicked(this, &FGeometryNodesBridgeActorDetails::HandleRefreshInputSchemaClicked)
        ]
        + SWrapBox::Slot()
        .Padding(0.f, 0.f, 8.f, 8.f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Request Cook")))
            .OnClicked(this, &FGeometryNodesBridgeActorDetails::HandleRequestCookClicked)
        ]
        + SWrapBox::Slot()
        .Padding(0.f, 0.f, 8.f, 8.f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Bake Output")))
            .OnClicked(this, &FGeometryNodesBridgeActorDetails::HandleBakeToStaticMeshClicked)
        ]
        + SWrapBox::Slot()
        .Padding(0.f, 0.f, 8.f, 8.f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Select Baked Output")))
            .IsEnabled(this, &FGeometryNodesBridgeActorDetails::CanSelectOrDeleteBakedOutput)
            .OnClicked(this, &FGeometryNodesBridgeActorDetails::HandleSelectBakedOutputClicked)
        ]
        + SWrapBox::Slot()
        .Padding(0.f, 0.f, 0.f, 8.f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Delete Baked Output")))
            .IsEnabled(this, &FGeometryNodesBridgeActorDetails::CanSelectOrDeleteBakedOutput)
            .OnClicked(this, &FGeometryNodesBridgeActorDetails::HandleDeleteBakedOutputClicked)
        ]
    ];
}

void FGeometryNodesBridgeActorDetails::BuildSetupCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& SetupCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Setup"),
        FText::FromString(TEXT("Setup")),
        ECategoryPriority::TypeSpecific);
    SetupCategory.SetSortOrder(SetupSortOrder);

    AddComponentProperty(SetupCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, Source));
    AddComponentProperty(SetupCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, TargetId));

    const TArray<UObject*> BridgeComponents = GetSelectedBridgeComponentObjects();
    if (!BridgeComponents.IsEmpty())
    {
        const TSharedPtr<IPropertyHandle> GeometryBindingsHandle = DetailBuilder.AddObjectPropertyData(
            BridgeComponents,
            GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, GeometryBindings));
        const TSharedPtr<IPropertyHandleArray> GeometryBindingsArray = GeometryBindingsHandle.IsValid()
            ? GeometryBindingsHandle->AsArray()
            : nullptr;

        uint32 BindingCount = 0;
        if (GeometryBindingsArray.IsValid()
            && GeometryBindingsArray->GetNumElements(BindingCount) == FPropertyAccess::Success)
        {
            const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
            for (uint32 BindingIndex = 0; BindingIndex < BindingCount; ++BindingIndex)
            {
                if (BridgeComponent == nullptr || !BridgeComponent->GeometryInputDescriptors.IsValidIndex(static_cast<int32>(BindingIndex)))
                {
                    continue;
                }

                const FBridgeGeometryInputDescriptor& Descriptor = BridgeComponent->GeometryInputDescriptors[static_cast<int32>(BindingIndex)];
                if (!DescriptorSupportsActorOverride(Descriptor))
                {
                    continue;
                }

                const TSharedRef<IPropertyHandle> BindingHandle = GeometryBindingsArray->GetElement(BindingIndex);
                const TSharedPtr<IPropertyHandle> ComponentReferenceHandle = BindingHandle->GetChildHandle(
                    GET_MEMBER_NAME_CHECKED(FGeometryNodesBridgeGeometryBinding, ComponentReference));
                const TSharedPtr<IPropertyHandle> ReferencedActorHandle = ComponentReferenceHandle.IsValid()
                    ? ComponentReferenceHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FComponentReference, OtherActor))
                    : nullptr;
                if (!ReferencedActorHandle.IsValid())
                {
                    continue;
                }

                IDetailPropertyRow& BindingRow = SetupCategory.AddProperty(ReferencedActorHandle.ToSharedRef());
                BindingRow.DisplayName(GetGeometryBindingDisplayName(BridgeComponent, Descriptor));
            }
        }
    }

    AddInfoMessageRow(
        SetupCategory,
        FText::FromString(TEXT("Setup Hint")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetSetupHintText),
        WarningBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetSetupHintVisibility));
}

void FGeometryNodesBridgeActorDetails::BuildParametersCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& ParametersCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Parameters"),
        FText::FromString(TEXT("Parameters")),
        ECategoryPriority::TypeSpecific);
    ParametersCategory.SetSortOrder(ParametersSortOrder);

    AddInfoMessageRow(
        ParametersCategory,
        FText::FromString(TEXT("Parameters Hint")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetParametersHintText),
        WarningBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetParametersHintVisibility));

    if (!ShouldShowParameterProperties())
    {
        return;
    }

    const TArray<UObject*> BridgeComponents = GetSelectedBridgeComponentObjects();
    if (BridgeComponents.IsEmpty())
    {
        return;
    }

    const TSharedPtr<IPropertyHandle> InputValuesHandle = DetailBuilder.AddObjectPropertyData(
        BridgeComponents,
        GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, InputValues));
    if (!InputValuesHandle.IsValid() || !InputValuesHandle->IsValidHandle())
    {
        return;
    }

    TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
    DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

    GeometryNodesBridge::PropertyBagBuilder::AddGroupedPropertyBagToCategory(
        InputValuesHandle.ToSharedRef(),
        ParametersCategory,
        CustomizedObjects,
        DetailBuilder.GetPropertyUtilities());
}

void FGeometryNodesBridgeActorDetails::BuildCurveCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& CurveCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Curve"),
        FText::FromString(TEXT("Curve")),
        ECategoryPriority::TypeSpecific);
    CurveCategory.SetSortOrder(CurveSortOrder);

    AddInfoMessageRow(
        CurveCategory,
        FText::FromString(TEXT("Curve Hint")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetCurveHintText),
        InfoBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetCurveHintVisibility));

    if (IDetailPropertyRow* SplineRow = AddActorProperty(
        DetailBuilder,
        CurveCategory,
        BridgeSplineComponentPropertyName))
    {
        SplineRow->DisplayName(FText::FromString(TEXT("Embedded Spline")));
        SplineRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetCurveSplinePropertyVisibility));
    }
}

void FGeometryNodesBridgeActorDetails::BuildGeometryCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& GeometryCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Geometry"),
        FText::FromString(TEXT("Geometry")),
        ECategoryPriority::TypeSpecific);
    GeometryCategory.SetSortOrder(GeometrySortOrder);

    AddInfoMessageRow(
        GeometryCategory,
        FText::FromString(TEXT("Geometry Hint")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetGeometryHintText),
        InfoBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetGeometryHintVisibility));

    const TArray<UObject*> BridgeComponents = GetSelectedBridgeComponentObjects();
    if (BridgeComponents.IsEmpty())
    {
        return;
    }

    const TSharedPtr<IPropertyHandle> GeometryBindingsHandle = DetailBuilder.AddObjectPropertyData(
        BridgeComponents,
        GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, GeometryBindings));
    if (!GeometryBindingsHandle.IsValid())
    {
        return;
    }

    const TSharedPtr<IPropertyHandleArray> GeometryBindingsArray = GeometryBindingsHandle->AsArray();
    if (!GeometryBindingsArray.IsValid())
    {
        return;
    }

    uint32 BindingCount = 0;
    if (GeometryBindingsArray->GetNumElements(BindingCount) != FPropertyAccess::Success)
    {
        return;
    }

    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    for (uint32 BindingIndex = 0; BindingIndex < BindingCount; ++BindingIndex)
    {
        const TSharedRef<IPropertyHandle> BindingHandle = GeometryBindingsArray->GetElement(BindingIndex);
        TSharedPtr<IPropertyHandle> VisibleHandle = BindingHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGeometryNodesBridgeGeometryBinding, ComponentReference));
        if (!VisibleHandle.IsValid())
        {
            VisibleHandle = BindingHandle;
        }

        IDetailPropertyRow& BindingRow = GeometryCategory.AddProperty(VisibleHandle.ToSharedRef());
        BindingRow.ShouldAutoExpand(true);
        BindingRow.Visibility(TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetGeometryPropertiesVisibility));

        FString DisplayLabel = FString::Printf(TEXT("Geometry Binding %u"), BindingIndex + 1);
        if (BridgeComponent != nullptr && BridgeComponent->GeometryInputDescriptors.IsValidIndex(static_cast<int32>(BindingIndex)))
        {
            DisplayLabel = BridgeComponent->GeometryInputDescriptors[static_cast<int32>(BindingIndex)].Label;
        }

        BindingRow.DisplayName(FText::FromString(DisplayLabel));
    }
}

void FGeometryNodesBridgeActorDetails::BuildMaterialsCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& MaterialsCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Materials"),
        FText::FromString(TEXT("Materials")),
        ECategoryPriority::TypeSpecific);
    MaterialsCategory.SetSortOrder(MaterialsSortOrder);

    AddInfoMessageRow(
        MaterialsCategory,
        FText::FromString(TEXT("Materials Hint")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetMaterialsHintText),
        InfoBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetMaterialsHintVisibility));

    if (IDetailPropertyRow* SlotsRow = AddComponentProperty(
        MaterialsCategory,
        GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, LastCookedMaterialSlots),
        true,
        true))
    {
        SlotsRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetMaterialPropertiesVisibility));
    }

    if (IDetailPropertyRow* OverridesRow = AddComponentProperty(
        MaterialsCategory,
        GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, MaterialOverrides),
        false,
        true))
    {
        OverridesRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetMaterialPropertiesVisibility));
    }
}

void FGeometryNodesBridgeActorDetails::BuildBakeCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& BakeCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Bake"),
        FText::FromString(TEXT("Bake")),
        ECategoryPriority::TypeSpecific);
    BakeCategory.SetSortOrder(BakeSortOrder);

    AddComponentProperty(BakeCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, BakeOutputMode));
    AddComponentProperty(BakeCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, bShareGeneratedBakeMaterials));
    AddComponentProperty(BakeCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, bHidePreviewAfterBake));

    AddInfoMessageRow(
        BakeCategory,
        FText::FromString(TEXT("Bake Mode Description")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetBakeModeDescriptionText),
        InfoBackgroundColor);
    AddInfoMessageRow(
        BakeCategory,
        FText::FromString(TEXT("Instanced Bake Note")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetInstancedBakeNoteText),
        WarningBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetInstancedBakeNoteVisibility));
}

void FGeometryNodesBridgeActorDetails::BuildOutputsCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& OutputsCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Outputs"),
        FText::FromString(TEXT("Outputs")),
        ECategoryPriority::TypeSpecific);
    OutputsCategory.SetSortOrder(OutputsSortOrder);

    AddInfoMessageRow(
        OutputsCategory,
        FText::FromString(TEXT("Outputs Summary")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetOutputsSummaryText),
        InfoBackgroundColor);

    if (IDetailPropertyRow* BakedActorRow = AddComponentProperty(
        OutputsCategory,
        GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, BakedActor),
        true))
    {
        BakedActorRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetBakedActorPropertyVisibility));
    }

    if (IDetailPropertyRow* BakedStaticMeshRow = AddComponentProperty(
        OutputsCategory,
        GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, BakedStaticMesh),
        true))
    {
        BakedStaticMeshRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetBakedStaticMeshPropertyVisibility));
    }

    if (IDetailPropertyRow* PrototypeMeshesRow = AddComponentProperty(
        OutputsCategory,
        GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, BakedPrototypeMeshes),
        true,
        true))
    {
        PrototypeMeshesRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetBakedPrototypePropertiesVisibility));
    }

    if (IDetailPropertyRow* GeneratedMaterialsRow = AddComponentProperty(
        OutputsCategory,
        GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, GeneratedBakeMaterials),
        true,
        true))
    {
        GeneratedMaterialsRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetGeneratedBakeMaterialsPropertyVisibility));
    }
}

void FGeometryNodesBridgeActorDetails::BuildAdvancedCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& AdvancedCategory = DetailBuilder.EditCategory(
        TEXT("Geometry Nodes Bridge Advanced"),
        FText::FromString(TEXT("Advanced")),
        ECategoryPriority::Uncommon);
    AdvancedCategory.SetSortOrder(AdvancedSortOrder);
    AdvancedCategory.InitiallyCollapsed(true);

    AddInfoMessageRow(
        AdvancedCategory,
        FText::FromString(TEXT("Advanced Hint")),
        TAttribute<FText>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetAdvancedHintText),
        AdvancedBackgroundColor);

    AddActorProperty(DetailBuilder, AdvancedCategory, BridgeComponentPropertyName);
    AddActorProperty(DetailBuilder, AdvancedCategory, BridgeSplineComponentPropertyName);

    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, ConnectionState), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, ComponentStatus), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, ComponentHandle), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, SchemaTargetId), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, ObservedGeometryDebugSummary), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, LastObservedGeometryEvent), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, AvailableTargets), true, true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, InputSchema), true, true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, TargetCapabilities), true, true);
}

