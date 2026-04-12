#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Layout/Visibility.h"
#include "Styling/SlateColor.h"

class AGeometryNodesBridgeActor;
class IDetailCategoryBuilder;
class IDetailLayoutBuilder;
class IDetailPropertyRow;
class IPropertyHandle;
class IPropertyUtilities;
class UGeometryNodesBridgeComponent;

class FGeometryNodesBridgeActorDetails final : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance();

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
    void HideDefaultBridgeCategories(IDetailLayoutBuilder& DetailBuilder) const;
    void RegisterRefreshOnPropertyChange(IDetailLayoutBuilder& DetailBuilder, UGeometryNodesBridgeComponent* BridgeComponent, FName PropertyName);

    void BuildOverviewCategory(IDetailLayoutBuilder& DetailBuilder);
    void BuildGenerateCategory(IDetailLayoutBuilder& DetailBuilder);
    void BuildSetupCategory(IDetailLayoutBuilder& DetailBuilder);
    void BuildParametersCategory(IDetailLayoutBuilder& DetailBuilder);
    void BuildGeometryCategory(IDetailLayoutBuilder& DetailBuilder);
    void BuildCurveCategory(IDetailLayoutBuilder& DetailBuilder);
    void BuildMaterialsCategory(IDetailLayoutBuilder& DetailBuilder);
    void BuildBakeCategory(IDetailLayoutBuilder& DetailBuilder);
    void BuildOutputsCategory(IDetailLayoutBuilder& DetailBuilder);
    void BuildAdvancedCategory(IDetailLayoutBuilder& DetailBuilder);

    void AddInfoMessageRow(
        IDetailCategoryBuilder& CategoryBuilder,
        const FText& FilterText,
        TAttribute<FText> MessageText,
        const FLinearColor& BackgroundColor,
        TAttribute<EVisibility> Visibility = TAttribute<EVisibility>(EVisibility::Visible)) const;
    void AddReadOnlyTextRow(
        IDetailCategoryBuilder& CategoryBuilder,
        const FText& Label,
        TAttribute<FText> ValueText,
        TAttribute<EVisibility> Visibility = TAttribute<EVisibility>(EVisibility::Visible)) const;

    IDetailPropertyRow* AddComponentProperty(
        IDetailCategoryBuilder& CategoryBuilder,
        FName PropertyName,
        bool bForceShow = false,
        bool bAutoExpand = false) const;
    IDetailPropertyRow* AddActorProperty(
        IDetailLayoutBuilder& DetailBuilder,
        IDetailCategoryBuilder& CategoryBuilder,
        FName PropertyName,
        bool bAutoExpand = false) const;

    void ForceRefreshDetails() const;
    void HandleRefreshDependentPropertyChanged();

    TArray<UObject*> GetSelectedBridgeComponentObjects() const;
    AGeometryNodesBridgeActor* GetSingleSelectedActor() const;
    UGeometryNodesBridgeComponent* GetSingleSelectedBridgeComponent() const;

    bool IsSingleSelection() const;
    bool HasSelectedActors() const;
    bool AnySelectedActorsHaveBakedOutput() const;
    bool ShouldShowGeometryCategory() const;
    bool ShouldShowCurveCategory() const;
    bool ShouldShowGeometryProperties() const;
    bool ShouldShowCurveSplineProperty() const;
    bool ShouldShowMaterialProperties() const;
    bool ShouldShowParameterProperties() const;
    bool ShouldShowBakedActorProperty() const;
    bool ShouldShowBakedStaticMeshProperty() const;
    bool ShouldShowBakedPrototypeProperties() const;
    bool ShouldShowGeneratedBakeMaterialsProperty() const;
    bool ShouldShowInstancedBakeNote() const;
    bool CanSelectOrDeleteBakedOutput() const;
    bool HasCachedInstanceBakePayload() const;

    EVisibility GetSingleSelectionVisibility() const;
    EVisibility GetMultiSelectionVisibility() const;
    EVisibility GetSetupHintVisibility() const;
    EVisibility GetParametersHintVisibility() const;
    EVisibility GetParametersPropertyVisibility() const;
    EVisibility GetGeometryHintVisibility() const;
    EVisibility GetGeometryPropertiesVisibility() const;
    EVisibility GetCurveHintVisibility() const;
    EVisibility GetCurveSplinePropertyVisibility() const;
    EVisibility GetMaterialsHintVisibility() const;
    EVisibility GetMaterialPropertiesVisibility() const;
    EVisibility GetInstancedBakeNoteVisibility() const;
    EVisibility GetBakedActorPropertyVisibility() const;
    EVisibility GetBakedStaticMeshPropertyVisibility() const;
    EVisibility GetBakedPrototypePropertiesVisibility() const;
    EVisibility GetGeneratedBakeMaterialsPropertyVisibility() const;

    FSlateColor GetStatusStripColor() const;
    FText GetStatusStripText() const;
    FText GetStatusStripToolTipText() const;
    FText GetOverviewSelectionSummaryText() const;
    FText GetSourceSummaryText() const;
    FText GetTargetSummaryText() const;
    FText GetBridgeBuildSummaryText() const;
    FText GetLastStatusSummaryText() const;
    FText GetMultiSelectionSummaryText() const;
    FText GetSetupHintText() const;
    FText GetParametersHintText() const;
    FText GetGeometryHintText() const;
    FText GetCurveHintText() const;
    FText GetMaterialsHintText() const;
    FText GetBakeModeDescriptionText() const;
    FText GetInstancedBakeNoteText() const;
    FText GetOutputsSummaryText() const;
    FText GetAdvancedHintText() const;

    FReply HandleRefreshInputSchemaClicked();
    FReply HandleRequestCookClicked();
    FReply HandleBakeToStaticMeshClicked();
    FReply HandleSelectBakedOutputClicked();
    FReply HandleDeleteBakedOutputClicked();

private:
    TWeakPtr<IPropertyUtilities> PropertyUtilities;
    TArray<TWeakObjectPtr<AGeometryNodesBridgeActor>> SelectedActors;
};
