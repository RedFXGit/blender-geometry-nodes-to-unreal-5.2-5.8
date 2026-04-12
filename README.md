# Blender Bridge Vertical Slice

This repository contains a working Unreal Editor to Blender bridge prototype for Geometry Nodes driven workflows.

Today it can:

- launch one shared headless Blender worker from Unreal Editor
- exchange control messages over a Windows named pipe
- stream cooked mesh data through double-buffered shared memory
- grow the shared-memory budget automatically when a cook exceeds the current slot size
- import a `.blend` file into the Content Browser as a configured `BlenderBridgeSource` asset
- drag a `BlenderBridgeSource` into the level to spawn a configured `BlenderBridgeActor`
- discover multiple Geometry Nodes targets per `.blend`
- reflect and drive one selected Geometry Nodes target per actor, including enums and Blender interface panel groupings
- import and drive a curve-owner spline target from Unreal when the target object is a Bezier curve with a GN modifier that consumes owner `Geometry`
- preview the cooked result in Unreal with `UProceduralMeshComponent`
- bake the cooked result either to one static mesh asset or to shared prototype meshes plus HISM components in the editor
- preserve one authored UV channel on baked static meshes even when the UE 5.7 build path pads render texcoords
- create generated placeholder materials for baked slots and allow per-slot material overrides

## Current Layout

- Plugin descriptor: `Plugins/BlenderBridge/BlenderBridge.uplugin`
- Runtime module: `Plugins/BlenderBridge/Source/BlenderBridgeRuntime`
- Editor module: `Plugins/BlenderBridge/Source/BlenderBridgeEditor`
- Blender worker: `Plugins/BlenderBridge/Scripts/bridge_server.py`
- Sync helpers: `scripts/Sync-BlenderBridgeToProject.ps1` and `scripts/Watch-BlenderBridgeToProject.ps1`
- Consolidated agent/status/testing guide: `AGENTS.md`

## What The Current Slice Proves

- Windows named pipe control messages work end to end.
- A double-buffered shared-memory contract is in place.
- Unreal Editor can launch one shared Blender worker, route actor requests through it, and consume the resulting payloads.
- A property-bag-backed input schema can drive per-actor cooks while actor input values remain isolated in Unreal.
- Multiple `.blend` sources can stay loaded in the same Blender session without reopening a new mainfile for every actor switch.
- A selected Geometry Nodes target can be loaded from a `.blend` file and streamed into Unreal as triangle-expanded geometry.
- World transforms and basic Blender-to-Unreal coordinate conversion are applied during extraction.
- A Geometry Nodes target can expose and accept `float`, `int`, `bool`, `vector3`, `color`, `string`, and `enum` inputs in real-mesh mode.
- Curve-owner targets can import their authored Blender Bezier curve into the embedded Unreal spline and recook from Unreal spline edits without mutating the loaded source object.
- Reflected GN inputs preserve Blender interface panel hierarchy in the Unreal details panel.
- Oversized cooks can trigger an automatic worker restart with a larger shared-memory mapping and then retry.
- The cooked preview can be baked to a static mesh asset with stable material slots, placeholder materials, and a spawned baked actor.
- The editor bake path can also preserve recoverable GN or spline instances as HISM output while reusing shared prototype meshes per `.blend` source.
- The current bake path preserves a single authored UV channel on the final baked static mesh asset.

## Unreal Usage

1. Copy or sync `Plugins/BlenderBridge` into an Unreal C++ project.
2. Enable the `BlenderBridge` plugin and ensure `ProceduralMeshComponent` is available.
3. Open `Edit > Project Settings > Plugins > Blender Bridge`.
4. Confirm `Automatically Start Session On Editor Launch`, `Blender Executable Path`, and `Bake Root Content Path`.
   Tip: if the Blender path is blank, the plugin will try to auto-detect the newest standard Blender install under `C:\Program Files\Blender Foundation\Blender *\blender.exe`.
5. If auto-start is disabled, use `Tools > Blender Bridge > Create Session`.
6. Create a [UBlenderBridgeSource](C:/Users/Lassiter/Documents/GitHub/blender-geometry-nodes-to-unreal/Plugins/BlenderBridge/Source/BlenderBridgeRuntime/Public/BlenderBridgeSource.h) asset and set `Blend File`.
   Tip: you can also drag a `.blend` file into the Content Browser and the plugin will auto-create a configured source asset.
7. Drag that source asset into the level to spawn an [ABlenderBridgeActor](C:/Users/Lassiter/Documents/GitHub/blender-geometry-nodes-to-unreal/Plugins/BlenderBridge/Source/BlenderBridgeRuntime/Public/BlenderBridgeActor.h) with its bridge component already configured.
8. If the source exposes multiple Geometry Nodes targets, choose one in `Target`.
9. Click `Refresh Input Schema` if needed and wait for `Input Schema` and `Input Values` to populate.
10. Edit reflected values in `Input Values`.
11. If the target is a curve-owner spline tool, confirm the embedded `Bridge Spline Component` imports the authored Blender curve and edit spline points directly in the viewport.
12. Click `Request Cook` and confirm the procedural preview updates in the viewport.
13. If you want a baked asset, choose `Bake Output Mode` and click `Bake Output`.
14. Use `Select Baked Output` to focus the baked actor or asset, and `Delete Baked Output` to remove generated bake content.

## Bake Workflow Notes

- Baking is editor-only.
- If you bake before a valid cook exists, the component automatically queues a cook first and then bakes when the mesh arrives.
- Baked assets are written under the `Bake Root Content Path` project setting, which defaults to `/Game/BlenderBridge/Baked`.
- `Bake Output Mode` defaults to `StaticMesh`.
- `StaticMesh` mode produces:
  - one baked static mesh asset
  - generated placeholder material instances for discovered slots when no override is provided
  - one spawned baked static mesh actor attached to the source actor
- `InstancedHierarchy` mode produces:
  - one baked actor with one `UHierarchicalInstancedStaticMeshComponent` per recovered prototype
  - one shared prototype mesh asset per recovered prototype under a source-scoped `_SharedPrototypes` folder
  - generated placeholder material instances for discovered slots when no override is provided
- Shared prototype meshes are deduped across baked actors that point at the same `.blend` source and recover the same prototype geometry plus material signature.
- Instanced bake succeeds only when the worker can recover at least one true instance placement from evaluated geometry. Mixed direct-mesh plus instance output is supported by emitting singleton prototype placements for the direct mesh.
- `Material Overrides` are tracked by slot id and are applied both to the live preview and to baked output.
- `Hide Preview After Bake` defaults to enabled so the procedural preview disappears when the baked actor is shown.
- The bake path uses the committed `MeshDescription` editor build route and applies a post-build UV correction when UE 5.7 pads render texcoords beyond the authored UV count.

## Geometry Nodes Notes

The current GN path works best when:

- one object clearly owns each target `NODES` modifier
- the saved `.blend` exposes stable object and modifier names
- the graph produces a realized mesh-like output

Target behavior today:

- each loaded `.blend` advertises discovered GN targets as `ObjectName|ModifierName`
- if a source has exactly one target, the component auto-selects it
- if a source has multiple targets, the component requires a target selection
- if a previously saved target no longer exists after reload, the component clears the selection and marks the target unresolved
- each cook evaluates only the selected target and returns that target's mesh output
- curve-owner mode is supported when the selected target object is a `CURVE`, its GN modifier consumes owner `Geometry`, and the curve contains exactly one Bezier spline
- curve-owner cooks use a hidden per-actor duplicate inside the Blender worker so multiple Unreal actors can share one source without overwriting each other

Curve-owner authoring contract today:

- Put the `NODES` modifier on the curve object that owns the authored spline.
- Build the graph from `Group Input -> Geometry`.
- Use one Bezier spline on that owner curve.
- Keep non-primitive sockets such as `Collection` authored in Blender; Unreal preserves those defaults but does not expose them for editing yet.
- The local machine acceptance example used during development is `C:\Users\Lassiter\Downloads\Blender_Geo\Curve.blend`.

Supported reflected socket types today:

- `float`
- `int`
- `bool`
- `vector3`
- `color`
- `string`
- `enum`

## Shared Session Notes

- Session controls are global and live under `Tools > Blender Bridge`.
- The editor can auto-start the shared session on launch through `Edit > Project Settings > Plugins > Blender Bridge > Automatically Start Session On Editor Launch`.
- Components no longer own Blender worker processes.
- Multiple actors can point at the same source and target while keeping separate `Input Values` in Unreal.
- Multiple sources can stay resident in one Blender worker process.
- The current behavior is editor-only. Packaged/runtime bridge execution is intentionally unsupported in this slice.

## Intentional Gaps

- No in-place shared-memory remap without worker restart yet.
- No packaged-runtime bridge execution yet.
- No custom render proxy or RDG path yet.
- Lightweight editor automation suites exist for smoke and perf-smoke coverage.

## Syncing To A UE Project

If you are developing the plugin from this repo and testing it from a separate Unreal project, use the PowerShell helpers in [scripts/Sync-BlenderBridgeToProject.ps1](C:/Users/Lassiter/Documents/GitHub/blender-geometry-nodes-to-unreal/scripts/Sync-BlenderBridgeToProject.ps1) and [scripts/Watch-BlenderBridgeToProject.ps1](C:/Users/Lassiter/Documents/GitHub/blender-geometry-nodes-to-unreal/scripts/Watch-BlenderBridgeToProject.ps1).

Manual one-shot sync:

```powershell
powershell -ExecutionPolicy Bypass -File "C:\Users\Lassiter\Documents\GitHub\blender-geometry-nodes-to-unreal\scripts\Sync-BlenderBridgeToProject.ps1"
```

Continuous watch-and-sync:

```powershell
powershell -ExecutionPolicy Bypass -File "C:\Users\Lassiter\Documents\GitHub\blender-geometry-nodes-to-unreal\scripts\Watch-BlenderBridgeToProject.ps1"
```

These scripts mirror the source plugin into the Unreal project plugin folder while ignoring generated folders such as `Binaries`, `Intermediate`, and `__pycache__`.

## Recommended Reading

- Consolidated repo guide for agents, status, paths, and testing: [AGENTS.md](C:/Users/Lassiter/Documents/GitHub/blender-geometry-nodes-to-unreal/AGENTS.md)
