void FGeometryNodesBridgeSessionManager::HandleSessionMessageReceived(const FGeometryNodesBridgeJsonMessage& Message)
{
    FString RequestId;
    TryGetRequestId(Message.Payload, RequestId);

    if (Message.Type == EGeometryNodesBridgeMessageType::SourceLoaded)
    {
        FPendingRequest PendingRequest;
        bool bHadPendingRequest = false;
        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                PendingRequest = *ExistingRequest;
                bHadPendingRequest = true;
                PendingRequests.Remove(RequestId);
            }
        }

        FString SourceHandle;
        if (!Message.Payload.IsValid() || !Message.Payload->TryGetStringField(SourceHandleFieldName, SourceHandle) || SourceHandle.IsEmpty())
        {
            LastSessionMessage = TEXT("SourceLoaded response was missing source_handle.");
            return;
        }

        TArray<FBridgeTargetDescriptor> Targets;
        GeometryNodesBridge::ParseTargetDescriptors(Message.Payload, Targets);

        FSourceCache& SourceCache = SourceCaches.FindOrAdd(SourceHandle);
        if (SourceCache.BlendFilePath.IsEmpty())
        {
            Message.Payload->TryGetStringField(BlendFileFieldName, SourceCache.BlendFilePath);
        }
        ClearCachedTargetData(SourceHandle);
        SourceCache.Targets = MoveTemp(Targets);
        SourceCache.bLoaded = true;

        const double ResponseWaitMs = PendingRequest.SentTimeSeconds > 0.0
            ? (FPlatformTime::Seconds() - PendingRequest.SentTimeSeconds) * 1000.0
            : -1.0;
        UE_LOG(
            LogGeometryNodesBridge,
            Log,
            TEXT("[GeometryNodesBridgeFlow] source_loaded request=%s matched_pending=%s source='%s' targets=%d pending_component=%s cook_after_describe=%s instance_bake_after_describe=%s response_wait=%s"),
            RequestId.IsEmpty() ? TEXT("<none>") : *RequestId,
            BoolToString(bHadPendingRequest),
            *SourceHandle,
            SourceCache.Targets.Num(),
            *DescribeBridgeComponentForLog(PendingRequest.Component.Get()),
            BoolToString(PendingRequest.bRequestCookAfterDescribe),
            BoolToString(PendingRequest.bRequestInstanceBakeAfterDescribe),
            *FormatTimingMs(ResponseWaitMs));

        ApplySourceCacheToComponents(
            SourceHandle,
            PendingRequest.bRequestCookAfterDescribe,
            PendingRequest.bRequestInstanceBakeAfterDescribe,
            PendingRequest.Component.Get());
        return;
    }

    if (Message.Type == EGeometryNodesBridgeMessageType::TargetDescription)
    {
        FPendingRequest PendingRequest;
        bool bHadPendingRequest = false;
        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                PendingRequest = *ExistingRequest;
                bHadPendingRequest = true;
                PendingRequests.Remove(RequestId);
            }
        }

        TArray<FBridgeInputSchema> Schema;
        if (!GeometryNodesBridge::ParseInputSchema(Message.Payload, Schema))
        {
            if (PendingRequest.bRequestInstanceBakeAfterDescribe && IsValid(PendingRequest.Component.Get()))
            {
                PendingRequest.Component->bPendingInstancedBake = false;
            }
            if (IsValid(PendingRequest.Component.Get()))
            {
                PendingRequest.Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to parse target schema from Blender."));
            }
            return;
        }

        FString SourceHandle = PendingRequest.SourceHandle;
        if (SourceHandle.IsEmpty() && Message.Payload.IsValid())
        {
            Message.Payload->TryGetStringField(SourceHandleFieldName, SourceHandle);
        }

        FString TargetId;
        if (Message.Payload.IsValid())
        {
            Message.Payload->TryGetStringField(TargetIdFieldName, TargetId);
        }

        if (TargetId.IsEmpty())
        {
            TargetId = PendingRequest.TargetId;
        }

        FBridgeTargetCapabilities Capabilities;
        GeometryNodesBridge::ParseTargetCapabilities(Message.Payload, Capabilities);
        TArray<FBridgeGeometryInputDescriptor> GeometryDescriptors;
        GeometryNodesBridge::ParseGeometryInputDescriptors(Message.Payload, GeometryDescriptors);

        if (!SourceHandle.IsEmpty() && !TargetId.IsEmpty())
        {
            FTargetSchemaCache& CachedSchema = TargetSchemaCaches.FindOrAdd(MakeTargetSchemaCacheKey(SourceHandle, TargetId));
            CachedSchema.Schema = Schema;
            CachedSchema.GeometryInputs = GeometryDescriptors;
            CachedSchema.Capabilities = Capabilities;
        }

        const double ResponseWaitMs = PendingRequest.SentTimeSeconds > 0.0
            ? (FPlatformTime::Seconds() - PendingRequest.SentTimeSeconds) * 1000.0
            : -1.0;
        FBridgeDescribeTimingBreakdown DescribeTimings;
        ParseDescribeTimingBreakdown(Message.Payload, DescribeTimings);
        UE_LOG(
            LogGeometryNodesBridge,
            Log,
            TEXT("[GeometryNodesBridgeFlow] target_description request=%s matched_pending=%s source='%s' target='%s' schema_inputs=%d geometry_inputs=%d pending_component=%s preserve_input_values=%s cook_after_describe=%s instance_bake_after_describe=%s response_wait=%s worker_total=%s worker_resolve=%s worker_capabilities=%s worker_geometry_inputs=%s worker_schema=%s worker_cache_hit=%s"),
            RequestId.IsEmpty() ? TEXT("<none>") : *RequestId,
            BoolToString(bHadPendingRequest),
            *SourceHandle,
            TargetId.IsEmpty() ? TEXT("<none>") : *TargetId,
            Schema.Num(),
            GeometryDescriptors.Num(),
            *DescribeBridgeComponentForLog(PendingRequest.Component.Get()),
            BoolToString(PendingRequest.bPreserveInputValues),
            BoolToString(PendingRequest.bRequestCookAfterDescribe),
            BoolToString(PendingRequest.bRequestInstanceBakeAfterDescribe),
            *FormatTimingMs(ResponseWaitMs),
            *FormatTimingMs(DescribeTimings.TotalMs),
            *FormatTimingMs(DescribeTimings.ResolveMs),
            *FormatTimingMs(DescribeTimings.CapabilitiesMs),
            *FormatTimingMs(DescribeTimings.GeometryInputsMs),
            *FormatTimingMs(DescribeTimings.SchemaMs),
            DescribeTimings.bHasCacheHit ? BoolToString(DescribeTimings.bCacheHit) : TEXT("n/a"));

        auto ApplyTargetDescriptionToComponent =
            [&](UGeometryNodesBridgeComponent* Component, const bool bPreserveInputValues, const bool bRequestCookAfterDescribe, const bool bRequestInstanceBakeAfterDescribe)
        {
            if (!IsValid(Component))
            {
                return;
            }

            Component->InputSchema = Schema;
            Component->GeometryInputDescriptors = GeometryDescriptors;
            Component->SchemaTargetId = TargetId;
            Component->RebuildInputValues(bPreserveInputValues);
            Component->RebuildGeometryBindings(bPreserveInputValues);
            const FString CurveOwnerStatus = Component->ApplyCurveOwnerDescription(TargetId, Capabilities, bPreserveInputValues);
            Component->SetBridgeStatus(
                EGeometryNodesBridgeComponentStatus::Ready,
                FString::Printf(
                    TEXT("Ready. Received %d bridge inputs and %d geometry binding target(s).%s"),
                    Component->InputSchema.Num(),
                    Component->GeometryInputDescriptors.Num(),
                    *CurveOwnerStatus));

            UE_LOG(
                LogGeometryNodesBridge,
                Log,
                TEXT("[GeometryNodesBridgeFlow] target_description applied source='%s' target='%s' component=%s preserve_input_values=%s cook_after_describe=%s instance_bake_after_describe=%s"),
                *SourceHandle,
                TargetId.IsEmpty() ? TEXT("<none>") : *TargetId,
                *DescribeBridgeComponentForLog(Component),
                BoolToString(bPreserveInputValues),
                BoolToString(bRequestCookAfterDescribe),
                BoolToString(bRequestInstanceBakeAfterDescribe));

            if (bRequestCookAfterDescribe)
            {
                SendCookRequest(Component);
                return;
            }

            if (bRequestInstanceBakeAfterDescribe)
            {
                SendInstanceBakeRequest(Component);
            }
        };

        UGeometryNodesBridgeComponent* Component = PendingRequest.Component.Get();
        if (IsValid(Component))
        {
            ApplyTargetDescriptionToComponent(
                Component,
                PendingRequest.bPreserveInputValues,
                PendingRequest.bRequestCookAfterDescribe,
                PendingRequest.bRequestInstanceBakeAfterDescribe);
            return;
        }

        int32 FallbackApplyCount = 0;
        if (!SourceHandle.IsEmpty() && !TargetId.IsEmpty())
        {
            ForEachBridgeComponent([&](UGeometryNodesBridgeComponent* BridgeComponent)
            {
                if (BridgeComponent->GetSourceHandle() != SourceHandle || BridgeComponent->TargetId != TargetId)
                {
                    return;
                }

                const bool bPreserveInputValues = BridgeComponent->SchemaTargetId == TargetId;
                ApplyTargetDescriptionToComponent(
                    BridgeComponent,
                    bPreserveInputValues,
                    bHadPendingRequest ? PendingRequest.bRequestCookAfterDescribe : false,
                    bHadPendingRequest ? PendingRequest.bRequestInstanceBakeAfterDescribe : false);
                ++FallbackApplyCount;
            });
        }

        if (FallbackApplyCount > 0)
        {
            UE_LOG(
                LogGeometryNodesBridge,
                Log,
                TEXT("[GeometryNodesBridgeFlow] target_description fallback_applied source='%s' target='%s' component_count=%d"),
                *SourceHandle,
                TargetId.IsEmpty() ? TEXT("<none>") : *TargetId,
                FallbackApplyCount);
        }
        else
        {
            UE_LOG(
                LogGeometryNodesBridge,
                Warning,
                TEXT("Received TargetDescription for source '%s' target '%s' but no live bridge component was available to consume it."),
                *SourceHandle,
                *TargetId);
        }
        return;
    }

    if (Message.Type == EGeometryNodesBridgeMessageType::CookComplete)
    {
        FPendingRequest PendingRequest;
        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                PendingRequest = *ExistingRequest;
                PendingRequests.Remove(RequestId);
            }
        }

        UGeometryNodesBridgeComponent* Component = PendingRequest.Component.Get();
        if (!IsValid(Component) || !Session.IsValid())
        {
            return;
        }

        const double ResponseReceivedSeconds = FPlatformTime::Seconds();
        FBridgeWorkerTimingBreakdown WorkerTimings;
        ParseWorkerTimingBreakdown(Message.Payload, WorkerTimings);

        double SlotNumber = 0.0;
        if (!Message.Payload.IsValid() || !Message.Payload->TryGetNumberField(TEXT("slot"), SlotNumber))
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("CookComplete was missing a shared-memory slot."));
            return;
        }

        const double ReadDecodeStartSeconds = FPlatformTime::Seconds();
        FBridgeDecodedMesh DecodedMesh;
        if (!Session->GetSharedMemory().ReadMesh(static_cast<uint32>(SlotNumber), DecodedMesh))
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to read cooked mesh from shared memory."));
            return;
        }

        GeometryNodesBridge::ParseCookedMeshMetadata(Message.Payload, DecodedMesh.MaterialSlots, DecodedMesh.Sections);
        const double ReadDecodeMs = (FPlatformTime::Seconds() - ReadDecodeStartSeconds) * 1000.0;
        if (!PendingRequest.CookCacheKey.IsEmpty())
        {
            FCookedTargetCache& CachedCook = CookedTargetCaches.FindOrAdd(PendingRequest.CookCacheKey);
            CachedCook.DecodedMesh = DecodedMesh;
        }

        const double PreviewApplyStartSeconds = FPlatformTime::Seconds();
        Component->ApplyDecodedMesh(MoveTemp(DecodedMesh));
        const double PreviewApplyMs = (FPlatformTime::Seconds() - PreviewApplyStartSeconds) * 1000.0;
        const double ApplyFinishedSeconds = FPlatformTime::Seconds();
        Component->SetBridgeStatus(
            EGeometryNodesBridgeComponentStatus::Cooked,
            FString::Printf(TEXT("Applied bridge mesh from slot %u"), static_cast<uint32>(SlotNumber)));

        if (ShouldLogCookTimings() && PendingRequest.SentTimeSeconds > 0.0)
        {
            const double ResponseWaitMs = (ResponseReceivedSeconds - PendingRequest.SentTimeSeconds) * 1000.0;
            const double TotalVisibleMs = (ApplyFinishedSeconds - PendingRequest.SentTimeSeconds) * 1000.0;
            const double ApproxOverheadMs =
                WorkerTimings.TotalMs >= 0.0
                    ? FMath::Max(0.0, TotalVisibleMs - WorkerTimings.TotalMs - ReadDecodeMs - PreviewApplyMs)
                    : -1.0;

            UE_LOG(
                LogGeometryNodesBridge,
                Log,
                TEXT("[GeometryNodesBridgeTiming] kind=cook req=%s actor=%s target=%s total_to_visible=%s response_wait=%s worker_total=%s worker_resolve=%s worker_inputs=%s worker_eval=%s worker_extract=%s extract_geometry_access=%s extract_build_parts=%s extract_mesh_prepare=%s extract_mesh_read=%s extract_mesh_expand=%s extract_transform=%s extract_merge=%s worker_shm_write=%s read_decode=%s preview_apply=%s overhead=%s"),
                RequestId.IsEmpty() ? TEXT("n/a") : *RequestId,
                *GetNameSafe(Component->GetOwner()),
                *Component->TargetId,
                *FormatTimingMs(TotalVisibleMs),
                *FormatTimingMs(ResponseWaitMs),
                *FormatTimingMs(WorkerTimings.TotalMs),
                *FormatTimingMs(WorkerTimings.ResolveMs),
                *FormatTimingMs(WorkerTimings.InputsMs),
                *FormatTimingMs(WorkerTimings.EvalMs),
                *FormatTimingMs(WorkerTimings.ExtractMs),
                *FormatTimingMs(WorkerTimings.ExtractGeometryAccessMs),
                *FormatTimingMs(WorkerTimings.ExtractBuildPartsMs),
                *FormatTimingMs(WorkerTimings.ExtractMeshPrepareMs),
                *FormatTimingMs(WorkerTimings.ExtractMeshReadMs),
                *FormatTimingMs(WorkerTimings.ExtractMeshExpandMs),
                *FormatTimingMs(WorkerTimings.ExtractTransformMs),
                *FormatTimingMs(WorkerTimings.ExtractMergeMs),
                *FormatTimingMs(WorkerTimings.SharedWriteMs),
                *FormatTimingMs(ReadDecodeMs),
                *FormatTimingMs(PreviewApplyMs),
                *FormatTimingMs(ApproxOverheadMs));

            FBridgeTimingHistoryRow HistoryRow;
            HistoryRow.TimestampUtc = FDateTime::UtcNow().ToIso8601();
            HistoryRow.Kind = TEXT("cook");
            HistoryRow.RunLabel = PendingRequest.RunLabel;
            HistoryRow.ScenarioId = PendingRequest.ScenarioId;
            HistoryRow.IterationIndex = PendingRequest.IterationIndex;
            HistoryRow.ActionKind = PendingRequest.ActionKind;
            HistoryRow.RequestId = RequestId;
            HistoryRow.ActorName = GetNameSafe(Component->GetOwner());
            HistoryRow.TargetId = Component->TargetId;
            HistoryRow.TotalToVisibleMs = TotalVisibleMs;
            HistoryRow.ResponseWaitMs = ResponseWaitMs;
            HistoryRow.WorkerTotalMs = WorkerTimings.TotalMs;
            HistoryRow.WorkerResolveMs = WorkerTimings.ResolveMs;
            HistoryRow.WorkerInputsMs = WorkerTimings.InputsMs;
            HistoryRow.WorkerEvalMs = WorkerTimings.EvalMs;
            HistoryRow.WorkerExtractMs = WorkerTimings.ExtractMs;
            HistoryRow.ExtractGeometryAccessMs = WorkerTimings.ExtractGeometryAccessMs;
            HistoryRow.ExtractBuildPartsMs = WorkerTimings.ExtractBuildPartsMs;
            HistoryRow.ExtractMeshPrepareMs = WorkerTimings.ExtractMeshPrepareMs;
            HistoryRow.ExtractMeshReadMs = WorkerTimings.ExtractMeshReadMs;
            HistoryRow.ExtractMeshExpandMs = WorkerTimings.ExtractMeshExpandMs;
            HistoryRow.ExtractTransformMs = WorkerTimings.ExtractTransformMs;
            HistoryRow.ExtractMergeMs = WorkerTimings.ExtractMergeMs;
            HistoryRow.WorkerSharedWriteMs = WorkerTimings.SharedWriteMs;
            HistoryRow.ReadDecodeMs = ReadDecodeMs;
            HistoryRow.PreviewApplyMs = PreviewApplyMs;
            HistoryRow.OverheadMs = ApproxOverheadMs;
            AppendTimingHistoryRow(HistoryRow);
        }
        return;
    }

    if (Message.Type == EGeometryNodesBridgeMessageType::InstanceBakeComplete)
    {
        FPendingRequest PendingRequest;
        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                PendingRequest = *ExistingRequest;
                PendingRequests.Remove(RequestId);
            }
        }

        UGeometryNodesBridgeComponent* Component = PendingRequest.Component.Get();
        if (!IsValid(Component) || !Session.IsValid())
        {
            return;
        }

        const double ResponseReceivedSeconds = FPlatformTime::Seconds();
        FBridgeWorkerTimingBreakdown WorkerTimings;
        ParseWorkerTimingBreakdown(Message.Payload, WorkerTimings);

        double SlotNumber = 0.0;
        if (!Message.Payload.IsValid() || !Message.Payload->TryGetNumberField(TEXT("slot"), SlotNumber))
        {
            Component->bPendingInstancedBake = false;
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("InstanceBakeComplete was missing a shared-memory slot."));
            return;
        }

        const double ReadDecodeStartSeconds = FPlatformTime::Seconds();
        FBridgeDecodedInstanceBake DecodedInstanceBake;
        if (!Session->GetInstanceBakeSharedMemory().ReadInstanceBake(static_cast<uint32>(SlotNumber), DecodedInstanceBake))
        {
            Component->bPendingInstancedBake = false;
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to read instanced bake data from shared memory."));
            return;
        }
        const double ReadDecodeMs = (FPlatformTime::Seconds() - ReadDecodeStartSeconds) * 1000.0;

        if (!PendingRequest.CookCacheKey.IsEmpty())
        {
            FCookedTargetCache& CachedCook = CookedTargetCaches.FindOrAdd(PendingRequest.CookCacheKey);
            CachedCook.DecodedInstanceBake = DecodedInstanceBake;
        }

        const double BakeExecuteStartSeconds = FPlatformTime::Seconds();
        const bool bBakeSucceeded = FGeometryNodesBridgeRuntimeModule::Get().ExecuteBakeBridgeComponent(Component);
        const double BakeExecuteMs = (FPlatformTime::Seconds() - BakeExecuteStartSeconds) * 1000.0;
        const double BakeFinishedSeconds = FPlatformTime::Seconds();

        if (ShouldLogCookTimings() && PendingRequest.SentTimeSeconds > 0.0)
        {
            const double ResponseWaitMs = (ResponseReceivedSeconds - PendingRequest.SentTimeSeconds) * 1000.0;
            const double TotalFinishedMs = (BakeFinishedSeconds - PendingRequest.SentTimeSeconds) * 1000.0;
            const double ApproxOverheadMs =
                WorkerTimings.TotalMs >= 0.0
                    ? FMath::Max(0.0, TotalFinishedMs - WorkerTimings.TotalMs - ReadDecodeMs - BakeExecuteMs)
                    : -1.0;

            UE_LOG(
                LogGeometryNodesBridge,
                Log,
                TEXT("[GeometryNodesBridgeTiming] kind=instance_bake req=%s actor=%s target=%s total_to_finish=%s response_wait=%s worker_total=%s worker_resolve=%s worker_inputs=%s worker_eval=%s worker_extract=%s extract_geometry_access=%s extract_build_parts=%s extract_mesh_prepare=%s extract_mesh_read=%s extract_mesh_expand=%s extract_transform=%s extract_merge=%s worker_serialize=%s worker_shm_write=%s read_decode=%s bake_execute=%s overhead=%s result=%s"),
                RequestId.IsEmpty() ? TEXT("n/a") : *RequestId,
                *GetNameSafe(Component->GetOwner()),
                *Component->TargetId,
                *FormatTimingMs(TotalFinishedMs),
                *FormatTimingMs(ResponseWaitMs),
                *FormatTimingMs(WorkerTimings.TotalMs),
                *FormatTimingMs(WorkerTimings.ResolveMs),
                *FormatTimingMs(WorkerTimings.InputsMs),
                *FormatTimingMs(WorkerTimings.EvalMs),
                *FormatTimingMs(WorkerTimings.ExtractMs),
                *FormatTimingMs(WorkerTimings.ExtractGeometryAccessMs),
                *FormatTimingMs(WorkerTimings.ExtractBuildPartsMs),
                *FormatTimingMs(WorkerTimings.ExtractMeshPrepareMs),
                *FormatTimingMs(WorkerTimings.ExtractMeshReadMs),
                *FormatTimingMs(WorkerTimings.ExtractMeshExpandMs),
                *FormatTimingMs(WorkerTimings.ExtractTransformMs),
                *FormatTimingMs(WorkerTimings.ExtractMergeMs),
                *FormatTimingMs(WorkerTimings.SerializeMs),
                *FormatTimingMs(WorkerTimings.SharedWriteMs),
                *FormatTimingMs(ReadDecodeMs),
                *FormatTimingMs(BakeExecuteMs),
                *FormatTimingMs(ApproxOverheadMs),
                bBakeSucceeded ? TEXT("success") : TEXT("failure"));

            FBridgeTimingHistoryRow HistoryRow;
            HistoryRow.TimestampUtc = FDateTime::UtcNow().ToIso8601();
            HistoryRow.Kind = TEXT("instance_bake");
            HistoryRow.RunLabel = PendingRequest.RunLabel;
            HistoryRow.ScenarioId = PendingRequest.ScenarioId;
            HistoryRow.IterationIndex = PendingRequest.IterationIndex;
            HistoryRow.ActionKind = PendingRequest.ActionKind;
            HistoryRow.RequestId = RequestId;
            HistoryRow.ActorName = GetNameSafe(Component->GetOwner());
            HistoryRow.TargetId = Component->TargetId;
            HistoryRow.Result = bBakeSucceeded ? TEXT("success") : TEXT("failure");
            HistoryRow.TotalToFinishMs = TotalFinishedMs;
            HistoryRow.ResponseWaitMs = ResponseWaitMs;
            HistoryRow.WorkerTotalMs = WorkerTimings.TotalMs;
            HistoryRow.WorkerResolveMs = WorkerTimings.ResolveMs;
            HistoryRow.WorkerInputsMs = WorkerTimings.InputsMs;
            HistoryRow.WorkerEvalMs = WorkerTimings.EvalMs;
            HistoryRow.WorkerExtractMs = WorkerTimings.ExtractMs;
            HistoryRow.ExtractGeometryAccessMs = WorkerTimings.ExtractGeometryAccessMs;
            HistoryRow.ExtractBuildPartsMs = WorkerTimings.ExtractBuildPartsMs;
            HistoryRow.ExtractMeshPrepareMs = WorkerTimings.ExtractMeshPrepareMs;
            HistoryRow.ExtractMeshReadMs = WorkerTimings.ExtractMeshReadMs;
            HistoryRow.ExtractMeshExpandMs = WorkerTimings.ExtractMeshExpandMs;
            HistoryRow.ExtractTransformMs = WorkerTimings.ExtractTransformMs;
            HistoryRow.ExtractMergeMs = WorkerTimings.ExtractMergeMs;
            HistoryRow.WorkerSerializeMs = WorkerTimings.SerializeMs;
            HistoryRow.WorkerSharedWriteMs = WorkerTimings.SharedWriteMs;
            HistoryRow.ReadDecodeMs = ReadDecodeMs;
            HistoryRow.BakeExecuteMs = BakeExecuteMs;
            HistoryRow.OverheadMs = ApproxOverheadMs;
            AppendTimingHistoryRow(HistoryRow);
        }

        if (bBakeSucceeded)
        {
            Component->bPendingInstancedBake = false;
            return;
        }

        Component->bPendingInstancedBake = false;
        if (Component->ComponentStatus != EGeometryNodesBridgeComponentStatus::Error)
        {
            Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, TEXT("Failed to bake the bridge output as instanced geometry."));
        }
        return;
    }

    if (Message.Type == EGeometryNodesBridgeMessageType::ResizeRequired)
    {
        uint64 RequiredSlotBytes = 0;
        double RequiredBytes = 0.0;
        if (Message.Payload.IsValid() && Message.Payload->TryGetNumberField(TEXT("required_bytes"), RequiredBytes))
        {
            RequiredSlotBytes = static_cast<uint64>(FMath::Max(RequiredBytes, 0.0));
        }

        UGeometryNodesBridgeComponent* RetryComponent = nullptr;
        EBridgeManagerRequestKind RetryKind = EBridgeManagerRequestKind::CookTarget;
        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                RetryComponent = ExistingRequest->Component.Get();
                RetryKind = ExistingRequest->Kind;
                PendingRequests.Remove(RequestId);
            }
        }

        RestartSessionForResize(RetryComponent, RetryKind, RequiredSlotBytes);
        return;
    }

    if (Message.Type == EGeometryNodesBridgeMessageType::Error)
    {
        FString ErrorMessage = TEXT("Blender worker returned an error.");
        if (Message.Payload.IsValid())
        {
            Message.Payload->TryGetStringField(TEXT("message"), ErrorMessage);
        }

        if (!RequestId.IsEmpty())
        {
            if (FPendingRequest* ExistingRequest = PendingRequests.Find(RequestId))
            {
                if (UGeometryNodesBridgeComponent* Component = ExistingRequest->Component.Get())
                {
                    if (ExistingRequest->Kind == EBridgeManagerRequestKind::RequestInstanceBake ||
                        ExistingRequest->bRequestInstanceBakeAfterDescribe)
                    {
                        Component->bPendingInstancedBake = false;
                    }
                    Component->SetBridgeStatus(EGeometryNodesBridgeComponentStatus::Error, ErrorMessage);
                }
                PendingRequests.Remove(RequestId);
                return;
            }
        }

        LastSessionMessage = ErrorMessage;
        return;
    }
}

void FGeometryNodesBridgeSessionManager::HandleSessionLogMessage(const FString& Message)
{
    LastSessionMessage = Message;
}

