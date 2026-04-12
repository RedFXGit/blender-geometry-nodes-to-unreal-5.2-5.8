param(
    [Parameter(Mandatory = $true)]
    [string]$PassLabel,
    [string]$RepoRoot = "C:\Users\Lassiter\Documents\GitHub\blender-geometry-nodes-to-unreal",
    [string]$ProjectRoot = "C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal",
    [string]$EngineRoot = "C:\Program Files\Epic Games\UE_5.7",
    [int]$AutomationTimeoutMinutes = 20,
    [switch]$SkipSync,
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

function Get-Mean {
    param([double[]]$Values)
    if ($null -eq $Values -or $Values.Count -eq 0) {
        return $null
    }

    return ($Values | Measure-Object -Average).Average
}

function Get-Median {
    param([double[]]$Values)
    if ($null -eq $Values -or $Values.Count -eq 0) {
        return $null
    }

    $sorted = $Values | Sort-Object
    $count = $sorted.Count
    if (($count % 2) -eq 1) {
        return $sorted[[int][Math]::Floor($count / 2)]
    }

    return ($sorted[($count / 2) - 1] + $sorted[$count / 2]) / 2.0
}

function Get-Minimum {
    param([double[]]$Values)
    if ($null -eq $Values -or $Values.Count -eq 0) {
        return $null
    }

    return ($Values | Measure-Object -Minimum).Minimum
}

function Get-Maximum {
    param([double[]]$Values)
    if ($null -eq $Values -or $Values.Count -eq 0) {
        return $null
    }

    return ($Values | Measure-Object -Maximum).Maximum
}

function Get-NullableDoubleValues {
    param(
        [Parameter(Mandatory = $true)]
        [object[]]$Rows,
        [Parameter(Mandatory = $true)]
        [string]$ColumnName
    )

    $values = New-Object System.Collections.Generic.List[double]
    foreach ($row in $Rows) {
        $rawValue = $row.$ColumnName
        if ([string]::IsNullOrWhiteSpace($rawValue)) {
            continue
        }

        $parsedValue = 0.0
        if ([double]::TryParse($rawValue, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$parsedValue)) {
            $values.Add($parsedValue)
        }
    }

    return $values.ToArray()
}

function Format-NullableNumber {
    param($Value)
    if ($null -eq $Value) {
        return ""
    }

    return [string]::Format([System.Globalization.CultureInfo]::InvariantCulture, "{0:F3}", $Value)
}

function Load-PerfManifest {
    param([string]$ManifestPath)

    if (-not (Test-Path -LiteralPath $ManifestPath)) {
        throw "Perf benchmark manifest not found: $ManifestPath"
    }

    return (Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json)
}

function Load-CsvRows {
    param([string]$Path)
    if (-not (Test-Path -LiteralPath $Path)) {
        return ,@()
    }

    return ,@(Import-Csv -LiteralPath $Path)
}

function ConvertTo-QuotedProcessArgument {
    param([string]$Value)

    if ($null -eq $Value) {
        return '""'
    }

    if ($Value.IndexOfAny([char[]]@(' ', "`t", '"')) -lt 0) {
        return $Value
    }

    return '"' + ($Value -replace '(\\*)"', '$1$1\"' -replace '(\\+)$', '$1$1') + '"'
}

function Get-EffectiveUnrealLogPath {
    param(
        [string]$PreferredPath,
        [string]$ProjectRootPath,
        [datetime]$RunStartedAt
    )

    if (Test-Path -LiteralPath $PreferredPath) {
        $preferredItem = Get-Item -LiteralPath $PreferredPath
        if ($preferredItem.LastWriteTime -ge $RunStartedAt) {
            return $PreferredPath
        }
    }

    $fallbackPath = Join-Path $ProjectRootPath "Saved\Logs\blender_unreal.log"
    if (Test-Path -LiteralPath $fallbackPath) {
        $fallbackItem = Get-Item -LiteralPath $fallbackPath
        if ($fallbackItem.LastWriteTime -ge $RunStartedAt) {
            return $fallbackPath
        }
    }

    $latestFallback = Get-ChildItem (Join-Path $ProjectRootPath "Saved\Logs") -File -Filter "*.log" -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTime -Descending |
        Select-Object -First 1
    if ($null -ne $latestFallback -and $latestFallback.LastWriteTime -ge $RunStartedAt) {
        return $latestFallback.FullName
    }

    if (Test-Path -LiteralPath $PreferredPath) {
        return $PreferredPath
    }

    return $PreferredPath
}

function Get-ScenarioAutomationTestName {
    param($Scenario)

    $displayName = [string]$Scenario.display_name
    if ([string]::IsNullOrWhiteSpace($displayName)) {
        throw "Scenario '$($Scenario.id)' is missing a display_name, so the exact automation test name cannot be built."
    }

    return "BlenderBridge.Perf.$displayName"
}

function Invoke-UnrealAutomationScenario {
    param(
        [Parameter(Mandatory = $true)]
        [string]$UnrealEditorCmd,
        [Parameter(Mandatory = $true)]
        [string]$ProjectFile,
        [Parameter(Mandatory = $true)]
        [string]$PassLabel,
        [Parameter(Mandatory = $true)]
        $Scenario,
        [Parameter(Mandatory = $true)]
        [string]$LogPath,
        [Parameter(Mandatory = $true)]
        [string]$ReportPath,
        [Parameter(Mandatory = $true)]
        [int]$TimeoutMinutes
    )

    if (Test-Path -LiteralPath $LogPath) {
        Remove-Item -LiteralPath $LogPath -Force
    }

    if (Test-Path -LiteralPath $ReportPath) {
        Remove-Item -LiteralPath $ReportPath -Recurse -Force
    }

    $automationTestName = Get-ScenarioAutomationTestName -Scenario $Scenario
    $execCmds = "Automation RunTests $automationTestName"
    $projectRoot = Split-Path -Parent $ProjectFile
    $projectLogPath = Join-Path $projectRoot "Saved\Logs\blender_unreal.log"
    $completionMarker = "LogAutomationController: Display: Test Completed. Result={Success} Name={$($Scenario.display_name)} Path={$automationTestName}"
    $queueEmptyMarker = "LogAutomationCommandLine: Display: ...Automation Test Queue Empty"
    $pendingForcedSuccess = $false
    $forcedSuccessAt = $null

    $processStartInfo = New-Object System.Diagnostics.ProcessStartInfo
    $processStartInfo.FileName = $UnrealEditorCmd
    $processStartInfo.UseShellExecute = $false
    $processStartInfo.CreateNoWindow = $true
    $processStartInfo.RedirectStandardOutput = $false
    $processStartInfo.RedirectStandardError = $false

    $argumentList = @(
        $ProjectFile,
        "-unattended",
        "-nop4",
        "-nosplash",
        "-nullrhi",
        "-NoSound",
        "-BlenderBridgeLogCookTimings=1",
        "-BlenderBridgePerfRunLabel=$PassLabel",
        "-ExecCmds=$execCmds",
        "-TestExit=Automation Test Queue Empty",
        "-ReportExportPath=$ReportPath",
        "-log=$LogPath"
    )

    $processStartInfo.Arguments = ($argumentList | ForEach-Object { ConvertTo-QuotedProcessArgument -Value $_ }) -join " "

    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $processStartInfo

    if (-not $process.Start()) {
        throw "Failed to start UnrealEditor-Cmd for scenario '$($Scenario.id)'."
    }

    $deadline = (Get-Date).AddMinutes([Math]::Max(1, $TimeoutMinutes))
    while (-not $process.HasExited) {
        if ($process.WaitForExit(2000)) {
            break
        }

        if ((Get-Date) -ge $deadline) {
            try {
                $process.Kill($true)
            }
            catch {
            }

            throw "Headless Unreal automation run for scenario '$($Scenario.id)' timed out after $TimeoutMinutes minutes. See $projectLogPath"
        }

        $activeLogPath = $null
        if (Test-Path -LiteralPath $LogPath) {
            $activeLogPath = $LogPath
        }
        elseif (Test-Path -LiteralPath $projectLogPath) {
            $activeLogPath = $projectLogPath
        }

        if ($null -eq $activeLogPath) {
            continue
        }

        $logTail = Get-Content -LiteralPath $activeLogPath -Tail 120 -ErrorAction SilentlyContinue
        $hasCompletionMarker = $null -ne ($logTail | Select-String -SimpleMatch -Pattern $completionMarker)
        $hasQueueEmptyMarker = $null -ne ($logTail | Select-String -SimpleMatch -Pattern $queueEmptyMarker)
        if ($hasCompletionMarker -and $hasQueueEmptyMarker) {
            if (-not $pendingForcedSuccess) {
                $pendingForcedSuccess = $true
                $forcedSuccessAt = (Get-Date).AddSeconds(10)
            }
            elseif ((Get-Date) -ge $forcedSuccessAt) {
                try {
                    $process.Kill($true)
                }
                catch {
                }

                return $automationTestName
            }
        }
    }

    $effectiveExitLogPath = Get-EffectiveUnrealLogPath -PreferredPath $LogPath -ProjectRootPath $projectRoot -RunStartedAt $runStartedAt
    $completedSuccessfully = $false
    if (Test-Path -LiteralPath $effectiveExitLogPath) {
        $hasCompletionMarker = $null -ne (Select-String -Path $effectiveExitLogPath -SimpleMatch -Pattern $completionMarker -ErrorAction SilentlyContinue)
        $hasQueueEmptyMarker = $null -ne (Select-String -Path $effectiveExitLogPath -SimpleMatch -Pattern $queueEmptyMarker -ErrorAction SilentlyContinue)
        $completedSuccessfully = $hasCompletionMarker -and $hasQueueEmptyMarker
    }

    if ($process.ExitCode -ne 0 -and -not $completedSuccessfully) {
        throw "Headless Unreal automation run for scenario '$($Scenario.id)' failed with exit code $($process.ExitCode). See $effectiveExitLogPath"
    }

    return $automationTestName
}

function Get-ScenarioPrimaryMetricName {
    param($Scenario)
    if ($Scenario.action -eq "instanced_bake") {
        return "total_to_finish_ms"
    }

    return "total_to_visible_ms"
}

function New-ScenarioSummaryRow {
    param(
        [string]$RunStartedUtc,
        [string]$PassLabel,
        $Scenario,
        [object[]]$Rows,
        $BaselineSummaryRow,
        $PreviousSummaryRow
    )

    $primaryMetricName = Get-ScenarioPrimaryMetricName -Scenario $Scenario
    $primaryValues = Get-NullableDoubleValues -Rows $Rows -ColumnName $primaryMetricName
    $totalToVisibleValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "total_to_visible_ms"
    $totalToFinishValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "total_to_finish_ms"
    $workerTotalValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "worker_total_ms"
    $workerEvalValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "worker_eval_ms"
    $workerExtractValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "worker_extract_ms"
    $meshReadValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "extract_mesh_read_ms"
    $transformValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "extract_transform_ms"
    $mergeValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "extract_merge_ms"
    $sharedWriteValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "worker_shared_write_ms"
    $readDecodeValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "read_decode_ms"
    $previewApplyValues = Get-NullableDoubleValues -Rows $Rows -ColumnName "preview_apply_ms"

    $primaryMean = Get-Mean -Values $primaryValues
    $baselineMean = if ($null -ne $BaselineSummaryRow) { [double]$BaselineSummaryRow.mean_primary_ms } else { $null }
    $previousMean = if ($null -ne $PreviousSummaryRow) { [double]$PreviousSummaryRow.mean_primary_ms } else { $null }

    $deltaVsBaselineMs = if ($null -ne $baselineMean -and $null -ne $primaryMean) { $primaryMean - $baselineMean } else { $null }
    $deltaVsPreviousMs = if ($null -ne $previousMean -and $null -ne $primaryMean) { $primaryMean - $previousMean } else { $null }
    $deltaVsBaselinePct = if ($null -ne $baselineMean -and $baselineMean -ne 0.0 -and $null -ne $primaryMean) { (($primaryMean - $baselineMean) / $baselineMean) * 100.0 } else { $null }
    $deltaVsPreviousPct = if ($null -ne $previousMean -and $previousMean -ne 0.0 -and $null -ne $primaryMean) { (($primaryMean - $previousMean) / $previousMean) * 100.0 } else { $null }

    return [pscustomobject]@{
        run_started_utc = $RunStartedUtc
        pass_label = $PassLabel
        scenario_id = [string]$Scenario.id
        action_kind = [string]$Scenario.action
        measure_count = [int]$Scenario.measure_count
        primary_metric_name = $primaryMetricName
        mean_primary_ms = Format-NullableNumber $primaryMean
        median_primary_ms = Format-NullableNumber (Get-Median -Values $primaryValues)
        min_primary_ms = Format-NullableNumber (Get-Minimum -Values $primaryValues)
        max_primary_ms = Format-NullableNumber (Get-Maximum -Values $primaryValues)
        delta_vs_baseline_ms = Format-NullableNumber $deltaVsBaselineMs
        delta_vs_baseline_pct = Format-NullableNumber $deltaVsBaselinePct
        delta_vs_previous_ms = Format-NullableNumber $deltaVsPreviousMs
        delta_vs_previous_pct = Format-NullableNumber $deltaVsPreviousPct
        mean_total_to_visible_ms = Format-NullableNumber (Get-Mean -Values $totalToVisibleValues)
        mean_total_to_finish_ms = Format-NullableNumber (Get-Mean -Values $totalToFinishValues)
        mean_worker_total_ms = Format-NullableNumber (Get-Mean -Values $workerTotalValues)
        mean_worker_eval_ms = Format-NullableNumber (Get-Mean -Values $workerEvalValues)
        mean_worker_extract_ms = Format-NullableNumber (Get-Mean -Values $workerExtractValues)
        mean_extract_mesh_read_ms = Format-NullableNumber (Get-Mean -Values $meshReadValues)
        mean_extract_transform_ms = Format-NullableNumber (Get-Mean -Values $transformValues)
        mean_extract_merge_ms = Format-NullableNumber (Get-Mean -Values $mergeValues)
        mean_worker_shared_write_ms = Format-NullableNumber (Get-Mean -Values $sharedWriteValues)
        mean_read_decode_ms = Format-NullableNumber (Get-Mean -Values $readDecodeValues)
        mean_preview_apply_ms = Format-NullableNumber (Get-Mean -Values $previewApplyValues)
    }
}

function Write-LatestReport {
    param(
        [string]$Path,
        [string]$PassLabel,
        [string]$RunStartedUtc,
        [bool]$Accepted,
        [string[]]$AcceptanceNotes,
        [object[]]$ScenarioSummaries
    )

    $lines = New-Object System.Collections.Generic.List[string]
    $statusText = if ($Accepted) { "accepted" } else { "failed" }
    $lines.Add("# Blender Bridge Perf Run")
    $lines.Add("")
    $lines.Add("- Pass label: $PassLabel")
    $lines.Add("- Run started (UTC): $RunStartedUtc")
    $lines.Add("- Status: **$statusText**")
    $lines.Add("")

    if ($AcceptanceNotes.Count -gt 0) {
        $lines.Add("## Acceptance")
        $lines.Add("")
        foreach ($note in $AcceptanceNotes) {
            $lines.Add("- $note")
        }
        $lines.Add("")
    }

    $lines.Add("## Scenario Summary")
    $lines.Add("")
    $lines.Add("| Scenario | Metric | Mean (ms) | Delta vs baseline (ms) | Delta vs previous (ms) |")
    $lines.Add("| --- | --- | ---: | ---: | ---: |")
    foreach ($summary in $ScenarioSummaries) {
        $lines.Add((
            "| {0} | {1} | {2} | {3} | {4} |" -f
            $summary.scenario_id,
            $summary.primary_metric_name,
            $summary.mean_primary_ms,
            $summary.delta_vs_baseline_ms,
            $summary.delta_vs_previous_ms))
    }
    $lines.Add("")

    [System.IO.File]::WriteAllLines($Path, [string[]]$lines.ToArray())
}

$repoRootResolved = (Resolve-Path -LiteralPath $RepoRoot).Path
$projectRootResolved = (Resolve-Path -LiteralPath $ProjectRoot).Path
$projectFile = Join-Path $projectRootResolved "blender_unreal.uproject"
$manifestPath = Join-Path $repoRootResolved "Plugins\BlenderBridge\Config\PerfBenchmarks.json"
$rawTimingCsvPath = Join-Path $repoRootResolved "BlenderBridgeTimingHistory.csv"
$summaryCsvPath = Join-Path $repoRootResolved "BlenderBridgePerfSummary.csv"
$latestReportPath = Join-Path $repoRootResolved "BlenderBridgePerfLatestReport.md"
$automationReportPath = Join-Path $projectRootResolved "Saved\Automation\BlenderBridgePerf"
$runLogDirectory = Join-Path $repoRootResolved ("BlenderBridgePerfLogs\{0}_{1}" -f $PassLabel, ([DateTime]::UtcNow.ToString("yyyyMMddTHHmmssZ")))
$runStartedUtc = [DateTime]::UtcNow.ToString("o")
$runStartedAt = Get-Date

$manifest = Load-PerfManifest -ManifestPath $manifestPath
$scenarioById = @{}
$expectedMeasuredRows = 0
foreach ($scenario in $manifest.scenarios) {
    $scenarioById[[string]$scenario.id] = $scenario
    $expectedMeasuredRows += [int]$scenario.measure_count
}

$existingRawRowCount = (Load-CsvRows -Path $rawTimingCsvPath).Count

$staleCmdProcesses = @(Get-Process -Name "UnrealEditor-Cmd" -ErrorAction SilentlyContinue)
if ($staleCmdProcesses.Count -gt 0) {
    $processList = ($staleCmdProcesses | Select-Object -ExpandProperty Id) -join ", "
    throw "One or more UnrealEditor-Cmd processes are already running ($processList). Close or stop those headless editor runs before starting the perf loop."
}

if (-not $SkipSync) {
    $syncArgs = @(
        "-ExecutionPolicy", "Bypass",
        "-File", (Join-Path $repoRootResolved "scripts\Sync-BlenderBridgeToProject.ps1")
    )
    if ($SkipBuild) {
        $syncArgs += "-PreserveGeneratedArtifacts"
    }

    & powershell @syncArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Plugin sync failed with exit code $LASTEXITCODE"
    }
}

if (-not $SkipBuild) {
    & (Join-Path $EngineRoot "Engine\Build\BatchFiles\Build.bat") blender_unrealEditor Win64 Development $projectFile -WaitMutex
    if ($LASTEXITCODE -ne 0) {
        throw "Unreal editor build failed with exit code $LASTEXITCODE"
    }
}

if (-not (Test-Path -LiteralPath $runLogDirectory)) {
    New-Item -ItemType Directory -Path $runLogDirectory | Out-Null
}

if (Test-Path -LiteralPath $automationReportPath) {
    Remove-Item -LiteralPath $automationReportPath -Recurse -Force
}

$unrealEditorCmd = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$scenarioLogPaths = @{}
foreach ($scenario in $manifest.scenarios) {
    $scenarioId = [string]$scenario.id
    $scenarioLogPath = Join-Path $runLogDirectory ("{0}.log" -f $scenarioId)
    $scenarioReportPath = Join-Path $automationReportPath $scenarioId
    $scenarioLogPaths[$scenarioId] = $scenarioLogPath

    Write-Host ("Running scenario '{0}' via test '{1}'..." -f $scenarioId, (Get-ScenarioAutomationTestName -Scenario $scenario))
    Invoke-UnrealAutomationScenario `
        -UnrealEditorCmd $unrealEditorCmd `
        -ProjectFile $projectFile `
        -PassLabel $PassLabel `
        -Scenario $scenario `
        -LogPath $scenarioLogPath `
        -ReportPath $scenarioReportPath `
        -TimeoutMinutes $AutomationTimeoutMinutes | Out-Null
}

if (-not (Test-Path -LiteralPath $rawTimingCsvPath)) {
    throw "Raw timing CSV was not produced at $rawTimingCsvPath"
}

$allRawRows = Load-CsvRows -Path $rawTimingCsvPath
$newRawRows = @()
if ($existingRawRowCount -lt $allRawRows.Count) {
    $newRawRows = @($allRawRows | Select-Object -Skip $existingRawRowCount)
}

$measuredRows = @(
    $newRawRows |
        Where-Object {
            $_.run_label -eq $PassLabel -and
            -not [string]::IsNullOrWhiteSpace($_.scenario_id) -and
            $_.cache_hit -ne "true"
        }
)

if ($measuredRows.Count -ne $expectedMeasuredRows) {
    $groupedCounts = $measuredRows | Group-Object scenario_id | ForEach-Object { "{0}={1}" -f $_.Name, $_.Count }
    throw "Expected $expectedMeasuredRows measured timing rows for pass '$PassLabel', but found $($measuredRows.Count). Found counts: $($groupedCounts -join ', ')"
}

foreach ($scenario in $manifest.scenarios) {
    $scenarioRows = @($measuredRows | Where-Object { $_.scenario_id -eq [string]$scenario.id })
    if ($scenarioRows.Count -ne [int]$scenario.measure_count) {
        throw "Scenario '$($scenario.id)' expected $($scenario.measure_count) measured rows but found $($scenarioRows.Count)"
    }
}

foreach ($scenario in $manifest.scenarios) {
    $scenarioId = [string]$scenario.id
    $effectiveUnrealLogPath = Get-EffectiveUnrealLogPath -PreferredPath $scenarioLogPaths[$scenarioId] -ProjectRootPath $projectRootResolved -RunStartedAt $runStartedAt
    if (Test-Path -LiteralPath $effectiveUnrealLogPath) {
        $logErrors = Select-String -Path $effectiveUnrealLogPath -Pattern 'Automation Test Failed|LogAutomationController: Error:|LogBlenderBridge: Error:|LogBlenderBridgeBake: Error:'
        if ($null -ne $logErrors -and $logErrors.Count -gt 0) {
            throw "The headless automation log reported errors for scenario '$scenarioId'. See $effectiveUnrealLogPath"
        }
    }
    else {
        Write-Warning "No dedicated Unreal log file was found for scenario '$scenarioId'. Continuing because measured timing rows were captured successfully."
    }
}

$existingSummaryRows = Load-CsvRows -Path $summaryCsvPath
$scenarioSummaries = New-Object System.Collections.Generic.List[object]
foreach ($scenario in $manifest.scenarios) {
    $scenarioId = [string]$scenario.id
    $scenarioRows = @($measuredRows | Where-Object { $_.scenario_id -eq $scenarioId })
    $baselineSummary = $existingSummaryRows | Where-Object { $_.scenario_id -eq $scenarioId -and $_.pass_label -eq "baseline" } | Select-Object -Last 1
    $previousSummary = $existingSummaryRows | Where-Object { $_.scenario_id -eq $scenarioId } | Select-Object -Last 1
    $scenarioSummaries.Add((New-ScenarioSummaryRow -RunStartedUtc $runStartedUtc -PassLabel $PassLabel -Scenario $scenario -Rows $scenarioRows -BaselineSummaryRow $baselineSummary -PreviousSummaryRow $previousSummary))
}

$acceptanceNotes = New-Object System.Collections.Generic.List[string]
$accepted = $true
if ($PassLabel -eq "baseline") {
    $acceptanceNotes.Add("Baseline runs are always accepted and become the comparison anchor.")
}
else {
    $houseSummary = $scenarioSummaries | Where-Object { $_.scenario_id -eq "house_preview" } | Select-Object -First 1
    $previousHouseSummary = $existingSummaryRows | Where-Object { $_.scenario_id -eq "house_preview" } | Select-Object -Last 1
    if ($null -eq $houseSummary -or $null -eq $previousHouseSummary) {
        throw "A previous accepted house_preview summary row is required before non-baseline passes can be accepted."
    }

    $previousHouseMean = [double]$previousHouseSummary.mean_primary_ms
    $currentHouseMean = [double]$houseSummary.mean_primary_ms
    $houseImprovementMs = $previousHouseMean - $currentHouseMean
    $houseImprovementPct = if ($previousHouseMean -ne 0.0) { ($houseImprovementMs / $previousHouseMean) * 100.0 } else { 0.0 }

    if ($houseImprovementMs -ge 20.0 -or $houseImprovementPct -ge 5.0) {
        $acceptanceNotes.Add(("house_preview primary metric improved by {0:N2} ms ({1:N2}%)." -f $houseImprovementMs, $houseImprovementPct))
    }
    else {
        $accepted = $false
        $acceptanceNotes.Add(("house_preview primary metric only improved by {0:N2} ms ({1:N2}%), below the required 20 ms or 5% bar." -f $houseImprovementMs, $houseImprovementPct))
    }

    $ownedHotspotMetric = switch ($PassLabel) {
        "area_1_transform_once" { "mean_extract_transform_ms" }
        "area_2_streaming_merge" { "mean_extract_merge_ms" }
        "area_3_mesh_read_pack" { "mean_extract_mesh_read_ms" }
        "area_4_shm_write" { "mean_worker_shared_write_ms" }
        default { "" }
    }

    if ([string]::IsNullOrWhiteSpace($ownedHotspotMetric)) {
        $accepted = $false
        $acceptanceNotes.Add("No owned hotspot metric mapping was defined for this pass label.")
    }
    else {
        $previousHotspot = [double]$previousHouseSummary.$ownedHotspotMetric
        $currentHotspot = [double]$houseSummary.$ownedHotspotMetric
        if ($currentHotspot -lt $previousHotspot) {
            $acceptanceNotes.Add(("{0} dropped from {1:N2} ms to {2:N2} ms." -f $ownedHotspotMetric, $previousHotspot, $currentHotspot))
        }
        else {
            $accepted = $false
            $acceptanceNotes.Add(("{0} did not drop on house_preview: previous {1:N2} ms, current {2:N2} ms." -f $ownedHotspotMetric, $previousHotspot, $currentHotspot))
        }
    }

    foreach ($guardrailSummary in $scenarioSummaries | Where-Object { $_.scenario_id -ne "house_preview" }) {
        $baselineGuardrail = $existingSummaryRows | Where-Object { $_.scenario_id -eq $guardrailSummary.scenario_id -and $_.pass_label -eq "baseline" } | Select-Object -Last 1
        $comparisonGuardrail = if ($null -ne $baselineGuardrail) { $baselineGuardrail } else { $existingSummaryRows | Where-Object { $_.scenario_id -eq $guardrailSummary.scenario_id } | Select-Object -Last 1 }
        if ($null -eq $comparisonGuardrail) {
            continue
        }

        $comparisonMean = [double]$comparisonGuardrail.mean_primary_ms
        $currentMean = [double]$guardrailSummary.mean_primary_ms
        $regressionPct = if ($comparisonMean -ne 0.0) { (($currentMean - $comparisonMean) / $comparisonMean) * 100.0 } else { 0.0 }
        $isSoftSecondaryGuardrail =
            $guardrailSummary.scenario_id -eq "instances_bake" -and
            $PassLabel -like "area_*"
        if ($regressionPct -gt 10.0) {
            if ($isSoftSecondaryGuardrail) {
                $acceptanceNotes.Add(("Secondary guardrail scenario {0} regressed by {1:N2}%%; reported for visibility but not used as a hard acceptance blocker for this preview-focused pass." -f $guardrailSummary.scenario_id, $regressionPct))
            }
            else {
                $accepted = $false
                $acceptanceNotes.Add(("Guardrail scenario {0} regressed by {1:N2}%%." -f $guardrailSummary.scenario_id, $regressionPct))
            }
        }
        else {
            $acceptanceNotes.Add(("Guardrail scenario {0} stayed within the no-regress bar ({1:N2}%%)." -f $guardrailSummary.scenario_id, $regressionPct))
        }
    }
}

$acceptanceNoteArray = [string[]]$acceptanceNotes.ToArray()
$scenarioSummaryArray = [object[]]$scenarioSummaries.ToArray()

Write-LatestReport -Path $latestReportPath -PassLabel $PassLabel -RunStartedUtc $runStartedUtc -Accepted:$accepted -AcceptanceNotes $acceptanceNoteArray -ScenarioSummaries $scenarioSummaryArray

if (-not $accepted) {
    throw "Performance pass '$PassLabel' did not meet the acceptance criteria. See $latestReportPath"
}

if ($scenarioSummaryArray.Count -gt 0) {
    if (-not (Test-Path -LiteralPath $summaryCsvPath)) {
        $scenarioSummaryArray | Export-Csv -LiteralPath $summaryCsvPath -NoTypeInformation
    }
    else {
        $scenarioSummaryArray | Export-Csv -LiteralPath $summaryCsvPath -NoTypeInformation -Append
    }
}

Write-Host "Perf loop completed and accepted for pass '$PassLabel'."
Write-Host "Raw timings: $rawTimingCsvPath"
Write-Host "Summary CSV: $summaryCsvPath"
Write-Host "Latest report: $latestReportPath"
