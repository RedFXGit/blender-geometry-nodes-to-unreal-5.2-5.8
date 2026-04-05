#include "BlenderBridgePropertyBagDetails.h"

#include "BlenderBridgeComponent.h"
#include "Styling/AppStyle.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "GameFramework/Actor.h"
#include "IDetailChildrenBuilder.h"
#include "InstancedStructDetails.h"
#include "Misc/Attribute.h"
#include "PropertyHandle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
    DEFINE_LOG_CATEGORY_STATIC(LogBlenderBridgeEditor, Log, All);

    constexpr TCHAR BlenderBridgeGroupedLayoutMetaKey[] = TEXT("BlenderBridgeGroupedLayout");
    constexpr float BlenderBridgeGroupIndentWidth = 14.0f;

    struct FBridgeOrderingContext
    {
        const UBlenderBridgeComponent* Component = nullptr;
        FString ContextLabel;
        TMap<FName, int32> PropertyOrder;
    };

    void InitializeOrderingContextFromComponent(
        FBridgeOrderingContext& Context,
        const UBlenderBridgeComponent* BridgeComponent)
    {
        Context.Component = BridgeComponent;
        Context.ContextLabel = BridgeComponent != nullptr ? BridgeComponent->GetPathName() : TEXT("<null-component>");
        Context.PropertyOrder.Reset();

        if (BridgeComponent == nullptr)
        {
            return;
        }

        Context.PropertyOrder.Reserve(BridgeComponent->InputSchema.Num());
        for (int32 Index = 0; Index < BridgeComponent->InputSchema.Num(); ++Index)
        {
            Context.PropertyOrder.Add(FInstancedPropertyBag::SanitizePropertyName(BridgeComponent->InputSchema[Index].Id), Index);
        }
    }

    void AppendContextCandidate(FBridgeOrderingContext& Context, const UObject* Candidate)
    {
        if (Candidate == nullptr)
        {
            return;
        }

        if (!Context.ContextLabel.IsEmpty())
        {
            Context.ContextLabel.Append(TEXT(", "));
        }

        Context.ContextLabel.Append(FString::Printf(TEXT("%s (%s)"), *Candidate->GetPathName(), *Candidate->GetClass()->GetName()));
    }

    const UBlenderBridgeComponent* ResolveBridgeComponentFromCandidate(
        UObject* CandidateObject,
        const TArray<const void*>& RawData)
    {
        const auto MatchesRawData = [&RawData](const UBlenderBridgeComponent* CandidateComponent)
        {
            return CandidateComponent != nullptr && RawData.Contains(static_cast<const void*>(&CandidateComponent->InputValues));
        };

        if (const UBlenderBridgeComponent* CandidateComponent = Cast<UBlenderBridgeComponent>(CandidateObject))
        {
            if (MatchesRawData(CandidateComponent) || RawData.IsEmpty())
            {
                return CandidateComponent;
            }
        }

        if (AActor* CandidateActor = Cast<AActor>(CandidateObject))
        {
            TArray<UBlenderBridgeComponent*> BridgeComponents;
            CandidateActor->GetComponents(BridgeComponents);

            for (const UBlenderBridgeComponent* BridgeComponent : BridgeComponents)
            {
                if (MatchesRawData(BridgeComponent))
                {
                    return BridgeComponent;
                }
            }

            if (BridgeComponents.Num() == 1 && RawData.IsEmpty())
            {
                return BridgeComponents[0];
            }
        }

        return nullptr;
    }

    FBridgeOrderingContext BuildBridgeOrderingContext(
        const TSharedRef<IPropertyHandle>& StructPropertyHandle,
        IDetailChildrenBuilder& StructBuilder)
    {
        FBridgeOrderingContext Context;
        Context.ContextLabel = FString::Printf(TEXT("Handle=%s"), *FString(StructPropertyHandle->GetPropertyPath()));

        TArray<const void*> RawData;
        StructPropertyHandle->AccessRawData(RawData);

        TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
        StructBuilder.GetParentCategory().GetParentLayout().GetObjectsBeingCustomized(CustomizedObjects);

        for (const TWeakObjectPtr<UObject>& ObjectPtr : CustomizedObjects)
        {
            UObject* CandidateObject = ObjectPtr.Get();
            if (CandidateObject == nullptr)
            {
                continue;
            }

            if (const UBlenderBridgeComponent* BridgeComponent = ResolveBridgeComponentFromCandidate(CandidateObject, RawData))
            {
                InitializeOrderingContextFromComponent(Context, BridgeComponent);
                return Context;
            }

            AppendContextCandidate(Context, CandidateObject);
        }

        TArray<UObject*> OuterObjects;
        StructPropertyHandle->GetOuterObjects(OuterObjects);
        for (UObject* OuterObject : OuterObjects)
        {
            if (const UBlenderBridgeComponent* BridgeComponent = ResolveBridgeComponentFromCandidate(OuterObject, RawData))
            {
                InitializeOrderingContextFromComponent(Context, BridgeComponent);
                return Context;
            }

            AppendContextCandidate(Context, OuterObject);
        }

        return Context;
    }

    bool AreBridgeCategoriesExpanded(const TMap<FString, bool>& ExpansionState, const TArray<FString>& CategoryKeys)
    {
        for (const FString& CategoryKey : CategoryKeys)
        {
            if (!ExpansionState.FindRef(CategoryKey))
            {
                return false;
            }
        }

        return true;
    }

    FString BuildBridgeCategoryString(const TSharedPtr<IPropertyHandle>& PropertyHandle)
    {
        if (!PropertyHandle.IsValid())
        {
            return TEXT("<invalid>");
        }

        const FString CategoryValue = PropertyHandle->GetMetaData(TEXT("Category"));
        return CategoryValue.IsEmpty() ? TEXT("<none>") : CategoryValue;
    }

    class FBlenderBridgeOrderedInstancedStructDetails final : public FInstancedStructDataDetails
    {
    public:
        FBlenderBridgeOrderedInstancedStructDetails(
            const TSharedPtr<IPropertyHandle>& InStructProperty,
            FBridgeOrderingContext InOrderingContext)
            : FInstancedStructDataDetails(InStructProperty)
            , OrderingContext(MoveTemp(InOrderingContext))
        {
        }

    protected:
        virtual void AddChildRows(
            IDetailChildrenBuilder& ChildBuilder,
            const TArray<TSharedPtr<IPropertyHandle>>& ChildProperties) override
        {
            TArray<FOrderedChildProperty> OrderedChildProperties;
            OrderedChildProperties.Reserve(ChildProperties.Num());

            for (int32 Index = 0; Index < ChildProperties.Num(); ++Index)
            {
                const TSharedPtr<IPropertyHandle>& ChildHandle = ChildProperties[Index];
                int32 SchemaOrder = OrderingContext.PropertyOrder.Num() + Index;

                if (ChildHandle.IsValid())
                {
                    if (const FProperty* ChildProperty = ChildHandle->GetProperty())
                    {
                        if (const int32* ExistingOrder = OrderingContext.PropertyOrder.Find(ChildProperty->GetFName()))
                        {
                            SchemaOrder = *ExistingOrder;
                        }
                    }
                }

                FOrderedChildProperty OrderedChild;
                OrderedChild.Handle = ChildHandle;
                OrderedChild.SchemaOrder = SchemaOrder;
                OrderedChild.OriginalOrder = Index;
                OrderedChildProperties.Add(MoveTemp(OrderedChild));
            }

            OrderedChildProperties.Sort([](const FOrderedChildProperty& A, const FOrderedChildProperty& B)
            {
                if (A.SchemaOrder != B.SchemaOrder)
                {
                    return A.SchemaOrder < B.SchemaOrder;
                }

                return A.OriginalOrder < B.OriginalOrder;
            });

            MaybeLogOrderingDebug(ChildProperties, OrderedChildProperties);

            TSet<FString> AddedCategoryHeaders;

            for (const FOrderedChildProperty& OrderedChild : OrderedChildProperties)
            {
                const TSharedPtr<IPropertyHandle>& ChildHandle = OrderedChild.Handle;
                TArray<FString> Categories = GetPropertyCategories(ChildHandle);
                if (Categories.IsEmpty())
                {
                    IDetailPropertyRow& Row = ChildBuilder.AddProperty(ChildHandle.ToSharedRef());
                    OnChildRowAdded(Row);
                    continue;
                }

                TArray<FString> CategoryKeys;
                FString CompleteCategory;

                for (int32 Index = 0; Index < Categories.Num(); ++Index)
                {
                    const FString& Category = Categories[Index];
                    CompleteCategory = CompleteCategory.IsEmpty()
                        ? Category
                        : FString::Printf(TEXT("%s|%s"), *CompleteCategory, *Category);
                    CategoryKeys.Add(CompleteCategory);

                    if (AddedCategoryHeaders.Contains(CompleteCategory))
                    {
                        continue;
                    }

                    AddedCategoryHeaders.Add(CompleteCategory);

                    TArray<FString> ParentCategoryKeys;
                    ParentCategoryKeys.Reserve(Index);
                    for (int32 ParentIndex = 0; ParentIndex < Index; ++ParentIndex)
                    {
                        ParentCategoryKeys.Add(CategoryKeys[ParentIndex]);
                    }

                    FDetailWidgetRow& HeaderRow = ChildBuilder.AddCustomRow(FText::FromString(Category));
                    HeaderRow.Visibility(TAttribute<EVisibility>::CreateLambda(
                        [ExpansionState = ExpansionState, ParentCategoryKeys]()
                        {
                            return AreBridgeCategoriesExpanded(*ExpansionState, ParentCategoryKeys)
                                ? EVisibility::Visible
                                : EVisibility::Collapsed;
                        }));
                    HeaderRow.WholeRowContent()
                    [
                        SNew(SBox)
                        .Padding(FMargin(static_cast<float>(Index) * BlenderBridgeGroupIndentWidth, 0.0f, 0.0f, 0.0f))
                        [
                            SNew(SBorder)
                            .BorderImage(FAppStyle::GetBrush("DetailsView.CategoryTop"))
                            [
                                SNew(SButton)
                                .ButtonStyle(FAppStyle::Get(), "NoBorder")
                                .HAlign(HAlign_Fill)
                                .VAlign(VAlign_Center)
                                .ContentPadding(FMargin(4.0f, 2.0f))
                                .OnClicked_Lambda([ExpansionState = ExpansionState, CompleteCategory]()
                                {
                                    ExpansionState->FindOrAdd(CompleteCategory) = !ExpansionState->FindRef(CompleteCategory);
                                    return FReply::Handled();
                                })
                                [
                                    SNew(SHorizontalBox)
                                    + SHorizontalBox::Slot()
                                    .AutoWidth()
                                    .VAlign(VAlign_Center)
                                    .Padding(0.0f, 0.0f, 4.0f, 0.0f)
                                    [
                                        SNew(SImage)
                                        .Image_Lambda([ExpansionState = ExpansionState, CompleteCategory]()
                                        {
                                            return FAppStyle::GetBrush(
                                                ExpansionState->FindRef(CompleteCategory)
                                                    ? "TreeArrow_Expanded"
                                                    : "TreeArrow_Collapsed");
                                        })
                                    ]
                                    + SHorizontalBox::Slot()
                                    .FillWidth(1.0f)
                                    .VAlign(VAlign_Center)
                                    [
                                        SNew(STextBlock)
                                        .Text(FText::FromString(Category))
                                        .TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
                                    ]
                                ]
                            ]
                        ]
                    ];
                }

                IDetailPropertyRow& Row = ChildBuilder.AddProperty(ChildHandle.ToSharedRef());
                Row.Visibility(TAttribute<EVisibility>::CreateLambda(
                    [ExpansionState = ExpansionState, CategoryKeys]()
                    {
                        return AreBridgeCategoriesExpanded(*ExpansionState, CategoryKeys)
                            ? EVisibility::Visible
                            : EVisibility::Collapsed;
                    }));
                OnChildRowAdded(Row);
            }
        }

    private:
        struct FOrderedChildProperty
        {
            TSharedPtr<IPropertyHandle> Handle;
            int32 SchemaOrder = MAX_int32;
            int32 OriginalOrder = MAX_int32;
        };

        void MaybeLogOrderingDebug(
            const TArray<TSharedPtr<IPropertyHandle>>& ChildProperties,
            const TArray<FOrderedChildProperty>& OrderedChildProperties) const
        {
            static TMap<FString, int32> LastLoggedRequestIds;

            const FString ContextKey = OrderingContext.ContextLabel.IsEmpty() ? TEXT("<unknown-context>") : OrderingContext.ContextLabel;
            const bool bHasResolvedComponent = OrderingContext.Component != nullptr;
            const int32 RequestId = bHasResolvedComponent ? OrderingContext.Component->GetInputOrderingDebugRequestId() : -1;
            const int32 PreviousRequestId = LastLoggedRequestIds.FindRef(ContextKey);
            if ((RequestId > 0 && PreviousRequestId == RequestId) || (RequestId <= 0 && PreviousRequestId == RequestId))
            {
                return;
            }

            LastLoggedRequestIds.Add(ContextKey, RequestId);

            UE_LOG(LogBlenderBridgeEditor, Log, TEXT("---- Blender Bridge Details Ordering Debug [%s] ----"), *ContextKey);
            UE_LOG(
                LogBlenderBridgeEditor,
                Log,
                TEXT("Build: %s"),
                OrderingContext.Component != nullptr ? *OrderingContext.Component->BridgeBuildVersion : TEXT("<unknown>"));
            UE_LOG(LogBlenderBridgeEditor, Log, TEXT("Resolved component: %s"), OrderingContext.Component != nullptr ? TEXT("yes") : TEXT("no"));
            UE_LOG(LogBlenderBridgeEditor, Log, TEXT("Raw child-handle count: %d"), ChildProperties.Num());

            for (int32 Index = 0; Index < ChildProperties.Num(); ++Index)
            {
                const TSharedPtr<IPropertyHandle>& ChildHandle = ChildProperties[Index];
                const FProperty* ChildProperty = ChildHandle.IsValid() ? ChildHandle->GetProperty() : nullptr;
                const int32* ExistingOrder = ChildProperty != nullptr ? OrderingContext.PropertyOrder.Find(ChildProperty->GetFName()) : nullptr;
                UE_LOG(
                    LogBlenderBridgeEditor,
                    Log,
                    TEXT("Raw[%02d] property=%s display=\"%s\" category=\"%s\" schema_order=%s path=%s"),
                    Index,
                    ChildProperty != nullptr ? *ChildProperty->GetFName().ToString() : TEXT("<invalid>"),
                    ChildHandle.IsValid() ? *ChildHandle->GetPropertyDisplayName().ToString() : TEXT("<invalid>"),
                    *BuildBridgeCategoryString(ChildHandle),
                    ExistingOrder != nullptr ? *FString::FromInt(*ExistingOrder) : TEXT("<none>"),
                    ChildHandle.IsValid() ? *FString(ChildHandle->GetPropertyPath()) : TEXT("<invalid>"));
            }

            UE_LOG(LogBlenderBridgeEditor, Log, TEXT("Final sorted child order: %d"), OrderedChildProperties.Num());

            for (int32 Index = 0; Index < OrderedChildProperties.Num(); ++Index)
            {
                const FOrderedChildProperty& OrderedChild = OrderedChildProperties[Index];
                const TSharedPtr<IPropertyHandle>& ChildHandle = OrderedChild.Handle;
                const FProperty* ChildProperty = ChildHandle.IsValid() ? ChildHandle->GetProperty() : nullptr;

                UE_LOG(
                    LogBlenderBridgeEditor,
                    Log,
                    TEXT("Sorted[%02d] property=%s display=\"%s\" category=\"%s\" schema_order=%d original_order=%d"),
                    Index,
                    ChildProperty != nullptr ? *ChildProperty->GetFName().ToString() : TEXT("<invalid>"),
                    ChildHandle.IsValid() ? *ChildHandle->GetPropertyDisplayName().ToString() : TEXT("<invalid>"),
                    *BuildBridgeCategoryString(ChildHandle),
                    OrderedChild.SchemaOrder,
                    OrderedChild.OriginalOrder);
            }
        }

        TSharedRef<TMap<FString, bool>> ExpansionState = MakeShared<TMap<FString, bool>>();
        FBridgeOrderingContext OrderingContext;
    };
}

TSharedRef<IPropertyTypeCustomization> FBlenderBridgePropertyBagDetails::MakeInstance()
{
    return MakeShared<FBlenderBridgePropertyBagDetails>();
}

void FBlenderBridgePropertyBagDetails::CustomizeHeader(
    TSharedRef<IPropertyHandle> StructPropertyHandle,
    FDetailWidgetRow& HeaderRow,
    IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    FPropertyBagDetails::CustomizeHeader(StructPropertyHandle, HeaderRow, StructCustomizationUtils);
}

void FBlenderBridgePropertyBagDetails::CustomizeChildren(
    TSharedRef<IPropertyHandle> StructPropertyHandle,
    IDetailChildrenBuilder& StructBuilder,
    IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    if (!ShouldUseGroupedLayout(StructPropertyHandle))
    {
        FPropertyBagDetails::CustomizeChildren(StructPropertyHandle, StructBuilder, StructCustomizationUtils);
        return;
    }

    const TSharedPtr<IPropertyHandle> ValueProperty = StructPropertyHandle->GetChildHandle(TEXT("Value"), false);
    if (!ValueProperty.IsValid() || !ValueProperty->IsValidHandle())
    {
        FPropertyBagDetails::CustomizeChildren(StructPropertyHandle, StructBuilder, StructCustomizationUtils);
        return;
    }

    const FBridgeOrderingContext OrderingContext = BuildBridgeOrderingContext(StructPropertyHandle, StructBuilder);
    UE_LOG(
        LogBlenderBridgeEditor,
        Log,
        TEXT("CustomizeChildren grouped layout. component_resolved=%s context=\"%s\" property_path=%s"),
        OrderingContext.Component != nullptr ? TEXT("yes") : TEXT("no"),
        *OrderingContext.ContextLabel,
        *FString(StructPropertyHandle->GetPropertyPath()));

    StructBuilder.AddCustomBuilder(MakeShared<FBlenderBridgeOrderedInstancedStructDetails>(
        ValueProperty,
        OrderingContext));
}

bool FBlenderBridgePropertyBagDetails::ShouldUseGroupedLayout(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
{
    const FProperty* MetaDataProperty = StructPropertyHandle->GetMetaDataProperty();
    return MetaDataProperty != nullptr && MetaDataProperty->HasMetaData(BlenderBridgeGroupedLayoutMetaKey);
}
