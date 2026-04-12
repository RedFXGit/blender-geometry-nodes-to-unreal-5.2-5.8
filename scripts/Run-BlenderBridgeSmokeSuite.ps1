param(
    [string]$RunLabel = "smoke",
    [string]$RepoRoot = "C:\Users\Lassiter\Documents\GitHub\blender-geometry-nodes-to-unreal",
    [string]$ProjectRoot = "C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal",
    [string]$EngineRoot = "C:\Program Files\Epic Games\UE_5.7",
    [int]$AutomationTimeoutMinutes = 10,
    [switch]$SkipSync,
    [switch]$SkipBuild,
    [switch]$FunctionalOnly,
    [switch]$PerfOnly
)

$ErrorActionPreference = "Stop"

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

function Load-JsonFile {
    param([Parameter(Mandatory = $true)][string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        throw "Required JSON file not found: $Path"
    }

    return Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
}

function Load-CsvRows {
    param([string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        return ,@()
    }

    return ,@(Import-Csv -LiteralPath $Path)
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

function Get-DoubleValues {
    param(
        [Parameter(Mandatory = $true)][object[]]$Rows,
        [Parameter(Mandatory = $true)][string]$ColumnName
    )

    $values = New-Object System.Collections.Generic.List[double]
    foreach ($row in $Rows) {
        $parsed = 0.0
        if ([double]::TryParse($row.$ColumnName, [System.Globalization.NumberStyles]::Float, [System.Globalization.CultureInfo]::InvariantCulture, [ref]$parsed)) {
            $values.Add($parsed)
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

    return $PreferredPath
}

function Invoke-UnrealAutomationScenario {
    param(
        [Parameter(Mandatory = $true)][string]$UnrealEditorCmd,
        [Parameter(Mandatory = $true)][string]$ProjectFile,
        [Parameter(Mandatory = $true)][string]$AutomationTestName,
        [Parameter(Mandatory = $true)][string]$DisplayName,
        [Parameter(Mandatory = $true)][string]$RunLabel,
        [Parameter(Mandatory = $true)][string]$LogPath,
        [Parameter(Mandatory = $true)][string]$ReportPath,
        [Parameter(Mandatory = $true)][int]$TimeoutMinutes
    )

    if (Test-Path -LiteralPath $LogPath) {
        Remove-Item -LiteralPath $LogPath -Force
    }

    if (Test-Path -LiteralPath $ReportPath) {
        Remove-Item -LiteralPath $ReportPath -Recurse -Force
    }

    $projectRoot = Split-Path -Parent $ProjectFile
    $projectLogPath = Join-Path $projectRoot "Saved\Logs\blender_unreal.log"
    $completionMarker = "LogAutomationController: Display: Test Completed. Result={Success} Name={$DisplayName} Path={$AutomationTestName}"
    $queueEmptyMarker = "LogAutomationCommandLine: Display: ...Automation Test Queue Empty"
    $runStartedAt = Get-Date
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
        "-BlenderBridgeAutomationRunLabel=$RunLabel",
        "-ExecCmds=Automation RunTests $AutomationTestName",
        "-TestExit=Automation Test Queue Empty",
        "-ReportExportPath=$ReportPath",
        "-log=$LogPath"
    )

    $processStartInfo.Arguments = ($argumentList | ForEach-Object { ConvertTo-QuotedProcessArgument -Value $_ }) -join " "

    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $processStartInfo
    if (-not $process.Start()) {
        throw "Failed to start UnrealEditor-Cmd for test '$AutomationTestName'."
    }

    $deadline = (Get-Date).AddMinutes([Math]::Max(1, $TimeoutMinutes))
    while (-not $process.HasExited) {
        if ($process.WaitForExit(2000)) {
            break
        }

        if ((Get-Date) -ge $deadline) {
            try { $process.Kill($true) } catch {}
            throw "Headless Unreal automation run for '$AutomationTestName' timed out after $TimeoutMinutes minutes. See $projectLogPath"
        }

        $activeLogPath = if (Test-Path -LiteralPath $LogPath) { $LogPath } elseif (Test-Path -LiteralPath $projectLogPath) { $projectLogPath } else { $null }
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
                try { $process.Kill($true) } catch {}
                return
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
        throw "Headless Unreal automation run for '$AutomationTestName' failed with exit code $($process.ExitCode). See $effectiveExitLogPath"
    }
}

function Write-SmokeReport {
    param(
        [string]$Path,
        [string]$RunLabel,
        [object[]]$FunctionalResults,
        [object[]]$PerfResults,
        [string[]]$Failures
    )

    $lines = New-Object System.Collections.Generic.List[string]
    $status = if ($Failures.Count -eq 0) { "passed" } else { "failed" }
    $lines.Add("# Blender Bridge Smoke Suite")
    $lines.Add("")
    $lines.Add("- Run label: $RunLabel")
    $lines.Add("- Status: **$status**")
    $lines.Add("")

    if ($FunctionalResults.Count -gt 0) {
        $lines.Add("## Functional")
        $lines.Add("")
        foreach ($result in $FunctionalResults) {
            $lines.Add(("- {0}: **{1}**" -f $result.name, $result.status))
        }
        $lines.Add("")
    }

    if ($PerfResults.Count -gt 0) {
        $lines.Add("## Performance")
        $lines.Add("")
        $lines.Add("| Scenario | Median (ms) | Threshold (ms) | Status |")
        $lines.Add("| --- | ---: | ---: | --- |")
        foreach ($result in $PerfResults) {
            $lines.Add((
                "| {0} | {1} | {2} | {3} |" -f
                $result.name,
                $result.median_ms,
                $result.threshold_ms,
                $result.status))
        }
        $lines.Add("")
    }

    if ($Failures.Count -gt 0) {
        $lines.Add("## Failures")
        $lines.Add("")
        foreach ($failure in $Failures) {
            $lines.Add("- $failure")
        }
        $lines.Add("")
    }

    [System.IO.File]::WriteAllLines($Path, [string[]]$lines.ToArray())
}

if ($FunctionalOnly -and $PerfOnly) {
    throw "Use either -FunctionalOnly or -PerfOnly, not both."
}

$repoRootResolved = (Resolve-Path -LiteralPath $RepoRoot).Path
$projectRootResolved = (Resolve-Path -LiteralPath $ProjectRoot).Path
$projectFile = Join-Path $projectRootResolved "blender_unreal.uproject"
$rawTimingCsvPath = Join-Path $repoRootResolved "BlenderBridgeTimingHistory.csv"
$latestReportPath = Join-Path $repoRootResolved "BlenderBridgeSmokeLatest.md"
$runLogDirectory = Join-Path $repoRootResolved ("BlenderBridgeSmokeLogs\{0}_{1}" -f $RunLabel, ([DateTime]::UtcNow.ToString("yyyyMMddTHHmmssZ")))
$automationReportPath = Join-Path $projectRootResolved "Saved\Automation\BlenderBridgeSmoke"
$functionalManifestPath = Join-Path $repoRootResolved "Plugins\BlenderBridge\Config\E2ESmokeScenarios.json"
$perfManifestPath = Join-Path $repoRootResolved "Plugins\BlenderBridge\Config\PerfSmokeBenchmarks.json"
$functionalPrefix = "BlenderBridge.E2E"
$perfPrefix = "BlenderBridge.PerfSmoke"
$functionalResults = New-Object System.Collections.Generic.List[object]
$perfResults = New-Object System.Collections.Generic.List[object]
$failures = New-Object System.Collections.Generic.List[string]

$functionalManifest = if ($PerfOnly) { $null } else { Load-JsonFile -Path $functionalManifestPath }
$perfManifest = if ($FunctionalOnly) { $null } else { Load-JsonFile -Path $perfManifestPath }
$expectedMeasuredRows = 0
if ($null -ne $perfManifest) {
    foreach ($scenario in $perfManifest.scenarios) {
        $expectedMeasuredRows += [int]$scenario.measure_count
    }
}

if (-not (Test-Path -LiteralPath $runLogDirectory)) {
    New-Item -ItemType Directory -Path $runLogDirectory | Out-Null
}

$staleCmdProcesses = @(Get-Process -Name "UnrealEditor-Cmd" -ErrorAction SilentlyContinue)
if ($staleCmdProcesses.Count -gt 0) {
    $processList = ($staleCmdProcesses | Select-Object -ExpandProperty Id) -join ", "
    throw "One or more UnrealEditor-Cmd processes are already running ($processList). Close or stop those headless editor runs before starting the smoke suite."
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

$unrealEditorCmd = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$existingRawRowCount = (Load-CsvRows -Path $rawTimingCsvPath).Count

try {
    if (Test-Path -LiteralPath $automationReportPath) {
        Remove-Item -LiteralPath $automationReportPath -Recurse -Force
    }

    if (-not $PerfOnly) {
        foreach ($scenario in $functionalManifest.scenarios) {
            $testName = "$functionalPrefix.$($scenario.display_name)"
            $scenarioLogPath = Join-Path $runLogDirectory ("functional_{0}.log" -f $scenario.id)
            $scenarioReportPath = Join-Path $automationReportPath ("functional_{0}" -f $scenario.id)
            Invoke-UnrealAutomationScenario `
                -UnrealEditorCmd $unrealEditorCmd `
                -ProjectFile $projectFile `
                -AutomationTestName $testName `
                -DisplayName ([string]$scenario.display_name) `
                -RunLabel $RunLabel `
                -LogPath $scenarioLogPath `
                -ReportPath $scenarioReportPath `
                -TimeoutMinutes $AutomationTimeoutMinutes

            $functionalResults.Add([pscustomobject]@{
                name = $testName
                status = "pass"
            })
        }
    }

    if (-not $FunctionalOnly) {
        foreach ($scenario in $perfManifest.scenarios) {
            $testName = "$perfPrefix.$($scenario.display_name)"
            $scenarioLogPath = Join-Path $runLogDirectory ("perf_{0}.log" -f $scenario.id)
            $scenarioReportPath = Join-Path $automationReportPath ("perf_{0}" -f $scenario.id)
            Invoke-UnrealAutomationScenario `
                -UnrealEditorCmd $unrealEditorCmd `
                -ProjectFile $projectFile `
                -AutomationTestName $testName `
                -DisplayName ([string]$scenario.display_name) `
                -RunLabel $RunLabel `
                -LogPath $scenarioLogPath `
                -ReportPath $scenarioReportPath `
                -TimeoutMinutes $AutomationTimeoutMinutes
        }

        $allRawRows = Load-CsvRows -Path $rawTimingCsvPath
        $newRawRows = if ($existingRawRowCount -lt $allRawRows.Count) { @($allRawRows | Select-Object -Skip $existingRawRowCount) } else { @() }
        $measuredRows = @(
            $newRawRows |
                Where-Object {
                    $_.run_label -eq $RunLabel -and
                    -not [string]::IsNullOrWhiteSpace($_.scenario_id) -and
                    -not [string]::IsNullOrWhiteSpace($_.iteration_index) -and
                    $_.cache_hit -ne "true"
                }
        )

        if ($measuredRows.Count -ne $expectedMeasuredRows) {
            throw "Expected $expectedMeasuredRows perf-smoke timing rows for run '$RunLabel', but found $($measuredRows.Count)."
        }

        foreach ($scenario in $perfManifest.scenarios) {
            $scenarioRows = @($measuredRows | Where-Object { $_.scenario_id -eq [string]$scenario.id })
            if ($scenarioRows.Count -ne [int]$scenario.measure_count) {
                throw "Perf-smoke scenario '$($scenario.id)' expected $($scenario.measure_count) timing rows but found $($scenarioRows.Count)."
            }

            $medianMs = Get-Median -Values (Get-DoubleValues -Rows $scenarioRows -ColumnName "total_to_visible_ms")
            $baselineMs = [double]$scenario.baseline_median_ms
            $percentAllowanceMs = $baselineMs * ([double]$scenario.allowed_regression_percent / 100.0)
            $thresholdMs = $baselineMs + [Math]::Max($percentAllowanceMs, [double]$scenario.allowed_regression_ms)
            $status = if ($null -ne $medianMs -and $medianMs -le $thresholdMs) { "pass" } else { "fail" }

            $perfResults.Add([pscustomobject]@{
                name = "$perfPrefix.$($scenario.display_name)"
                median_ms = Format-NullableNumber $medianMs
                threshold_ms = Format-NullableNumber $thresholdMs
                status = $status
            })

            if ($status -ne "pass") {
                $failures.Add(("Perf-smoke scenario {0} exceeded its threshold: median {1} ms vs allowed {2} ms." -f $scenario.id, (Format-NullableNumber $medianMs), (Format-NullableNumber $thresholdMs)))
            }
        }
    }
}
catch {
    $failures.Add($_.Exception.Message)
}

Write-SmokeReport `
    -Path $latestReportPath `
    -RunLabel $RunLabel `
    -FunctionalResults $functionalResults.ToArray() `
    -PerfResults $perfResults.ToArray() `
    -Failures $failures.ToArray()

if ($failures.Count -gt 0) {
    throw ($failures -join [Environment]::NewLine)
}
