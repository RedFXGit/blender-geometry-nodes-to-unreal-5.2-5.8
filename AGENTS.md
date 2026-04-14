# Geometry Nodes Bridge Agent Guide

This file is the consolidated, Codex-oriented source of truth for this repo. It replaces the older `PATHS.md`, `PLAN.md`, and `TESTING.md`.

Validated on `2026-04-13` against the current repository, `README.md`, plugin source, PowerShell scripts, and local machine paths that were referenced by the older docs.

## Scope

- Repo type: Unreal plugin repo, not a full `.uproject`.
- Primary plugin root: `Plugins/GeometryNodesBridge`
- Runtime module: `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime`
- Editor module: `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor`
- Blender worker: `Plugins/GeometryNodesBridge/Scripts/geometry_nodes_bridge_server.py`
- Worker staging root: `blender-worker`
- Helper scripts:
  - `scripts/Sync-GeometryNodesBridgeToProject.ps1`
  - `scripts/Stage-GeometryNodesBridgeWorker.ps1`
  - `scripts/Watch-GeometryNodesBridgeToProject.ps1`
  - `scripts/Run-GeometryNodesBridgeSmokeSuite.ps1`
  - `scripts/Run-GeometryNodesBridgePerfLoop.ps1`

## Current Product State

These capabilities are implemented and verified in the current codebase:

- Windows-only named-pipe control plane between Unreal and Blender.
- Double-buffered shared-memory mesh transport.
- Shared editor-global Blender worker session.
- Automatic shared-memory growth by restarting the worker with a larger buffer.
- `.blend` import into a configured `GeometryNodesBridgeSource` asset.
- Dragging a source into the level spawns a configured `GeometryNodesBridgeActor`.
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

- Plugin descriptor: `Plugins/GeometryNodesBridge/GeometryNodesBridge.uplugin`
- Runtime module entry:
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime/Public/GeometryNodesBridgeRuntimeModule.h`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime/Private/GeometryNodesBridgeRuntimeModule.cpp`
- Main runtime types:
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime/Public/GeometryNodesBridgeActor.h`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime/Public/GeometryNodesBridgeComponent.h`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime/Public/GeometryNodesBridgeSource.h`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime/Public/GeometryNodesBridgeSessionManager.h`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime/Public/GeometryNodesBridgeWorkerSession.h`
- Editor bake/details/import code:
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor/Private/GeometryNodesBridgeBakeService.cpp`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor/Private/GeometryNodesBridgeActorDetails.cpp`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor/Private/GeometryNodesBridgeSourceFactory.cpp`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor/Private/ActorFactoryGeometryNodesBridgeSource.cpp`
- Automation tests:
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor/Private/Tests/GeometryNodesBridgeSmokeAutomation.cpp`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor/Private/Tests/GeometryNodesBridgePerfAutomation.cpp`
  - `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor/Private/Tests/GeometryNodesBridgeObservedGeometryAutomation.cpp`

## Agent Rules For This Repo

- Resolve repo paths from the current workspace first. Older docs and some helper scripts still hard-code the previous repo directory name `blender-unreal-bridge`.
- Treat repo-relative facts as authoritative. Treat machine-specific paths as examples that must be re-checked before use.
- Prefer editing the plugin under `Plugins/GeometryNodesBridge`; the copied plugin inside a separate Unreal project is a synced test target, not the source of truth.
- The bundled worker under `Plugins/GeometryNodesBridge/Scripts` remains the current source of truth; `blender-worker` is the phase-1 staging area for packaging an external worker copy.
- If changing C++ code, expect an Unreal rebuild.
- If changing `geometry_nodes_bridge_server.py`, expect the shared Blender session to need a restart.

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
powershell -ExecutionPolicy Bypass -File scripts\Sync-GeometryNodesBridgeToProject.ps1
```

What it does today:

- Mirrors `Plugins\GeometryNodesBridge` into the external Unreal project plugin folder.
- Removes generated target artifacts unless `-PreserveGeneratedArtifacts` is passed.
- Excludes `Binaries`, `Intermediate`, `.vs`, `__pycache__`, and `*.pyc`.

Validation note:

- The sync script defaults were updated to this repo's current workspace path.
- If this repo is cloned elsewhere, override `-SourcePlugin` instead of assuming the checked-in local default matches that machine.

## Worker Packaging Workflow

Use the bundled plugin worker as the source of truth and stage a separate worker package when you need an external install path.

Script:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\Stage-GeometryNodesBridgeWorker.ps1
```

What it does today:

- Copies `geometry_nodes_bridge_server.py` and `geometry_nodes_bridge_worker` into `blender-worker\staging\GeometryNodesBridgeWorker`.
- Copies `blender-worker\README.md` into the staged worker folder.
- Removes `__pycache__` and `*.pyc` from the staged output.

Runtime note:

- `Bridge Worker Script Path` in project settings can point at an external staged worker.
- Leaving that setting empty keeps using the bundled plugin worker.

## Smoke And Perf Test Workflow

Automated suites verified in code:

- `GeometryNodesBridge.E2E`
- `GeometryNodesBridge.PerfSmoke`

In-editor commands:

```text
Automation RunTests GeometryNodesBridge.E2E
Automation RunTests GeometryNodesBridge.PerfSmoke
```

Headless runner:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\Run-GeometryNodesBridgeSmokeSuite.ps1
```

Verified runner flags:

- `-FunctionalOnly`
- `-PerfOnly`
- `-SkipSync`
- `-SkipBuild`
- `-RunLabel <label>`

Verified runner outputs:

- `GeometryNodesBridgeSmokeLatest.md`
- `GeometryNodesBridgeTimingHistory.csv`
- `GeometryNodesBridgeSmokeLogs\<runlabel_timestamp>\...`

Validation note:

- `scripts\Run-GeometryNodesBridgeSmokeSuite.ps1` now defaults `-RepoRoot` to this workspace path.
- Its default project and engine roots do match real local paths on this machine.

## Fast Manual Validation Flow

1. Open a UE 5.7 C++ project that contains the synced `Plugins/GeometryNodesBridge` plugin.
2. Open `Edit > Project Settings > Plugins > Geometry Nodes Bridge`.
3. Confirm session settings, especially `Blender Executable Path` and `Bake Root Content Path`.
4. Start the shared session from `Tools > Geometry Nodes Bridge` if auto-start is disabled.
5. Import a `.blend` into the Content Browser or create a `GeometryNodesBridgeSource`.
6. Drag the source into the level to spawn a `GeometryNodesBridgeActor`.
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
