#include "BlenderBridgeActorDetails.h"

#include "BlenderBridgeActor.h"
#include "BlenderBridgeComponent.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSessionManager.h"
#include "BlenderBridgeSource.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IPropertyUtilities.h"
#include "IDetailPropertyRow.h"
#include "PropertyHandle.h"
#include "Modules/ModuleManager.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
    const FName BridgeComponentPropertyName(TEXT("BridgeComponent"));
    const FName BridgeSplineComponentPropertyName(TEXT("BridgeSplineComponent"));

    constexpr int32 OverviewSortOrder = -200;
    constexpr int32 GenerateSortOrder = -190;
    constexpr int32 SetupSortOrder = -180;
    constexpr int32 ParametersSortOrder = -170;
    constexpr int32 CurveSortOrder = -160;
    constexpr int32 MaterialsSortOrder = -150;
    constexpr int32 BakeSortOrder = -140;
    constexpr int32 OutputsSortOrder = -130;
    constexpr int32 AdvancedSortOrder = 500;

    const FLinearColor InfoBackgroundColor(0.08f, 0.12f, 0.18f, 0.75f);
    const FLinearColor WarningBackgroundColor(0.21f, 0.15f, 0.05f, 0.80f);
    const FLinearColor AdvancedBackgroundColor(0.10f, 0.10f, 0.10f, 0.65f);

    template <typename EnumType>
    FText GetEnumDisplayText(const EnumType Value)
    {
        if (const UEnum* Enum = StaticEnum<EnumType>())
        {
            return Enum->GetDisplayNameTextByValue(static_cast<int64>(Value));
        }

        return FText::FromString(TEXT("Unknown"));
    }

    bool HasBakedOutput(const UBlenderBridgeComponent* BridgeComponent)
    {
        return BridgeComponent != nullptr
            && (BridgeComponent->BakedActor.IsValid()
                || !BridgeComponent->BakedStaticMesh.IsNull()
                || !BridgeComponent->BakedPrototypeMeshes.IsEmpty());
    }

    FLinearColor GetConnectionStateColor(const EBlenderBridgeConnectionState ConnectionState)
    {
        switch (ConnectionState)
        {
        case EBlenderBridgeConnectionState::Connected:
            return FLinearColor(0.18f, 0.72f, 0.32f);
        case EBlenderBridgeConnectionState::Launching:
            return FLinearColor(0.92f, 0.73f, 0.18f);
        case EBlenderBridgeConnectionState::Disconnected:
            return FLinearColor(0.85f, 0.22f, 0.22f);
        case EBlenderBridgeConnectionState::Error:
            return FLinearColor(0.78f, 0.12f, 0.12f);
        default:
            return FLinearColor(0.55f, 0.55f, 0.55f);
        }
    }

    FLinearColor GetComponentStatusColor(const EBlenderBridgeComponentStatus ComponentStatus)
    {
        switch (ComponentStatus)
        {
        case EBlenderBridgeComponentStatus::Ready:
        case EBlenderBridgeComponentStatus::Cooked:
            return FLinearColor(0.18f, 0.72f, 0.32f);
        case EBlenderBridgeComponentStatus::CookQueued:
        case EBlenderBridgeComponentStatus::Cooking:
        case EBlenderBridgeComponentStatus::SchemaPending:
            return FLinearColor(0.92f, 0.73f, 0.18f);
        case EBlenderBridgeComponentStatus::Error:
            return FLinearColor(0.85f, 0.22f, 0.22f);
        default:
            return FLinearColor(0.60f, 0.60f, 0.60f);
        }
    }
}

TSharedRef<IDetailCustomization> FBlenderBridgeActorDetails::MakeInstance()
{
    return MakeShared<FBlenderBridgeActorDetails>();
}

void FBlenderBridgeActorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
    DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

    SelectedActors.Reset();
    for (const TWeakObjectPtr<UObject>& CustomizedObject : CustomizedObjects)
    {
        if (ABlenderBridgeActor* Actor = Cast<ABlenderBridgeActor>(CustomizedObject.Get()))
        {
            SelectedActors.Add(Actor);
        }
    }

    PropertyUtilities = DetailBuilder.GetPropertyUtilities();
    HideDefaultBridgeCategories(DetailBuilder);

    if (UBlenderBridgeComponent* SingleBridgeComponent = GetSingleSelectedBridgeComponent())
    {
        RegisterRefreshOnPropertyChange(DetailBuilder, SingleBridgeComponent, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, Source));
        RegisterRefreshOnPropertyChange(DetailBuilder, SingleBridgeComponent, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, TargetId));
        RegisterRefreshOnPropertyChange(DetailBuilder, SingleBridgeComponent, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, BakeOutputMode));
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

void FBlenderBridgeActorDetails::HideDefaultBridgeCategories(IDetailLayoutBuilder& DetailBuilder) const
{
    DetailBuilder.HideCategory(TEXT("Blender Bridge"));
    DetailBuilder.HideCategory(TEXT("Blender Bridge|Bake"));
    DetailBuilder.HideCategory(TEXT("Blender Bridge|Spline"));
}

void FBlenderBridgeActorDetails::RegisterRefreshOnPropertyChange(
    IDetailLayoutBuilder& DetailBuilder,
    UBlenderBridgeComponent* BridgeComponent,
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
        PropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FBlenderBridgeActorDetails::HandleRefreshDependentPropertyChanged));
    }
}

void FBlenderBridgeActorDetails::BuildOverviewCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& OverviewCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Overview"),
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
                    .BorderBackgroundColor(this, &FBlenderBridgeActorDetails::GetStatusStripColor)
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
                    .Text(this, &FBlenderBridgeActorDetails::GetStatusStripText)
                    .ToolTipText(this, &FBlenderBridgeActorDetails::GetStatusStripToolTipText)
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.f, 4.f, 0.f, 0.f)
                [
                    SNew(STextBlock)
                    .Font(IDetailLayoutBuilder::GetDetailFont())
                    .ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.80f, 0.80f)))
                    .Text(this, &FBlenderBridgeActorDetails::GetOverviewSelectionSummaryText)
                    .AutoWrapText(true)
                ]
            ]
        ]
    ];

    AddInfoMessageRow(
        OverviewCategory,
        FText::FromString(TEXT("Multi Selection Summary")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetMultiSelectionSummaryText),
        InfoBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetMultiSelectionVisibility));

    AddReadOnlyTextRow(
        OverviewCategory,
        FText::FromString(TEXT("Source Asset")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetSourceSummaryText),
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetSingleSelectionVisibility));
    AddReadOnlyTextRow(
        OverviewCategory,
        FText::FromString(TEXT("Target")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetTargetSummaryText),
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetSingleSelectionVisibility));
    AddReadOnlyTextRow(
        OverviewCategory,
        FText::FromString(TEXT("Bridge Build")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetBridgeBuildSummaryText),
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetSingleSelectionVisibility));
    AddReadOnlyTextRow(
        OverviewCategory,
        FText::FromString(TEXT("Last Message")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetLastStatusSummaryText),
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetSingleSelectionVisibility));
}

void FBlenderBridgeActorDetails::BuildGenerateCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& GenerateCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Generate"),
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
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleRefreshInputSchemaClicked)
        ]
        + SWrapBox::Slot()
        .Padding(0.f, 0.f, 8.f, 8.f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Request Cook")))
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleRequestCookClicked)
        ]
        + SWrapBox::Slot()
        .Padding(0.f, 0.f, 8.f, 8.f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Bake Output")))
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleBakeToStaticMeshClicked)
        ]
        + SWrapBox::Slot()
        .Padding(0.f, 0.f, 8.f, 8.f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Select Baked Output")))
            .IsEnabled(this, &FBlenderBridgeActorDetails::CanSelectOrDeleteBakedOutput)
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleSelectBakedOutputClicked)
        ]
        + SWrapBox::Slot()
        .Padding(0.f, 0.f, 0.f, 8.f)
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Delete Baked Output")))
            .IsEnabled(this, &FBlenderBridgeActorDetails::CanSelectOrDeleteBakedOutput)
            .OnClicked(this, &FBlenderBridgeActorDetails::HandleDeleteBakedOutputClicked)
        ]
    ];
}

void FBlenderBridgeActorDetails::BuildSetupCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& SetupCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Setup"),
        FText::FromString(TEXT("Setup")),
        ECategoryPriority::TypeSpecific);
    SetupCategory.SetSortOrder(SetupSortOrder);

    AddComponentProperty(SetupCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, Source));
    AddComponentProperty(SetupCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, TargetId));

    AddInfoMessageRow(
        SetupCategory,
        FText::FromString(TEXT("Setup Hint")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetSetupHintText),
        WarningBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetSetupHintVisibility));
}

void FBlenderBridgeActorDetails::BuildParametersCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& ParametersCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Parameters"),
        FText::FromString(TEXT("Parameters")),
        ECategoryPriority::TypeSpecific);
    ParametersCategory.SetSortOrder(ParametersSortOrder);

    AddInfoMessageRow(
        ParametersCategory,
        FText::FromString(TEXT("Parameters Hint")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetParametersHintText),
        WarningBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetParametersHintVisibility));

    if (IDetailPropertyRow* InputValuesRow = AddComponentProperty(
        ParametersCategory,
        GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, InputValues),
        false,
        true))
    {
        InputValuesRow->DisplayName(FText::FromString(TEXT("Input Values")));
        InputValuesRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetParametersPropertyVisibility));
    }
}

void FBlenderBridgeActorDetails::BuildCurveCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& CurveCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Curve"),
        FText::FromString(TEXT("Curve")),
        ECategoryPriority::TypeSpecific);
    CurveCategory.SetSortOrder(CurveSortOrder);

    AddInfoMessageRow(
        CurveCategory,
        FText::FromString(TEXT("Curve Hint")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetCurveHintText),
        InfoBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetCurveHintVisibility));

    if (IDetailPropertyRow* SplineRow = AddActorProperty(
        DetailBuilder,
        CurveCategory,
        BridgeSplineComponentPropertyName))
    {
        SplineRow->DisplayName(FText::FromString(TEXT("Embedded Spline")));
        SplineRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetCurveSplinePropertyVisibility));
    }
}

void FBlenderBridgeActorDetails::BuildMaterialsCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& MaterialsCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Materials"),
        FText::FromString(TEXT("Materials")),
        ECategoryPriority::TypeSpecific);
    MaterialsCategory.SetSortOrder(MaterialsSortOrder);

    AddInfoMessageRow(
        MaterialsCategory,
        FText::FromString(TEXT("Materials Hint")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetMaterialsHintText),
        InfoBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetMaterialsHintVisibility));

    if (IDetailPropertyRow* SlotsRow = AddComponentProperty(
        MaterialsCategory,
        GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, LastCookedMaterialSlots),
        true,
        true))
    {
        SlotsRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetMaterialPropertiesVisibility));
    }

    if (IDetailPropertyRow* OverridesRow = AddComponentProperty(
        MaterialsCategory,
        GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, MaterialOverrides),
        false,
        true))
    {
        OverridesRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetMaterialPropertiesVisibility));
    }
}

void FBlenderBridgeActorDetails::BuildBakeCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& BakeCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Bake"),
        FText::FromString(TEXT("Bake")),
        ECategoryPriority::TypeSpecific);
    BakeCategory.SetSortOrder(BakeSortOrder);

    AddComponentProperty(BakeCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, BakeOutputMode));
    AddComponentProperty(BakeCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, bHidePreviewAfterBake));

    AddInfoMessageRow(
        BakeCategory,
        FText::FromString(TEXT("Bake Mode Description")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetBakeModeDescriptionText),
        InfoBackgroundColor);
    AddInfoMessageRow(
        BakeCategory,
        FText::FromString(TEXT("Instanced Bake Note")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetInstancedBakeNoteText),
        WarningBackgroundColor,
        TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetInstancedBakeNoteVisibility));
}

void FBlenderBridgeActorDetails::BuildOutputsCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& OutputsCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Outputs"),
        FText::FromString(TEXT("Outputs")),
        ECategoryPriority::TypeSpecific);
    OutputsCategory.SetSortOrder(OutputsSortOrder);

    AddInfoMessageRow(
        OutputsCategory,
        FText::FromString(TEXT("Outputs Summary")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetOutputsSummaryText),
        InfoBackgroundColor);

    if (IDetailPropertyRow* BakedActorRow = AddComponentProperty(
        OutputsCategory,
        GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, BakedActor),
        true))
    {
        BakedActorRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetBakedActorPropertyVisibility));
    }

    if (IDetailPropertyRow* BakedStaticMeshRow = AddComponentProperty(
        OutputsCategory,
        GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, BakedStaticMesh),
        true))
    {
        BakedStaticMeshRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetBakedStaticMeshPropertyVisibility));
    }

    if (IDetailPropertyRow* PrototypeMeshesRow = AddComponentProperty(
        OutputsCategory,
        GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, BakedPrototypeMeshes),
        true,
        true))
    {
        PrototypeMeshesRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetBakedPrototypePropertiesVisibility));
    }

    if (IDetailPropertyRow* GeneratedMaterialsRow = AddComponentProperty(
        OutputsCategory,
        GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, GeneratedBakeMaterials),
        true,
        true))
    {
        GeneratedMaterialsRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FBlenderBridgeActorDetails::GetGeneratedBakeMaterialsPropertyVisibility));
    }
}

void FBlenderBridgeActorDetails::BuildAdvancedCategory(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& AdvancedCategory = DetailBuilder.EditCategory(
        TEXT("Blender Bridge Advanced"),
        FText::FromString(TEXT("Advanced")),
        ECategoryPriority::Uncommon);
    AdvancedCategory.SetSortOrder(AdvancedSortOrder);
    AdvancedCategory.InitiallyCollapsed(true);

    AddInfoMessageRow(
        AdvancedCategory,
        FText::FromString(TEXT("Advanced Hint")),
        TAttribute<FText>::CreateSP(this, &FBlenderBridgeActorDetails::GetAdvancedHintText),
        AdvancedBackgroundColor);

    AddActorProperty(DetailBuilder, AdvancedCategory, BridgeComponentPropertyName);
    AddActorProperty(DetailBuilder, AdvancedCategory, BridgeSplineComponentPropertyName);

    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, ConnectionState), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, ComponentStatus), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, ComponentHandle), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, SchemaTargetId), true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, AvailableTargets), true, true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, InputSchema), true, true);
    AddComponentProperty(AdvancedCategory, GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, TargetCapabilities), true, true);
}

void FBlenderBridgeActorDetails::AddInfoMessageRow(
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

void FBlenderBridgeActorDetails::AddReadOnlyTextRow(
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

IDetailPropertyRow* FBlenderBridgeActorDetails::AddComponentProperty(
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

IDetailPropertyRow* FBlenderBridgeActorDetails::AddActorProperty(
    IDetailLayoutBuilder& DetailBuilder,
    IDetailCategoryBuilder& CategoryBuilder,
    const FName PropertyName,
    const bool bAutoExpand) const
{
    const TSharedRef<IPropertyHandle> PropertyHandle = DetailBuilder.GetProperty(PropertyName, ABlenderBridgeActor::StaticClass());
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

void FBlenderBridgeActorDetails::ForceRefreshDetails() const
{
    if (const TSharedPtr<IPropertyUtilities> PinnedUtilities = PropertyUtilities.Pin())
    {
        PinnedUtilities->ForceRefresh();
    }
}

void FBlenderBridgeActorDetails::HandleRefreshDependentPropertyChanged()
{
    ForceRefreshDetails();
}

TArray<UObject*> FBlenderBridgeActorDetails::GetSelectedBridgeComponentObjects() const
{
    TArray<UObject*> BridgeComponents;
    BridgeComponents.Reserve(SelectedActors.Num());

    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponents.Add(BridgeComponent);
            }
        }
    }

    return BridgeComponents;
}

ABlenderBridgeActor* FBlenderBridgeActorDetails::GetSingleSelectedActor() const
{
    return SelectedActors.Num() == 1 ? SelectedActors[0].Get() : nullptr;
}

UBlenderBridgeComponent* FBlenderBridgeActorDetails::GetSingleSelectedBridgeComponent() const
{
    if (ABlenderBridgeActor* Actor = GetSingleSelectedActor())
    {
        return Actor->GetBridgeComponent();
    }

    return nullptr;
}

bool FBlenderBridgeActorDetails::IsSingleSelection() const
{
    return GetSingleSelectedActor() != nullptr;
}

bool FBlenderBridgeActorDetails::HasSelectedActors() const
{
    return !SelectedActors.IsEmpty();
}

bool FBlenderBridgeActorDetails::AnySelectedActorsHaveBakedOutput() const
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (const ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (HasBakedOutput(Actor->GetBridgeComponent()))
            {
                return true;
            }
        }
    }

    return false;
}

bool FBlenderBridgeActorDetails::ShouldShowCurveCategory() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->TargetCapabilities.TargetKind == EBlenderBridgeTargetKind::CurveOwner
            || (GetSingleSelectedActor() != nullptr && GetSingleSelectedActor()->GetBridgeSplineComponent() != nullptr);
    }

    return false;
}

bool FBlenderBridgeActorDetails::ShouldShowCurveSplineProperty() const
{
    return GetSingleSelectedActor() != nullptr && GetSingleSelectedActor()->GetBridgeSplineComponent() != nullptr;
}

bool FBlenderBridgeActorDetails::ShouldShowMaterialProperties() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->LastCookedMaterialSlots.IsEmpty();
    }

    return false;
}

bool FBlenderBridgeActorDetails::ShouldShowParameterProperties() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->InputSchema.IsEmpty();
    }

    return false;
}

bool FBlenderBridgeActorDetails::ShouldShowBakedActorProperty() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->BakedActor.IsValid();
    }

    return false;
}

bool FBlenderBridgeActorDetails::ShouldShowBakedStaticMeshProperty() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->BakedStaticMesh.IsNull();
    }

    return false;
}

bool FBlenderBridgeActorDetails::ShouldShowBakedPrototypeProperties() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->BakedPrototypeMeshes.IsEmpty();
    }

    return false;
}

bool FBlenderBridgeActorDetails::ShouldShowGeneratedBakeMaterialsProperty() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->GeneratedBakeMaterials.IsEmpty();
    }

    return false;
}

bool FBlenderBridgeActorDetails::ShouldShowInstancedBakeNote() const
{
    const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    return BridgeComponent != nullptr
        && BridgeComponent->BakeOutputMode == EBlenderBridgeBakeOutputMode::InstancedHierarchy;
}

bool FBlenderBridgeActorDetails::CanSelectOrDeleteBakedOutput() const
{
    return AnySelectedActorsHaveBakedOutput();
}

bool FBlenderBridgeActorDetails::HasCachedInstanceBakePayload() const
{
    const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return false;
    }

    if (!FModuleManager::Get().IsModuleLoaded(TEXT("BlenderBridgeRuntime")))
    {
        return false;
    }

    return FBlenderBridgeRuntimeModule::Get().GetSessionManager().FindCachedInstanceBake(BridgeComponent) != nullptr;
}

EVisibility FBlenderBridgeActorDetails::GetSingleSelectionVisibility() const
{
    return IsSingleSelection() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FBlenderBridgeActorDetails::GetMultiSelectionVisibility() const
{
    return SelectedActors.Num() > 1 ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FBlenderBridgeActorDetails::GetSetupHintVisibility() const
{
    return GetSetupHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FBlenderBridgeActorDetails::GetParametersHintVisibility() const
{
    return GetParametersHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FBlenderBridgeActorDetails::GetParametersPropertyVisibility() const
{
    return ShouldShowParameterProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FBlenderBridgeActorDetails::GetCurveHintVisibility() const
{
    return GetCurveHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FBlenderBridgeActorDetails::GetCurveSplinePropertyVisibility() const
{
    return ShouldShowCurveSplineProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FBlenderBridgeActorDetails::GetMaterialsHintVisibility() const
{
    return GetMaterialsHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FBlenderBridgeActorDetails::GetMaterialPropertiesVisibility() const
{
    return ShouldShowMaterialProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FBlenderBridgeActorDetails::GetInstancedBakeNoteVisibility() const
{
    return ShouldShowInstancedBakeNote() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FBlenderBridgeActorDetails::GetBakedActorPropertyVisibility() const
{
    return ShouldShowBakedActorProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FBlenderBridgeActorDetails::GetBakedStaticMeshPropertyVisibility() const
{
    return ShouldShowBakedStaticMeshProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FBlenderBridgeActorDetails::GetBakedPrototypePropertiesVisibility() const
{
    return ShouldShowBakedPrototypeProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FBlenderBridgeActorDetails::GetGeneratedBakeMaterialsPropertyVisibility() const
{
    return ShouldShowGeneratedBakeMaterialsProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

FSlateColor FBlenderBridgeActorDetails::GetStatusStripColor() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        if (BridgeComponent->ConnectionState != EBlenderBridgeConnectionState::Connected)
        {
            return FSlateColor(GetConnectionStateColor(BridgeComponent->ConnectionState));
        }

        return FSlateColor(GetComponentStatusColor(BridgeComponent->ComponentStatus));
    }

    if (HasSelectedActors())
    {
        return FSlateColor(GetConnectionStateColor(EBlenderBridgeConnectionState::Connected));
    }

    return FSlateColor(FLinearColor(0.55f, 0.55f, 0.55f));
}

FText FBlenderBridgeActorDetails::GetStatusStripText() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return FText::Format(
            FText::FromString(TEXT("{0} | {1}")),
            GetEnumDisplayText(BridgeComponent->ConnectionState),
            GetEnumDisplayText(BridgeComponent->ComponentStatus));
    }

    if (SelectedActors.Num() > 1)
    {
        return FText::FromString(TEXT("Batch Actions Available"));
    }

    return FText::FromString(TEXT("No Bridge Actor Selected"));
}

FText FBlenderBridgeActorDetails::GetStatusStripToolTipText() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->LastStatusMessage.IsEmpty()
            ? GetStatusStripText()
            : FText::FromString(BridgeComponent->LastStatusMessage);
    }

    return FText::FromString(TEXT("Select one bridge actor to inspect source, targets, parameters, and outputs."));
}

FText FBlenderBridgeActorDetails::GetOverviewSelectionSummaryText() const
{
    if (IsSingleSelection())
    {
        return FText::FromString(TEXT("One bridge actor selected. The controls below edit the live bridge component directly."));
    }

    if (SelectedActors.Num() > 1)
    {
        return FText::Format(
            FText::FromString(TEXT("{0} bridge actors selected. Batch actions stay available here; select one actor to edit parameters and outputs.")),
            FText::AsNumber(SelectedActors.Num()));
    }

    return FText::FromString(TEXT("Select a Blender Bridge actor to manage its source, target, parameters, bake settings, and outputs."));
}

FText FBlenderBridgeActorDetails::GetSourceSummaryText() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->Source != nullptr
            ? FText::FromString(BridgeComponent->Source->GetName())
            : FText::FromString(TEXT("None"));
    }

    return FText::GetEmpty();
}

FText FBlenderBridgeActorDetails::GetTargetSummaryText() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        if (!BridgeComponent->TargetDisplayLabel.IsEmpty())
        {
            return FText::FromString(BridgeComponent->TargetDisplayLabel);
        }

        if (!BridgeComponent->TargetId.IsEmpty())
        {
            return FText::FromString(BridgeComponent->TargetId);
        }

        return FText::FromString(TEXT("No target selected"));
    }

    return FText::GetEmpty();
}

FText FBlenderBridgeActorDetails::GetBridgeBuildSummaryText() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return FText::FromString(
            BridgeComponent->BridgeBuildVersion.IsEmpty()
                ? TEXT("Unknown build")
                : BridgeComponent->BridgeBuildVersion);
    }

    return FText::GetEmpty();
}

FText FBlenderBridgeActorDetails::GetLastStatusSummaryText() const
{
    if (const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return FText::FromString(
            BridgeComponent->LastStatusMessage.IsEmpty()
                ? TEXT("No status message yet.")
                : BridgeComponent->LastStatusMessage);
    }

    return FText::GetEmpty();
}

FText FBlenderBridgeActorDetails::GetMultiSelectionSummaryText() const
{
    if (SelectedActors.Num() <= 1)
    {
        return FText::GetEmpty();
    }

    return FText::FromString(
        TEXT("Batch actions stay on the actor surface for multi-selection. Choose a single bridge actor when you want to edit source assignment, target selection, parameters, bake settings, or outputs."));
}

FText FBlenderBridgeActorDetails::GetSetupHintText() const
{
    const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->Source == nullptr)
    {
        return FText::FromString(TEXT("Assign a Blender Bridge source to load targets and reflect parameters."));
    }

    if (BridgeComponent->AvailableTargets.IsEmpty())
    {
        return FText::FromString(TEXT("No targets are available yet. Use Refresh Input Schema to load the source and populate target options."));
    }

    if (BridgeComponent->TargetId.IsEmpty())
    {
        return FText::FromString(TEXT("Choose a target to reflect its schema and unlock parameters, materials, and bake output details."));
    }

    const bool bTargetExists = BridgeComponent->AvailableTargets.ContainsByPredicate([BridgeComponent](const FBridgeTargetDescriptor& TargetDescriptor)
    {
        return TargetDescriptor.TargetId == BridgeComponent->TargetId;
    });

    if (!bTargetExists)
    {
        return FText::FromString(TEXT("The current target is not available in the loaded source. Pick a valid target or refresh the source data."));
    }

    return FText::GetEmpty();
}

FText FBlenderBridgeActorDetails::GetParametersHintText() const
{
    const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->Source == nullptr || BridgeComponent->TargetId.IsEmpty())
    {
        return FText::FromString(TEXT("Parameters appear after a source and target are selected."));
    }

    if (BridgeComponent->ComponentStatus == EBlenderBridgeComponentStatus::SchemaPending)
    {
        return FText::FromString(TEXT("The target schema is still loading."));
    }

    if (BridgeComponent->InputSchema.IsEmpty())
    {
        return FText::FromString(TEXT("This target does not expose any editable inputs."));
    }

    return FText::GetEmpty();
}

FText FBlenderBridgeActorDetails::GetCurveHintText() const
{
    const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->TargetCapabilities.TargetKind != EBlenderBridgeTargetKind::CurveOwner)
    {
        return FText::GetEmpty();
    }

    if (!BridgeComponent->TargetCapabilities.bSupportsCurveOwner)
    {
        return BridgeComponent->TargetCapabilities.CompatibilityMessage.IsEmpty()
            ? FText::FromString(TEXT("This target is curve-owner based, but embedded spline editing is unavailable."))
            : FText::FromString(FString::Printf(
                TEXT("Embedded spline editing is unavailable for this target: %s"),
                *BridgeComponent->TargetCapabilities.CompatibilityMessage));
    }

    if (!ShouldShowCurveSplineProperty())
    {
        return FText::FromString(TEXT("This target is curve-owner compatible, but the embedded spline is not currently available."));
    }

    return FText::FromString(TEXT("Edit the embedded spline in the viewport to drive this target. Spline edits continue to queue cooks using the existing manual workflow."));
}

FText FBlenderBridgeActorDetails::GetMaterialsHintText() const
{
    const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->LastCookedMaterialSlots.IsEmpty())
    {
        return FText::FromString(TEXT("No cooked material slots are available yet. Request Cook to populate slot mappings and override materials."));
    }

    return FText::Format(
        FText::FromString(TEXT("{0} cooked material slot(s) are available. Material overrides apply to preview and bake output; generated bake materials appear in Outputs after baking.")),
        FText::AsNumber(BridgeComponent->LastCookedMaterialSlots.Num()));
}

FText FBlenderBridgeActorDetails::GetBakeModeDescriptionText() const
{
    const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->BakeOutputMode == EBlenderBridgeBakeOutputMode::InstancedHierarchy)
    {
        return FText::FromString(TEXT("InstancedHierarchy preserves recoverable repeated elements as HISM output and shares prototype meshes across duplicate actors from the same blend source."));
    }

    return FText::FromString(TEXT("StaticMesh preserves the existing one-mesh bake flow by collapsing the cooked result into a single baked mesh asset."));
}

FText FBlenderBridgeActorDetails::GetInstancedBakeNoteText() const
{
    const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr || BridgeComponent->BakeOutputMode != EBlenderBridgeBakeOutputMode::InstancedHierarchy)
    {
        return FText::GetEmpty();
    }

    if (!BridgeComponent->BakedPrototypeMeshes.IsEmpty())
    {
        return FText::Format(
            FText::FromString(TEXT("This actor currently references {0} shared prototype mesh(es) from the last instanced bake.")),
            FText::AsNumber(BridgeComponent->BakedPrototypeMeshes.Num()));
    }

    if (HasCachedInstanceBakePayload())
    {
        return FText::FromString(TEXT("An instance-bake payload is already cached for this actor and ready for Bake Output."));
    }

    return FText::FromString(TEXT("No instance-bake payload is cached yet. Bake Output will request dedicated instance data from Blender when you run it."));
}

FText FBlenderBridgeActorDetails::GetOutputsSummaryText() const
{
    const UBlenderBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (!HasBakedOutput(BridgeComponent))
    {
        return FText::FromString(TEXT("No baked output exists yet. Use Bake Output to create actor assets from the current bridge state."));
    }

    if (!BridgeComponent->BakedPrototypeMeshes.IsEmpty())
    {
        return FText::Format(
            FText::FromString(TEXT("Instanced bake output is active. This actor has a baked actor reference, {0} shared prototype mesh(es), and {1} generated bake material(s).")),
            FText::AsNumber(BridgeComponent->BakedPrototypeMeshes.Num()),
            FText::AsNumber(BridgeComponent->GeneratedBakeMaterials.Num()));
    }

    return FText::Format(
        FText::FromString(TEXT("Static bake output is active. This actor references a baked actor, {0} baked static mesh asset, and {1} generated bake material(s).")),
        FText::AsNumber(BridgeComponent->BakedStaticMesh.IsNull() ? 0 : 1),
        FText::AsNumber(BridgeComponent->GeneratedBakeMaterials.Num()));
}

FText FBlenderBridgeActorDetails::GetAdvancedHintText() const
{
    return FText::FromString(TEXT("Raw implementation details live here for debugging and escape-hatch workflows. The sections above are the intended day-to-day control surface."));
}

FReply FBlenderBridgeActorDetails::HandleRefreshInputSchemaClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->RefreshInputSchema();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FBlenderBridgeActorDetails::HandleRequestCookClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->RequestCook();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FBlenderBridgeActorDetails::HandleBakeToStaticMeshClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->BakeToStaticMesh();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FBlenderBridgeActorDetails::HandleSelectBakedOutputClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->SelectBakedOutput();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FBlenderBridgeActorDetails::HandleDeleteBakedOutputClicked()
{
    for (const TWeakObjectPtr<ABlenderBridgeActor>& SelectedActor : SelectedActors)
    {
        if (ABlenderBridgeActor* Actor = SelectedActor.Get())
        {
            if (UBlenderBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->DeleteBakedOutput();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}
