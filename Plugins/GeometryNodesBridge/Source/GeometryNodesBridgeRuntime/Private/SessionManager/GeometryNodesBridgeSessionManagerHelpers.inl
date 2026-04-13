namespace
{
    constexpr TCHAR RequestIdFieldName[] = TEXT("request_id");
    constexpr TCHAR SourceHandleFieldName[] = TEXT("source_handle");
    constexpr TCHAR BlendFileFieldName[] = TEXT("blend_file");
    constexpr TCHAR TargetIdFieldName[] = TEXT("target_id");
    constexpr TCHAR ComponentHandleFieldName[] = TEXT("component_handle");
    constexpr TCHAR CurveHashFieldName[] = TEXT("curve_hash");
    constexpr TCHAR ForceReloadFieldName[] = TEXT("force_reload");
    constexpr TCHAR CacheKeyDelimiter[] = TEXT("|");
    constexpr TCHAR TimingWorkerTotalFieldName[] = TEXT("timing_worker_total_ms");
    constexpr TCHAR TimingWorkerResolveFieldName[] = TEXT("timing_worker_resolve_ms");
    constexpr TCHAR TimingWorkerInputsFieldName[] = TEXT("timing_worker_inputs_ms");
    constexpr TCHAR TimingWorkerEvalFieldName[] = TEXT("timing_worker_eval_ms");
    constexpr TCHAR TimingWorkerExtractFieldName[] = TEXT("timing_worker_extract_ms");
    constexpr TCHAR TimingWorkerExtractGeometryAccessFieldName[] = TEXT("timing_worker_extract_geometry_access_ms");
    constexpr TCHAR TimingWorkerExtractBuildPartsFieldName[] = TEXT("timing_worker_extract_build_parts_ms");
    constexpr TCHAR TimingWorkerExtractMeshPrepareFieldName[] = TEXT("timing_worker_extract_mesh_prepare_ms");
    constexpr TCHAR TimingWorkerExtractMeshReadFieldName[] = TEXT("timing_worker_extract_mesh_read_ms");
    constexpr TCHAR TimingWorkerExtractMeshExpandFieldName[] = TEXT("timing_worker_extract_mesh_expand_ms");
    constexpr TCHAR TimingWorkerExtractTransformFieldName[] = TEXT("timing_worker_extract_transform_ms");
    constexpr TCHAR TimingWorkerExtractMergeFieldName[] = TEXT("timing_worker_extract_merge_ms");
    constexpr TCHAR TimingWorkerSerializeFieldName[] = TEXT("timing_worker_serialize_ms");
    constexpr TCHAR TimingWorkerSharedWriteFieldName[] = TEXT("timing_worker_shared_write_ms");
    constexpr TCHAR TimingHistoryFileName[] = TEXT("GeometryNodesBridgeTimingHistory.csv");

    TAutoConsoleVariable<int32> CVarGeometryNodesBridgeLogCookTimings(
        TEXT("blenderbridge.LogCookTimings"),
        0,
        TEXT("Logs per-request timing breakdowns for Geometry Nodes Bridge cook and instance-bake requests."),
        ECVF_Default);

    struct FBridgeWorkerTimingBreakdown
    {
        double TotalMs = -1.0;
        double ResolveMs = -1.0;
        double InputsMs = -1.0;
        double EvalMs = -1.0;
        double ExtractMs = -1.0;
        double ExtractGeometryAccessMs = -1.0;
        double ExtractBuildPartsMs = -1.0;
        double ExtractMeshPrepareMs = -1.0;
        double ExtractMeshReadMs = -1.0;
        double ExtractMeshExpandMs = -1.0;
        double ExtractTransformMs = -1.0;
        double ExtractMergeMs = -1.0;
        double SerializeMs = -1.0;
        double SharedWriteMs = -1.0;
    };

    struct FBridgeTimingHistoryRow
    {
        FString TimestampUtc;
        FString Kind;
        bool bCacheHit = false;
        FString RunLabel;
        FString ScenarioId;
        int32 IterationIndex = INDEX_NONE;
        FString ActionKind;
        FString RequestId;
        FString ActorName;
        FString TargetId;
        FString Result;
        double TotalToVisibleMs = -1.0;
        double TotalToFinishMs = -1.0;
        double ResponseWaitMs = -1.0;
        double WorkerTotalMs = -1.0;
        double WorkerResolveMs = -1.0;
        double WorkerInputsMs = -1.0;
        double WorkerEvalMs = -1.0;
        double WorkerExtractMs = -1.0;
        double ExtractGeometryAccessMs = -1.0;
        double ExtractBuildPartsMs = -1.0;
        double ExtractMeshPrepareMs = -1.0;
        double ExtractMeshReadMs = -1.0;
        double ExtractMeshExpandMs = -1.0;
        double ExtractTransformMs = -1.0;
        double ExtractMergeMs = -1.0;
        double WorkerSerializeMs = -1.0;
        double WorkerSharedWriteMs = -1.0;
        double ReadDecodeMs = -1.0;
        double PreviewApplyMs = -1.0;
        double BakeExecuteMs = -1.0;
        double OverheadMs = -1.0;
    };

    bool ShouldLogCookTimings()
    {
        static bool bInitializedFromCommandLine = false;
        static bool bCommandLineOverrideEnabled = false;
        static bool bAutomationRunEnabled = false;

        if (!bInitializedFromCommandLine)
        {
            bInitializedFromCommandLine = true;
            const FString CommandLine = FCommandLine::Get();

            FString ExplicitValue;
            if (FParse::Value(FCommandLine::Get(), TEXT("GeometryNodesBridgeLogCookTimings="), ExplicitValue))
            {
                ExplicitValue.TrimQuotesInline();
                bCommandLineOverrideEnabled =
                    ExplicitValue == TEXT("1") ||
                    ExplicitValue.Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
                    ExplicitValue.Equals(TEXT("yes"), ESearchCase::IgnoreCase) ||
                    ExplicitValue.Equals(TEXT("on"), ESearchCase::IgnoreCase);
            }
            else if (FParse::Param(FCommandLine::Get(), TEXT("GeometryNodesBridgeLogCookTimings")))
            {
                bCommandLineOverrideEnabled = true;
            }
            else if (CommandLine.Contains(TEXT("GeometryNodesBridgeLogCookTimings"), ESearchCase::IgnoreCase))
            {
                bCommandLineOverrideEnabled = !CommandLine.Contains(TEXT("GeometryNodesBridgeLogCookTimings=0"), ESearchCase::IgnoreCase);
            }

            FString AutomationRunLabel;
            if (FParse::Value(FCommandLine::Get(), TEXT("GeometryNodesBridgeAutomationRunLabel="), AutomationRunLabel))
            {
                AutomationRunLabel.TrimQuotesInline();
                bAutomationRunEnabled = !AutomationRunLabel.IsEmpty();
            }
            else
            {
                bAutomationRunEnabled = CommandLine.Contains(TEXT("GeometryNodesBridgeAutomationRunLabel="), ESearchCase::IgnoreCase);
            }
        }

        return bAutomationRunEnabled || bCommandLineOverrideEnabled || CVarGeometryNodesBridgeLogCookTimings.GetValueOnGameThread() != 0;
    }

    bool TryGetTimingFieldMs(const TSharedPtr<FJsonObject>& Payload, const TCHAR* FieldName, double& OutValueMs)
    {
        OutValueMs = -1.0;
        return Payload.IsValid() && Payload->TryGetNumberField(FieldName, OutValueMs);
    }

    void ParseWorkerTimingBreakdown(const TSharedPtr<FJsonObject>& Payload, FBridgeWorkerTimingBreakdown& OutTimings)
    {
        TryGetTimingFieldMs(Payload, TimingWorkerTotalFieldName, OutTimings.TotalMs);
        TryGetTimingFieldMs(Payload, TimingWorkerResolveFieldName, OutTimings.ResolveMs);
        TryGetTimingFieldMs(Payload, TimingWorkerInputsFieldName, OutTimings.InputsMs);
        TryGetTimingFieldMs(Payload, TimingWorkerEvalFieldName, OutTimings.EvalMs);
        TryGetTimingFieldMs(Payload, TimingWorkerExtractFieldName, OutTimings.ExtractMs);
        TryGetTimingFieldMs(Payload, TimingWorkerExtractGeometryAccessFieldName, OutTimings.ExtractGeometryAccessMs);
        TryGetTimingFieldMs(Payload, TimingWorkerExtractBuildPartsFieldName, OutTimings.ExtractBuildPartsMs);
        TryGetTimingFieldMs(Payload, TimingWorkerExtractMeshPrepareFieldName, OutTimings.ExtractMeshPrepareMs);
        TryGetTimingFieldMs(Payload, TimingWorkerExtractMeshReadFieldName, OutTimings.ExtractMeshReadMs);
        TryGetTimingFieldMs(Payload, TimingWorkerExtractMeshExpandFieldName, OutTimings.ExtractMeshExpandMs);
        TryGetTimingFieldMs(Payload, TimingWorkerExtractTransformFieldName, OutTimings.ExtractTransformMs);
        TryGetTimingFieldMs(Payload, TimingWorkerExtractMergeFieldName, OutTimings.ExtractMergeMs);
        TryGetTimingFieldMs(Payload, TimingWorkerSerializeFieldName, OutTimings.SerializeMs);
        TryGetTimingFieldMs(Payload, TimingWorkerSharedWriteFieldName, OutTimings.SharedWriteMs);
    }

    void PopulateTimingHistoryMetadataFromComponent(const UGeometryNodesBridgeComponent* Component, FBridgeTimingHistoryRow& OutRow)
    {
        if (Component == nullptr)
        {
            return;
        }

        OutRow.RunLabel = Component->GetAutomationRunLabel();
        OutRow.ScenarioId = Component->GetAutomationScenarioId();
        OutRow.IterationIndex = Component->GetAutomationIterationIndex();
        OutRow.ActionKind = Component->GetAutomationActionKind();
    }

    FString FormatTimingMs(const double TimingMs)
    {
        return TimingMs >= 0.0
            ? FString::Printf(TEXT("%.2fms"), TimingMs)
            : FString(TEXT("n/a"));
    }

    FString FormatTimingCsvValue(const double TimingMs)
    {
        return TimingMs >= 0.0
            ? FString::Printf(TEXT("%.3f"), TimingMs)
            : FString();
    }

    FString EscapeCsvField(FString Value)
    {
        const bool bNeedsQuotes =
            Value.Contains(TEXT(",")) ||
            Value.Contains(TEXT("\"")) ||
            Value.Contains(TEXT("\n")) ||
            Value.Contains(TEXT("\r"));

        Value.ReplaceInline(TEXT("\""), TEXT("\"\""));
        return bNeedsQuotes ? FString::Printf(TEXT("\"%s\""), *Value) : Value;
    }

    FString GetTimingHistoryFilePath()
    {
        FString OutputDirectory;
        if (const UGeometryNodesBridgeSettings* Settings = GetDefault<UGeometryNodesBridgeSettings>())
        {
            OutputDirectory = Settings->TimingHistoryOutputDirectory.Path;
        }

        if (OutputDirectory.IsEmpty())
        {
            OutputDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
        }
        else
        {
            OutputDirectory = FPaths::ConvertRelativePathToFull(OutputDirectory);
        }

        FPaths::NormalizeDirectoryName(OutputDirectory);
        return FPaths::Combine(OutputDirectory, TimingHistoryFileName);
    }

    void AppendTimingHistoryRow(const FBridgeTimingHistoryRow& Row)
    {
        const FString FilePath = GetTimingHistoryFilePath();
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);
        const FString Header =
            TEXT("timestamp_utc,kind,cache_hit,run_label,scenario_id,iteration_index,action_kind,request_id,actor,target,result,total_to_visible_ms,total_to_finish_ms,response_wait_ms,")
            TEXT("worker_total_ms,worker_resolve_ms,worker_inputs_ms,worker_eval_ms,worker_extract_ms,")
            TEXT("extract_geometry_access_ms,extract_build_parts_ms,extract_mesh_prepare_ms,extract_mesh_read_ms,")
            TEXT("extract_mesh_expand_ms,extract_transform_ms,extract_merge_ms,worker_serialize_ms,worker_shared_write_ms,")
            TEXT("read_decode_ms,preview_apply_ms,bake_execute_ms,overhead_ms");

        bool bFileExists = IFileManager::Get().FileExists(*FilePath);
        if (bFileExists)
        {
            FString ExistingContents;
            if (FFileHelper::LoadFileToString(ExistingContents, *FilePath))
            {
                FString ExistingHeader;
                if (ExistingContents.Split(TEXT("\n"), &ExistingHeader, nullptr))
                {
                    ExistingHeader.TrimEndInline();
                    if (ExistingHeader != Header)
                    {
                        const FString LegacyFilePath = FPaths::Combine(
                            FPaths::GetPath(FilePath),
                            FString::Printf(
                                TEXT("GeometryNodesBridgeTimingHistory_Legacy_%s.csv"),
                                *FDateTime::UtcNow().ToString(TEXT("%Y%m%dT%H%M%SZ"))));
                        if (IFileManager::Get().Move(*LegacyFilePath, *FilePath, true, true))
                        {
                            UE_LOG(LogGeometryNodesBridge, Warning, TEXT("Moved legacy timing history to %s due to a CSV schema change."), *LegacyFilePath);
                            bFileExists = false;
                        }
                    }
                }
            }
        }

        TArray<FString> Columns;
        Columns.Reserve(32);
        Columns.Add(EscapeCsvField(Row.TimestampUtc));
        Columns.Add(EscapeCsvField(Row.Kind));
        Columns.Add(Row.bCacheHit ? TEXT("true") : TEXT("false"));
        Columns.Add(EscapeCsvField(Row.RunLabel));
        Columns.Add(EscapeCsvField(Row.ScenarioId));
        Columns.Add(Row.IterationIndex >= 0 ? LexToString(Row.IterationIndex) : FString());
        Columns.Add(EscapeCsvField(Row.ActionKind));
        Columns.Add(EscapeCsvField(Row.RequestId));
        Columns.Add(EscapeCsvField(Row.ActorName));
        Columns.Add(EscapeCsvField(Row.TargetId));
        Columns.Add(EscapeCsvField(Row.Result));
        Columns.Add(FormatTimingCsvValue(Row.TotalToVisibleMs));
        Columns.Add(FormatTimingCsvValue(Row.TotalToFinishMs));
        Columns.Add(FormatTimingCsvValue(Row.ResponseWaitMs));
        Columns.Add(FormatTimingCsvValue(Row.WorkerTotalMs));
        Columns.Add(FormatTimingCsvValue(Row.WorkerResolveMs));
        Columns.Add(FormatTimingCsvValue(Row.WorkerInputsMs));
        Columns.Add(FormatTimingCsvValue(Row.WorkerEvalMs));
        Columns.Add(FormatTimingCsvValue(Row.WorkerExtractMs));
        Columns.Add(FormatTimingCsvValue(Row.ExtractGeometryAccessMs));
        Columns.Add(FormatTimingCsvValue(Row.ExtractBuildPartsMs));
        Columns.Add(FormatTimingCsvValue(Row.ExtractMeshPrepareMs));
        Columns.Add(FormatTimingCsvValue(Row.ExtractMeshReadMs));
        Columns.Add(FormatTimingCsvValue(Row.ExtractMeshExpandMs));
        Columns.Add(FormatTimingCsvValue(Row.ExtractTransformMs));
        Columns.Add(FormatTimingCsvValue(Row.ExtractMergeMs));
        Columns.Add(FormatTimingCsvValue(Row.WorkerSerializeMs));
        Columns.Add(FormatTimingCsvValue(Row.WorkerSharedWriteMs));
        Columns.Add(FormatTimingCsvValue(Row.ReadDecodeMs));
        Columns.Add(FormatTimingCsvValue(Row.PreviewApplyMs));
        Columns.Add(FormatTimingCsvValue(Row.BakeExecuteMs));
        Columns.Add(FormatTimingCsvValue(Row.OverheadMs));
        const FString RowLine = FString::Join(Columns, TEXT(","));
        const FString FileContents = bFileExists
            ? FString::Printf(TEXT("%s%s"), *RowLine, LINE_TERMINATOR)
            : FString::Printf(TEXT("%s%s%s%s"), *Header, LINE_TERMINATOR, *RowLine, LINE_TERMINATOR);

        if (!FFileHelper::SaveStringToFile(
                FileContents,
                *FilePath,
                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,
                &IFileManager::Get(),
                FILEWRITE_Append))
        {
            UE_LOG(LogGeometryNodesBridge, Warning, TEXT("Failed to append timing history to %s"), *FilePath);
        }
    }

    uint64 ComputeExpandedSharedMemoryBytes(const uint64 CurrentTotalBytes, const uint64 RequiredSlotBytes)
    {
        const uint64 MinRequiredTotalBytes = sizeof(FBridgeSharedHeader) + (RequiredSlotBytes * GeometryNodesBridge::SharedSlotCount);
        uint64 ExpandedBytes = FMath::Max(CurrentTotalBytes, GeometryNodesBridge::DefaultSharedMemoryBytes);

        while (ExpandedBytes < MinRequiredTotalBytes)
        {
            ExpandedBytes *= 2ull;
        }

        return ExpandedBytes;
    }

    template <typename CallbackType>
    void ForEachBridgeComponent(const CallbackType& Callback)
    {
        for (TObjectIterator<UGeometryNodesBridgeComponent> It; It; ++It)
        {
            UGeometryNodesBridgeComponent* Component = *It;
            if (!IsValid(Component) || Component->HasAnyFlags(RF_ClassDefaultObject))
            {
                continue;
            }

            const AActor* Owner = Component->GetOwner();
            if (!IsValid(Owner) || Owner->HasAnyFlags(RF_ClassDefaultObject) || Owner->bIsEditorPreviewActor)
            {
                continue;
            }

            const UWorld* World = Owner->GetWorld();
            if (World == nullptr || World->WorldType == EWorldType::EditorPreview || World->WorldType == EWorldType::Inactive)
            {
                continue;
            }

            Callback(Component);
        }
    }

    FString GetComponentBlendFilePath(const UGeometryNodesBridgeComponent* Component)
    {
        if (Component == nullptr || Component->Source == nullptr)
        {
            return FString();
        }

        return FPaths::ConvertRelativePathToFull(Component->Source->BlendFile.FilePath);
    }

    bool TryResolveSourceHandleAndBlendFilePath(
        const UGeometryNodesBridgeSource* Source,
        FString& OutSourceHandle,
        FString& OutBlendFilePath)
    {
        OutSourceHandle.Reset();
        OutBlendFilePath.Reset();

        if (Source == nullptr || Source->BlendFile.FilePath.IsEmpty())
        {
            return false;
        }

        OutBlendFilePath = FPaths::ConvertRelativePathToFull(Source->BlendFile.FilePath);
        FPaths::NormalizeFilename(OutBlendFilePath);
        OutSourceHandle = OutBlendFilePath;
        return !OutSourceHandle.IsEmpty();
    }

    bool TryGetRequestId(const TSharedPtr<FJsonObject>& Payload, FString& OutRequestId)
    {
        OutRequestId.Reset();
        return Payload.IsValid() && Payload->TryGetStringField(RequestIdFieldName, OutRequestId) && !OutRequestId.IsEmpty();
    }

    bool IsRetryComponent(const TArray<TWeakObjectPtr<UGeometryNodesBridgeComponent>>& PendingRetryComponents, const UGeometryNodesBridgeComponent* Component)
    {
        return PendingRetryComponents.ContainsByPredicate([Component](const TWeakObjectPtr<UGeometryNodesBridgeComponent>& Entry)
        {
            return Entry.IsValid() && Entry.Get() == Component;
        });
    }

    FString MakeTargetSchemaCacheKey(const FString& SourceHandle, const FString& TargetId)
    {
        return FString::Printf(TEXT("%s%s%s"), *SourceHandle, CacheKeyDelimiter, *TargetId);
    }

    bool SerializeJsonObject(const TSharedRef<FJsonObject>& JsonObject, FString& OutSerializedJson)
    {
        OutSerializedJson.Reset();

        using FBridgeCacheJsonWriter = TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>;
        TSharedRef<FBridgeCacheJsonWriter> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutSerializedJson);
        return FJsonSerializer::Serialize(JsonObject, Writer);
    }

    bool BuildCookCacheKey(
        const FString& SourceHandle,
        const FString& TargetId,
        const TArray<FBridgeInputSchema>& Schema,
        const FInstancedPropertyBag& InputValues,
        const FString& CurveHash,
        const FString& GeometryBindingsHash,
        FString& OutCookCacheKey)
    {
        OutCookCacheKey.Reset();

        if (SourceHandle.IsEmpty() || TargetId.IsEmpty())
        {
            return false;
        }

        TSharedRef<FJsonObject> ValuesPayload = MakeShared<FJsonObject>();
        GeometryNodesBridge::AddInputValuesToPayload(Schema, InputValues, ValuesPayload);

        FString SerializedValuesPayload;
        if (!SerializeJsonObject(ValuesPayload, SerializedValuesPayload))
        {
            return false;
        }

        OutCookCacheKey = FString::Printf(
            TEXT("%s%s%s%s%s%s%s%s%s"),
            *SourceHandle,
            CacheKeyDelimiter,
            *TargetId,
            CacheKeyDelimiter,
            *SerializedValuesPayload,
            CacheKeyDelimiter,
            *CurveHash,
            CacheKeyDelimiter,
            *GeometryBindingsHash);
        return true;
    }

    bool NormalizeExecutableCandidate(FString& InOutPath)
    {
        InOutPath = InOutPath.TrimStartAndEnd();
        if (InOutPath.IsEmpty())
        {
            return false;
        }

        FPaths::NormalizeFilename(InOutPath);
        if (FPaths::DirectoryExists(InOutPath))
        {
            InOutPath = FPaths::Combine(InOutPath, TEXT("blender.exe"));
        }

        return IFileManager::Get().FileExists(*InOutPath);
    }

    TArray<int32> ParseVersionKey(const FString& DirectoryName)
    {
        FString VersionText = DirectoryName;
        VersionText.RemoveFromStart(TEXT("Blender "));

        TArray<FString> Parts;
        VersionText.ParseIntoArray(Parts, TEXT("."), true);

        TArray<int32> VersionParts;
        VersionParts.Reserve(Parts.Num());
        for (const FString& Part : Parts)
        {
            VersionParts.Add(FCString::Atoi(*Part));
        }

        return VersionParts;
    }

    bool IsVersionKeyGreater(const TArray<int32>& Left, const TArray<int32>& Right)
    {
        const int32 MaxCount = FMath::Max(Left.Num(), Right.Num());
        for (int32 Index = 0; Index < MaxCount; ++Index)
        {
            const int32 LeftValue = Left.IsValidIndex(Index) ? Left[Index] : 0;
            const int32 RightValue = Right.IsValidIndex(Index) ? Right[Index] : 0;
            if (LeftValue != RightValue)
            {
                return LeftValue > RightValue;
            }
        }

        return false;
    }

    FString FindDefaultBlenderExecutable()
    {
        const FString BaseDirectory = TEXT("C:/Program Files/Blender Foundation");
        if (!IFileManager::Get().DirectoryExists(*BaseDirectory))
        {
            return FString();
        }

        TArray<FString> CandidateDirectories;
        IFileManager::Get().FindFiles(CandidateDirectories, *(BaseDirectory / TEXT("Blender *")), false, true);

        FString BestExecutablePath;
        TArray<int32> BestVersionKey;
        for (const FString& DirectoryName : CandidateDirectories)
        {
            FString CandidatePath = FPaths::Combine(BaseDirectory, DirectoryName, TEXT("blender.exe"));
            if (!IFileManager::Get().FileExists(*CandidatePath))
            {
                continue;
            }

            const TArray<int32> CandidateVersionKey = ParseVersionKey(DirectoryName);
            if (BestExecutablePath.IsEmpty() || IsVersionKeyGreater(CandidateVersionKey, BestVersionKey))
            {
                BestExecutablePath = MoveTemp(CandidatePath);
                BestVersionKey = CandidateVersionKey;
            }
        }

        return BestExecutablePath;
    }

    FString ResolveBlenderExecutablePath(UGeometryNodesBridgeSettings* Settings, FString& OutResolutionNote)
    {
        OutResolutionNote.Reset();

        FString ConfiguredPath = Settings != nullptr ? Settings->BlenderExecutablePath.FilePath : FString();
        if (NormalizeExecutableCandidate(ConfiguredPath))
        {
            return ConfiguredPath;
        }

        const FString AutoDetectedPath = FindDefaultBlenderExecutable();
        if (!AutoDetectedPath.IsEmpty())
        {
            OutResolutionNote = FString::Printf(TEXT("Auto-detected Blender executable: %s"), *AutoDetectedPath);

            if (Settings != nullptr)
            {
                Settings->BlenderExecutablePath.FilePath = AutoDetectedPath;
                Settings->SaveConfig();
            }

            return AutoDetectedPath;
        }

        OutResolutionNote = TEXT("Blender executable path is not configured, and no default Blender install was found.");
        return FString();
    }

    bool IsEditorBridgeSupported()
    {
        return GIsEditor;
    }

    void SetEditorOnlyUnsupported(UGeometryNodesBridgeComponent* Component)
    {
        if (IsValid(Component))
        {
            Component->ComponentStatus = EGeometryNodesBridgeComponentStatus::Error;
            Component->LastStatusMessage = TEXT("Geometry Nodes Bridge shared sessions are only supported in the Unreal Editor.");
        }
    }
}

