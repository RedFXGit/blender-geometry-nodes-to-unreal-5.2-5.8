# Geometry Nodes Bridge

Geometry Nodes Bridge is an Unreal Editor plugin that connects Unreal Engine to Blender Geometry Nodes through a shared background Blender worker.

The goal is fast iteration inside the editor: import a `.blend`, choose a Geometry Nodes target, tweak supported inputs from Unreal, preview the cooked result, and bake it into project assets when you are ready.

> `.blend` source -> `GeometryNodesBridgeSource` asset -> `GeometryNodesBridgeActor` -> live cook preview -> baked output

This repository is the plugin source, not a full Unreal project.

https://github.com/user-attachments/assets/52baceec-bde3-4e53-af80-4063b05e3fc0

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

https://github.com/user-attachments/assets/398e8a37-8950-42c3-b0b7-160cd1bb24e4

## Current Scope

- Windows only
- Editor only
- Source of truth lives under `Plugins/GeometryNodesBridge`
- Validated in the current repo against Unreal Engine 5.7 and Blender 5.1 on the local development machine

https://github.com/user-attachments/assets/46c2ffc6-8911-4b87-8430-71fe32b62f84

## Quick Start

1. Copy or sync `Plugins/GeometryNodesBridge` into a UE 5.7 C++ project.
2. Enable the `GeometryNodesBridge` plugin.
3. Open the `Geometry Nodes Bridge` settings page in Unreal.
4. Confirm `Blender Executable Path`, `Bridge Worker Script Path`, `Bake Root Content Path`, and optionally `Automatically Start Session On Editor Launch`.
   For this repo layout, point `Bridge Worker Script Path` at `Plugins/GeometryNodesBridgeWorker/geometry_nodes_bridge_server.py`.
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

https://github.com/user-attachments/assets/7d7cce7b-44c9-492c-a54f-a22fe2ec4e32

## Repo Layout

- `Plugins/GeometryNodesBridge` - plugin root
- `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeRuntime` - runtime, session, and component code
- `Plugins/GeometryNodesBridge/Source/GeometryNodesBridgeEditor` - import pipeline, details UI, bake tooling, and automation tests
- `Plugins/GeometryNodesBridgeWorker` - Blender worker source of truth and GPL-licensed worker files
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
<img width="1134" height="733" alt="Automated Testing" src="https://github.com/user-attachments/assets/b9c99aab-fc85-4eb8-bd69-0c3c72648435" />

## Licensing

This repo uses a split-license layout:

- MIT for the Unreal plugin side
- GPL-3.0-or-later for `Plugins/GeometryNodesBridgeWorker`

See `LICENSE` and `Licensing.md` for the exact path mapping.

## Not Implemented Yet

- Packaged runtime support
- In-place shared-memory remap without restarting the worker
- Custom render proxy / RDG upload path

## Thanks

Without these wonderful people sharing creations into the world I would have no inspiration

Pavel Oliva
https://paveloliva.gumroad.com/l/buildify

Shahriar Shahrabi
https://github.com/IRCSS/Blender-Geometry-Node-French-Houses

Ian Fontova
https://ianfontova.gumroad.com/l/FantasyHutGenerator

Miettinen Jesse
https://blenderesse.gumroad.com/
