#include "GeometryNodesBridgePropertyBagBuilder.h"

#include "GeometryNodesBridgeComponent.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "GameFramework/Actor.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "PropertyBagDetails.h"
#include "PropertyHandle.h"
#include "StructUtilsMetadata.h"

namespace
{
    constexpr TCHAR GeometryNodesBridgeGroupedLayoutMetaKey[] = TEXT("GeometryNodesBridgeGroupedLayout");

    struct FBridgeOrderingContext
    {
        TMap<FName, int32> PropertyOrder;
    };

    struct FBridgePropertyBagDisplaySettings
    {
        bool bAllowContainers = true;
        EPropertyBagChildRowFeatures ChildRowFeatures = EPropertyBagChildRowFeatures::Default;
    };

    void InitializeOrderingContextFromComponent(
        FBridgeOrderingContext& Context,
        const UGeometryNodesBridgeComponent* BridgeComponent)
    {
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

    const UGeometryNodesBridgeComponent* ResolveBridgeComponentFromCandidate(
        UObject* CandidateObject,
        const TArray<const void*>& RawData)
    {
        const auto MatchesRawData = [&RawData](const UGeometryNodesBridgeComponent* CandidateComponent)
        {
            return CandidateComponent != nullptr && RawData.Contains(static_cast<const void*>(&CandidateComponent->InputValues));
        };

        if (const UGeometryNodesBridgeComponent* CandidateComponent = Cast<UGeometryNodesBridgeComponent>(CandidateObject))
        {
            if (MatchesRawData(CandidateComponent) || RawData.IsEmpty())
            {
                return CandidateComponent;
            }
        }

        if (AActor* CandidateActor = Cast<AActor>(CandidateObject))
        {
            TArray<UGeometryNodesBridgeComponent*> BridgeComponents;
            CandidateActor->GetComponents(BridgeComponents);

            for (const UGeometryNodesBridgeComponent* BridgeComponent : BridgeComponents)
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
        const TArray<TWeakObjectPtr<UObject>>& CustomizedObjects)
    {
        FBridgeOrderingContext Context;

        TArray<const void*> RawData;
        StructPropertyHandle->AccessRawData(RawData);

        for (const TWeakObjectPtr<UObject>& ObjectPtr : CustomizedObjects)
        {
            UObject* CandidateObject = ObjectPtr.Get();
            if (CandidateObject == nullptr)
            {
                continue;
            }

            if (const UGeometryNodesBridgeComponent* BridgeComponent = ResolveBridgeComponentFromCandidate(CandidateObject, RawData))
            {
                InitializeOrderingContextFromComponent(Context, BridgeComponent);
                return Context;
            }
        }

        TArray<UObject*> OuterObjects;
        StructPropertyHandle->GetOuterObjects(OuterObjects);
        for (UObject* OuterObject : OuterObjects)
        {
            if (const UGeometryNodesBridgeComponent* BridgeComponent = ResolveBridgeComponentFromCandidate(OuterObject, RawData))
            {
                InitializeOrderingContextFromComponent(Context, BridgeComponent);
                return Context;
            }
        }

        return Context;
    }

    FBridgePropertyBagDisplaySettings BuildPropertyBagDisplaySettings(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
    {
        FBridgePropertyBagDisplaySettings Settings;

        const FProperty* MetaDataProperty = StructPropertyHandle->GetMetaDataProperty();
        if (MetaDataProperty == nullptr)
        {
            return Settings;
        }

        const bool bFixedLayout = MetaDataProperty->HasMetaData(UE::StructUtils::Metadata::FixedLayoutName);
        Settings.bAllowContainers = MetaDataProperty->HasMetaData(UE::StructUtils::Metadata::AllowContainersName)
            ? MetaDataProperty->GetBoolMetaData(UE::StructUtils::Metadata::AllowContainersName)
            : true;

        const FString* ChildRowFeaturesString = StructPropertyHandle->GetInstanceMetaData(UE::StructUtils::Metadata::ChildRowFeaturesName);
        if (ChildRowFeaturesString == nullptr)
        {
            ChildRowFeaturesString = MetaDataProperty->FindMetaData(UE::StructUtils::Metadata::ChildRowFeaturesName);
        }

        if (ChildRowFeaturesString != nullptr)
        {
            if (const UEnum* Enum = StaticEnum<EPropertyBagChildRowFeatures>())
            {
                const int32 EnumIndex = Enum->GetIndexByNameString(*ChildRowFeaturesString);
                if (EnumIndex != INDEX_NONE)
                {
                    Settings.ChildRowFeatures = static_cast<EPropertyBagChildRowFeatures>(Enum->GetValueByIndex(EnumIndex));
                }
            }
        }

        if (bFixedLayout)
        {
            Settings.ChildRowFeatures = EPropertyBagChildRowFeatures::Fixed;
        }

        return Settings;
    }

    class FGeometryNodesBridgeOrderedPropertyBagInstanceDetails final : public FPropertyBagInstanceDataDetails
    {
    public:
        FGeometryNodesBridgeOrderedPropertyBagInstanceDetails(
            const FConstructParams& InParams,
            FBridgeOrderingContext InOrderingContext)
            : FPropertyBagInstanceDataDetails(InParams)
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

            TArray<TSharedPtr<IPropertyHandle>> SortedChildProperties;
            SortedChildProperties.Reserve(OrderedChildProperties.Num());
            for (const FOrderedChildProperty& OrderedChild : OrderedChildProperties)
            {
                SortedChildProperties.Add(OrderedChild.Handle);
            }

            TMap<FString, IDetailGroup*> CategoryToGroup;

            for (const TSharedPtr<IPropertyHandle>& ChildHandle : SortedChildProperties)
            {
                TArray<FString> CategoriesToAdd = GetPropertyCategories(ChildHandle);
                if (CategoriesToAdd.IsEmpty())
                {
                    IDetailPropertyRow& Row = ChildBuilder.AddProperty(ChildHandle.ToSharedRef());
                    OnChildRowAdded(Row);
                    continue;
                }

                FString CompleteCategory;
                IDetailGroup* CurrentGroup = nullptr;

                for (int32 CategoryIndex = 0; CategoryIndex < CategoriesToAdd.Num(); ++CategoryIndex)
                {
                    const FString& CategoryToAdd = CategoriesToAdd[CategoryIndex];
                    CompleteCategory = CompleteCategory.IsEmpty()
                        ? CategoryToAdd
                        : FString::Printf(TEXT("%s|%s"), *CompleteCategory, *CategoryToAdd);

                    if (IDetailGroup** ExistingGroup = CategoryToGroup.Find(CompleteCategory))
                    {
                        CurrentGroup = *ExistingGroup;
                        continue;
                    }

                    CurrentGroup = CurrentGroup != nullptr
                        ? &CurrentGroup->AddGroup(FName(CompleteCategory), FText::FromString(CategoryToAdd))
                        : &ChildBuilder.AddGroup(FName(CompleteCategory), FText::FromString(CategoryToAdd));

                    OnGroupRowAdded(*CurrentGroup, CategoryIndex, CategoryToAdd);
                    CategoryToGroup.Add(CompleteCategory, CurrentGroup);
                }

                check(CurrentGroup != nullptr);
                IDetailPropertyRow& Row = CurrentGroup->AddPropertyRow(ChildHandle.ToSharedRef());
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

        FBridgeOrderingContext OrderingContext;
    };

    void AddGroupedPropertyBagBuilder(
        const TSharedRef<IPropertyHandle>& StructPropertyHandle,
        const FBridgeOrderingContext& OrderingContext,
        const TSharedPtr<IPropertyUtilities>& PropertyUtilities,
        TFunctionRef<void(TSharedRef<IDetailCustomNodeBuilder>)> AddBuilder)
    {
        const FBridgePropertyBagDisplaySettings DisplaySettings = BuildPropertyBagDisplaySettings(StructPropertyHandle);
        FPropertyBagInstanceDataDetails::FConstructParams Params
        {
            .BagStructProperty = StructPropertyHandle,
            .PropUtils = PropertyUtilities,
            .bAllowContainers = DisplaySettings.bAllowContainers,
            .ChildRowFeatures = DisplaySettings.ChildRowFeatures
        };

        AddBuilder(MakeShared<FGeometryNodesBridgeOrderedPropertyBagInstanceDetails>(Params, OrderingContext));
    }
}

bool GeometryNodesBridge::PropertyBagBuilder::ShouldUseGroupedLayout(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
{
    const FProperty* MetaDataProperty = StructPropertyHandle->GetMetaDataProperty();
    return MetaDataProperty != nullptr && MetaDataProperty->HasMetaData(GeometryNodesBridgeGroupedLayoutMetaKey);
}

void GeometryNodesBridge::PropertyBagBuilder::AddGroupedPropertyBagToChildren(
    const TSharedRef<IPropertyHandle>& StructPropertyHandle,
    IDetailChildrenBuilder& StructBuilder,
    const TSharedPtr<IPropertyUtilities>& PropertyUtilities)
{
    TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
    StructBuilder.GetParentCategory().GetParentLayout().GetObjectsBeingCustomized(CustomizedObjects);

    const FBridgeOrderingContext OrderingContext = BuildBridgeOrderingContext(StructPropertyHandle, CustomizedObjects);
    AddGroupedPropertyBagBuilder(
        StructPropertyHandle,
        OrderingContext,
        PropertyUtilities,
        [&StructBuilder](TSharedRef<IDetailCustomNodeBuilder> Builder)
        {
            StructBuilder.AddCustomBuilder(Builder);
        });
}

void GeometryNodesBridge::PropertyBagBuilder::AddGroupedPropertyBagToCategory(
    const TSharedRef<IPropertyHandle>& StructPropertyHandle,
    IDetailCategoryBuilder& CategoryBuilder,
    const TArray<TWeakObjectPtr<UObject>>& CustomizedObjects,
    const TSharedPtr<IPropertyUtilities>& PropertyUtilities)
{
    const FBridgeOrderingContext OrderingContext = BuildBridgeOrderingContext(StructPropertyHandle, CustomizedObjects);
    AddGroupedPropertyBagBuilder(
        StructPropertyHandle,
        OrderingContext,
        PropertyUtilities,
        [&CategoryBuilder](TSharedRef<IDetailCustomNodeBuilder> Builder)
        {
            CategoryBuilder.AddCustomBuilder(Builder);
        });
}
