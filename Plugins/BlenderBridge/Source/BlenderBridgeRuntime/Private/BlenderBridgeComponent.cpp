#include "BlenderBridgeComponent.h"

#include "BlenderBridgeActor.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSessionManager.h"
#include "BlenderBridgeSource.h"
#include "BlenderBridgeMessages.h"
#include "BlenderBridgeMoveProbeComponent.h"
#include "Components/SplineComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/UserDefinedEnum.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"
#include "Modules/ModuleManager.h"
#include "ProceduralMeshComponent.h"
#include "StaticMeshResources.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"

#ifndef BLENDER_BRIDGE_BUILD_VERSION
#define BLENDER_BRIDGE_BUILD_VERSION TEXT("Unknown build")
#endif

namespace
{
    constexpr TCHAR BridgeEnableCategoriesMetaKey[] = TEXT("EnableCategories");
    constexpr TCHAR BridgeCategoryMetaKey[] = TEXT("Category");
    constexpr TCHAR BridgeToolTipMetaKey[] = TEXT("ToolTip");

    const TCHAR* LexToString(const EBlenderBridgeInputType Type)
    {
        switch (Type)
        {
        case EBlenderBridgeInputType::Float:
            return TEXT("Float");
        case EBlenderBridgeInputType::Integer:
            return TEXT("Integer");
        case EBlenderBridgeInputType::Boolean:
            return TEXT("Boolean");
        case EBlenderBridgeInputType::Vector3:
            return TEXT("Vector3");
        case EBlenderBridgeInputType::Color:
            return TEXT("Color");
        case EBlenderBridgeInputType::String:
            return TEXT("String");
        case EBlenderBridgeInputType::Enum:
            return TEXT("Enum");
        case EBlenderBridgeInputType::Unsupported:
        default:
            return TEXT("Unsupported");
        }
    }

    const TCHAR* LexToString(const EBlenderBridgeGeometrySourceKind SourceKind)
    {
        switch (SourceKind)
        {
        case EBlenderBridgeGeometrySourceKind::StaticMeshComponent:
            return TEXT("StaticMeshComponent");
        case EBlenderBridgeGeometrySourceKind::SplineComponent:
            return TEXT("SplineComponent");
        case EBlenderBridgeGeometrySourceKind::EmbeddedSpline:
            return TEXT("EmbeddedSpline");
        case EBlenderBridgeGeometrySourceKind::None:
        default:
            return TEXT("None");
        }
    }

    ESplinePointType::Type ToSplinePointType(const EBlenderBridgeSplinePointType Type)
    {
        switch (Type)
        {
        case EBlenderBridgeSplinePointType::Linear:
            return ESplinePointType::Linear;
        case EBlenderBridgeSplinePointType::Constant:
            return ESplinePointType::Constant;
        case EBlenderBridgeSplinePointType::CurveClamped:
            return ESplinePointType::CurveClamped;
        case EBlenderBridgeSplinePointType::CurveCustomTangent:
            return ESplinePointType::CurveCustomTangent;
        case EBlenderBridgeSplinePointType::Curve:
        default:
            return ESplinePointType::Curve;
        }
    }

    EBlenderBridgeSplinePointType ToBridgeSplinePointType(const ESplinePointType::Type Type)
    {
        switch (Type)
        {
        case ESplinePointType::Linear:
            return EBlenderBridgeSplinePointType::Linear;
        case ESplinePointType::Constant:
            return EBlenderBridgeSplinePointType::Constant;
        case ESplinePointType::CurveClamped:
            return EBlenderBridgeSplinePointType::CurveClamped;
        case ESplinePointType::CurveCustomTangent:
            return EBlenderBridgeSplinePointType::CurveCustomTangent;
        case ESplinePointType::Curve:
        default:
            return EBlenderBridgeSplinePointType::Curve;
        }
    }

    FString MakeCurveOwnerHash(const FBridgeCurveOwnerData& CurveOwnerData)
    {
        FString SerializedCurve;
        if (!BlenderBridge::SerializeCurveOwnerData(CurveOwnerData, SerializedCurve))
        {
            return FString();
        }

        return FMD5::HashAnsiString(*SerializedCurve);
    }

    bool IsGeometrySourceKindAllowed(
        const FBridgeGeometryInputDescriptor& Descriptor,
        const EBlenderBridgeGeometrySourceKind SourceKind)
    {
        return Descriptor.AllowedSourceKinds.Contains(SourceKind);
    }

    bool SupportsComponentReferenceBinding(const FBridgeGeometryInputDescriptor& Descriptor)
    {
        return Descriptor.AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::StaticMeshComponent)
            || Descriptor.AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::SplineComponent);
    }

    template <typename ComponentType>
    ComponentType* FindFirstComponentOnActor(AActor* Actor)
    {
        if (Actor == nullptr)
        {
            return nullptr;
        }

        if (ComponentType* RootComponent = Cast<ComponentType>(Actor->GetRootComponent()))
        {
            return RootComponent;
        }

        TInlineComponentArray<ComponentType*> MatchingComponents(Actor);
        for (ComponentType* MatchingComponent : MatchingComponents)
        {
            if (MatchingComponent != nullptr)
            {
                return MatchingComponent;
            }
        }

        return nullptr;
    }

    template <typename ComponentType>
    ComponentType* ResolveReferencedComponent(AActor* OwningActor, const FComponentReference& Reference)
    {
        AActor* SearchActor = Reference.OtherActor.Get();
        if (SearchActor == nullptr)
        {
            return nullptr;
        }

        if (ComponentType* DirectComponent = Cast<ComponentType>(Reference.GetComponent(OwningActor)))
        {
            return DirectComponent;
        }

        return FindFirstComponentOnActor<ComponentType>(SearchActor);
    }

    USceneComponent* ResolveReferencedSceneComponentForDescriptor(
        AActor* OwningActor,
        const FComponentReference& Reference,
        const FBridgeGeometryInputDescriptor& Descriptor)
    {
        if (Descriptor.AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::SplineComponent))
        {
            if (USplineComponent* SplineComponent = ResolveReferencedComponent<USplineComponent>(OwningActor, Reference))
            {
                return SplineComponent;
            }
        }

        if (Descriptor.AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::StaticMeshComponent))
        {
            if (UStaticMeshComponent* StaticMeshComponent = ResolveReferencedComponent<UStaticMeshComponent>(OwningActor, Reference))
            {
                return StaticMeshComponent;
            }
        }

        return nullptr;
    }

    void NormalizeComponentReferenceForDescriptor(
        FComponentReference& Reference,
        AActor* OwningActor,
        const FBridgeGeometryInputDescriptor& Descriptor)
    {
        Reference.ComponentProperty = NAME_None;

        if (!Reference.OtherActor.IsValid())
        {
            Reference.PathToComponent.Reset();
            return;
        }

        if (USceneComponent* SceneComponent = ResolveReferencedSceneComponentForDescriptor(OwningActor, Reference, Descriptor))
        {
            Reference.PathToComponent = SceneComponent->GetPathName(Reference.OtherActor.Get());
            return;
        }

        Reference.PathToComponent.Reset();
    }

    FString DescribeSceneComponentForLog(const USceneComponent* SceneComponent)
    {
        if (SceneComponent == nullptr)
        {
            return TEXT("None");
        }

        const FTransform Transform = SceneComponent->GetComponentTransform();
        return FString::Printf(
            TEXT("%s owner=%s loc=%s rot=%s scale=%s mobility=%d"),
            *GetPathNameSafe(SceneComponent),
            *GetPathNameSafe(SceneComponent->GetOwner()),
            *Transform.GetLocation().ToCompactString(),
            *Transform.Rotator().ToCompactString(),
            *Transform.GetScale3D().ToCompactString(),
            static_cast<int32>(SceneComponent->Mobility));
    }

    FName GetBridgeInputBagPropertyName(const FBridgeInputSchema& Entry)
    {
        return FInstancedPropertyBag::SanitizePropertyName(Entry.Id);
    }

    bool IsBridgeEnumValueRepresentable(const int32 Value)
    {
        return Value >= 0 && Value <= MAX_uint8;
    }

    bool HasBridgeEnumValue(const FBridgeInputSchema& Entry, const int32 Value)
    {
        for (const FBridgeEnumOption& Option : Entry.EnumOptions)
        {
            if (Option.Value == Value)
            {
                return true;
            }
        }

        return false;
    }

    int32 GetPreferredBridgeEnumValue(const FBridgeInputSchema& Entry)
    {
        if (IsBridgeEnumValueRepresentable(Entry.DefaultEnumValue) && HasBridgeEnumValue(Entry, Entry.DefaultEnumValue))
        {
            return Entry.DefaultEnumValue;
        }

        for (const FBridgeEnumOption& Option : Entry.EnumOptions)
        {
            if (IsBridgeEnumValueRepresentable(Option.Value))
            {
                return Option.Value;
            }
        }

        return 0;
    }

    uint32 ComputeBridgeEnumSignature(const FBridgeInputSchema& Entry)
    {
        uint32 Hash = GetTypeHash(Entry.Id);
        Hash = HashCombine(Hash, GetTypeHash(Entry.DefaultEnumValue));

        for (const FBridgeEnumOption& Option : Entry.EnumOptions)
        {
            Hash = HashCombine(Hash, GetTypeHash(Option.Id));
            Hash = HashCombine(Hash, GetTypeHash(Option.Label));
            Hash = HashCombine(Hash, GetTypeHash(Option.Value));
        }

        return Hash;
    }

    FString MakeBridgeEnumObjectBaseName(const FBridgeInputSchema& Entry)
    {
        const FString BagPropertyName = GetBridgeInputBagPropertyName(Entry).ToString();
        return BagPropertyName.IsEmpty() ? TEXT("BridgeInputEnum") : FString::Printf(TEXT("BridgeInputEnum_%s"), *BagPropertyName);
    }

    FString ComputeBridgeEnumObjectKey(const FBridgeInputSchema& Entry)
    {
        FString Serialized = Entry.Id.ToString();
        Serialized += TEXT("|");
        Serialized += FString::FromInt(Entry.DefaultEnumValue);

        for (const FBridgeEnumOption& Option : Entry.EnumOptions)
        {
            Serialized += TEXT("|");
            Serialized += Option.Id;
            Serialized += TEXT("|");
            Serialized += Option.Label;
            Serialized += TEXT("|");
            Serialized += FString::FromInt(Option.Value);
        }

        return FMD5::HashAnsiString(*Serialized);
    }

    FString MakeBridgeEnumObjectName(const FBridgeInputSchema& Entry)
    {
        return FString::Printf(TEXT("%s_%s"), *MakeBridgeEnumObjectBaseName(Entry), *ComputeBridgeEnumObjectKey(Entry));
    }

    FString MakeBridgeEnumEntryShortName(const FBridgeEnumOption& Option, const int32 Index)
    {
        FString CandidateName = !Option.Id.IsEmpty() ? Option.Id : Option.Label;
        if (CandidateName.IsEmpty())
        {
            CandidateName = FString::Printf(TEXT("Option_%d"), Index);
        }

        FName SanitizedName = FInstancedPropertyBag::SanitizePropertyName(CandidateName);
        if (SanitizedName.IsNone())
        {
            SanitizedName = FName(*FString::Printf(TEXT("Option_%d"), Index));
        }

        return SanitizedName.ToString();
    }

    int32 GetStoredBridgeEnumValue(const FInstancedPropertyBag& Bag, const FName PropertyName)
    {
        if (const FPropertyBagPropertyDesc* Desc = Bag.FindPropertyDescByName(PropertyName))
        {
            if (const UEnum* EnumDefinition = Cast<UEnum>(Desc->ValueTypeObject))
            {
                if (const auto EnumValue = Bag.GetValueEnum(PropertyName, EnumDefinition); EnumValue.IsValid())
                {
                    return static_cast<int32>(EnumValue.GetValue());
                }
            }
        }

        if (const auto IntegerValue = Bag.GetValueInt32(PropertyName); IntegerValue.IsValid())
        {
            return IntegerValue.GetValue();
        }

        return INDEX_NONE;
    }
}

UBlenderBridgeComponent::UBlenderBridgeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bTickInEditor = false;
    BridgeBuildVersion = BLENDER_BRIDGE_BUILD_VERSION;
}

void UBlenderBridgeComponent::AssignSourceFromActorFactory(UBlenderBridgeSource* InSource)
{
    const bool bPreviousSuppressPostEditNotifications = bSuppressPostEditNotifications;
    bSuppressPostEditNotifications = true;
    Source = InSource;
    bSuppressPostEditNotifications = bPreviousSuppressPostEditNotifications;
}

void UBlenderBridgeComponent::SetAutomationRequestContext(
    const FString& InRunLabel,
    const FString& InScenarioId,
    const int32 InIterationIndex,
    const FString& InActionKind,
    const bool bInBypassCookCache,
    const bool bInUseAutomationScratchBakeRoot)
{
    AutomationRunLabel = InRunLabel;
    AutomationScenarioId = InScenarioId;
    AutomationIterationIndex = InIterationIndex;
    AutomationActionKind = InActionKind;
    bAutomationBypassCookCache = bInBypassCookCache;
    bUseAutomationScratchBakeRoot = bInUseAutomationScratchBakeRoot;
}

void UBlenderBridgeComponent::ClearAutomationRequestContext()
{
    AutomationRunLabel.Reset();
    AutomationScenarioId.Reset();
    AutomationActionKind.Reset();
    AutomationIterationIndex = INDEX_NONE;
    bAutomationBypassCookCache = false;
    bUseAutomationScratchBakeRoot = false;
}

bool UBlenderBridgeComponent::HasAutomationTimingContext() const
{
    return !AutomationRunLabel.IsEmpty()
        && !AutomationScenarioId.IsEmpty()
        && !AutomationActionKind.IsEmpty()
        && AutomationIterationIndex >= 0;
}

bool UBlenderBridgeComponent::ShouldBypassCookCacheForAutomation() const
{
    return bAutomationBypassCookCache;
}

bool UBlenderBridgeComponent::ShouldUseAutomationScratchBakeRoot() const
{
    return bUseAutomationScratchBakeRoot;
}

void UBlenderBridgeComponent::OnRegister()
{
    Super::OnRegister();

    RefreshComponentHandle();
    BindSplineComponentDelegates();

    if (FModuleManager::Get().IsModuleLoaded(TEXT("BlenderBridgeRuntime")))
    {
        ConnectionState = FBlenderBridgeRuntimeModule::Get().GetSessionManager().GetConnectionState();
    }

    UpdatePreviewVisibility();
    UpdateBakedOutputVisibility();
}

void UBlenderBridgeComponent::RefreshInputSchema()
{
    FBlenderBridgeRuntimeModule::Get().GetSessionManager().RefreshComponentSchema(this, true);
}

void UBlenderBridgeComponent::RequestCook()
{
    SetShowingBakedOutput(false);
    FBlenderBridgeRuntimeModule::Get().GetSessionManager().RequestCook(this);
}

void UBlenderBridgeComponent::BakeToStaticMesh()
{
    if (!GIsEditor)
    {
        SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Baking is only supported in the Unreal Editor."));
        return;
    }

    if (BakeOutputMode == EBlenderBridgeBakeOutputMode::InstancedHierarchy)
    {
        bPendingInstancedBake = true;
        SetShowingBakedOutput(false);
        FBlenderBridgeRuntimeModule::Get().GetSessionManager().RequestInstanceBake(this);
        return;
    }

    if (!HasCookedMesh())
    {
        bPendingBakeAfterCook = true;
        SetBridgeStatus(EBlenderBridgeComponentStatus::CookQueued, TEXT("No cooked mesh is available yet. Requesting a cook before baking."));
        RequestCook();
        return;
    }

    if (FBlenderBridgeRuntimeModule::Get().ExecuteBakeBridgeComponent(this))
    {
        bPendingBakeAfterCook = false;
        return;
    }

    if (ComponentStatus != EBlenderBridgeComponentStatus::Error)
    {
        SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to bake the bridge output to a static mesh."));
    }
}

void UBlenderBridgeComponent::SelectBakedOutput()
{
    if (!GIsEditor)
    {
        SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Selecting baked outputs is only supported in the Unreal Editor."));
        return;
    }

    if (!FBlenderBridgeRuntimeModule::Get().ExecuteSelectBakedOutput(this))
    {
        SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("There is no baked bridge output to select."));
    }
}

void UBlenderBridgeComponent::DeleteBakedOutput()
{
    if (!GIsEditor)
    {
        SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Deleting baked outputs is only supported in the Unreal Editor."));
        return;
    }

    if (!FBlenderBridgeRuntimeModule::Get().ExecuteDeleteBakedOutput(this))
    {
        SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("There is no baked bridge output to delete."));
    }
}

TArray<FString> UBlenderBridgeComponent::GetAvailableTargetOptions() const
{
    TArray<FString> Options;
    Options.Reserve(AvailableTargets.Num() + (TargetId.IsEmpty() ? 0 : 1));

    for (const FBridgeTargetDescriptor& Target : AvailableTargets)
    {
        Options.Add(Target.TargetId);
    }

    if (!TargetId.IsEmpty() && !Options.Contains(TargetId))
    {
        Options.Add(TargetId);
    }

    return Options;
}

const FBridgeGeometryInputDescriptor* UBlenderBridgeComponent::FindGeometryInputDescriptor(const FString& InInputId) const
{
    return GeometryInputDescriptors.FindByPredicate([&InInputId](const FBridgeGeometryInputDescriptor& Descriptor)
    {
        return Descriptor.InputId == InInputId;
    });
}

EBlenderBridgeGeometrySourceKind UBlenderBridgeComponent::GetDefaultGeometryBindingSourceKind(
    const FBridgeGeometryInputDescriptor& Descriptor) const
{
    if (Descriptor.AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::EmbeddedSpline))
    {
        return EBlenderBridgeGeometrySourceKind::EmbeddedSpline;
    }

    return EBlenderBridgeGeometrySourceKind::None;
}

EBlenderBridgeGeometrySourceKind UBlenderBridgeComponent::GetEffectiveGeometryBindingSourceKind(const FString& InputId) const
{
    const FBridgeGeometryInputDescriptor* Descriptor = FindGeometryInputDescriptor(InputId);
    if (Descriptor == nullptr)
    {
        return EBlenderBridgeGeometrySourceKind::None;
    }

    if (Descriptor->AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::StaticMeshComponent)
        && ResolveBoundStaticMeshComponent(InputId) != nullptr)
    {
        return EBlenderBridgeGeometrySourceKind::StaticMeshComponent;
    }

    if (Descriptor->AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::SplineComponent)
        && ResolveBoundSplineComponent(InputId) != nullptr)
    {
        return EBlenderBridgeGeometrySourceKind::SplineComponent;
    }

    const FBlenderBridgeGeometryBinding* ExistingBinding = GeometryBindings.FindByPredicate([&InputId](const FBlenderBridgeGeometryBinding& Binding)
    {
        return Binding.InputId == InputId;
    });

    if (ExistingBinding != nullptr && IsGeometrySourceKindAllowed(*Descriptor, ExistingBinding->SourceKind))
    {
        return ExistingBinding->SourceKind;
    }

    return GetDefaultGeometryBindingSourceKind(*Descriptor);
}

void UBlenderBridgeComponent::RebuildGeometryBindings(const bool bPreserveExistingValues)
{
    TMap<FString, FBlenderBridgeGeometryBinding> ExistingBindings;
    if (bPreserveExistingValues)
    {
        for (const FBlenderBridgeGeometryBinding& ExistingBinding : GeometryBindings)
        {
            if (!ExistingBinding.InputId.IsEmpty())
            {
                ExistingBindings.Add(ExistingBinding.InputId, ExistingBinding);
            }
        }
    }

    TArray<FBlenderBridgeGeometryBinding> NewBindings;
    NewBindings.Reserve(GeometryInputDescriptors.Num());
    for (const FBridgeGeometryInputDescriptor& Descriptor : GeometryInputDescriptors)
    {
        FBlenderBridgeGeometryBinding Binding;
        Binding.InputId = Descriptor.InputId;
        Binding.SourceKind = GetDefaultGeometryBindingSourceKind(Descriptor);

        if (const FBlenderBridgeGeometryBinding* ExistingBinding = ExistingBindings.Find(Descriptor.InputId))
        {
            Binding = *ExistingBinding;
            Binding.InputId = Descriptor.InputId;
            if (!IsGeometrySourceKindAllowed(Descriptor, Binding.SourceKind))
            {
                Binding.SourceKind = GetDefaultGeometryBindingSourceKind(Descriptor);
            }
        }

        if (!SupportsComponentReferenceBinding(Descriptor))
        {
            Binding.ComponentReference = FComponentReference();
        }

        NewBindings.Add(MoveTemp(Binding));
    }

    const bool bPreviousSuppressPostEditNotifications = bSuppressPostEditNotifications;
    bSuppressPostEditNotifications = true;
    GeometryBindings = MoveTemp(NewBindings);
    bSuppressPostEditNotifications = bPreviousSuppressPostEditNotifications;
}

void UBlenderBridgeComponent::NormalizeGeometryBindings()
{
    AActor* OwnerActor = GetOwner();
    for (FBlenderBridgeGeometryBinding& Binding : GeometryBindings)
    {
        const FBridgeGeometryInputDescriptor* Descriptor = FindGeometryInputDescriptor(Binding.InputId);
        if (Descriptor == nullptr)
        {
            Binding.SourceKind = EBlenderBridgeGeometrySourceKind::None;
            Binding.ComponentReference = FComponentReference();
            continue;
        }

        if (!IsGeometrySourceKindAllowed(*Descriptor, Binding.SourceKind))
        {
            Binding.SourceKind = GetDefaultGeometryBindingSourceKind(*Descriptor);
        }

        if (!SupportsComponentReferenceBinding(*Descriptor))
        {
            Binding.ComponentReference = FComponentReference();
            continue;
        }

        NormalizeComponentReferenceForDescriptor(Binding.ComponentReference, OwnerActor, *Descriptor);
    }
}

void UBlenderBridgeComponent::OnComponentDestroyed(const bool bDestroyingHierarchy)
{
    CancelPendingSplineCook();
    UnbindSplineComponentDelegates();
    if (FModuleManager::Get().IsModuleLoaded(TEXT("BlenderBridgeRuntime")))
    {
        FBlenderBridgeRuntimeModule::Get().GetSessionManager().ReleaseComponentState(this);
    }
    CancelPendingBakedOutputVisibilitySwap();
    ResetBridgeData(false);
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

#if WITH_EDITOR
void UBlenderBridgeComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (bSuppressPostEditNotifications)
    {
        return;
    }

    const FName ChangedPropertyName = PropertyChangedEvent.GetMemberPropertyName();
    FBlenderBridgeSessionManager& SessionManager = FBlenderBridgeRuntimeModule::Get().GetSessionManager();

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, Source))
    {
        SessionManager.NotifySourceChanged(this);
        return;
    }

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, TargetId))
    {
        SessionManager.NotifyTargetChanged(this);
        return;
    }

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, MaterialOverrides))
    {
        ApplyPreviewMaterials();
        return;
    }

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, GeometryBindings))
    {
        NormalizeGeometryBindings();
        RefreshObservedSplineComponents();
        FBlenderBridgeRuntimeModule::Get().GetSessionManager().ReleaseComponentState(this);
        ApplyCurveOwnerDescription(TargetId, TargetCapabilities, true);
        if (ConnectionState == EBlenderBridgeConnectionState::Connected)
        {
            SessionManager.RequestCook(this);
        }
        return;
    }

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, bHidePreviewAfterBake))
    {
        UpdatePreviewVisibility();
        return;
    }

    if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UBlenderBridgeComponent, InputValues)
        && ConnectionState == EBlenderBridgeConnectionState::Connected)
    {
        SessionManager.RequestCook(this);
    }
}
#endif

void UBlenderBridgeComponent::EnsurePreviewMeshComponent()
{
    if (PreviewMeshComponent != nullptr)
    {
        return;
    }

    AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return;
    }

    PreviewMeshComponent = NewObject<UProceduralMeshComponent>(Owner, TEXT("BlenderBridgePreviewMesh"));
    Owner->AddInstanceComponent(PreviewMeshComponent);
    PreviewMeshComponent->SetupAttachment(this);
    PreviewMeshComponent->SetMobility(EComponentMobility::Movable);
    PreviewMeshComponent->SetCanEverAffectNavigation(false);
    PreviewMeshComponent->RegisterComponent();
    UpdatePreviewVisibility();
}

void UBlenderBridgeComponent::RefreshComponentHandle()
{
    ComponentHandle = GetPathNameSafe(this);
}

void UBlenderBridgeComponent::BindSplineComponentDelegates()
{
    RefreshObservedSplineComponents();
}

void UBlenderBridgeComponent::UnbindSplineComponentDelegates()
{
    for (const TWeakObjectPtr<USplineComponent>& ObservedSplineComponent : ObservedSplineComponents)
    {
        if (USplineComponent* SplineComponent = ObservedSplineComponent.Get())
        {
            SplineComponent->GetOnSplineChanged().RemoveAll(this);
            SplineComponent->GetOnSplineUpdated().RemoveAll(this);
        }
    }

    ObservedSplineComponents.Reset();
}

void UBlenderBridgeComponent::ResetObservedGeometryTracking()
{
    ReleaseObservedMoveProbes();
    ObservedGeometryComponents.Reset();
    ObservedGeometryDebugSummary = TEXT("No observed reference actors.");
    LastObservedGeometryEvent.Reset();
}

USplineComponent* UBlenderBridgeComponent::GetBridgeSplineComponent() const
{
    if (const ABlenderBridgeActor* BridgeActor = Cast<ABlenderBridgeActor>(GetOwner()))
    {
        if (USplineComponent* BridgeSplineComponent = BridgeActor->GetBridgeSplineComponent())
        {
            return BridgeSplineComponent;
        }
    }

    if (AActor* Owner = GetOwner())
    {
        TInlineComponentArray<USplineComponent*> SplineComponents(Owner);
        for (USplineComponent* SplineComponent : SplineComponents)
        {
            if (SplineComponent != nullptr && SplineComponent->GetFName() == TEXT("BridgeSplineComponent"))
            {
                return SplineComponent;
            }
        }

        return Owner->FindComponentByClass<USplineComponent>();
    }

    return nullptr;
}

USplineComponent* UBlenderBridgeComponent::EnsureBridgeSplineComponent()
{
    if (ABlenderBridgeActor* BridgeActor = Cast<ABlenderBridgeActor>(GetOwner()))
    {
        USplineComponent* SplineComponent = BridgeActor->EnsureBridgeSplineComponent();
        if (SplineComponent != nullptr)
        {
            BindSplineComponentDelegates();
        }
        return SplineComponent;
    }

    if (USplineComponent* ExistingSplineComponent = GetBridgeSplineComponent())
    {
        return ExistingSplineComponent;
    }

    AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return nullptr;
    }

    USplineComponent* SplineComponent = NewObject<USplineComponent>(Owner, TEXT("BridgeSplineComponent"), RF_Transactional);
    if (SplineComponent == nullptr)
    {
        return nullptr;
    }

    Owner->AddInstanceComponent(SplineComponent);
    SplineComponent->SetupAttachment(this);
    SplineComponent->SetMobility(EComponentMobility::Movable);
    SplineComponent->SetUsingAbsoluteLocation(false);
    SplineComponent->SetUsingAbsoluteRotation(false);
    SplineComponent->SetUsingAbsoluteScale(false);
    SplineComponent->ComponentTags.AddUnique(TEXT("BlenderBridgeOwnedSpline"));
    SplineComponent->RegisterComponent();
    BindSplineComponentDelegates();
    return SplineComponent;
}

void UBlenderBridgeComponent::RemoveOwnedBridgeSplineComponent()
{
    UnbindSplineComponentDelegates();

    if (ABlenderBridgeActor* BridgeActor = Cast<ABlenderBridgeActor>(GetOwner()))
    {
        BridgeActor->RemoveBridgeSplineComponent();
        return;
    }

    AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return;
    }

    TInlineComponentArray<USplineComponent*> SplineComponents(Owner);
    for (USplineComponent* SplineComponent : SplineComponents)
    {
        if (SplineComponent == nullptr || !SplineComponent->ComponentTags.Contains(TEXT("BlenderBridgeOwnedSpline")))
        {
            continue;
        }

        Owner->RemoveInstanceComponent(SplineComponent);
        SplineComponent->DestroyComponent();
        break;
    }
}

void UBlenderBridgeComponent::HandleBridgeSplineChanged()
{
    if (bSuppressSplineNotifications || !TargetCapabilities.bSupportsCurveOwner)
    {
        return;
    }

    bSplineEditedSinceImport = true;
    QueueCookFromSplineEdit();
}

USplineComponent* UBlenderBridgeComponent::ResolveBoundSplineComponent(const FString& InputId) const
{
    const FBlenderBridgeGeometryBinding* Binding = GeometryBindings.FindByPredicate([&InputId](const FBlenderBridgeGeometryBinding& Entry)
    {
        return Entry.InputId == InputId;
    });

    if (Binding == nullptr)
    {
        return nullptr;
    }

    return ResolveReferencedComponent<USplineComponent>(GetOwner(), Binding->ComponentReference);
}

UStaticMeshComponent* UBlenderBridgeComponent::ResolveBoundStaticMeshComponent(const FString& InputId) const
{
    const FBlenderBridgeGeometryBinding* Binding = GeometryBindings.FindByPredicate([&InputId](const FBlenderBridgeGeometryBinding& Entry)
    {
        return Entry.InputId == InputId;
    });

    if (Binding == nullptr)
    {
        return nullptr;
    }

    return ResolveReferencedComponent<UStaticMeshComponent>(GetOwner(), Binding->ComponentReference);
}

USplineComponent* UBlenderBridgeComponent::GetActiveCurveSourceSplineComponent() const
{
    if (!TargetCapabilities.bSupportsCurveOwner)
    {
        return nullptr;
    }

    for (const FBridgeGeometryInputDescriptor& Descriptor : GeometryInputDescriptors)
    {
        if (!Descriptor.AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::EmbeddedSpline)
            && !Descriptor.AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::SplineComponent))
        {
            continue;
        }

        const EBlenderBridgeGeometrySourceKind SourceKind = GetEffectiveGeometryBindingSourceKind(Descriptor.InputId);
        if (SourceKind == EBlenderBridgeGeometrySourceKind::SplineComponent)
        {
            return ResolveBoundSplineComponent(Descriptor.InputId);
        }

        return GetBridgeSplineComponent();
    }

    return GetBridgeSplineComponent();
}

void UBlenderBridgeComponent::RefreshObservedSplineComponents()
{
    const FString PreviousObservedGeometryDebugSummary = ObservedGeometryDebugSummary;

    UnbindSplineComponentDelegates();
    ResetObservedGeometryTracking();

    if (!TargetCapabilities.bSupportsCurveOwner)
    {
        // Static-mesh-backed owner geometry still needs transform tracking.
    }
    else if (USplineComponent* ActiveSplineComponent = GetActiveCurveSourceSplineComponent())
    {
        ActiveSplineComponent->GetOnSplineChanged().AddUObject(this, &UBlenderBridgeComponent::HandleBridgeSplineChanged);
        ActiveSplineComponent->GetOnSplineUpdated().AddUObject(this, &UBlenderBridgeComponent::HandleBridgeSplineChanged);
        ObservedSplineComponents.Add(ActiveSplineComponent);
    }

    auto AddObservedGeometryComponent = [this](USceneComponent* SceneComponent)
    {
        if (SceneComponent == nullptr)
        {
            return;
        }

        const bool bAlreadyObserved = ObservedGeometryComponents.ContainsByPredicate([SceneComponent](const TWeakObjectPtr<USceneComponent>& ObservedComponent)
        {
            return ObservedComponent.Get() == SceneComponent;
        });

        if (bAlreadyObserved)
        {
            return;
        }

        ObservedGeometryComponents.Add(SceneComponent);
        UE_LOG(
            LogBlenderBridge,
            Verbose,
            TEXT("Observed reference component registered for bridge '%s': %s"),
            *GetPathNameSafe(this),
            *DescribeSceneComponentForLog(SceneComponent));

        if (UBlenderBridgeMoveProbeComponent* MoveProbe = FindOrCreateObservedMoveProbe(SceneComponent))
        {
            MoveProbe->AddWatchingBridge(this);
            ObservedMoveProbes.Add(MoveProbe);
        }
        else
        {
            UE_LOG(
                LogBlenderBridge,
                Warning,
                TEXT("Observed reference move probe could not be created for bridge '%s' component '%s'."),
                *GetPathNameSafe(this),
                *GetPathNameSafe(SceneComponent));
        }
    };

    for (const FBlenderBridgeGeometryBinding& Binding : GeometryBindings)
    {
        const EBlenderBridgeGeometrySourceKind SourceKind = GetEffectiveGeometryBindingSourceKind(Binding.InputId);
        if (SourceKind == EBlenderBridgeGeometrySourceKind::StaticMeshComponent)
        {
            AddObservedGeometryComponent(ResolveBoundStaticMeshComponent(Binding.InputId));
        }
        else if (SourceKind == EBlenderBridgeGeometrySourceKind::SplineComponent)
        {
            AddObservedGeometryComponent(ResolveBoundSplineComponent(Binding.InputId));
        }
    }

    ObservedGeometryDebugSummary = BuildObservedGeometryDebugSummary();
    if (!ObservedGeometryDebugSummary.Equals(PreviousObservedGeometryDebugSummary, ESearchCase::CaseSensitive))
    {
        if (ObservedGeometryComponents.IsEmpty())
        {
            UE_LOG(
                LogBlenderBridge,
                Verbose,
                TEXT("Observed reference watch updated for '%s': %s"),
                *GetPathNameSafe(this),
                *ObservedGeometryDebugSummary);
        }
        else
        {
            UE_LOG(
                LogBlenderBridge,
                Log,
                TEXT("Observed reference watch updated for '%s': %s"),
                *GetPathNameSafe(this),
                *ObservedGeometryDebugSummary);
        }
    }
}

UBlenderBridgeMoveProbeComponent* UBlenderBridgeComponent::FindOrCreateObservedMoveProbe(USceneComponent* SceneComponent)
{
    if (SceneComponent == nullptr)
    {
        return nullptr;
    }

    AActor* OwnerActor = SceneComponent->GetOwner();
    if (OwnerActor == nullptr)
    {
        return nullptr;
    }

    TInlineComponentArray<UBlenderBridgeMoveProbeComponent*> ExistingProbes(OwnerActor);
    for (UBlenderBridgeMoveProbeComponent* ExistingProbe : ExistingProbes)
    {
        if (ExistingProbe != nullptr && ExistingProbe->MatchesObservedComponent(SceneComponent))
        {
            ExistingProbe->InitializeForObservedComponent(SceneComponent);
            return ExistingProbe;
        }
    }

    UBlenderBridgeMoveProbeComponent* MoveProbe = NewObject<UBlenderBridgeMoveProbeComponent>(
        OwnerActor,
        MakeUniqueObjectName(OwnerActor, UBlenderBridgeMoveProbeComponent::StaticClass(), TEXT("BlenderBridgeMoveProbe")),
        RF_Transient);
    if (MoveProbe == nullptr)
    {
        return nullptr;
    }

    OwnerActor->AddOwnedComponent(MoveProbe);
    MoveProbe->SetupAttachment(SceneComponent);
    MoveProbe->SetRelativeTransform(FTransform::Identity);
    MoveProbe->RegisterComponent();
    MoveProbe->InitializeForObservedComponent(SceneComponent);

    UE_LOG(
        LogBlenderBridge,
        Verbose,
        TEXT("Observed reference move probe attached for bridge '%s': probe=%s observed=%s"),
        *GetPathNameSafe(this),
        *GetPathNameSafe(MoveProbe),
        *GetPathNameSafe(SceneComponent));

    return MoveProbe;
}

void UBlenderBridgeComponent::ReleaseObservedMoveProbes()
{
    for (const TWeakObjectPtr<UBlenderBridgeMoveProbeComponent>& ObservedMoveProbe : ObservedMoveProbes)
    {
        UBlenderBridgeMoveProbeComponent* MoveProbe = ObservedMoveProbe.Get();
        if (MoveProbe == nullptr)
        {
            continue;
        }

        MoveProbe->RemoveWatchingBridge(this);
        if (!MoveProbe->HasWatchingBridges())
        {
            UE_LOG(
                LogBlenderBridge,
                Verbose,
                TEXT("Observed reference move probe destroyed after bridge '%s' released it: probe=%s"),
                *GetPathNameSafe(this),
                *GetPathNameSafe(MoveProbe));
            MoveProbe->DestroyComponent();
        }
    }

    ObservedMoveProbes.Reset();
}

void UBlenderBridgeComponent::HandleObservedMoveProbeTransformUpdated(
    UBlenderBridgeMoveProbeComponent* MoveProbeComponent,
    const FTransform& PreviousTransform,
    const FTransform& CurrentTransform,
    const ETeleportType Teleport)
{
    (void)PreviousTransform;
    (void)CurrentTransform;
    (void)Teleport;

    if (MoveProbeComponent == nullptr)
    {
        return;
    }

    USceneComponent* ObservedComponent = MoveProbeComponent->GetObservedComponent();
    if (ObservedComponent == nullptr)
    {
        return;
    }

    const bool bStillWatched = ObservedGeometryComponents.ContainsByPredicate([ObservedComponent](const TWeakObjectPtr<USceneComponent>& ObservedGeometryComponent)
    {
        return ObservedGeometryComponent.Get() == ObservedComponent;
    });
    if (!bStillWatched)
    {
        return;
    }

    LastObservedGeometryEvent = FString::Printf(
        TEXT("Move probe transform event: %s"),
        *GetPathNameSafe(ObservedComponent));
    UE_LOG(
        LogBlenderBridge,
        Verbose,
        TEXT("Reference follow-cook move probe detected for '%s' on bridge '%s'."),
        *GetPathNameSafe(ObservedComponent),
        *GetPathNameSafe(this));
    QueueCookFromObservedGeometryChange(TEXT("Reference actor changed. Waiting to cook."));
}

void UBlenderBridgeComponent::QueueCookFromObservedGeometryChange(const TCHAR* StatusMessage)
{
    const bool bWasAlreadyPending = PendingSplineCookTickerHandle.IsValid();
    CancelPendingSplineCook();

    if (Source == nullptr || TargetId.IsEmpty() || ConnectionState != EBlenderBridgeConnectionState::Connected)
    {
        LastObservedGeometryEvent = FString::Printf(
            TEXT("%s Cook skipped because source/target/session is not ready."),
            StatusMessage);
        UE_LOG(
            LogBlenderBridge,
            Log,
            TEXT("Reference follow-cook skipped for bridge '%s': source=%d targetEmpty=%d connection=%d."),
            *GetPathNameSafe(this),
            Source != nullptr ? 1 : 0,
            TargetId.IsEmpty() ? 1 : 0,
            static_cast<int32>(ConnectionState));
        return;
    }

    SetBridgeStatus(EBlenderBridgeComponentStatus::CookQueued, StatusMessage);
    if (bWasAlreadyPending)
    {
        LastObservedGeometryEvent = FString::Printf(
            TEXT("%s Debounced recook refreshed for '%s'."),
            StatusMessage,
            *TargetId);
    }
    else
    {
        LastObservedGeometryEvent = FString::Printf(
            TEXT("%s Debounced recook queued for '%s'."),
            StatusMessage,
            *TargetId);
        UE_LOG(
            LogBlenderBridge,
            Log,
            TEXT("Reference follow-cook queued for bridge '%s' target '%s'."),
            *GetPathNameSafe(this),
            *TargetId);
    }
    PendingSplineCookTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateWeakLambda(
            this,
            [this](const float)
            {
                PendingSplineCookTickerHandle.Reset();
                if (Source != nullptr && !TargetId.IsEmpty() && ConnectionState == EBlenderBridgeConnectionState::Connected)
                {
                    UE_LOG(
                        LogBlenderBridge,
                        Log,
                        TEXT("Reference follow-cook executing RequestCook for bridge '%s' target '%s'."),
                        *GetPathNameSafe(this),
                        *TargetId);
                    RequestCook();
                }
                return false;
            }),
        0.25f);
}

void UBlenderBridgeComponent::QueueCookFromSplineEdit()
{
    QueueCookFromObservedGeometryChange(TEXT("Spline changed. Waiting to cook."));
}

void UBlenderBridgeComponent::CancelPendingSplineCook()
{
    if (PendingSplineCookTickerHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(PendingSplineCookTickerHandle);
        PendingSplineCookTickerHandle.Reset();
    }
}

FString UBlenderBridgeComponent::BuildObservedGeometryDebugSummary() const
{
    if (ObservedGeometryComponents.IsEmpty())
    {
        return TEXT("No observed reference actors.");
    }

    TArray<FString> WatchedEntries;
    WatchedEntries.Reserve(ObservedGeometryComponents.Num());
    for (const TWeakObjectPtr<USceneComponent>& ObservedGeometryComponent : ObservedGeometryComponents)
    {
        const USceneComponent* SceneComponent = ObservedGeometryComponent.Get();
        if (SceneComponent == nullptr)
        {
            continue;
        }

        const AActor* OwnerActor = SceneComponent->GetOwner();
        WatchedEntries.Add(FString::Printf(
            TEXT("%s -> %s"),
            *GetNameSafe(OwnerActor),
            *SceneComponent->GetName()));
    }

    if (WatchedEntries.IsEmpty())
    {
        return TEXT("Reference watch list contains only stale entries.");
    }

    return FString::Printf(
        TEXT("Watching %d reference component(s): %s"),
        WatchedEntries.Num(),
        *FString::Join(WatchedEntries, TEXT("; ")));
}

void UBlenderBridgeComponent::ApplyDecodedMesh(FBridgeDecodedMesh&& DecodedMesh)
{
    EnsurePreviewMeshComponent();
    if (PreviewMeshComponent == nullptr)
    {
        SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to create the preview mesh component."));
        return;
    }

    LastCookedMesh = MoveTemp(DecodedMesh);
    bHasValidCookedMesh = LastCookedMesh.Positions.Num() > 0 && LastCookedMesh.Triangles.Num() > 0;
    LastCookedMaterialSlots = LastCookedMesh.MaterialSlots;

    if (LastCookedMesh.Sections.IsEmpty() && bHasValidCookedMesh)
    {
        FBridgeMaterialSlotDescriptor DefaultSlot;
        DefaultSlot.SlotId = TEXT("Default");
        DefaultSlot.SlotName = TEXT("Default");
        DefaultSlot.BlenderMaterialName = TEXT("Default");
        DefaultSlot.SectionIndex = 0;
        LastCookedMesh.MaterialSlots = { DefaultSlot };
        LastCookedMaterialSlots = LastCookedMesh.MaterialSlots;

        FBridgeMeshSectionDescriptor DefaultSection;
        DefaultSection.SectionIndex = 0;
        DefaultSection.FirstVertex = 0;
        DefaultSection.VertexCount = LastCookedMesh.Positions.Num();
        DefaultSection.FirstIndex = 0;
        DefaultSection.IndexCount = LastCookedMesh.Triangles.Num();
        DefaultSection.SlotId = DefaultSlot.SlotId;
        DefaultSection.SlotName = DefaultSlot.SlotName;
        DefaultSection.BlenderMaterialName = DefaultSlot.BlenderMaterialName;
        LastCookedMesh.Sections = { DefaultSection };
    }

    SyncMaterialOverridesFromCookedSlots();

    PreviewMeshComponent->ClearAllMeshSections();

    TArray<FColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;
    for (const FBridgeMeshSectionDescriptor& Section : LastCookedMesh.Sections)
    {
        const bool bHasValidVertexRange =
            Section.FirstVertex >= 0 &&
            Section.VertexCount > 0 &&
            Section.FirstVertex + Section.VertexCount <= LastCookedMesh.Positions.Num() &&
            Section.FirstVertex + Section.VertexCount <= LastCookedMesh.Normals.Num() &&
            Section.FirstVertex + Section.VertexCount <= LastCookedMesh.UV0.Num();

        const bool bHasValidIndexRange =
            Section.FirstIndex >= 0 &&
            Section.IndexCount >= 3 &&
            Section.FirstIndex + Section.IndexCount <= LastCookedMesh.Triangles.Num();

        if (!bHasValidVertexRange || !bHasValidIndexRange)
        {
            continue;
        }

        TArray<FVector> SectionPositions;
        TArray<FVector> SectionNormals;
        TArray<FVector2D> SectionUv0;
        TArray<int32> SectionTriangles;
        SectionPositions.Reserve(Section.VertexCount);
        SectionNormals.Reserve(Section.VertexCount);
        SectionUv0.Reserve(Section.VertexCount);
        SectionTriangles.Reserve(Section.IndexCount);

        for (int32 VertexIndex = 0; VertexIndex < Section.VertexCount; ++VertexIndex)
        {
            const int32 SourceVertexIndex = Section.FirstVertex + VertexIndex;
            SectionPositions.Add(LastCookedMesh.Positions[SourceVertexIndex]);
            SectionNormals.Add(LastCookedMesh.Normals[SourceVertexIndex]);
            SectionUv0.Add(LastCookedMesh.UV0[SourceVertexIndex]);
        }

        for (int32 IndexOffset = 0; IndexOffset < Section.IndexCount; ++IndexOffset)
        {
            const int32 SourceTriangleIndex = LastCookedMesh.Triangles[Section.FirstIndex + IndexOffset];
            SectionTriangles.Add(SourceTriangleIndex - Section.FirstVertex);
        }

        PreviewMeshComponent->CreateMeshSection(
            Section.SectionIndex,
            SectionPositions,
            SectionTriangles,
            SectionNormals,
            SectionUv0,
            VertexColors,
            Tangents,
            false);
    }

    ApplyPreviewMaterials();
    SetShowingBakedOutput(false);

    if (bPendingBakeAfterCook)
    {
        BakeToStaticMesh();
    }
}

UUserDefinedEnum* UBlenderBridgeComponent::GetOrCreateRuntimeEnum(const FBridgeInputSchema& Entry)
{
    if (Entry.Type != EBlenderBridgeInputType::Enum)
    {
        return nullptr;
    }

    if (Entry.EnumOptions.IsEmpty())
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Bridge enum input '%s' has no options to expose."), *Entry.Id.ToString());
        RuntimeInputEnums.Remove(Entry.Id);
        RuntimeInputEnumHashes.Remove(Entry.Id);
        return nullptr;
    }

    for (const FBridgeEnumOption& Option : Entry.EnumOptions)
    {
        if (!IsBridgeEnumValueRepresentable(Option.Value))
        {
            UE_LOG(
                LogBlenderBridge,
                Warning,
                TEXT("Bridge enum input '%s' uses unsupported value '%d'. Falling back to integer editing."),
                *Entry.Id.ToString(),
                Option.Value);
            RuntimeInputEnums.Remove(Entry.Id);
            RuntimeInputEnumHashes.Remove(Entry.Id);
            return nullptr;
        }
    }

    const uint32 Signature = ComputeBridgeEnumSignature(Entry);
    if (const uint32* ExistingSignature = RuntimeInputEnumHashes.Find(Entry.Id))
    {
        if (*ExistingSignature == Signature)
        {
            if (const TObjectPtr<UUserDefinedEnum>* ExistingEnum = RuntimeInputEnums.Find(Entry.Id))
            {
                return ExistingEnum->Get();
            }
        }
    }

    const FString RuntimeEnumObjectName = MakeBridgeEnumObjectName(Entry);
    if (UUserDefinedEnum* ExistingGlobalEnum = FindObject<UUserDefinedEnum>(GetTransientPackage(), *RuntimeEnumObjectName))
    {
        RuntimeInputEnums.Add(Entry.Id, ExistingGlobalEnum);
        RuntimeInputEnumHashes.Add(Entry.Id, Signature);
        return ExistingGlobalEnum;
    }

    UUserDefinedEnum* RuntimeEnum = NewObject<UUserDefinedEnum>(
        GetTransientPackage(),
        *RuntimeEnumObjectName,
        RF_Transient);
    if (RuntimeEnum == nullptr)
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Failed to allocate runtime enum for bridge input '%s'."), *Entry.Id.ToString());
        return nullptr;
    }

    TArray<TPair<FName, int64>> EmptyNames;
    RuntimeEnum->SetEnums(EmptyNames, UEnum::ECppForm::Namespaced);

    TArray<TPair<FName, int64>> Names;
    Names.Reserve(Entry.EnumOptions.Num());

    TSet<FName> UsedNames;
    for (int32 Index = 0; Index < Entry.EnumOptions.Num(); ++Index)
    {
        const FString BaseShortName = MakeBridgeEnumEntryShortName(Entry.EnumOptions[Index], Index);
        int32 Suffix = 0;

        while (true)
        {
            const FString CandidateShortName = Suffix == 0
                ? BaseShortName
                : FString::Printf(TEXT("%s_%d"), *BaseShortName, Suffix);
            const FName FullName(*RuntimeEnum->GenerateFullEnumName(*CandidateShortName));
            if (!UsedNames.Contains(FullName))
            {
                UsedNames.Add(FullName);
                Names.Emplace(FullName, Entry.EnumOptions[Index].Value);
                break;
            }

            ++Suffix;
        }
    }

    if (!RuntimeEnum->SetEnums(Names, UEnum::ECppForm::Namespaced))
    {
        UE_LOG(LogBlenderBridge, Warning, TEXT("Failed to configure runtime enum for bridge input '%s'."), *Entry.Id.ToString());
        return nullptr;
    }

    RuntimeEnum->DisplayNameMap.Reset();
    for (int32 Index = 0; Index < Entry.EnumOptions.Num(); ++Index)
    {
        const FString EnumEntryName = RuntimeEnum->GetNameStringByIndex(Index);
        const FString DisplayLabel = Entry.EnumOptions[Index].Label.IsEmpty()
            ? (Entry.EnumOptions[Index].Id.IsEmpty() ? EnumEntryName : Entry.EnumOptions[Index].Id)
            : Entry.EnumOptions[Index].Label;
        RuntimeEnum->DisplayNameMap.Add(*EnumEntryName, FText::FromString(DisplayLabel));
    }

    RuntimeInputEnums.Add(Entry.Id, RuntimeEnum);
    RuntimeInputEnumHashes.Add(Entry.Id, Signature);
    return RuntimeEnum;
}

void UBlenderBridgeComponent::RebuildInputValues(const bool bPreserveExistingValues)
{
    FInstancedPropertyBag NewBag;
    TArray<FPropertyBagPropertyDesc> Descriptors;
    Descriptors.Reserve(InputSchema.Num());
    TSet<FName> ActiveEnumIds;

    for (const FBridgeInputSchema& Entry : InputSchema)
    {
        EPropertyBagPropertyType ValueType = EPropertyBagPropertyType::Float;
        const UObject* ValueTypeObject = nullptr;
        switch (Entry.Type)
        {
        case EBlenderBridgeInputType::Integer:
            ValueType = EPropertyBagPropertyType::Int32;
            break;
        case EBlenderBridgeInputType::Boolean:
            ValueType = EPropertyBagPropertyType::Bool;
            break;
        case EBlenderBridgeInputType::Vector3:
            ValueType = EPropertyBagPropertyType::Struct;
            ValueTypeObject = TBaseStructure<FVector>::Get();
            break;
        case EBlenderBridgeInputType::Color:
            ValueType = EPropertyBagPropertyType::Struct;
            ValueTypeObject = TBaseStructure<FLinearColor>::Get();
            break;
        case EBlenderBridgeInputType::String:
            ValueType = EPropertyBagPropertyType::String;
            break;
        case EBlenderBridgeInputType::Enum:
            ActiveEnumIds.Add(Entry.Id);
            if (UUserDefinedEnum* RuntimeEnum = GetOrCreateRuntimeEnum(Entry))
            {
                ValueType = EPropertyBagPropertyType::Enum;
                ValueTypeObject = RuntimeEnum;
            }
            else
            {
                ValueType = EPropertyBagPropertyType::Int32;
            }
            break;
        case EBlenderBridgeInputType::Float:
        default:
            ValueType = EPropertyBagPropertyType::Float;
            break;
        }

        FPropertyBagPropertyDesc Descriptor(GetBridgeInputBagPropertyName(Entry), ValueType, ValueTypeObject);
#if WITH_EDITOR
        Descriptor.SetMetaData(TEXT("DisplayName"), Entry.Label);
        if (!Entry.CategoryPath.IsEmpty())
        {
            Descriptor.SetMetaData(BridgeEnableCategoriesMetaKey, TEXT(""));
            Descriptor.SetMetaData(BridgeCategoryMetaKey, Entry.CategoryPath);
        }

        if (!Entry.Description.IsEmpty())
        {
            Descriptor.SetMetaData(BridgeToolTipMetaKey, Entry.Description);
        }

        if (Entry.Type == EBlenderBridgeInputType::Float || Entry.Type == EBlenderBridgeInputType::Integer)
        {
            Descriptor.SetMetaData(TEXT("ClampMin"), ::LexToString(Entry.MinNumericValue));
            Descriptor.SetMetaData(TEXT("ClampMax"), ::LexToString(Entry.MaxNumericValue));
            Descriptor.SetMetaData(TEXT("Delta"), Entry.Type == EBlenderBridgeInputType::Integer ? TEXT("1") : TEXT("0.01"));
        }
#endif
        Descriptors.Add(MoveTemp(Descriptor));
    }

    for (auto It = RuntimeInputEnums.CreateIterator(); It; ++It)
    {
        if (!ActiveEnumIds.Contains(It->Key))
        {
            RuntimeInputEnumHashes.Remove(It->Key);
            It.RemoveCurrent();
        }
    }

    for (auto It = RuntimeInputEnumHashes.CreateIterator(); It; ++It)
    {
        if (!ActiveEnumIds.Contains(It->Key))
        {
            It.RemoveCurrent();
        }
    }

    NewBag.AddProperties(Descriptors);

    for (const FBridgeInputSchema& Entry : InputSchema)
    {
        const FName BagPropertyName = GetBridgeInputBagPropertyName(Entry);

        switch (Entry.Type)
        {
        case EBlenderBridgeInputType::Integer:
            NewBag.SetValueInt32(BagPropertyName, static_cast<int32>(Entry.DefaultNumericValue));
            break;
        case EBlenderBridgeInputType::Boolean:
            NewBag.SetValueBool(BagPropertyName, Entry.bDefaultBoolValue);
            break;
        case EBlenderBridgeInputType::Vector3:
            NewBag.SetValueStruct(BagPropertyName, Entry.DefaultVectorValue);
            break;
        case EBlenderBridgeInputType::Color:
            NewBag.SetValueStruct(BagPropertyName, Entry.DefaultColorValue);
            break;
        case EBlenderBridgeInputType::String:
            NewBag.SetValueString(BagPropertyName, Entry.DefaultStringValue);
            break;
        case EBlenderBridgeInputType::Enum:
            if (const TObjectPtr<UUserDefinedEnum>* RuntimeEnum = RuntimeInputEnums.Find(Entry.Id); RuntimeEnum != nullptr && RuntimeEnum->Get() != nullptr)
            {
                NewBag.SetValueEnum(BagPropertyName, static_cast<uint8>(GetPreferredBridgeEnumValue(Entry)), RuntimeEnum->Get());
            }
            else
            {
                NewBag.SetValueInt32(BagPropertyName, GetPreferredBridgeEnumValue(Entry));
            }
            break;
        case EBlenderBridgeInputType::Float:
        default:
            NewBag.SetValueFloat(BagPropertyName, static_cast<float>(Entry.DefaultNumericValue));
            break;
        }
    }

    if (bPreserveExistingValues)
    {
        NewBag.CopyMatchingValuesByName(InputValues);
    }

    for (const FBridgeInputSchema& Entry : InputSchema)
    {
        if (!bPreserveExistingValues || Entry.Type != EBlenderBridgeInputType::Enum)
        {
            continue;
        }

        const FName BagPropertyName = GetBridgeInputBagPropertyName(Entry);
        const int32 PreviousValue = GetStoredBridgeEnumValue(InputValues, BagPropertyName);
        if (!IsBridgeEnumValueRepresentable(PreviousValue) || !HasBridgeEnumValue(Entry, PreviousValue))
        {
            continue;
        }

        if (const TObjectPtr<UUserDefinedEnum>* RuntimeEnum = RuntimeInputEnums.Find(Entry.Id); RuntimeEnum != nullptr && RuntimeEnum->Get() != nullptr)
        {
            NewBag.SetValueEnum(BagPropertyName, static_cast<uint8>(PreviousValue), RuntimeEnum->Get());
        }
        else
        {
            NewBag.SetValueInt32(BagPropertyName, PreviousValue);
        }
    }

    InputValues = MoveTemp(NewBag);
}

bool UBlenderBridgeComponent::BuildGeometryBindingPayloads(
    TArray<FBridgeGeometryBindingPayload>& OutBindingPayloads,
    FString& OutBindingPayloadHash,
    FString& OutErrorMessage) const
{
    OutBindingPayloads.Reset();
    OutBindingPayloadHash.Reset();
    OutErrorMessage.Reset();

    for (const FBlenderBridgeGeometryBinding& Binding : GeometryBindings)
    {
        const FBridgeGeometryInputDescriptor* Descriptor = FindGeometryInputDescriptor(Binding.InputId);
        if (Descriptor == nullptr)
        {
            continue;
        }

        const EBlenderBridgeGeometrySourceKind SourceKind = GetEffectiveGeometryBindingSourceKind(Binding.InputId);
        if (SourceKind == EBlenderBridgeGeometrySourceKind::None
            || SourceKind == EBlenderBridgeGeometrySourceKind::EmbeddedSpline
            || SourceKind == EBlenderBridgeGeometrySourceKind::SplineComponent)
        {
            continue;
        }

        if (SourceKind != EBlenderBridgeGeometrySourceKind::StaticMeshComponent)
        {
            continue;
        }

        UStaticMeshComponent* MeshComponent = ResolveBoundStaticMeshComponent(Binding.InputId);
        if (MeshComponent == nullptr)
        {
            OutErrorMessage = FString::Printf(
                TEXT("Geometry binding '%s' requires a valid StaticMeshComponent reference."),
                *Descriptor->Label);
            return false;
        }

        UStaticMesh* StaticMesh = MeshComponent->GetStaticMesh();
        if (StaticMesh == nullptr)
        {
            OutErrorMessage = FString::Printf(
                TEXT("Geometry binding '%s' references a StaticMeshComponent with no static mesh assigned."),
                *Descriptor->Label);
            return false;
        }

        const FStaticMeshRenderData* RenderData = StaticMesh->GetRenderData();
        if (RenderData == nullptr || RenderData->LODResources.IsEmpty())
        {
            OutErrorMessage = FString::Printf(
                TEXT("Geometry binding '%s' could not read render data from static mesh '%s'."),
                *Descriptor->Label,
                *StaticMesh->GetName());
            return false;
        }

        const FStaticMeshLODResources& LODResources = RenderData->LODResources[0];
        const uint32 VertexCount = LODResources.VertexBuffers.PositionVertexBuffer.GetNumVertices();
        const int32 IndexCount = LODResources.IndexBuffer.GetNumIndices();
        if (VertexCount <= 0 || IndexCount < 3)
        {
            OutErrorMessage = FString::Printf(
                TEXT("Geometry binding '%s' did not produce any triangles from static mesh '%s'."),
                *Descriptor->Label,
                *StaticMesh->GetName());
            return false;
        }

        FBridgeGeometryBindingPayload BindingPayload;
        BindingPayload.InputId = Binding.InputId;
        BindingPayload.SourceKind = SourceKind;
        BindingPayload.MeshData.Positions.Reserve(static_cast<int32>(VertexCount));
        for (uint32 VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex)
        {
            const FVector3f Position = LODResources.VertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex);
            BindingPayload.MeshData.Positions.Add(FVector(Position));
        }

        const FIndexArrayView IndexArray = LODResources.IndexBuffer.GetArrayView();
        BindingPayload.MeshData.Triangles.Reserve(IndexArray.Num());
        for (int32 TriangleIndexIndex = 0; TriangleIndexIndex < IndexArray.Num(); ++TriangleIndexIndex)
        {
            BindingPayload.MeshData.Triangles.Add(static_cast<int32>(IndexArray[TriangleIndexIndex]));
        }

        BindingPayload.MeshData.RelativeTransform = MeshComponent->GetComponentTransform().GetRelativeTransform(GetComponentTransform());
        OutBindingPayloads.Add(MoveTemp(BindingPayload));
    }

    if (OutBindingPayloads.IsEmpty())
    {
        return true;
    }

    FString SerializedBindingPayloads;
    if (!BlenderBridge::SerializeGeometryBindingPayloads(OutBindingPayloads, SerializedBindingPayloads))
    {
        OutErrorMessage = TEXT("Failed to serialize geometry bindings for caching.");
        return false;
    }

    OutBindingPayloadHash = FMD5::HashAnsiString(*SerializedBindingPayloads);
    return true;
}

bool UBlenderBridgeComponent::BuildCurveOwnerData(FBridgeCurveOwnerData& OutCurveData, FString& OutCurveHash) const
{
    OutCurveData = FBridgeCurveOwnerData();
    OutCurveHash.Reset();

    if (!TargetCapabilities.bSupportsCurveOwner)
    {
        return false;
    }

    const USplineComponent* SplineComponent = GetActiveCurveSourceSplineComponent();
    return BuildCurveOwnerDataFromSplineComponent(SplineComponent, OutCurveData, OutCurveHash);
}

bool UBlenderBridgeComponent::BuildCurveOwnerDataFromSplineComponent(
    const USplineComponent* SplineComponent,
    FBridgeCurveOwnerData& OutCurveData,
    FString& OutCurveHash) const
{
    OutCurveData = FBridgeCurveOwnerData();
    OutCurveHash.Reset();

    if (SplineComponent == nullptr || SplineComponent->GetNumberOfSplinePoints() <= 0)
    {
        return false;
    }

    const FTransform BridgeTransform = GetComponentTransform();

    OutCurveData.CurveType = EBlenderBridgeCurveType::Bezier;
    OutCurveData.bClosedLoop = SplineComponent->IsClosedLoop();
    OutCurveData.Points.Reserve(SplineComponent->GetNumberOfSplinePoints());

    for (int32 PointIndex = 0; PointIndex < SplineComponent->GetNumberOfSplinePoints(); ++PointIndex)
    {
        FBridgeCurveOwnerPoint Point;
        const FVector WorldPosition = SplineComponent->GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);
        const FVector WorldArriveTangent = SplineComponent->GetArriveTangentAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);
        const FVector WorldLeaveTangent = SplineComponent->GetLeaveTangentAtSplinePoint(PointIndex, ESplineCoordinateSpace::World);

        Point.Position = BridgeTransform.InverseTransformPosition(WorldPosition);
        Point.ArriveTangent = BridgeTransform.InverseTransformVectorNoScale(WorldArriveTangent);
        Point.LeaveTangent = BridgeTransform.InverseTransformVectorNoScale(WorldLeaveTangent);
        Point.RollDegrees = SplineComponent->GetRollAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local);
        Point.Scale = SplineComponent->GetScaleAtSplinePoint(PointIndex);
        Point.Type = ToBridgeSplinePointType(SplineComponent->GetSplinePointType(PointIndex));
        OutCurveData.Points.Add(MoveTemp(Point));
    }

    OutCurveHash = MakeCurveOwnerHash(OutCurveData);
    return true;
}

FString UBlenderBridgeComponent::ApplyCurveOwnerDescription(
    const FString& InTargetId,
    const FBridgeTargetCapabilities& InCapabilities,
    const bool bPreserveExistingSplineState)
{
    TargetCapabilities = InCapabilities;
    RefreshObservedSplineComponents();

    if (TargetCapabilities.TargetKind != EBlenderBridgeTargetKind::CurveOwner)
    {
        RemoveOwnedBridgeSplineComponent();
        ImportedCurveTargetId.Reset();
        ImportedCurveHash.Reset();
        bSplineEditedSinceImport = false;
        return FString();
    }

    if (!TargetCapabilities.bSupportsCurveOwner)
    {
        RemoveOwnedBridgeSplineComponent();
        ImportedCurveTargetId.Reset();
        ImportedCurveHash.Reset();
        bSplineEditedSinceImport = false;
        return TargetCapabilities.CompatibilityMessage.IsEmpty()
            ? TEXT(" Curve-owner spline editing is unavailable for this target.")
            : FString::Printf(TEXT(" Curve-owner spline editing unavailable: %s"), *TargetCapabilities.CompatibilityMessage);
    }

    bool bUseExternalSplineBinding = false;
    for (const FBridgeGeometryInputDescriptor& Descriptor : GeometryInputDescriptors)
    {
        if (Descriptor.AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::SplineComponent)
            || Descriptor.AllowedSourceKinds.Contains(EBlenderBridgeGeometrySourceKind::EmbeddedSpline))
        {
            bUseExternalSplineBinding = GetEffectiveGeometryBindingSourceKind(Descriptor.InputId) == EBlenderBridgeGeometrySourceKind::SplineComponent;
            break;
        }
    }

    if (bUseExternalSplineBinding)
    {
        RemoveOwnedBridgeSplineComponent();
        ImportedCurveTargetId = InTargetId;
        ImportedCurveHash = MakeCurveOwnerHash(TargetCapabilities.OwnerCurve);
        bSplineEditedSinceImport = false;
        RefreshObservedSplineComponents();
        return TEXT(" External spline binding is active.");
    }

    if (EnsureBridgeSplineComponent() == nullptr)
    {
        ImportedCurveTargetId.Reset();
        ImportedCurveHash.Reset();
        bSplineEditedSinceImport = false;
        return TEXT(" Curve-owner target is compatible, but the embedded Unreal spline could not be created.");
    }

    const FString IncomingCurveHash = MakeCurveOwnerHash(TargetCapabilities.OwnerCurve);
    const bool bShouldImportCurve =
        ImportedCurveTargetId != InTargetId ||
        !bPreserveExistingSplineState ||
        !bSplineEditedSinceImport ||
        ImportedCurveHash.IsEmpty();

    if (bShouldImportCurve)
    {
        FString ImportedHash;
        if (!ImportCurveOwnerData(TargetCapabilities.OwnerCurve, ImportedHash))
        {
            return TEXT(" Curve-owner target is compatible, but the embedded Unreal spline could not be initialized.");
        }

        ImportedCurveTargetId = InTargetId;
        ImportedCurveHash = ImportedHash.IsEmpty() ? IncomingCurveHash : ImportedHash;
        RefreshObservedSplineComponents();
        return TEXT(" Imported owner curve from Blender.");
    }

    ImportedCurveTargetId = InTargetId;
    if (ImportedCurveHash.IsEmpty())
    {
        ImportedCurveHash = IncomingCurveHash;
    }
    RefreshObservedSplineComponents();
    return TEXT(" Preserved local spline edits.");
}

bool UBlenderBridgeComponent::ImportCurveOwnerData(const FBridgeCurveOwnerData& InCurveData, FString& OutCurveHash)
{
    OutCurveHash.Reset();

    USplineComponent* SplineComponent = GetBridgeSplineComponent();
    if (SplineComponent == nullptr || InCurveData.CurveType != EBlenderBridgeCurveType::Bezier || InCurveData.Points.IsEmpty())
    {
        return false;
    }

    const bool bPreviousSuppressSplineNotifications = bSuppressSplineNotifications;
    bSuppressSplineNotifications = true;

    SplineComponent->ClearSplinePoints(false);
    for (const FBridgeCurveOwnerPoint& Point : InCurveData.Points)
    {
        SplineComponent->AddSplinePoint(Point.Position, ESplineCoordinateSpace::Local, false);
    }

    for (int32 PointIndex = 0; PointIndex < InCurveData.Points.Num(); ++PointIndex)
    {
        const FBridgeCurveOwnerPoint& Point = InCurveData.Points[PointIndex];
        SplineComponent->SetSplinePointType(PointIndex, ToSplinePointType(Point.Type), false);
        SplineComponent->SetTangentsAtSplinePoint(
            PointIndex,
            Point.ArriveTangent,
            Point.LeaveTangent,
            ESplineCoordinateSpace::Local,
            false);
        if (!FMath::IsNearlyZero(Point.RollDegrees))
        {
            const FQuat CurrentQuaternion = SplineComponent->GetQuaternionAtSplinePoint(PointIndex, ESplineCoordinateSpace::Local);
            const FQuat RollQuaternion(FVector::ForwardVector, FMath::DegreesToRadians(Point.RollDegrees));
            SplineComponent->SetQuaternionAtSplinePoint(
                PointIndex,
                CurrentQuaternion * RollQuaternion,
                ESplineCoordinateSpace::Local,
                false);
        }
        SplineComponent->SetScaleAtSplinePoint(PointIndex, Point.Scale, false);
    }

    SplineComponent->SetClosedLoop(InCurveData.bClosedLoop, false);
    SplineComponent->UpdateSpline();

    bSuppressSplineNotifications = bPreviousSuppressSplineNotifications;
    bSplineEditedSinceImport = false;
    OutCurveHash = MakeCurveOwnerHash(InCurveData);
    return true;
}

void UBlenderBridgeComponent::ResetBridgeData(const bool bClearTargetSelection)
{
    CancelPendingSplineCook();
    CancelPendingBakedOutputVisibilitySwap();
    UnbindSplineComponentDelegates();
    ResetObservedGeometryTracking();
    RemoveOwnedBridgeSplineComponent();

    if (PreviewMeshComponent != nullptr)
    {
        PreviewMeshComponent->ClearAllMeshSections();
    }

    AvailableTargets.Reset();
    SchemaTargetId.Reset();
    InputSchema.Reset();
    GeometryInputDescriptors.Reset();
    GeometryBindings.Reset();
    TargetCapabilities = FBridgeTargetCapabilities();
    TargetDisplayLabel.Reset();
    LastCookedMaterialSlots.Reset();
    LastCookedMesh.Reset();
    bHasValidCookedMesh = false;
    bPendingBakeAfterCook = false;
    bPendingInstancedBake = false;
    bShowingBakedOutput = false;
    RuntimeInputEnums.Reset();
    RuntimeInputEnumHashes.Reset();
    ImportedCurveTargetId.Reset();
    ImportedCurveHash.Reset();
    bSplineEditedSinceImport = false;
    ObservedGeometryDebugSummary = TEXT("No observed reference actors.");
    LastObservedGeometryEvent.Reset();
    InputValues = FInstancedPropertyBag();

    if (bClearTargetSelection)
    {
        TargetId.Reset();
        MaterialOverrides.Reset();
    }

    UpdatePreviewVisibility();
}

FString UBlenderBridgeComponent::GetSourceHandle() const
{
    if (Source == nullptr || Source->BlendFile.FilePath.IsEmpty())
    {
        return FString();
    }

    FString SourceHandle = FPaths::ConvertRelativePathToFull(Source->BlendFile.FilePath);
    FPaths::NormalizeFilename(SourceHandle);
    return SourceHandle;
}

const FBridgeTargetDescriptor* UBlenderBridgeComponent::FindTargetDescriptor(const FString& InTargetId) const
{
    return AvailableTargets.FindByPredicate([&InTargetId](const FBridgeTargetDescriptor& Target)
    {
        return Target.TargetId == InTargetId;
    });
}

void UBlenderBridgeComponent::UpdateTargetDisplayLabel()
{
    if (const FBridgeTargetDescriptor* TargetDescriptor = FindTargetDescriptor(TargetId))
    {
        TargetDisplayLabel = TargetDescriptor->DisplayLabel;
        return;
    }

    TargetDisplayLabel = TargetId;
}

void UBlenderBridgeComponent::SetBridgeStatus(const EBlenderBridgeComponentStatus NewStatus, const FString& Message)
{
    ComponentStatus = NewStatus;
    LastStatusMessage = Message;
}

void UBlenderBridgeComponent::SyncMaterialOverridesFromCookedSlots()
{
    TMap<FString, TObjectPtr<UMaterialInterface>> ExistingOverrides;
    for (const FBlenderBridgeMaterialOverride& Entry : MaterialOverrides)
    {
        if (!Entry.SlotId.IsEmpty())
        {
            ExistingOverrides.Add(Entry.SlotId, Entry.Material);
        }
    }

    TArray<FBlenderBridgeMaterialOverride> UpdatedOverrides;
    UpdatedOverrides.Reserve(LastCookedMaterialSlots.Num());
    for (const FBridgeMaterialSlotDescriptor& Slot : LastCookedMaterialSlots)
    {
        FBlenderBridgeMaterialOverride OverrideEntry;
        OverrideEntry.SlotId = Slot.SlotId;
        OverrideEntry.SlotName = Slot.SlotName;
        OverrideEntry.BlenderMaterialName = Slot.BlenderMaterialName;
        if (const TObjectPtr<UMaterialInterface>* ExistingMaterial = ExistingOverrides.Find(Slot.SlotId))
        {
            OverrideEntry.Material = *ExistingMaterial;
        }

        UpdatedOverrides.Add(MoveTemp(OverrideEntry));
    }

    MaterialOverrides = MoveTemp(UpdatedOverrides);
}

void UBlenderBridgeComponent::QueueShowBakedOutputOnNextTick()
{
    CancelPendingBakedOutputVisibilitySwap();

    PendingShowBakedOutputTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateWeakLambda(
            this,
            [this](const float)
            {
                PendingShowBakedOutputTickerHandle.Reset();
                SetShowingBakedOutput(true);
                return false;
            }),
        0.0f);
}

void UBlenderBridgeComponent::CancelPendingBakedOutputVisibilitySwap()
{
    if (PendingShowBakedOutputTickerHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(PendingShowBakedOutputTickerHandle);
        PendingShowBakedOutputTickerHandle.Reset();
    }
}

void UBlenderBridgeComponent::SetShowingBakedOutput(const bool bInShowingBakedOutput)
{
    if (!bInShowingBakedOutput)
    {
        CancelPendingBakedOutputVisibilitySwap();
    }

    bShowingBakedOutput = bInShowingBakedOutput;
    UpdatePreviewVisibility();
    UpdateBakedOutputVisibility();
}

void UBlenderBridgeComponent::UpdatePreviewVisibility()
{
    if (PreviewMeshComponent == nullptr)
    {
        return;
    }

    const bool bShouldHidePreview = bHidePreviewAfterBake && bShowingBakedOutput && BakedActor.IsValid();
    PreviewMeshComponent->SetVisibility(!bShouldHidePreview);
    PreviewMeshComponent->SetHiddenInGame(bShouldHidePreview);
}

void UBlenderBridgeComponent::UpdateBakedOutputVisibility()
{
    if (AActor* ExistingBakedActor = BakedActor.Get())
    {
        const bool bShouldShowBakedOutput = bShowingBakedOutput;
#if WITH_EDITOR
        ExistingBakedActor->SetIsTemporarilyHiddenInEditor(!bShouldShowBakedOutput);
#endif
        ExistingBakedActor->SetActorHiddenInGame(!bShouldShowBakedOutput);

        TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents(ExistingBakedActor);
        for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
        {
            if (PrimitiveComponent == nullptr)
            {
                continue;
            }

            PrimitiveComponent->SetVisibility(bShouldShowBakedOutput);
            PrimitiveComponent->SetHiddenInGame(!bShouldShowBakedOutput);
        }
    }
}

void UBlenderBridgeComponent::ApplyPreviewMaterials()
{
    if (PreviewMeshComponent == nullptr)
    {
        return;
    }

    for (const FBridgeMeshSectionDescriptor& Section : LastCookedMesh.Sections)
    {
        PreviewMeshComponent->SetMaterial(Section.SectionIndex, FindOverrideMaterialBySlotId(Section.SlotId));
    }
}

UMaterialInterface* UBlenderBridgeComponent::FindOverrideMaterialBySlotId(const FString& SlotId) const
{
    for (const FBlenderBridgeMaterialOverride& Entry : MaterialOverrides)
    {
        if (Entry.SlotId == SlotId)
        {
            return Entry.Material;
        }
    }

    return nullptr;
}

bool UBlenderBridgeComponent::HasCookedMesh() const
{
    return bHasValidCookedMesh && LastCookedMesh.Positions.Num() > 0 && LastCookedMesh.Triangles.Num() > 0;
}

bool UBlenderBridgeComponent::HasBakedOutput() const
{
    return BakedActor.IsValid() || !BakedStaticMesh.IsNull() || !BakedPrototypeMeshes.IsEmpty();
}
