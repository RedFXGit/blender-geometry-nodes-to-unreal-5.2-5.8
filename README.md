# Geometry Nodes Bridge

Geometry Nodes Bridge is an Unreal Editor plugin that connects Unreal Engine to Blender Geometry Nodes through a shared background Blender worker.

The goal is fast iteration inside the editor: import a `.blend`, choose a Geometry Nodes target, tweak supported inputs from Unreal, preview the cooked result, and bake it into project assets when you are ready.

> `.blend` source -> `GeometryNodesBridgeSource` asset -> `GeometryNodesBridgeActor` -> live cook preview -> baked output

This repository is the plugin source, not a full Unreal project.

## What It Does Today

- Starts one shared Blender session for the Unreal Editor.
- Sends control messages over Windows named pipes and streams mesh data through shared memory.
- Imports `.blend` files into configured `GeometryNodesBridgeSource` assets.
- Discovers multiple Geometry Nodes targets per source.
- Reflects supported Geometry Nodes inputs into Unreal: `float`, `int`, `bool`, `vector3`, `color`, `string`, and `enum`.
- Supports curve-owner workflows with an editable Unreal spline for supported Bezier-curve setups.
- Shows cooked output with a procedural mesh preview in the editor.
- Bakes output to either `StaticMesh` or `InstancedHierarchy`.
- Discovers material slots, supports per-slot overrides, and generates placeholder bake materials when needed.
- Restarts the worker automatically with a larger shared-memory budget when a cook outgrows the current buffer.
- Includes lightweight smoke and perf-smoke automation coverage.

## Current Scope

- Windows only
- Editor only
- Source of truth lives under `Plugins/GeometryNodesBridge`
- Validated in the current repo against Unreal Engine 5.7 and Blender 5.1 on the local development machine

## Quick Start

1. Copy or sync `Plugins/GeometryNodesBridge` into a UE 5.7 C++ project.
2. Enable the `GeometryNodesBridge` plugin.
3. Open `Edit > Project Settings > Plugins > Geometry Nodes Bridge`.
4. Confirm `Blender Executable Path`, `Bake Root Content Path`, and optionally `Automatically Start Session On Editor Launch`.
5. Start the session from `Tools > Geometry Nodes Bridge` if auto-start is disabled.
6. Import a `.blend` file into the Content Browser, or create a `GeometryNodesBridgeSource` asset and set its blend file.
7. Drag the source into the level to spawn a `GeometryNodesBridgeActor`.
8. Choose a `Target` if the source exposes more than one Geometry Nodes modifier.
9. Refresh the input schema if needed, edit `Input Values`, and click `Request Cook`.
10. Use the live preview while iterating, then click `Bake Output` when you want project assets.

## Bake Modes

- `StaticMesh`: creates a baked static mesh asset and baked actor.
- `InstancedHierarchy`: rebuilds recoverable instances as shared prototype meshes plus HISM components.

Material slot discovery and overrides apply to both live preview and baked output.

## Repo Layout

- `Plugins/GeometryNodesBridge` - plugin root
- `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime` - runtime, session, and component code
- `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor` - import pipeline, details UI, bake tooling, and automation tests
- `Plugins/GeometryNodesBridge/Scripts/geometry_nodes_bridge_server.py` - Blender worker entry point
- `scripts/Sync-GeometryNodesBridgeToProject.ps1` - sync the plugin into an external Unreal test project
- `scripts/Watch-GeometryNodesBridgeToProject.ps1` - watch and re-sync during development
- `scripts/Run-GeometryNodesBridgeSmokeSuite.ps1` - run smoke and perf-smoke automation
- `AGENTS.md` - consolidated deeper notes for paths, workflows, and validation

## Syncing Into A Test Project

If you are developing the plugin from this repo and testing it from a separate Unreal project, use:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\Sync-GeometryNodesBridgeToProject.ps1
```

For watch mode:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\Watch-GeometryNodesBridgeToProject.ps1
```

## Automation

Available suites:

- `GeometryNodesBridge.E2E`
- `GeometryNodesBridge.PerfSmoke`

Headless runner:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\Run-GeometryNodesBridgeSmokeSuite.ps1
```

## Not Implemented Yet

- Packaged runtime support
- In-place shared-memory remap without restarting the worker
- Custom render proxy / RDG upload path

## More Detail

`README.md` is intentionally high level.

For repo-validated paths, deeper workflow notes, testing details, and manual validation guidance, see `AGENTS.md`.
