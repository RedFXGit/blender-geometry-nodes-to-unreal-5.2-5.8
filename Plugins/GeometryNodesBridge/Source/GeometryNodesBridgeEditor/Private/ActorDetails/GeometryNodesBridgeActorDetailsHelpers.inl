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

