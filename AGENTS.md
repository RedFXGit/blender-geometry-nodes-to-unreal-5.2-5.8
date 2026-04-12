# Blender Bridge Agent Guide

This file is the consolidated, Codex-oriented source of truth for this repo. It replaces the older `PATHS.md`, `PLAN.md`, and `TESTING.md`.

Validated on `2026-04-12` against the current repository, `README.md`, plugin source, PowerShell scripts, and local machine paths that were referenced by the older docs.

## Scope

- Repo type: Unreal plugin repo, not a full `.uproject`.
- Primary plugin root: `Plugins/BlenderBridge`
- Runtime module: `Plugins/BlenderBridge/Source/BlenderBridgeRuntime`
- Editor module: `Plugins/BlenderBridge/Source/BlenderBridgeEditor`
- Blender worker: `Plugins/BlenderBridge/Scripts/bridge_server.py`
- Helper scripts:
  - `scripts/Sync-BlenderBridgeToProject.ps1`
  - `scripts/Watch-BlenderBridgeToProject.ps1`
  - `scripts/Run-BlenderBridgeSmokeSuite.ps1`
  - `scripts/Run-BlenderBridgePerfLoop.ps1`

## Current Product State

These capabilities are implemented and verified in the current codebase:

- Windows-only named-pipe control plane between Unreal and Blender.
- Double-buffered shared-memory mesh transport.
- Shared editor-global Blender worker session.
- Automatic shared-memory growth by restarting the worker with a larger buffer.
- `.blend` import into a configured `BlenderBridgeSource` asset.
- Dragging a source into the level spawns a configured `BlenderBridgeActor`.
- Per-actor Geometry Nodes target selection.
- Reflected GN inputs backed by `FInstancedPropertyBag`.
- Supported reflected input types: `float`, `int`, `bool`, `vector3`, `color`, `string`, and `enum`.
- Blender interface panel grouping in the Unreal details UI.
- Curve-owner workflow using an embedded Unreal spline when the selected target is a Bezier `CURVE` owner that feeds `Group Input -> Geometry`.
- Procedural preview via `UProceduralMeshComponent`.
- Bake output modes:
  - `StaticMesh`
  - `InstancedHierarchy`
- Material-slot discovery and per-slot material overrides.
- Generated placeholder bake materials.
- Shared prototype reuse for instanced bakes.
- Lightweight editor automation coverage for smoke and perf-smoke scenarios.

These items are still intentionally not implemented:

- In-place shared-memory remap without worker restart.
- Packaged runtime support.
- Custom render proxy / RDG upload path.

## Repo-Validated Layout

Important source files that exist today:

- Plugin descriptor: `Plugins/BlenderBridge/BlenderBridge.uplugin`
- Runtime module entry:
  - `Plugins/BlenderBridge/Source/BlenderBridgeRuntime/Public/BlenderBridgeRuntimeModule.h`
  - `Plugins/BlenderBridge/Source/BlenderBridgeRuntime/Private/BlenderBridgeRuntimeModule.cpp`
- Main runtime types:
  - `Plugins/BlenderBridge/Source/BlenderBridgeRuntime/Public/BlenderBridgeActor.h`
  - `Plugins/BlenderBridge/Source/BlenderBridgeRuntime/Public/BlenderBridgeComponent.h`
  - `Plugins/BlenderBridge/Source/BlenderBridgeRuntime/Public/BlenderBridgeSource.h`
  - `Plugins/BlenderBridge/Source/BlenderBridgeRuntime/Public/BlenderBridgeSessionManager.h`
  - `Plugins/BlenderBridge/Source/BlenderBridgeRuntime/Public/BlenderBridgeWorkerSession.h`
- Editor bake/details/import code:
  - `Plugins/BlenderBridge/Source/BlenderBridgeEditor/Private/BlenderBridgeBakeService.cpp`
  - `Plugins/BlenderBridge/Source/BlenderBridgeEditor/Private/BlenderBridgeActorDetails.cpp`
  - `Plugins/BlenderBridge/Source/BlenderBridgeEditor/Private/BlenderBridgeSourceFactory.cpp`
  - `Plugins/BlenderBridge/Source/BlenderBridgeEditor/Private/ActorFactoryBlenderBridgeSource.cpp`
- Automation tests:
  - `Plugins/BlenderBridge/Source/BlenderBridgeEditor/Private/Tests/BlenderBridgeSmokeAutomation.cpp`
  - `Plugins/BlenderBridge/Source/BlenderBridgeEditor/Private/Tests/BlenderBridgePerfAutomation.cpp`
  - `Plugins/BlenderBridge/Source/BlenderBridgeEditor/Private/Tests/BlenderBridgeObservedGeometryAutomation.cpp`

## Agent Rules For This Repo

- Resolve repo paths from the current workspace first. Older docs and some helper scripts still hard-code the previous repo directory name `blender-unreal-bridge`.
- Treat repo-relative facts as authoritative. Treat machine-specific paths as examples that must be re-checked before use.
- Prefer editing the plugin under `Plugins/BlenderBridge`; the copied plugin inside a separate Unreal project is a synced test target, not the source of truth.
- If changing C++ code, expect an Unreal rebuild.
- If changing `bridge_server.py`, expect the shared Blender session to need a restart.

## Local Machine Paths Verified On This Machine

These paths were validated on `2026-04-12` and do exist locally:

- Unreal Editor: `C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe`
- Unreal headless editor: `C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe`
- Blender 5.1: `C:\Program Files\Blender Foundation\Blender 5.1\blender.exe`
- Test project: `C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal\blender_unreal.uproject`
- Curve-owner example: `C:\Users\Lassiter\Downloads\Blender_Geo\Curve.blend`

These should still be treated as local-machine assumptions, not portable repo requirements.

## Sync Workflow

Use the repo plugin as the source of truth and sync it into the Unreal test project when needed.

Script:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\Sync-BlenderBridgeToProject.ps1
```

What it does today:

- Mirrors `Plugins\BlenderBridge` into the external Unreal project plugin folder.
- Removes generated target artifacts unless `-PreserveGeneratedArtifacts` is passed.
- Excludes `Binaries`, `Intermediate`, `.vs`, `__pycache__`, and `*.pyc`.

Validation note:

- The sync script defaults were updated to this repo's current workspace path.
- If this repo is cloned elsewhere, override `-SourcePlugin` instead of assuming the checked-in local default matches that machine.

## Smoke And Perf Test Workflow

Automated suites verified in code:

- `BlenderBridge.E2E`
- `BlenderBridge.PerfSmoke`

In-editor commands:

```text
Automation RunTests BlenderBridge.E2E
Automation RunTests BlenderBridge.PerfSmoke
```

Headless runner:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\Run-BlenderBridgeSmokeSuite.ps1
```

Verified runner flags:

- `-FunctionalOnly`
- `-PerfOnly`
- `-SkipSync`
- `-SkipBuild`
- `-RunLabel <label>`

Verified runner outputs:

- `BlenderBridgeSmokeLatest.md`
- `BlenderBridgeTimingHistory.csv`
- `BlenderBridgeSmokeLogs\<runlabel_timestamp>\...`

Validation note:

- `scripts\Run-BlenderBridgeSmokeSuite.ps1` now defaults `-RepoRoot` to this workspace path.
- Its default project and engine roots do match real local paths on this machine.

## Fast Manual Validation Flow

1. Open a UE 5.7 C++ project that contains the synced `Plugins/BlenderBridge` plugin.
2. Open `Edit > Project Settings > Plugins > Blender Bridge`.
3. Confirm session settings, especially `Blender Executable Path` and `Bake Root Content Path`.
4. Start the shared session from `Tools > Blender Bridge` if auto-start is disabled.
5. Import a `.blend` into the Content Browser or create a `BlenderBridgeSource`.
6. Drag the source into the level to spawn a `BlenderBridgeActor`.
7. Select `Target` if multiple GN targets are present.
8. Click `Refresh Input Schema` if needed.
9. Edit `Input Values` and click `Request Cook`.
10. Confirm the procedural preview updates.
11. Choose `Bake Output Mode` and click `Bake Output`.
12. Confirm baked output appears under the configured bake root.

## High-Value Manual Checks

- Enum-backed GN input reflection and editing.
- Curve-owner spline import, viewport spline edits, and recook isolation per actor.
- Instanced bake shared-prototype reuse across duplicate actors.
- Material-slot discovery and preview/bake override behavior.
- Automatic session restart after oversized cooks.

## Known Documentation Corrections From The Old Files

- The old docs referenced the repo as `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge`; the current workspace is `C:\Users\Lassiter\Documents\GitHub\blender-geometry-nodes-to-unreal`.
- The old plan said automated end-to-end tests were not implemented. That is outdated; lightweight automation suites are present.
- The old docs split status, paths, and testing into three files. This file replaces that duplication.
