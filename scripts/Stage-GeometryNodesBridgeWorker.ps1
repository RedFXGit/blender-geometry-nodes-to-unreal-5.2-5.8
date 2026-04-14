param(
    [string]$SourceScripts = "",
    [string]$OutputRoot = ""
)

$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".."))

if ([string]::IsNullOrWhiteSpace($SourceScripts)) {
    $SourceScripts = Join-Path $repoRoot "Plugins\GeometryNodesBridge\Scripts"
}

if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
    $OutputRoot = Join-Path $repoRoot "blender-worker\staging\GeometryNodesBridgeWorker"
}

$sourceResolved = [System.IO.Path]::GetFullPath($SourceScripts)
$outputResolved = [System.IO.Path]::GetFullPath($OutputRoot)

if (-not (Test-Path -LiteralPath $sourceResolved)) {
    throw "Worker source scripts path does not exist: $sourceResolved"
}

$entryScript = Join-Path $sourceResolved "geometry_nodes_bridge_server.py"
$workerPackage = Join-Path $sourceResolved "geometry_nodes_bridge_worker"

if (-not (Test-Path -LiteralPath $entryScript)) {
    throw "Worker entry script was not found: $entryScript"
}

if (-not (Test-Path -LiteralPath $workerPackage)) {
    throw "Worker package directory was not found: $workerPackage"
}

if (Test-Path -LiteralPath $outputResolved) {
    throw "Output root already exists: $outputResolved`nRemove it first or choose a different -OutputRoot."
}

New-Item -ItemType Directory -Path $outputResolved -Force | Out-Null
Copy-Item -LiteralPath $entryScript -Destination (Join-Path $outputResolved "geometry_nodes_bridge_server.py") -Force
Copy-Item -LiteralPath $workerPackage -Destination (Join-Path $outputResolved "geometry_nodes_bridge_worker") -Recurse -Force

$stagedReadme = Join-Path $repoRoot "blender-worker\README.md"
if (Test-Path -LiteralPath $stagedReadme) {
    Copy-Item -LiteralPath $stagedReadme -Destination (Join-Path $outputResolved "README.md") -Force
}

$pycFiles = Get-ChildItem -LiteralPath $outputResolved -Recurse -File -Filter "*.pyc" -ErrorAction SilentlyContinue
foreach ($pycFile in $pycFiles) {
    Remove-Item -LiteralPath $pycFile.FullName -Force -ErrorAction Stop
}

$pyCacheDirectories = Get-ChildItem -LiteralPath $outputResolved -Recurse -Directory -Filter "__pycache__" -ErrorAction SilentlyContinue
foreach ($pyCacheDirectory in $pyCacheDirectories) {
    Remove-Item -LiteralPath $pyCacheDirectory.FullName -Recurse -Force -ErrorAction Stop
}

Write-Host "Staged Blender worker package:"
Write-Host "  $outputResolved"
Write-Host ""
Write-Host "Point the Unreal plugin's Bridge Worker Script Path at:"
Write-Host "  $(Join-Path $outputResolved 'geometry_nodes_bridge_server.py')"
