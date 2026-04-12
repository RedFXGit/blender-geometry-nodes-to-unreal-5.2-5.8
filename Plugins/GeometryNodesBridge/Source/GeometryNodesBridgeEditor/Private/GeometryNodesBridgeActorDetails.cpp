#include "GeometryNodesBridgeActorDetails.h"

#include "GeometryNodesBridgeActor.h"
#include "GeometryNodesBridgeComponent.h"
#include "GeometryNodesBridgeRuntimeModule.h"
#include "GeometryNodesBridgeSessionManager.h"
#include "GeometryNodesBridgeSource.h"
#include "Components/SplineComponent.h"
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
    constexpr int32 GeometrySortOrder = -165;
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

    bool HasBakedOutput(const UGeometryNodesBridgeComponent* BridgeComponent)
    {
        return BridgeComponent != nullptr
            && (BridgeComponent->BakedActor.IsValid()
                || !BridgeComponent->BakedStaticMesh.IsNull()
                || !BridgeComponent->BakedPrototypeMeshes.IsEmpty());
    }

    FLinearColor GetConnectionStateColor(const EGeometryNodesBridgeConnectionState ConnectionState)
    {
        switch (ConnectionState)
        {
        case EGeometryNodesBridgeConnectionState::Connected:
            return FLinearColor(0.18f, 0.72f, 0.32f);
        case EGeometryNodesBridgeConnectionState::Launching:
            return FLinearColor(0.92f, 0.73f, 0.18f);
        case EGeometryNodesBridgeConnectionState::Disconnected:
            return FLinearColor(0.85f, 0.22f, 0.22f);
        case EGeometryNodesBridgeConnectionState::Error:
            return FLinearColor(0.78f, 0.12f, 0.12f);
        default:
            return FLinearColor(0.55f, 0.55f, 0.55f);
        }
    }

    FLinearColor GetComponentStatusColor(const EGeometryNodesBridgeComponentStatus ComponentStatus)
    {
        switch (ComponentStatus)
        {
        case EGeometryNodesBridgeComponentStatus::Ready:
        case EGeometryNodesBridgeComponentStatus::Cooked:
            return FLinearColor(0.18f, 0.72f, 0.32f);
        case EGeometryNodesBridgeComponentStatus::CookQueued:
        case EGeometryNodesBridgeComponentStatus::Cooking:
        case EGeometryNodesBridgeComponentStatus::SchemaPending:
            return FLinearColor(0.92f, 0.73f, 0.18f);
        case EGeometryNodesBridgeComponentStatus::Error:
            return FLinearColor(0.85f, 0.22f, 0.22f);
        default:
            return FLinearColor(0.60f, 0.60f, 0.60f);
        }
    }

    const FGeometryNodesBridgeGeometryBinding* FindGeometryBinding(const UGeometryNodesBridgeComponent* BridgeComponent, const FString& InputId)
    {
        if (BridgeComponent == nullptr)
        {
            return nullptr;
        }

        return BridgeComponent->GeometryBindings.FindByPredicate([&InputId](const FGeometryNodesBridgeGeometryBinding& Binding)
        {
            return Binding.InputId == InputId;
        });
    }

    bool DescriptorSupportsSplineBinding(const FBridgeGeometryInputDescriptor& Descriptor)
    {
        return Descriptor.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::EmbeddedSpline)
            || Descriptor.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::SplineComponent);
    }

    bool DescriptorSupportsActorOverride(const FBridgeGeometryInputDescriptor& Descriptor)
    {
        return Descriptor.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent)
            || Descriptor.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::SplineComponent);
    }

    template <typename ComponentType>
    ComponentType* ResolveReferencedComponent(AActor* ContextActor, const FComponentReference& ComponentReference)
    {
        AActor* SearchActor = ComponentReference.OtherActor.Get();
        if (SearchActor == nullptr)
        {
            return nullptr;
        }

        if (ComponentType* DirectComponent = Cast<ComponentType>(ComponentReference.GetComponent(ContextActor)))
        {
            return DirectComponent;
        }

        if (ComponentType* RootComponent = Cast<ComponentType>(SearchActor->GetRootComponent()))
        {
            return RootComponent;
        }

        TInlineComponentArray<ComponentType*> MatchingComponents(SearchActor);
        for (ComponentType* MatchingComponent : MatchingComponents)
        {
            if (MatchingComponent != nullptr)
            {
                return MatchingComponent;
            }
        }

        return nullptr;
    }

    FText GetGeometryBindingDisplayName(
        const UGeometryNodesBridgeComponent* BridgeComponent,
        const FBridgeGeometryInputDescriptor& Descriptor)
    {
        const bool bSupportsStaticMeshBinding = Descriptor.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent);
        const bool bSupportsSplineBinding = Descriptor.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::SplineComponent);

        if (bSupportsStaticMeshBinding && !bSupportsSplineBinding)
        {
            int32 StaticMeshBindingCount = 0;
            if (BridgeComponent != nullptr)
            {
                for (const FBridgeGeometryInputDescriptor& Entry : BridgeComponent->GeometryInputDescriptors)
                {
                    if (Entry.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent)
                        && !Entry.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::SplineComponent))
                    {
                        ++StaticMeshBindingCount;
                    }
                }
            }

            if (StaticMeshBindingCount <= 1)
            {
                return FText::FromString(TEXT("Optional Reference Actor Override"));
            }
        }

        if (!Descriptor.Label.IsEmpty())
        {
            return FText::FromString(FString::Printf(TEXT("Optional %s Actor Override"), *Descriptor.Label));
        }

        if (bSupportsSplineBinding && !bSupportsStaticMeshBinding)
        {
            return FText::FromString(TEXT("Optional Spline Actor Override"));
        }

        return FText::FromString(TEXT("Optional Reference Actor Override"));
    }

    bool IsExternalSplineBindingActive(const UGeometryNodesBridgeComponent* BridgeComponent)
    {
        if (BridgeComponent == nullptr)
        {
            return false;
        }

        for (const FBridgeGeometryInputDescriptor& Descriptor : BridgeComponent->GeometryInputDescriptors)
        {
            if (!DescriptorSupportsSplineBinding(Descriptor))
            {
                continue;
            }

            if (const FGeometryNodesBridgeGeometryBinding* Binding = FindGeometryBinding(BridgeComponent, Descriptor.InputId))
            {
                if (ResolveReferencedComponent<USplineComponent>(const_cast<AActor*>(BridgeComponent->GetOwner()), Binding->ComponentReference) != nullptr)
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool HasValidExternalSplineBinding(const UGeometryNodesBridgeComponent* BridgeComponent, AActor* ContextActor)
    {
        if (BridgeComponent == nullptr || ContextActor == nullptr)
        {
            return false;
        }

        for (const FBridgeGeometryInputDescriptor& Descriptor : BridgeComponent->GeometryInputDescriptors)
        {
            if (!DescriptorSupportsSplineBinding(Descriptor))
            {
                continue;
            }

            const FGeometryNodesBridgeGeometryBinding* Binding = FindGeometryBinding(BridgeComponent, Descriptor.InputId);
            if (Binding == nullptr)
            {
                continue;
            }

            if (ResolveReferencedComponent<USplineComponent>(ContextActor, Binding->ComponentReference) != nullptr)
            {
                return true;
            }
        }

        return false;
    }
}

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

    if (IDetailPropertyRow* InputValuesRow = AddComponentProperty(
        ParametersCategory,
        GET_MEMBER_NAME_CHECKED(UGeometryNodesBridgeComponent, InputValues),
        false,
        true))
    {
        InputValuesRow->DisplayName(FText::FromString(TEXT("Input Values")));
        InputValuesRow->Visibility(TAttribute<EVisibility>::CreateSP(this, &FGeometryNodesBridgeActorDetails::GetParametersPropertyVisibility));
    }
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

TArray<UObject*> FGeometryNodesBridgeActorDetails::GetSelectedBridgeComponentObjects() const
{
    TArray<UObject*> BridgeComponents;
    BridgeComponents.Reserve(SelectedActors.Num());

    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponents.Add(BridgeComponent);
            }
        }
    }

    return BridgeComponents;
}

AGeometryNodesBridgeActor* FGeometryNodesBridgeActorDetails::GetSingleSelectedActor() const
{
    return SelectedActors.Num() == 1 ? SelectedActors[0].Get() : nullptr;
}

UGeometryNodesBridgeComponent* FGeometryNodesBridgeActorDetails::GetSingleSelectedBridgeComponent() const
{
    if (AGeometryNodesBridgeActor* Actor = GetSingleSelectedActor())
    {
        return Actor->GetBridgeComponent();
    }

    return nullptr;
}

bool FGeometryNodesBridgeActorDetails::IsSingleSelection() const
{
    return GetSingleSelectedActor() != nullptr;
}

bool FGeometryNodesBridgeActorDetails::HasSelectedActors() const
{
    return !SelectedActors.IsEmpty();
}

bool FGeometryNodesBridgeActorDetails::AnySelectedActorsHaveBakedOutput() const
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (const AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (HasBakedOutput(Actor->GetBridgeComponent()))
            {
                return true;
            }
        }
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowCurveCategory() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->TargetCapabilities.TargetKind == EGeometryNodesBridgeTargetKind::CurveOwner
            || (GetSingleSelectedActor() != nullptr && GetSingleSelectedActor()->GetBridgeSplineComponent() != nullptr);
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowGeometryCategory() const
{
    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowGeometryProperties() const
{
    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowCurveSplineProperty() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr || IsExternalSplineBindingActive(BridgeComponent))
    {
        return false;
    }

    return GetSingleSelectedActor() != nullptr && GetSingleSelectedActor()->GetBridgeSplineComponent() != nullptr;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowMaterialProperties() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->LastCookedMaterialSlots.IsEmpty();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowParameterProperties() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->InputSchema.IsEmpty();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowBakedActorProperty() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->BakedActor.IsValid();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowBakedStaticMeshProperty() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->BakedStaticMesh.IsNull();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowBakedPrototypeProperties() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->BakedPrototypeMeshes.IsEmpty();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowGeneratedBakeMaterialsProperty() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return !BridgeComponent->GeneratedBakeMaterials.IsEmpty();
    }

    return false;
}

bool FGeometryNodesBridgeActorDetails::ShouldShowInstancedBakeNote() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    return BridgeComponent != nullptr
        && BridgeComponent->BakeOutputMode == EGeometryNodesBridgeBakeOutputMode::InstancedHierarchy;
}

bool FGeometryNodesBridgeActorDetails::CanSelectOrDeleteBakedOutput() const
{
    return AnySelectedActorsHaveBakedOutput();
}

bool FGeometryNodesBridgeActorDetails::HasCachedInstanceBakePayload() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return false;
    }

    if (!FModuleManager::Get().IsModuleLoaded(TEXT("GeometryNodesBridgeRuntime")))
    {
        return false;
    }

    return FGeometryNodesBridgeRuntimeModule::Get().GetSessionManager().FindCachedInstanceBake(BridgeComponent) != nullptr;
}

EVisibility FGeometryNodesBridgeActorDetails::GetSingleSelectionVisibility() const
{
    return IsSingleSelection() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetMultiSelectionVisibility() const
{
    return SelectedActors.Num() > 1 ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetSetupHintVisibility() const
{
    return GetSetupHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetParametersHintVisibility() const
{
    return GetParametersHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetParametersPropertyVisibility() const
{
    return ShouldShowParameterProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetGeometryHintVisibility() const
{
    return GetGeometryHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetGeometryPropertiesVisibility() const
{
    return ShouldShowGeometryProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetCurveHintVisibility() const
{
    return GetCurveHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetCurveSplinePropertyVisibility() const
{
    return ShouldShowCurveSplineProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetMaterialsHintVisibility() const
{
    return GetMaterialsHintText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility FGeometryNodesBridgeActorDetails::GetMaterialPropertiesVisibility() const
{
    return ShouldShowMaterialProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetInstancedBakeNoteVisibility() const
{
    return ShouldShowInstancedBakeNote() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetBakedActorPropertyVisibility() const
{
    return ShouldShowBakedActorProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetBakedStaticMeshPropertyVisibility() const
{
    return ShouldShowBakedStaticMeshProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetBakedPrototypePropertiesVisibility() const
{
    return ShouldShowBakedPrototypeProperties() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FGeometryNodesBridgeActorDetails::GetGeneratedBakeMaterialsPropertyVisibility() const
{
    return ShouldShowGeneratedBakeMaterialsProperty() ? EVisibility::Visible : EVisibility::Collapsed;
}

FSlateColor FGeometryNodesBridgeActorDetails::GetStatusStripColor() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        if (BridgeComponent->ConnectionState != EGeometryNodesBridgeConnectionState::Connected)
        {
            return FSlateColor(GetConnectionStateColor(BridgeComponent->ConnectionState));
        }

        return FSlateColor(GetComponentStatusColor(BridgeComponent->ComponentStatus));
    }

    if (HasSelectedActors())
    {
        return FSlateColor(GetConnectionStateColor(EGeometryNodesBridgeConnectionState::Connected));
    }

    return FSlateColor(FLinearColor(0.55f, 0.55f, 0.55f));
}

FText FGeometryNodesBridgeActorDetails::GetStatusStripText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
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

FText FGeometryNodesBridgeActorDetails::GetStatusStripToolTipText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->LastStatusMessage.IsEmpty()
            ? GetStatusStripText()
            : FText::FromString(BridgeComponent->LastStatusMessage);
    }

    return FText::FromString(TEXT("Select one bridge actor to inspect source, targets, parameters, and outputs."));
}

FText FGeometryNodesBridgeActorDetails::GetOverviewSelectionSummaryText() const
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

    return FText::FromString(TEXT("Select a Geometry Nodes Bridge actor to manage its source, target, parameters, bake settings, and outputs."));
}

FText FGeometryNodesBridgeActorDetails::GetSourceSummaryText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return BridgeComponent->Source != nullptr
            ? FText::FromString(BridgeComponent->Source->GetName())
            : FText::FromString(TEXT("None"));
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetTargetSummaryText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
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

FText FGeometryNodesBridgeActorDetails::GetBridgeBuildSummaryText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return FText::FromString(
            BridgeComponent->BridgeBuildVersion.IsEmpty()
                ? TEXT("Unknown build")
                : BridgeComponent->BridgeBuildVersion);
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetLastStatusSummaryText() const
{
    if (const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent())
    {
        return FText::FromString(
            BridgeComponent->LastStatusMessage.IsEmpty()
                ? TEXT("No status message yet.")
                : BridgeComponent->LastStatusMessage);
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetMultiSelectionSummaryText() const
{
    if (SelectedActors.Num() <= 1)
    {
        return FText::GetEmpty();
    }

    return FText::FromString(
        TEXT("Batch actions stay on the actor surface for multi-selection. Choose a single bridge actor when you want to edit source assignment, target selection, parameters, bake settings, or outputs."));
}

FText FGeometryNodesBridgeActorDetails::GetSetupHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->Source == nullptr)
    {
        return FText::FromString(TEXT("Assign a Geometry Nodes Bridge source to load targets and reflect parameters."));
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

FText FGeometryNodesBridgeActorDetails::GetParametersHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->Source == nullptr || BridgeComponent->TargetId.IsEmpty())
    {
        return FText::FromString(TEXT("Parameters appear after a source and target are selected."));
    }

    if (BridgeComponent->ComponentStatus == EGeometryNodesBridgeComponentStatus::SchemaPending)
    {
        return FText::FromString(TEXT("The target schema is still loading."));
    }

    if (BridgeComponent->InputSchema.IsEmpty())
    {
        if (!BridgeComponent->GeometryInputDescriptors.IsEmpty())
        {
            return FText::FromString(TEXT("This target does not expose scalar inputs. Optional reference actor overrides are available in Setup."));
        }

        return FText::FromString(TEXT("This target does not expose any editable inputs."));
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetGeometryHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->Source == nullptr || BridgeComponent->TargetId.IsEmpty())
    {
        return FText::FromString(TEXT("Geometry bindings appear after a source and target are selected."));
    }

    if (BridgeComponent->ComponentStatus == EGeometryNodesBridgeComponentStatus::SchemaPending)
    {
        return FText::FromString(TEXT("Geometry binding options are still loading."));
    }

    if (BridgeComponent->GeometryInputDescriptors.IsEmpty())
    {
        return FText::FromString(TEXT("This target does not expose any bindable geometry inputs."));
    }

    const bool bSupportsStaticMeshBinding = BridgeComponent->GeometryInputDescriptors.ContainsByPredicate([](const FBridgeGeometryInputDescriptor& Descriptor)
    {
        return Descriptor.AllowedSourceKinds.Contains(EGeometryNodesBridgeGeometrySourceKind::StaticMeshComponent);
    });

    const bool bSupportsSplineBinding = BridgeComponent->GeometryInputDescriptors.ContainsByPredicate([](const FBridgeGeometryInputDescriptor& Descriptor)
    {
        return DescriptorSupportsSplineBinding(Descriptor);
    });

    if (bSupportsStaticMeshBinding && !bSupportsSplineBinding)
    {
        return FText::FromString(TEXT("Pick a level StaticMeshComponent here to override owner-geometry inputs such as floorplan meshes. Leaving it empty keeps the blend file's original geometry."));
    }

    if (bSupportsSplineBinding)
    {
        return FText::FromString(TEXT("Leave the picker empty to use the embedded bridge spline, or assign a level spline component to override it from Unreal."));
    }

    return FText::GetEmpty();
}

FText FGeometryNodesBridgeActorDetails::GetCurveHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->TargetCapabilities.TargetKind != EGeometryNodesBridgeTargetKind::CurveOwner)
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

    if (IsExternalSplineBindingActive(BridgeComponent))
    {
        if (HasValidExternalSplineBinding(BridgeComponent, GetSingleSelectedActor()))
        {
            return FText::FromString(TEXT("This target is being driven by an external spline component selected in Setup. Edit that spline in the viewport and cooks will follow its shape."));
        }

        return FText::FromString(TEXT("This target is set to use an external spline component, but the current binding is invalid. Pick a valid spline actor override in Setup to cook this target."));
    }

    if (!ShouldShowCurveSplineProperty())
    {
        return FText::FromString(TEXT("This target is curve-owner compatible, but the embedded spline is not currently available."));
    }

    return FText::FromString(TEXT("Edit the embedded spline in the viewport to drive this target. Spline edits continue to queue cooks using the existing manual workflow."));
}

FText FGeometryNodesBridgeActorDetails::GetMaterialsHintText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
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

FText FGeometryNodesBridgeActorDetails::GetBakeModeDescriptionText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr)
    {
        return FText::GetEmpty();
    }

    if (BridgeComponent->BakeOutputMode == EGeometryNodesBridgeBakeOutputMode::InstancedHierarchy)
    {
        return FText::FromString(TEXT("InstancedHierarchy preserves recoverable repeated elements as HISM output and shares prototype meshes across duplicate actors from the same blend source."));
    }

    return FText::FromString(TEXT("StaticMesh preserves the existing one-mesh bake flow by collapsing the cooked result into a single baked mesh asset."));
}

FText FGeometryNodesBridgeActorDetails::GetInstancedBakeNoteText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
    if (BridgeComponent == nullptr || BridgeComponent->BakeOutputMode != EGeometryNodesBridgeBakeOutputMode::InstancedHierarchy)
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

FText FGeometryNodesBridgeActorDetails::GetOutputsSummaryText() const
{
    const UGeometryNodesBridgeComponent* BridgeComponent = GetSingleSelectedBridgeComponent();
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

FText FGeometryNodesBridgeActorDetails::GetAdvancedHintText() const
{
    return FText::FromString(TEXT("Raw implementation details live here for debugging and escape-hatch workflows. The sections above are the intended day-to-day control surface."));
}

FReply FGeometryNodesBridgeActorDetails::HandleRefreshInputSchemaClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->RefreshInputSchema();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FGeometryNodesBridgeActorDetails::HandleRequestCookClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->RequestCook();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FGeometryNodesBridgeActorDetails::HandleBakeToStaticMeshClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->BakeToStaticMesh();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FGeometryNodesBridgeActorDetails::HandleSelectBakedOutputClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->SelectBakedOutput();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}

FReply FGeometryNodesBridgeActorDetails::HandleDeleteBakedOutputClicked()
{
    for (const TWeakObjectPtr<AGeometryNodesBridgeActor>& SelectedActor : SelectedActors)
    {
        if (AGeometryNodesBridgeActor* Actor = SelectedActor.Get())
        {
            if (UGeometryNodesBridgeComponent* BridgeComponent = Actor->GetBridgeComponent())
            {
                BridgeComponent->DeleteBakedOutput();
            }
        }
    }

    ForceRefreshDetails();
    return FReply::Handled();
}
