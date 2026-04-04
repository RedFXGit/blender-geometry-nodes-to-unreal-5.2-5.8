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
