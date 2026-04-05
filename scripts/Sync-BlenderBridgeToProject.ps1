param(
    [string]$SourcePlugin = "C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge",
    [string]$TargetPlugin = "C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal\Plugins\BlenderBridge",
    [switch]$DryRun
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $SourcePlugin)) {
    throw "Source plugin path does not exist: $SourcePlugin"
}

if (-not (Test-Path -LiteralPath $TargetPlugin)) {
    New-Item -ItemType Directory -Path $TargetPlugin -Force | Out-Null
}

$sourceResolved = (Resolve-Path -LiteralPath $SourcePlugin).Path
$targetResolved = (Resolve-Path -LiteralPath $TargetPlugin).Path

Write-Host "Syncing BlenderBridge plugin..."
Write-Host "Source: $sourceResolved"
Write-Host "Target: $targetResolved"

$generatedDirectories = @(
    "Binaries",
    "Intermediate",
    ".vs",
    "__pycache__"
)

Write-Host "Clearing generated plugin artifacts from target..."
foreach ($directoryName in $generatedDirectories) {
    $generatedPath = Join-Path $targetResolved $directoryName
    if (-not (Test-Path -LiteralPath $generatedPath)) {
        continue
    }

    if ($DryRun) {
        Write-Host "[DryRun] Would remove directory: $generatedPath"
        continue
    }

    try {
        Remove-Item -LiteralPath $generatedPath -Recurse -Force -ErrorAction Stop
    }
    catch {
        Write-Warning "Could not remove generated directory '$generatedPath'. Continuing sync. $($_.Exception.Message)"
    }
}

$pycFiles = Get-ChildItem -LiteralPath $targetResolved -Recurse -File -Filter "*.pyc" -ErrorAction SilentlyContinue
foreach ($pycFile in $pycFiles) {
    if ($DryRun) {
        Write-Host "[DryRun] Would remove file: $($pycFile.FullName)"
        continue
    }

    try {
        Remove-Item -LiteralPath $pycFile.FullName -Force -ErrorAction Stop
    }
    catch {
        Write-Warning "Could not remove generated file '$($pycFile.FullName)'. Continuing sync. $($_.Exception.Message)"
    }
}

$robocopyArgs = @(
    $sourceResolved,
    $targetResolved,
    "/MIR",
    "/R:1",
    "/W:1",
    "/NFL",
    "/NDL",
    "/NP",
    "/XD", "Binaries", "Intermediate", ".vs", "__pycache__",
    "/XF", "*.pyc"
)

if ($DryRun) {
    $robocopyArgs += "/L"
}

$robocopyOutput = & robocopy @robocopyArgs
$robocopyOutput | Out-Host
$exitCode = $LASTEXITCODE

if (($robocopyOutput -join "`n") -match "ERROR \d+" -or ($robocopyOutput -join "`n") -match "Access is denied") {
    throw "Robocopy reported an error."
}

if ($exitCode -ge 8) {
    throw "Robocopy failed with exit code $exitCode"
}

Write-Host "Sync complete. Robocopy exit code: $exitCode"
