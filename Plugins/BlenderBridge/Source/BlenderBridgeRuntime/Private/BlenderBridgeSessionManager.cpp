#include "BlenderBridgeSessionManager.h"

#include "BlenderBridgeComponent.h"
#include "BlenderBridgeMessages.h"
#include "BlenderBridgeRuntimeModule.h"
#include "BlenderBridgeSettings.h"
#include "BlenderBridgeSource.h"
#include "BlenderBridgeWorkerSession.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformTime.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/UObjectIterator.h"

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
    constexpr TCHAR TimingHistoryFileName[] = TEXT("BlenderBridgeTimingHistory.csv");

    TAutoConsoleVariable<int32> CVarBlenderBridgeLogCookTimings(
        TEXT("blenderbridge.LogCookTimings"),
        0,
        TEXT("Logs per-request timing breakdowns for Blender Bridge cook and instance-bake requests."),
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

        if (!bInitializedFromCommandLine)
        {
            bInitializedFromCommandLine = true;

            FString ExplicitValue;
            if (FParse::Value(FCommandLine::Get(), TEXT("BlenderBridgeLogCookTimings="), ExplicitValue))
            {
                ExplicitValue.TrimQuotesInline();
                bCommandLineOverrideEnabled =
                    ExplicitValue == TEXT("1") ||
                    ExplicitValue.Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
                    ExplicitValue.Equals(TEXT("yes"), ESearchCase::IgnoreCase) ||
                    ExplicitValue.Equals(TEXT("on"), ESearchCase::IgnoreCase);
            }
            else if (FParse::Param(FCommandLine::Get(), TEXT("BlenderBridgeLogCookTimings")))
            {
                bCommandLineOverrideEnabled = true;
            }
        }

        return bCommandLineOverrideEnabled || CVarBlenderBridgeLogCookTimings.GetValueOnGameThread() != 0;
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

    void PopulateTimingHistoryMetadataFromComponent(const UBlenderBridgeComponent* Component, FBridgeTimingHistoryRow& OutRow)
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
        if (const UBlenderBridgeSettings* Settings = GetDefault<UBlenderBridgeSettings>())
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
                                TEXT("BlenderBridgeTimingHistory_Legacy_%s.csv"),
                                *FDateTime::UtcNow().ToString(TEXT("%Y%m%dT%H%M%SZ"))));
                        if (IFileManager::Get().Move(*LegacyFilePath, *FilePath, true, true))
                        {
                            UE_LOG(LogBlenderBridge, Warning, TEXT("Moved legacy timing history to %s due to a CSV schema change."), *LegacyFilePath);
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
            UE_LOG(LogBlenderBridge, Warning, TEXT("Failed to append timing history to %s"), *FilePath);
        }
    }

    uint64 ComputeExpandedSharedMemoryBytes(const uint64 CurrentTotalBytes, const uint64 RequiredSlotBytes)
    {
        const uint64 MinRequiredTotalBytes = sizeof(FBridgeSharedHeader) + (RequiredSlotBytes * BlenderBridge::SharedSlotCount);
        uint64 ExpandedBytes = FMath::Max(CurrentTotalBytes, BlenderBridge::DefaultSharedMemoryBytes);

        while (ExpandedBytes < MinRequiredTotalBytes)
        {
            ExpandedBytes *= 2ull;
        }

        return ExpandedBytes;
    }

    template <typename CallbackType>
    void ForEachBridgeComponent(const CallbackType& Callback)
    {
        for (TObjectIterator<UBlenderBridgeComponent> It; It; ++It)
        {
            UBlenderBridgeComponent* Component = *It;
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

    FString GetComponentBlendFilePath(const UBlenderBridgeComponent* Component)
    {
        if (Component == nullptr || Component->Source == nullptr)
        {
            return FString();
        }

        return FPaths::ConvertRelativePathToFull(Component->Source->BlendFile.FilePath);
    }

    bool TryResolveSourceHandleAndBlendFilePath(
        const UBlenderBridgeSource* Source,
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

    bool IsRetryComponent(const TArray<TWeakObjectPtr<UBlenderBridgeComponent>>& PendingRetryComponents, const UBlenderBridgeComponent* Component)
    {
        return PendingRetryComponents.ContainsByPredicate([Component](const TWeakObjectPtr<UBlenderBridgeComponent>& Entry)
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
        BlenderBridge::AddInputValuesToPayload(Schema, InputValues, ValuesPayload);

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

    FString ResolveBlenderExecutablePath(UBlenderBridgeSettings* Settings, FString& OutResolutionNote)
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

    void SetEditorOnlyUnsupported(UBlenderBridgeComponent* Component)
    {
        if (IsValid(Component))
        {
            Component->ComponentStatus = EBlenderBridgeComponentStatus::Error;
            Component->LastStatusMessage = TEXT("Blender Bridge shared sessions are only supported in the Unreal Editor.");
        }
    }
}

FBlenderBridgeSessionManager::FBlenderBridgeSessionManager() = default;

FBlenderBridgeSessionManager::~FBlenderBridgeSessionManager()
{
    Shutdown();
}

bool FBlenderBridgeSessionManager::CreateSession()
{
    if (!IsEditorBridgeSupported())
    {
        LastSessionMessage = TEXT("Blender Bridge shared sessions are only supported in the Unreal Editor.");
        SessionState = EBlenderBridgeConnectionState::Error;
        RefreshAllComponentConnectionStates();
        return false;
    }

    if (Session.IsValid() && SessionState == EBlenderBridgeConnectionState::Error)
    {
        LastSessionMessage = TEXT("Restarting the global Blender session after the previous error.");
        RestartSession();
        return Session.IsValid();
    }

    if (Session.IsValid() && SessionState != EBlenderBridgeConnectionState::Disconnected)
    {
        LastSessionMessage = TEXT("Global Blender session is already running.");
        return true;
    }

    StartSessionInternal();
    return Session.IsValid();
}

void FBlenderBridgeSessionManager::StopSession()
{
    PendingRequests.Reset();
    PendingCookRetryComponents.Reset();
    PendingInstanceBakeRetryComponents.Reset();
    PendingWarmupPreviewComponents.Reset();
    TargetSchemaCaches.Reset();
    CookedTargetCaches.Reset();
    bRestoreSourcesAfterReconnect = false;

    if (Session.IsValid())
    {
        Session->OnStateChanged.RemoveAll(this);
        Session->OnMessageReceived.RemoveAll(this);
        Session->OnLogMessage.RemoveAll(this);
        Session->Stop();
        Session.Reset();
    }

    for (TPair<FString, FSourceCache>& Entry : SourceCaches)
    {
        Entry.Value.bLoaded = false;
    }

    SessionState = EBlenderBridgeConnectionState::Disconnected;
    LastSessionMessage = TEXT("Global Blender session stopped.");
    RefreshAllComponentConnectionStates();

    ForEachBridgeComponent([](UBlenderBridgeComponent* Component)
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
    });
}

bool FBlenderBridgeSessionManager::RestartSession()
{
    const TArray<TWeakObjectPtr<UBlenderBridgeComponent>> RetryComponents = PendingCookRetryComponents;
    const TArray<TWeakObjectPtr<UBlenderBridgeComponent>> PendingInstanceRetryComponents = PendingInstanceBakeRetryComponents;
    TMap<FString, TWeakObjectPtr<UBlenderBridgeComponent>> WarmupPreviewComponents;
    for (const TPair<FString, TWeakObjectPtr<UBlenderBridgeComponent>>& Entry : PendingWarmupPreviewComponents)
    {
        if (Entry.Value.IsValid())
        {
            WarmupPreviewComponents.Add(Entry.Key, Entry.Value);
        }
    }
    StopSession();
    PendingCookRetryComponents = RetryComponents;
    PendingInstanceBakeRetryComponents = PendingInstanceRetryComponents;
    PendingWarmupPreviewComponents = WarmupPreviewComponents;
    bRestoreSourcesAfterReconnect = true;
    StartSessionInternal();
    return Session.IsValid();
}

void FBlenderBridgeSessionManager::Shutdown()
{
    StopSession();
}

void FBlenderBridgeSessionManager::ReleaseComponentState(UBlenderBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    for (auto It = PendingRequests.CreateIterator(); It; ++It)
    {
        if (It.Value().Component.Get() == Component)
        {
            It.RemoveCurrent();
        }
    }

    if (!Session.IsValid() || !Session->IsConnected())
    {
        return;
    }

    Component->RefreshComponentHandle();
    if (Component->GetComponentHandle().IsEmpty())
    {
        return;
    }

    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(ComponentHandleFieldName, Component->GetComponentHandle());

    const FString SourceHandle = Component->GetSourceHandle();
    if (!SourceHandle.IsEmpty())
    {
        Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    }

    Session->QueueMessage(EBlenderBridgeMessageType::ReleaseComponentState, Payload);
}

void FBlenderBridgeSessionManager::NotifySourceChanged(UBlenderBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    ReleaseComponentState(Component);
    Component->ResetBridgeData(true);

    if (Component->Source == nullptr || GetComponentBlendFilePath(Component).IsEmpty())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    RequestEnsureSourceLoaded(Component, false, false, false);
}

void FBlenderBridgeSessionManager::WarmSourceOnPreviewDrag(UBlenderBridgeSource* Source, UBlenderBridgeComponent* PreviewComponent)
{
    if (!IsEditorBridgeSupported())
    {
        return;
    }

    FString SourceHandle;
    FString BlendFilePath;
    if (!TryResolveSourceHandleAndBlendFilePath(Source, SourceHandle, BlendFilePath))
    {
        return;
    }

    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache != nullptr && SourceCache->bLoaded)
    {
        QueueEnsureSourceLoadedRequest(SourceHandle, BlendFilePath, PreviewComponent, false, true, false);
        return;
    }

    if (Session.IsValid() && Session->IsConnected())
    {
        QueueEnsureSourceLoadedRequest(SourceHandle, BlendFilePath, PreviewComponent, false, true, false);
        return;
    }

    PendingWarmupPreviewComponents.Add(SourceHandle, PreviewComponent);

    if (SessionState == EBlenderBridgeConnectionState::Disconnected || SessionState == EBlenderBridgeConnectionState::Error)
    {
        CreateSession();
    }
}

void FBlenderBridgeSessionManager::NotifyTargetChanged(UBlenderBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    ReleaseComponentState(Component);
    Component->CancelPendingSplineCook();
    Component->SchemaTargetId.Reset();
    Component->InputSchema.Reset();
    Component->GeometryInputDescriptors.Reset();
    Component->GeometryBindings.Reset();
    Component->ApplyCurveOwnerDescription(Component->TargetId, FBridgeTargetCapabilities(), false);
    Component->RebuildInputValues(false);
    Component->UpdateTargetDisplayLabel();
    if (Component->PreviewMeshComponent != nullptr)
    {
        Component->PreviewMeshComponent->ClearAllMeshSections();
    }

    if (Component->TargetId.IsEmpty())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    Component->SetShowingBakedOutput(false);
    RequestDescribeTarget(Component, false, true, false);
}

void FBlenderBridgeSessionManager::RefreshComponentSchema(UBlenderBridgeComponent* Component, const bool bForceReload)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    Component->EnsurePreviewMeshComponent();

    if (Component->Source == nullptr || GetComponentBlendFilePath(Component).IsEmpty())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (!bForceReload && SourceCache != nullptr && SourceCache->bLoaded)
    {
        ApplySourceCacheToComponents(SourceHandle, false, false, nullptr);
        return;
    }

    RequestEnsureSourceLoaded(Component, bForceReload, false, false);
}

void FBlenderBridgeSessionManager::RequestCook(UBlenderBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    Component->EnsurePreviewMeshComponent();

    if (Component->Source == nullptr || GetComponentBlendFilePath(Component).IsEmpty())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        return;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        RequestEnsureSourceLoaded(Component, false, true, false);
        return;
    }

    ApplySourceCacheToComponents(SourceHandle, false, false, nullptr);
    SendCookRequest(Component);
}

void FBlenderBridgeSessionManager::RequestInstanceBake(UBlenderBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return;
    }

    if (!IsEditorBridgeSupported())
    {
        SetEditorOnlyUnsupported(Component);
        return;
    }

    Component->SetShowingBakedOutput(false);

    if (Component->Source == nullptr || GetComponentBlendFilePath(Component).IsEmpty())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        Component->bPendingInstancedBake = false;
        return;
    }

    if (!IsSessionConnected())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        Component->bPendingInstancedBake = false;
        return;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        RequestEnsureSourceLoaded(Component, false, false, true);
        return;
    }

    ApplySourceCacheToComponents(SourceHandle, false, false, nullptr);
    SendInstanceBakeRequest(Component);
}

bool FBlenderBridgeSessionManager::IsSessionConnected() const
{
    return Session.IsValid() && Session->IsConnected();
}

const FBridgeDecodedInstanceBake* FBlenderBridgeSessionManager::FindCachedInstanceBake(const UBlenderBridgeComponent* Component) const
{
    if (!IsValid(Component))
    {
        return nullptr;
    }

    FString CurveHash;
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        FBridgeCurveOwnerData CurveData;
        if (!Component->BuildCurveOwnerData(CurveData, CurveHash))
        {
            return nullptr;
        }
    }

    TArray<FBridgeGeometryBindingPayload> GeometryBindingPayloads;
    FString GeometryBindingsHash;
    FString GeometryBindingsError;
    if (!Component->BuildGeometryBindingPayloads(GeometryBindingPayloads, GeometryBindingsHash, GeometryBindingsError))
    {
        return nullptr;
    }

    FString CookCacheKey;
    if (!BuildCookCacheKey(
            Component->GetSourceHandle(),
            Component->TargetId,
            Component->InputSchema,
            Component->InputValues,
            CurveHash,
            GeometryBindingsHash,
            CookCacheKey))
    {
        return nullptr;
    }

    const FCookedTargetCache* CachedCook = CookedTargetCaches.Find(CookCacheKey);
    if (CachedCook == nullptr || CachedCook->DecodedInstanceBake.Prototypes.IsEmpty() || CachedCook->DecodedInstanceBake.Placements.IsEmpty())
    {
        return nullptr;
    }

    return &CachedCook->DecodedInstanceBake;
}

void FBlenderBridgeSessionManager::StartSessionInternal()
{
    UBlenderBridgeSettings* Settings = GetMutableDefault<UBlenderBridgeSettings>();
    const FString WorkerScriptPath = ResolveWorkerScriptPath();
    if (WorkerScriptPath.IsEmpty())
    {
        LastSessionMessage = TEXT("Could not resolve Blender bridge worker script path.");
        SessionState = EBlenderBridgeConnectionState::Error;
        RefreshAllComponentConnectionStates();
        return;
    }

    FString BlenderExecutableResolutionNote;
    const FString BlenderExecutablePath = ResolveBlenderExecutablePath(Settings, BlenderExecutableResolutionNote);
    if (BlenderExecutablePath.IsEmpty())
    {
        LastSessionMessage = BlenderExecutableResolutionNote.IsEmpty()
            ? TEXT("Blender executable path is not configured.")
            : BlenderExecutableResolutionNote;
        SessionState = EBlenderBridgeConnectionState::Error;
        RefreshAllComponentConnectionStates();
        return;
    }

    FBlenderBridgeWorkerConfig Config;
    Config.BlenderExecutablePath = BlenderExecutablePath;
    Config.WorkerScriptPath = WorkerScriptPath;
    Config.SharedMemoryBytes = Settings != nullptr
        ? static_cast<uint64>(FMath::Max<int64>(Settings->SharedMemoryBytes, static_cast<int64>(BlenderBridge::DefaultSharedMemoryBytes)))
        : BlenderBridge::DefaultSharedMemoryBytes;

    Session = MakeShared<FBlenderBridgeWorkerSession>();
    Session->OnStateChanged.AddRaw(this, &FBlenderBridgeSessionManager::HandleSessionStateChanged);
    Session->OnMessageReceived.AddRaw(this, &FBlenderBridgeSessionManager::HandleSessionMessageReceived);
    Session->OnLogMessage.AddRaw(this, &FBlenderBridgeSessionManager::HandleSessionLogMessage);
    if (!Session->Start(Config))
    {
        Session->OnStateChanged.RemoveAll(this);
        Session->OnMessageReceived.RemoveAll(this);
        Session->OnLogMessage.RemoveAll(this);
        Session.Reset();
        SessionState = EBlenderBridgeConnectionState::Error;
        LastSessionMessage = TEXT("Failed to start the global Blender session.");
        RefreshAllComponentConnectionStates();
        return;
    }

    SessionState = EBlenderBridgeConnectionState::Launching;
    LastSessionMessage = BlenderExecutableResolutionNote.IsEmpty()
        ? TEXT("Launching global Blender session.")
        : BlenderExecutableResolutionNote;
    RefreshAllComponentConnectionStates();
}

void FBlenderBridgeSessionManager::HandleSessionStateChanged(const EBlenderBridgeConnectionState NewState)
{
    SessionState = NewState;

    switch (NewState)
    {
    case EBlenderBridgeConnectionState::Connected:
        LastSessionMessage = TEXT("Global Blender session connected.");
        break;
    case EBlenderBridgeConnectionState::Launching:
        LastSessionMessage = TEXT("Launching global Blender session.");
        break;
    case EBlenderBridgeConnectionState::Disconnected:
        LastSessionMessage = TEXT("Global Blender session disconnected.");
        break;
    case EBlenderBridgeConnectionState::Error:
        LastSessionMessage = TEXT("Global Blender session encountered an error.");
        break;
    default:
        break;
    }

    RefreshAllComponentConnectionStates();

    if (NewState == EBlenderBridgeConnectionState::Connected && bRestoreSourcesAfterReconnect)
    {
        bRestoreSourcesAfterReconnect = false;

        const TMap<FString, FString> SourcesToRestore = GatherSourcesToRestore();
        for (const TPair<FString, FString>& Entry : SourcesToRestore)
        {
            QueueEnsureSourceLoadedRequest(Entry.Key, Entry.Value, nullptr, false, false, false);
        }
    }

    if (NewState == EBlenderBridgeConnectionState::Connected && !PendingWarmupPreviewComponents.IsEmpty())
    {
        TMap<FString, TWeakObjectPtr<UBlenderBridgeComponent>> WarmupPreviewComponents = MoveTemp(PendingWarmupPreviewComponents);
        PendingWarmupPreviewComponents.Reset();

        for (const TPair<FString, TWeakObjectPtr<UBlenderBridgeComponent>>& Entry : WarmupPreviewComponents)
        {
            if (!Entry.Value.IsValid())
            {
                continue;
            }

            QueueEnsureSourceLoadedRequest(Entry.Key, Entry.Key, Entry.Value.Get(), false, Entry.Value.IsValid(), false);
        }
    }
}

void FBlenderBridgeSessionManager::HandleSessionMessageReceived(const FBlenderBridgeJsonMessage& Message)
{
    FString RequestId;
    TryGetRequestId(Message.Payload, RequestId);

    if (Message.Type == EBlenderBridgeMessageType::SourceLoaded)
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

        FString SourceHandle;
        if (!Message.Payload.IsValid() || !Message.Payload->TryGetStringField(SourceHandleFieldName, SourceHandle) || SourceHandle.IsEmpty())
        {
            LastSessionMessage = TEXT("SourceLoaded response was missing source_handle.");
            return;
        }

        TArray<FBridgeTargetDescriptor> Targets;
        BlenderBridge::ParseTargetDescriptors(Message.Payload, Targets);

        FSourceCache& SourceCache = SourceCaches.FindOrAdd(SourceHandle);
        if (SourceCache.BlendFilePath.IsEmpty())
        {
            Message.Payload->TryGetStringField(BlendFileFieldName, SourceCache.BlendFilePath);
        }
        ClearCachedTargetData(SourceHandle);
        SourceCache.Targets = MoveTemp(Targets);
        SourceCache.bLoaded = true;

        ApplySourceCacheToComponents(
            SourceHandle,
            PendingRequest.bRequestCookAfterDescribe,
            PendingRequest.bRequestInstanceBakeAfterDescribe,
            PendingRequest.Component.Get());
        return;
    }

    if (Message.Type == EBlenderBridgeMessageType::TargetDescription)
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

        UBlenderBridgeComponent* Component = PendingRequest.Component.Get();
        if (!IsValid(Component))
        {
            return;
        }

        TArray<FBridgeInputSchema> Schema;
        if (!BlenderBridge::ParseInputSchema(Message.Payload, Schema))
        {
            if (PendingRequest.bRequestInstanceBakeAfterDescribe)
            {
                Component->bPendingInstancedBake = false;
            }
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to parse target schema from Blender."));
            return;
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
        BlenderBridge::ParseTargetCapabilities(Message.Payload, Capabilities);
        TArray<FBridgeGeometryInputDescriptor> GeometryDescriptors;
        BlenderBridge::ParseGeometryInputDescriptors(Message.Payload, GeometryDescriptors);

        Component->InputSchema = MoveTemp(Schema);
        Component->GeometryInputDescriptors = MoveTemp(GeometryDescriptors);
        Component->SchemaTargetId = TargetId;
        Component->RebuildInputValues(PendingRequest.bPreserveInputValues);
        Component->RebuildGeometryBindings(PendingRequest.bPreserveInputValues);
        const FString CurveOwnerStatus = Component->ApplyCurveOwnerDescription(TargetId, Capabilities, PendingRequest.bPreserveInputValues);
        if (!PendingRequest.SourceHandle.IsEmpty() && !TargetId.IsEmpty())
        {
            FTargetSchemaCache& CachedSchema = TargetSchemaCaches.FindOrAdd(MakeTargetSchemaCacheKey(PendingRequest.SourceHandle, TargetId));
            CachedSchema.Schema = Component->InputSchema;
            CachedSchema.GeometryInputs = Component->GeometryInputDescriptors;
            CachedSchema.Capabilities = Capabilities;
        }
        Component->SetBridgeStatus(
            EBlenderBridgeComponentStatus::Ready,
            FString::Printf(
                TEXT("Ready. Received %d bridge inputs and %d geometry binding target(s).%s"),
                Component->InputSchema.Num(),
                Component->GeometryInputDescriptors.Num(),
                *CurveOwnerStatus));

        if (PendingRequest.bRequestCookAfterDescribe)
        {
            SendCookRequest(Component);
            return;
        }

        if (PendingRequest.bRequestInstanceBakeAfterDescribe)
        {
            SendInstanceBakeRequest(Component);
        }
        return;
    }

    if (Message.Type == EBlenderBridgeMessageType::CookComplete)
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

        UBlenderBridgeComponent* Component = PendingRequest.Component.Get();
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
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("CookComplete was missing a shared-memory slot."));
            return;
        }

        const double ReadDecodeStartSeconds = FPlatformTime::Seconds();
        FBridgeDecodedMesh DecodedMesh;
        if (!Session->GetSharedMemory().ReadMesh(static_cast<uint32>(SlotNumber), DecodedMesh))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to read cooked mesh from shared memory."));
            return;
        }

        BlenderBridge::ParseCookedMeshMetadata(Message.Payload, DecodedMesh.MaterialSlots, DecodedMesh.Sections);
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
            EBlenderBridgeComponentStatus::Cooked,
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
                LogBlenderBridge,
                Log,
                TEXT("[BlenderBridgeTiming] kind=cook req=%s actor=%s target=%s total_to_visible=%s response_wait=%s worker_total=%s worker_resolve=%s worker_inputs=%s worker_eval=%s worker_extract=%s extract_geometry_access=%s extract_build_parts=%s extract_mesh_prepare=%s extract_mesh_read=%s extract_mesh_expand=%s extract_transform=%s extract_merge=%s worker_shm_write=%s read_decode=%s preview_apply=%s overhead=%s"),
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

    if (Message.Type == EBlenderBridgeMessageType::InstanceBakeComplete)
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

        UBlenderBridgeComponent* Component = PendingRequest.Component.Get();
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
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("InstanceBakeComplete was missing a shared-memory slot."));
            return;
        }

        const double ReadDecodeStartSeconds = FPlatformTime::Seconds();
        FBridgeDecodedInstanceBake DecodedInstanceBake;
        if (!Session->GetInstanceBakeSharedMemory().ReadInstanceBake(static_cast<uint32>(SlotNumber), DecodedInstanceBake))
        {
            Component->bPendingInstancedBake = false;
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to read instanced bake data from shared memory."));
            return;
        }
        const double ReadDecodeMs = (FPlatformTime::Seconds() - ReadDecodeStartSeconds) * 1000.0;

        if (!PendingRequest.CookCacheKey.IsEmpty())
        {
            FCookedTargetCache& CachedCook = CookedTargetCaches.FindOrAdd(PendingRequest.CookCacheKey);
            CachedCook.DecodedInstanceBake = DecodedInstanceBake;
        }

        const double BakeExecuteStartSeconds = FPlatformTime::Seconds();
        const bool bBakeSucceeded = FBlenderBridgeRuntimeModule::Get().ExecuteBakeBridgeComponent(Component);
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
                LogBlenderBridge,
                Log,
                TEXT("[BlenderBridgeTiming] kind=instance_bake req=%s actor=%s target=%s total_to_finish=%s response_wait=%s worker_total=%s worker_resolve=%s worker_inputs=%s worker_eval=%s worker_extract=%s extract_geometry_access=%s extract_build_parts=%s extract_mesh_prepare=%s extract_mesh_read=%s extract_mesh_expand=%s extract_transform=%s extract_merge=%s worker_serialize=%s worker_shm_write=%s read_decode=%s bake_execute=%s overhead=%s result=%s"),
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
        if (Component->ComponentStatus != EBlenderBridgeComponentStatus::Error)
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to bake the bridge output as instanced geometry."));
        }
        return;
    }

    if (Message.Type == EBlenderBridgeMessageType::ResizeRequired)
    {
        uint64 RequiredSlotBytes = 0;
        double RequiredBytes = 0.0;
        if (Message.Payload.IsValid() && Message.Payload->TryGetNumberField(TEXT("required_bytes"), RequiredBytes))
        {
            RequiredSlotBytes = static_cast<uint64>(FMath::Max(RequiredBytes, 0.0));
        }

        UBlenderBridgeComponent* RetryComponent = nullptr;
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

    if (Message.Type == EBlenderBridgeMessageType::Error)
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
                if (UBlenderBridgeComponent* Component = ExistingRequest->Component.Get())
                {
                    if (ExistingRequest->Kind == EBridgeManagerRequestKind::RequestInstanceBake ||
                        ExistingRequest->bRequestInstanceBakeAfterDescribe)
                    {
                        Component->bPendingInstancedBake = false;
                    }
                    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, ErrorMessage);
                }
                PendingRequests.Remove(RequestId);
                return;
            }
        }

        LastSessionMessage = ErrorMessage;
        return;
    }
}

void FBlenderBridgeSessionManager::HandleSessionLogMessage(const FString& Message)
{
    LastSessionMessage = Message;
}

void FBlenderBridgeSessionManager::RefreshAllComponentConnectionStates() const
{
    ForEachBridgeComponent([this](UBlenderBridgeComponent* Component)
    {
        Component->ConnectionState = SessionState;
    });
}

TMap<FString, FString> FBlenderBridgeSessionManager::GatherSourcesToRestore() const
{
    TMap<FString, FString> SourcesToRestore;

    ForEachBridgeComponent([&SourcesToRestore](UBlenderBridgeComponent* Component)
    {
        const FString SourceHandle = Component->GetSourceHandle();
        if (SourceHandle.IsEmpty())
        {
            return;
        }

        SourcesToRestore.FindOrAdd(SourceHandle, SourceHandle);
    });

    return SourcesToRestore;
}

void FBlenderBridgeSessionManager::ClearCachedTargetData(const FString& SourceHandle)
{
    if (SourceHandle.IsEmpty())
    {
        return;
    }

    const FString SourcePrefix = FString::Printf(TEXT("%s%s"), *SourceHandle, CacheKeyDelimiter);

    for (auto It = TargetSchemaCaches.CreateIterator(); It; ++It)
    {
        if (It.Key().StartsWith(SourcePrefix))
        {
            It.RemoveCurrent();
        }
    }

    for (auto It = CookedTargetCaches.CreateIterator(); It; ++It)
    {
        if (It.Key().StartsWith(SourcePrefix))
        {
            It.RemoveCurrent();
        }
    }
}

bool FBlenderBridgeSessionManager::TryApplyCachedTargetDescription(
    UBlenderBridgeComponent* Component,
    const bool bPreserveInputValues,
    const bool bRequestCookAfterDescribe,
    const bool bRequestInstanceBakeAfterDescribe)
{
    if (!IsValid(Component) || Component->TargetId.IsEmpty())
    {
        return false;
    }

    const FTargetSchemaCache* CachedSchema = TargetSchemaCaches.Find(MakeTargetSchemaCacheKey(Component->GetSourceHandle(), Component->TargetId));
    if (CachedSchema == nullptr)
    {
        return false;
    }

    Component->InputSchema = CachedSchema->Schema;
    Component->GeometryInputDescriptors = CachedSchema->GeometryInputs;
    Component->SchemaTargetId = Component->TargetId;
    Component->RebuildInputValues(bPreserveInputValues);
    Component->RebuildGeometryBindings(bPreserveInputValues);
    const FString CurveOwnerStatus = Component->ApplyCurveOwnerDescription(Component->TargetId, CachedSchema->Capabilities, bPreserveInputValues);
    Component->SetBridgeStatus(
        EBlenderBridgeComponentStatus::Ready,
        FString::Printf(
            TEXT("Ready. Reused %d cached bridge inputs and %d geometry binding target(s).%s"),
            Component->InputSchema.Num(),
            Component->GeometryInputDescriptors.Num(),
            *CurveOwnerStatus));

    if (bRequestCookAfterDescribe)
    {
        return SendCookRequest(Component);
    }

    if (bRequestInstanceBakeAfterDescribe)
    {
        return SendInstanceBakeRequest(Component);
    }

    return true;
}

bool FBlenderBridgeSessionManager::TryApplyCachedCook(UBlenderBridgeComponent* Component)
{
    if (!IsValid(Component))
    {
        return false;
    }

    if (Component->ShouldBypassCookCacheForAutomation())
    {
        return false;
    }

    FString CurveHash;
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        FBridgeCurveOwnerData CurveData;
        if (!Component->BuildCurveOwnerData(CurveData, CurveHash))
        {
            return false;
        }
    }

    TArray<FBridgeGeometryBindingPayload> GeometryBindingPayloads;
    FString GeometryBindingsHash;
    FString GeometryBindingsError;
    if (!Component->BuildGeometryBindingPayloads(GeometryBindingPayloads, GeometryBindingsHash, GeometryBindingsError))
    {
        return false;
    }

    FString CookCacheKey;
    if (!BuildCookCacheKey(
            Component->GetSourceHandle(),
            Component->TargetId,
            Component->InputSchema,
            Component->InputValues,
            CurveHash,
            GeometryBindingsHash,
            CookCacheKey))
    {
        return false;
    }

    const FCookedTargetCache* CachedCook = CookedTargetCaches.Find(CookCacheKey);
    if (CachedCook == nullptr)
    {
        return false;
    }

    const double PreviewApplyStartSeconds = FPlatformTime::Seconds();
    FBridgeDecodedMesh CachedDecodedMesh = CachedCook->DecodedMesh;
    Component->ApplyDecodedMesh(MoveTemp(CachedDecodedMesh));
    const double PreviewApplyMs = (FPlatformTime::Seconds() - PreviewApplyStartSeconds) * 1000.0;
    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Cooked, TEXT("Applied cached bridge mesh."));

    if (ShouldLogCookTimings())
    {
        UE_LOG(
            LogBlenderBridge,
            Log,
            TEXT("[BlenderBridgeTiming] kind=cook cache_hit=true actor=%s target=%s preview_apply=%s"),
            *GetNameSafe(Component->GetOwner()),
            *Component->TargetId,
            *FormatTimingMs(PreviewApplyMs));

        FBridgeTimingHistoryRow HistoryRow;
        HistoryRow.TimestampUtc = FDateTime::UtcNow().ToIso8601();
        HistoryRow.Kind = TEXT("cook");
        HistoryRow.bCacheHit = true;
        PopulateTimingHistoryMetadataFromComponent(Component, HistoryRow);
        HistoryRow.ActorName = GetNameSafe(Component->GetOwner());
        HistoryRow.TargetId = Component->TargetId;
        HistoryRow.PreviewApplyMs = PreviewApplyMs;
        AppendTimingHistoryRow(HistoryRow);
    }
    return true;
}

bool FBlenderBridgeSessionManager::TryApplyCachedInstanceBake(UBlenderBridgeComponent* Component)
{
    if (IsValid(Component) && Component->ShouldBypassCookCacheForAutomation())
    {
        return false;
    }

    return FindCachedInstanceBake(Component) != nullptr;
}

void FBlenderBridgeSessionManager::ApplySourceCacheToComponents(
    const FString& SourceHandle,
    const bool bRequestCookForPendingComponent,
    const bool bRequestInstanceBakeForPendingComponent,
    UBlenderBridgeComponent* PendingComponent)
{
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr)
    {
        return;
    }

    auto ProcessComponent = [&](UBlenderBridgeComponent* Component)
    {
        if (Component->GetSourceHandle() != SourceHandle)
        {
            return;
        }

        const FString PreviousTargetId = Component->TargetId;
        const FString PreviousSchemaTargetId = Component->SchemaTargetId;

        Component->ConnectionState = SessionState;

        Component->AvailableTargets = SourceCache->Targets;

        if (Component->FindTargetDescriptor(Component->TargetId) == nullptr)
        {
            if (!SourceCache->Targets.IsEmpty())
            {
                Component->TargetId = SourceCache->Targets[0].TargetId;
            }
            else
            {
                Component->TargetId.Reset();
            }
        }

        Component->UpdateTargetDisplayLabel();

        if (Component->TargetId != PreviousTargetId)
        {
            ReleaseComponentState(Component);
            Component->CancelPendingSplineCook();
            Component->SchemaTargetId.Reset();
            Component->InputSchema.Reset();
            Component->GeometryInputDescriptors.Reset();
            Component->GeometryBindings.Reset();
            Component->ApplyCurveOwnerDescription(Component->TargetId, FBridgeTargetCapabilities(), false);
            Component->RebuildInputValues(false);
            if (Component->PreviewMeshComponent != nullptr)
            {
                Component->PreviewMeshComponent->ClearAllMeshSections();
            }
        }

        if (SourceCache->Targets.IsEmpty())
        {
            Component->SchemaTargetId.Reset();
            Component->InputSchema.Reset();
            Component->GeometryInputDescriptors.Reset();
            Component->GeometryBindings.Reset();
            Component->ApplyCurveOwnerDescription(Component->TargetId, FBridgeTargetCapabilities(), false);
            Component->RebuildInputValues(false);
            if (Component->PreviewMeshComponent != nullptr)
            {
                Component->PreviewMeshComponent->ClearAllMeshSections();
            }
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("No Geometry Nodes targets were found in this blend source."));
            return;
        }

        if (Component->TargetId.IsEmpty())
        {
            Component->SchemaTargetId.Reset();
            Component->InputSchema.Reset();
            Component->GeometryInputDescriptors.Reset();
            Component->GeometryBindings.Reset();
            Component->ApplyCurveOwnerDescription(Component->TargetId, FBridgeTargetCapabilities(), false);
            Component->RebuildInputValues(false);
            if (Component->PreviewMeshComponent != nullptr)
            {
                Component->PreviewMeshComponent->ClearAllMeshSections();
            }
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
            return;
        }

        const bool bRequestCook = (PendingComponent != nullptr && PendingComponent == Component && bRequestCookForPendingComponent)
            || IsRetryComponent(PendingCookRetryComponents, Component);
        const bool bRequestInstanceBake =
            (PendingComponent != nullptr && PendingComponent == Component && bRequestInstanceBakeForPendingComponent)
            || IsRetryComponent(PendingInstanceBakeRetryComponents, Component);
        const bool bPreserveInputValues = (PreviousSchemaTargetId == Component->TargetId);

        PendingCookRetryComponents.RemoveAll([Component](const TWeakObjectPtr<UBlenderBridgeComponent>& Entry)
        {
            return !Entry.IsValid() || Entry.Get() == Component;
        });
        PendingInstanceBakeRetryComponents.RemoveAll([Component](const TWeakObjectPtr<UBlenderBridgeComponent>& Entry)
        {
            return !Entry.IsValid() || Entry.Get() == Component;
        });

        RequestDescribeTarget(Component, bPreserveInputValues, bRequestCook, bRequestInstanceBake);
    };

    bool bProcessedPendingComponent = false;
    ForEachBridgeComponent([&](UBlenderBridgeComponent* Component)
    {
        if (Component == PendingComponent)
        {
            bProcessedPendingComponent = true;
        }

        ProcessComponent(Component);
    });

    if (IsValid(PendingComponent) && !bProcessedPendingComponent)
    {
        ProcessComponent(PendingComponent);
    }
}

bool FBlenderBridgeSessionManager::RequestEnsureSourceLoaded(
    UBlenderBridgeComponent* Component,
    const bool bForceReload,
    const bool bRequestCookAfterDescribe,
    const bool bRequestInstanceBakeAfterDescribe)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FString BlendFilePath = GetComponentBlendFilePath(Component);
    if (SourceHandle.IsEmpty() || BlendFilePath.IsEmpty())
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        return false;
    }

    FSourceCache& SourceCache = SourceCaches.FindOrAdd(SourceHandle);
    SourceCache.BlendFilePath = BlendFilePath;

    return QueueEnsureSourceLoadedRequest(
        SourceHandle,
        BlendFilePath,
        Component,
        bForceReload,
        bRequestCookAfterDescribe,
        bRequestInstanceBakeAfterDescribe);
}

bool FBlenderBridgeSessionManager::QueueEnsureSourceLoadedRequest(
    const FString& SourceHandle,
    const FString& BlendFilePath,
    UBlenderBridgeComponent* Component,
    const bool bForceReload,
    const bool bRequestCookAfterDescribe,
    const bool bRequestInstanceBakeAfterDescribe)
{
    if (!Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    if (SourceHandle.IsEmpty() || BlendFilePath.IsEmpty())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SourceNotLoaded, TEXT("Select a BlenderBridgeSource with a valid Blend File."));
        }
        return false;
    }

    FSourceCache& SourceCache = SourceCaches.FindOrAdd(SourceHandle);
    SourceCache.BlendFilePath = BlendFilePath;

    if (SourceCache.bLoaded && !bForceReload)
    {
        if (IsValid(Component))
        {
            ApplySourceCacheToComponents(SourceHandle, bRequestCookAfterDescribe, bRequestInstanceBakeAfterDescribe, Component);
        }
        return true;
    }

    for (TPair<FString, FPendingRequest>& Entry : PendingRequests)
    {
        FPendingRequest& ExistingRequest = Entry.Value;
        if (ExistingRequest.Kind != EBridgeManagerRequestKind::EnsureSourceLoaded
            || ExistingRequest.SourceHandle != SourceHandle)
        {
            continue;
        }

        if (bForceReload && !ExistingRequest.bForceReload)
        {
            continue;
        }

        if (!ExistingRequest.Component.IsValid() && IsValid(Component))
        {
            ExistingRequest.Component = Component;
        }

        ExistingRequest.bForceReload |= bForceReload;
        ExistingRequest.bRequestCookAfterDescribe |= bRequestCookAfterDescribe;
        ExistingRequest.bRequestInstanceBakeAfterDescribe |= bRequestInstanceBakeAfterDescribe;
        return true;
    }

    const FString RequestId = MakeRequestId();
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(RequestIdFieldName, RequestId);
    Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    Payload->SetStringField(BlendFileFieldName, BlendFilePath);
    Payload->SetBoolField(ForceReloadFieldName, bForceReload);

    FPendingRequest PendingRequest;
    PendingRequest.Kind = EBridgeManagerRequestKind::EnsureSourceLoaded;
    PendingRequest.Component = Component;
    PendingRequest.SourceHandle = SourceHandle;
    PendingRequest.bForceReload = bForceReload;
    PendingRequest.bRequestCookAfterDescribe = bRequestCookAfterDescribe;
    PendingRequest.bRequestInstanceBakeAfterDescribe = bRequestInstanceBakeAfterDescribe;
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    if (IsValid(Component))
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SchemaPending, TEXT("Loading source into the global Blender session."));
    }

    return Session->QueueMessage(EBlenderBridgeMessageType::EnsureSourceLoaded, Payload);
}

bool FBlenderBridgeSessionManager::RequestDescribeTarget(
    UBlenderBridgeComponent* Component,
    const bool bPreserveInputValues,
    const bool bRequestCookAfterDescribe,
    const bool bRequestInstanceBakeAfterDescribe)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        return RequestEnsureSourceLoaded(Component, false, bRequestCookAfterDescribe, bRequestInstanceBakeAfterDescribe);
    }

    if (Component->TargetId.IsEmpty() || Component->FindTargetDescriptor(Component->TargetId) == nullptr)
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return false;
    }

    if (TryApplyCachedTargetDescription(
        Component,
        bPreserveInputValues,
        bRequestCookAfterDescribe,
        bRequestInstanceBakeAfterDescribe))
    {
        return true;
    }

    for (TPair<FString, FPendingRequest>& Entry : PendingRequests)
    {
        FPendingRequest& ExistingRequest = Entry.Value;
        if (ExistingRequest.Kind != EBridgeManagerRequestKind::DescribeTarget
            || ExistingRequest.Component.Get() != Component
            || ExistingRequest.SourceHandle != SourceHandle
            || ExistingRequest.TargetId != Component->TargetId)
        {
            continue;
        }

        ExistingRequest.bPreserveInputValues |= bPreserveInputValues;
        ExistingRequest.bRequestCookAfterDescribe |= bRequestCookAfterDescribe;
        ExistingRequest.bRequestInstanceBakeAfterDescribe |= bRequestInstanceBakeAfterDescribe;
        return true;
    }

    const FString RequestId = MakeRequestId();
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(RequestIdFieldName, RequestId);
    Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    Payload->SetStringField(TargetIdFieldName, Component->TargetId);

    FPendingRequest PendingRequest;
    PendingRequest.Kind = EBridgeManagerRequestKind::DescribeTarget;
    PendingRequest.Component = Component;
    PendingRequest.SourceHandle = SourceHandle;
    PendingRequest.TargetId = Component->TargetId;
    PendingRequest.bPreserveInputValues = bPreserveInputValues;
    PendingRequest.bRequestCookAfterDescribe = bRequestCookAfterDescribe;
    PendingRequest.bRequestInstanceBakeAfterDescribe = bRequestInstanceBakeAfterDescribe;
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::SchemaPending, TEXT("Refreshing target schema from Blender."));
    return Session->QueueMessage(EBlenderBridgeMessageType::DescribeTarget, Payload);
}

bool FBlenderBridgeSessionManager::SendCookRequest(UBlenderBridgeComponent* Component)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        return RequestEnsureSourceLoaded(Component, false, true, false);
    }

    if (Component->TargetId.IsEmpty() || Component->FindTargetDescriptor(Component->TargetId) == nullptr)
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return false;
    }

    // A target can legitimately expose zero editable inputs. Once we've described the
    // currently selected target, we should still allow a cook with an empty values payload.
    if (Component->SchemaTargetId != Component->TargetId)
    {
        return RequestDescribeTarget(Component, Component->SchemaTargetId == Component->TargetId, true, false);
    }

    FBridgeCurveOwnerData CurveOwnerData;
    FString CurveHash;
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        if (!Component->BuildCurveOwnerData(CurveOwnerData, CurveHash))
        {
            Component->SetBridgeStatus(
                EBlenderBridgeComponentStatus::Error,
                TEXT("Curve-owner target requires a valid spline source before it can cook."));
            return false;
        }
    }

    TArray<FBridgeGeometryBindingPayload> GeometryBindingPayloads;
    FString GeometryBindingsHash;
    FString GeometryBindingsError;
    if (!Component->BuildGeometryBindingPayloads(GeometryBindingPayloads, GeometryBindingsHash, GeometryBindingsError))
    {
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, GeometryBindingsError);
        return false;
    }

    if (TryApplyCachedCook(Component))
    {
        return true;
    }

    Component->RefreshComponentHandle();
    const FString RequestId = MakeRequestId();
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(RequestIdFieldName, RequestId);
    Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    Payload->SetStringField(TargetIdFieldName, Component->TargetId);
    Payload->SetStringField(ComponentHandleFieldName, Component->GetComponentHandle());
    BlenderBridge::AddInputValuesToPayload(Component->InputSchema, Component->InputValues, Payload);
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        Payload->SetStringField(CurveHashFieldName, CurveHash);
        BlenderBridge::AddCurveOwnerDataToPayload(CurveOwnerData, Payload);
    }
    if (!GeometryBindingPayloads.IsEmpty())
    {
        BlenderBridge::AddGeometryBindingPayloadsToPayload(GeometryBindingPayloads, Payload);
    }

    FPendingRequest PendingRequest;
    PendingRequest.Kind = EBridgeManagerRequestKind::CookTarget;
    PendingRequest.Component = Component;
    PendingRequest.SourceHandle = SourceHandle;
    PendingRequest.TargetId = Component->TargetId;
    PendingRequest.RunLabel = Component->GetAutomationRunLabel();
    PendingRequest.ScenarioId = Component->GetAutomationScenarioId();
    PendingRequest.ActionKind = Component->GetAutomationActionKind();
    PendingRequest.IterationIndex = Component->GetAutomationIterationIndex();
    BuildCookCacheKey(
        SourceHandle,
        Component->TargetId,
        Component->InputSchema,
        Component->InputValues,
        CurveHash,
        GeometryBindingsHash,
        PendingRequest.CookCacheKey);
    PendingRequest.SentTimeSeconds = FPlatformTime::Seconds();
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Cooking, TEXT("Cooking selected target."));
    return Session->QueueMessage(EBlenderBridgeMessageType::CookTarget, Payload);
}

bool FBlenderBridgeSessionManager::SendInstanceBakeRequest(UBlenderBridgeComponent* Component)
{
    if (!IsValid(Component) || !Session.IsValid() || !Session->IsConnected())
    {
        if (IsValid(Component))
        {
            Component->bPendingInstancedBake = false;
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::NoSession, TEXT("Global Blender session is not running."));
        }
        return false;
    }

    const FString SourceHandle = Component->GetSourceHandle();
    const FSourceCache* SourceCache = SourceCaches.Find(SourceHandle);
    if (SourceCache == nullptr || !SourceCache->bLoaded)
    {
        return RequestEnsureSourceLoaded(Component, false, false, true);
    }

    if (Component->TargetId.IsEmpty() || Component->FindTargetDescriptor(Component->TargetId) == nullptr)
    {
        Component->bPendingInstancedBake = false;
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::TargetUnresolved, TEXT("Select a target for this source."));
        return false;
    }

    if (Component->SchemaTargetId != Component->TargetId)
    {
        return RequestDescribeTarget(Component, Component->SchemaTargetId == Component->TargetId, false, true);
    }

    if (TryApplyCachedInstanceBake(Component))
    {
        const double BakeExecuteStartSeconds = FPlatformTime::Seconds();
        const bool bBakeSucceeded = FBlenderBridgeRuntimeModule::Get().ExecuteBakeBridgeComponent(Component);
        const double BakeExecuteMs = (FPlatformTime::Seconds() - BakeExecuteStartSeconds) * 1000.0;

        if (ShouldLogCookTimings())
        {
            UE_LOG(
                LogBlenderBridge,
                Log,
                TEXT("[BlenderBridgeTiming] kind=instance_bake cache_hit=true actor=%s target=%s bake_execute=%s result=%s"),
                *GetNameSafe(Component->GetOwner()),
                *Component->TargetId,
                *FormatTimingMs(BakeExecuteMs),
                bBakeSucceeded ? TEXT("success") : TEXT("failure"));

            FBridgeTimingHistoryRow HistoryRow;
            HistoryRow.TimestampUtc = FDateTime::UtcNow().ToIso8601();
            HistoryRow.Kind = TEXT("instance_bake");
            HistoryRow.bCacheHit = true;
            PopulateTimingHistoryMetadataFromComponent(Component, HistoryRow);
            HistoryRow.ActorName = GetNameSafe(Component->GetOwner());
            HistoryRow.TargetId = Component->TargetId;
            HistoryRow.Result = bBakeSucceeded ? TEXT("success") : TEXT("failure");
            HistoryRow.BakeExecuteMs = BakeExecuteMs;
            AppendTimingHistoryRow(HistoryRow);
        }

        if (bBakeSucceeded)
        {
            Component->bPendingInstancedBake = false;
            return true;
        }

        Component->bPendingInstancedBake = false;
        if (Component->ComponentStatus != EBlenderBridgeComponentStatus::Error)
        {
            Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, TEXT("Failed to bake the bridge output as instanced geometry."));
        }
        return false;
    }

    FBridgeCurveOwnerData CurveOwnerData;
    FString CurveHash;
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        if (!Component->BuildCurveOwnerData(CurveOwnerData, CurveHash))
        {
            Component->bPendingInstancedBake = false;
            Component->SetBridgeStatus(
                EBlenderBridgeComponentStatus::Error,
                TEXT("Curve-owner target requires a valid spline source before it can bake instances."));
            return false;
        }
    }

    TArray<FBridgeGeometryBindingPayload> GeometryBindingPayloads;
    FString GeometryBindingsHash;
    FString GeometryBindingsError;
    if (!Component->BuildGeometryBindingPayloads(GeometryBindingPayloads, GeometryBindingsHash, GeometryBindingsError))
    {
        Component->bPendingInstancedBake = false;
        Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Error, GeometryBindingsError);
        return false;
    }

    Component->RefreshComponentHandle();
    const FString RequestId = MakeRequestId();
    TSharedRef<FJsonObject> Payload = MakeShared<FJsonObject>();
    Payload->SetStringField(RequestIdFieldName, RequestId);
    Payload->SetStringField(SourceHandleFieldName, SourceHandle);
    Payload->SetStringField(TargetIdFieldName, Component->TargetId);
    Payload->SetStringField(ComponentHandleFieldName, Component->GetComponentHandle());
    BlenderBridge::AddInputValuesToPayload(Component->InputSchema, Component->InputValues, Payload);
    if (Component->TargetCapabilities.bSupportsCurveOwner)
    {
        Payload->SetStringField(CurveHashFieldName, CurveHash);
        BlenderBridge::AddCurveOwnerDataToPayload(CurveOwnerData, Payload);
    }
    if (!GeometryBindingPayloads.IsEmpty())
    {
        BlenderBridge::AddGeometryBindingPayloadsToPayload(GeometryBindingPayloads, Payload);
    }

    FPendingRequest PendingRequest;
    PendingRequest.Kind = EBridgeManagerRequestKind::RequestInstanceBake;
    PendingRequest.Component = Component;
    PendingRequest.SourceHandle = SourceHandle;
    PendingRequest.TargetId = Component->TargetId;
    PendingRequest.RunLabel = Component->GetAutomationRunLabel();
    PendingRequest.ScenarioId = Component->GetAutomationScenarioId();
    PendingRequest.ActionKind = Component->GetAutomationActionKind();
    PendingRequest.IterationIndex = Component->GetAutomationIterationIndex();
    BuildCookCacheKey(
        SourceHandle,
        Component->TargetId,
        Component->InputSchema,
        Component->InputValues,
        CurveHash,
        GeometryBindingsHash,
        PendingRequest.CookCacheKey);
    PendingRequest.SentTimeSeconds = FPlatformTime::Seconds();
    PendingRequests.Add(RequestId, MoveTemp(PendingRequest));

    Component->SetBridgeStatus(EBlenderBridgeComponentStatus::Cooking, TEXT("Preparing instanced bake payload."));
    if (Session->QueueMessage(EBlenderBridgeMessageType::RequestInstanceBake, Payload))
    {
        return true;
    }

    Component->bPendingInstancedBake = false;
    return false;
}

void FBlenderBridgeSessionManager::RestartSessionForResize(
    UBlenderBridgeComponent* RetryComponent,
    const EBridgeManagerRequestKind RetryKind,
    const uint64 RequiredSlotBytes)
{
    UBlenderBridgeSettings* Settings = GetMutableDefault<UBlenderBridgeSettings>();
    const uint64 CurrentTotalBytes = Settings != nullptr
        ? static_cast<uint64>(FMath::Max<int64>(Settings->SharedMemoryBytes, static_cast<int64>(BlenderBridge::DefaultSharedMemoryBytes)))
        : BlenderBridge::DefaultSharedMemoryBytes;
    const uint64 ExpandedSharedMemoryBytes = ComputeExpandedSharedMemoryBytes(CurrentTotalBytes, RequiredSlotBytes);
    if (ExpandedSharedMemoryBytes <= CurrentTotalBytes)
    {
        if (IsValid(RetryComponent))
        {
            RetryComponent->SetBridgeStatus(
                EBlenderBridgeComponentStatus::Error,
                FString::Printf(
                    TEXT("ResizeRequired requested %llu bytes per slot, but the current shared-memory budget of %llu bytes is already large enough."),
                    RequiredSlotBytes,
                    CurrentTotalBytes));
        }
        return;
    }

    if (Settings != nullptr)
    {
        Settings->SharedMemoryBytes = static_cast<int64>(ExpandedSharedMemoryBytes);
        Settings->SaveConfig();
    }

    if (IsValid(RetryComponent))
    {
        if (RetryKind == EBridgeManagerRequestKind::RequestInstanceBake)
        {
            PendingInstanceBakeRetryComponents.Add(RetryComponent);
        }
        else
        {
            PendingCookRetryComponents.Add(RetryComponent);
        }
        RetryComponent->SetBridgeStatus(
            EBlenderBridgeComponentStatus::SchemaPending,
            FString::Printf(
                TEXT("Growing shared memory from %llu to %llu bytes. Restarting the global session and retrying the pending bridge request."),
                CurrentTotalBytes,
                ExpandedSharedMemoryBytes));
    }

    bRestoreSourcesAfterReconnect = true;
    RestartSession();
}

FString FBlenderBridgeSessionManager::MakeRequestId()
{
    return FString::Printf(TEXT("Request_%llu"), NextRequestId++);
}

FString FBlenderBridgeSessionManager::ResolveWorkerScriptPath() const
{
    if (const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("BlenderBridge")))
    {
        return FPaths::Combine(Plugin->GetBaseDir(), TEXT("Scripts/bridge_server.py"));
    }

    return FString();
}
