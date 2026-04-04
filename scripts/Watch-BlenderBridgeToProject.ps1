param(
    [string]$SourcePlugin = "C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge",
    [string]$TargetPlugin = "C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal\Plugins\BlenderBridge",
    [int]$DebounceMilliseconds = 600
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $SourcePlugin)) {
    throw "Source plugin path does not exist: $SourcePlugin"
}

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$syncScript = Join-Path $scriptRoot "Sync-BlenderBridgeToProject.ps1"

if (-not (Test-Path -LiteralPath $syncScript)) {
    throw "Sync script not found: $syncScript"
}

$resolvedSource = (Resolve-Path -LiteralPath $SourcePlugin).Path
$resolvedTarget = $TargetPlugin

Write-Host "Watching BlenderBridge source for changes..."
Write-Host "Source: $resolvedSource"
Write-Host "Target: $resolvedTarget"
Write-Host "Press Ctrl+C to stop."

$watcher = New-Object System.IO.FileSystemWatcher
$watcher.Path = $resolvedSource
$watcher.IncludeSubdirectories = $true
$watcher.NotifyFilter = [System.IO.NotifyFilters]'FileName, DirectoryName, LastWrite, Size, CreationTime'
$watcher.EnableRaisingEvents = $true

$script:pendingSync = $false
$script:lastEventUtc = [DateTime]::MinValue

$onChange = {
    $fullPath = $Event.SourceEventArgs.FullPath
    if ($fullPath -match "\\Binaries\\|\\Intermediate\\|\\__pycache__\\") {
        return
    }

    $script:lastEventUtc = [DateTime]::UtcNow
    $script:pendingSync = $true
}

$registrations = @(
    (Register-ObjectEvent -InputObject $watcher -EventName Changed -Action $onChange),
    (Register-ObjectEvent -InputObject $watcher -EventName Created -Action $onChange),
    (Register-ObjectEvent -InputObject $watcher -EventName Deleted -Action $onChange),
    (Register-ObjectEvent -InputObject $watcher -EventName Renamed -Action $onChange)
)

try {
    & $syncScript -SourcePlugin $resolvedSource -TargetPlugin $resolvedTarget

    while ($true) {
        Start-Sleep -Milliseconds $DebounceMilliseconds

        if (-not $script:pendingSync) {
            continue
        }

        $elapsed = ([DateTime]::UtcNow - $script:lastEventUtc).TotalMilliseconds
        if ($elapsed -lt $DebounceMilliseconds) {
            continue
        }

        $script:pendingSync = $false
        Write-Host ""
        Write-Host ("[{0}] Change detected, syncing..." -f (Get-Date -Format "HH:mm:ss"))
        & $syncScript -SourcePlugin $resolvedSource -TargetPlugin $resolvedTarget
    }
}
finally {
    foreach ($registration in $registrations) {
        Unregister-Event -SourceIdentifier $registration.Name -ErrorAction SilentlyContinue
        Remove-Job -Id $registration.Id -Force -ErrorAction SilentlyContinue
    }

    $watcher.EnableRaisingEvents = $false
    $watcher.Dispose()
}
