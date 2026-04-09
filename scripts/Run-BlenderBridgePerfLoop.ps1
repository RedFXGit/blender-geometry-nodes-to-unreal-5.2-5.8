param(
    [Parameter(Mandatory = $true)]
    [string]$PassLabel,
    [string]$RepoRoot = "C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge",
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
        return $sorted[[int]($count / 2)]
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
        return @()
    }

    return @(Import-Csv -LiteralPath $Path)
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
        $lines.Add("| {0} | {1} | {2} | {3} | {4} |" -f $summary.scenario_id, $summary.primary_metric_name, $summary.mean_primary_ms, $summary.delta_vs_baseline_ms, $summary.delta_vs_previous_ms)
    }
    $lines.Add("")

    [System.IO.File]::WriteAllLines($Path, $lines)
}

$repoRootResolved = (Resolve-Path -LiteralPath $RepoRoot).Path
$projectRootResolved = (Resolve-Path -LiteralPath $ProjectRoot).Path
$projectFile = Join-Path $projectRootResolved "blender_unreal.uproject"
$manifestPath = Join-Path $repoRootResolved "Plugins\BlenderBridge\Config\PerfBenchmarks.json"
$rawTimingCsvPath = Join-Path $repoRootResolved "BlenderBridgeTimingHistory.csv"
$summaryCsvPath = Join-Path $repoRootResolved "BlenderBridgePerfSummary.csv"
$latestReportPath = Join-Path $repoRootResolved "BlenderBridgePerfLatestReport.md"
$automationReportPath = Join-Path $projectRootResolved "Saved\Automation\BlenderBridgePerf"
$unrealLogPath = Join-Path $repoRootResolved ("BlenderBridgePerf_{0}_{1}.log" -f $PassLabel, ([DateTime]::UtcNow.ToString("yyyyMMddTHHmmssZ")))
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
    & powershell -ExecutionPolicy Bypass -File (Join-Path $repoRootResolved "scripts\Sync-BlenderBridgeToProject.ps1")
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

if (Test-Path -LiteralPath $automationReportPath) {
    Remove-Item -LiteralPath $automationReportPath -Recurse -Force
}

$unrealEditorCmd = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$execCmds = "blenderbridge.LogCookTimings 1;Automation RunTests BlenderBridge.Perf"
$automationArgs = @(
    $projectFile,
    "-unattended",
    "-nop4",
    "-nosplash",
    "-nullrhi",
    "-NoSound",
    "-BlenderBridgePerfRunLabel=$PassLabel",
    "-ExecCmds=$execCmds",
    "-TestExit=Automation Test Queue Empty",
    "-ReportExportPath=$automationReportPath",
    "-log=$unrealLogPath"
)

$wrapperScriptPath = Join-Path ([System.IO.Path]::GetTempPath()) ("BlenderBridgePerfRunner_{0}.ps1" -f ([Guid]::NewGuid().ToString("N")))
$wrapperScriptLines = New-Object System.Collections.Generic.List[string]
$wrapperScriptLines.Add('$ErrorActionPreference = ''Stop''')
$wrapperScriptLines.Add(("& '{0}' @(" -f ($unrealEditorCmd -replace "'", "''")))
foreach ($argument in $automationArgs) {
    $wrapperScriptLines.Add(("    '{0}'" -f ($argument -replace "'", "''")))
}
$wrapperScriptLines.Add(')')
$wrapperScriptLines.Add('exit $LASTEXITCODE')
[System.IO.File]::WriteAllLines($wrapperScriptPath, $wrapperScriptLines)

$wrapperCommandLine = "-ExecutionPolicy Bypass -File {0}" -f (ConvertTo-QuotedProcessArgument -Value $wrapperScriptPath)
$unrealProcess = Start-Process -FilePath "powershell.exe" -ArgumentList $wrapperCommandLine -PassThru
$deadline = (Get-Date).AddMinutes($AutomationTimeoutMinutes)
while (-not $unrealProcess.HasExited -and (Get-Date) -lt $deadline) {
    Start-Sleep -Seconds 5
    $unrealProcess.Refresh()
}

if (-not $unrealProcess.HasExited) {
    Stop-Process -Id $unrealProcess.Id -Force
    if (Test-Path -LiteralPath $wrapperScriptPath) {
        Remove-Item -LiteralPath $wrapperScriptPath -Force
    }
    $effectiveLogPath = Get-EffectiveUnrealLogPath -PreferredPath $unrealLogPath -ProjectRootPath $projectRootResolved -RunStartedAt $runStartedAt
    throw "Headless Unreal automation run timed out after $AutomationTimeoutMinutes minutes and was terminated. See $effectiveLogPath"
}

if (Test-Path -LiteralPath $wrapperScriptPath) {
    Remove-Item -LiteralPath $wrapperScriptPath -Force
}

if ($unrealProcess.ExitCode -ne 0) {
    $effectiveLogPath = Get-EffectiveUnrealLogPath -PreferredPath $unrealLogPath -ProjectRootPath $projectRootResolved -RunStartedAt $runStartedAt
    throw "Headless Unreal automation run failed with exit code $($unrealProcess.ExitCode). See $effectiveLogPath"
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

$effectiveUnrealLogPath = Get-EffectiveUnrealLogPath -PreferredPath $unrealLogPath -ProjectRootPath $projectRootResolved -RunStartedAt $runStartedAt
if (-not (Test-Path -LiteralPath $effectiveUnrealLogPath)) {
    throw "The Unreal automation log file was not written: $effectiveUnrealLogPath"
}

$logErrors = Select-String -Path $effectiveUnrealLogPath -Pattern 'Automation Test Failed|LogAutomationController: Error:|LogBlenderBridge: Error:|LogBlenderBridgeBake: Error:'
if ($null -ne $logErrors -and $logErrors.Count -gt 0) {
    throw "The headless automation log reported errors. See $effectiveUnrealLogPath"
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
        $previousGuardrail = $existingSummaryRows | Where-Object { $_.scenario_id -eq $guardrailSummary.scenario_id } | Select-Object -Last 1
        if ($null -eq $previousGuardrail) {
            continue
        }

        $previousMean = [double]$previousGuardrail.mean_primary_ms
        $currentMean = [double]$guardrailSummary.mean_primary_ms
        $regressionPct = if ($previousMean -ne 0.0) { (($currentMean - $previousMean) / $previousMean) * 100.0 } else { 0.0 }
        if ($regressionPct -gt 10.0) {
            $accepted = $false
            $acceptanceNotes.Add(("Guardrail scenario {0} regressed by {1:N2}%%." -f $guardrailSummary.scenario_id, $regressionPct))
        }
        else {
            $acceptanceNotes.Add(("Guardrail scenario {0} stayed within the no-regress bar ({1:N2}%%)." -f $guardrailSummary.scenario_id, $regressionPct))
        }
    }
}

Write-LatestReport -Path $latestReportPath -PassLabel $PassLabel -RunStartedUtc $runStartedUtc -Accepted:$accepted -AcceptanceNotes @($acceptanceNotes) -ScenarioSummaries @($scenarioSummaries)

if (-not $accepted) {
    throw "Performance pass '$PassLabel' did not meet the acceptance criteria. See $latestReportPath"
}

if ($scenarioSummaries.Count -gt 0) {
    if (-not (Test-Path -LiteralPath $summaryCsvPath)) {
        $scenarioSummaries | Export-Csv -LiteralPath $summaryCsvPath -NoTypeInformation
    }
    else {
        $scenarioSummaries | Export-Csv -LiteralPath $summaryCsvPath -NoTypeInformation -Append
    }
}

Write-Host "Perf loop completed and accepted for pass '$PassLabel'."
Write-Host "Raw timings: $rawTimingCsvPath"
Write-Host "Summary CSV: $summaryCsvPath"
Write-Host "Latest report: $latestReportPath"
