# Blender Bridge Vertical Slice

This repository contains a working Unreal-to-Blender editor bridge prototype.

Today it can:

- launch one shared headless Blender worker from Unreal Editor
- exchange control messages over a Windows named pipe
- stream cooked mesh data through double-buffered shared memory
- grow the shared-memory budget automatically when a cook exceeds the current slot size
- display the result in Unreal with `UProceduralMeshComponent`
- keep multiple `.blend` sources loaded in one Blender process
- import a `.blend` file into the Content Browser as a configured `BlenderBridgeSource` asset
- discover multiple Geometry Nodes targets per `.blend`
- reflect and drive one selected Geometry Nodes target per actor, including enums and Blender interface panel groupings

## Current Layout

- Plugin descriptor: `Plugins/BlenderBridge/BlenderBridge.uplugin`
- Runtime module: `Plugins/BlenderBridge/Source/BlenderBridgeRuntime`
- Blender worker: `Plugins/BlenderBridge/Scripts/bridge_server.py`
- Sync helpers: `scripts/Sync-BlenderBridgeToProject.ps1` and `scripts/Watch-BlenderBridgeToProject.ps1`
- Implementation/status plan: `PLAN.md`
- Path reference notes: `PATHS.md`

## What The Current Slice Proves

- Windows named pipe control messages work end to end.
- A double-buffered shared-memory contract is in place.
- Unreal Editor can launch one shared Blender worker, route actor requests through it, and consume the resulting payloads.
- A minimal property-bag-backed input schema can drive per-actor cooks while actor input values remain isolated in Unreal.
- Multiple `.blend` sources can stay loaded in the same Blender session without reopening a new mainfile for every actor switch.
- A selected Geometry Nodes target can be loaded from a `.blend` file and streamed into Unreal as triangle-expanded geometry.
- World transforms and basic Blender-to-Unreal coordinate conversion are applied during extraction.
- A Geometry Nodes target can expose and accept `float`, `int`, `bool`, `vector3`, `color`, `string`, and `enum` inputs in real-mesh mode.
- Reflected GN inputs preserve Blender interface panel hierarchy in the Unreal details panel.
- Shared-session startup, schema requests, and cooks all run asynchronously enough to keep the editor usable during the current workflow.
- Oversized cooks can trigger an automatic worker restart with a larger shared-memory mapping and then retry.

## Unreal Usage

1. Copy or sync `Plugins/BlenderBridge` into an Unreal C++ project.
2. Enable the `BlenderBridge` plugin and ensure `ProceduralMeshComponent` is available.
3. Open `Edit > Project Settings > Plugins > Blender Bridge`.
4. Confirm `Blender Executable Path` is correct.
   Tip: if it is blank, the plugin will try to auto-detect the newest standard Blender install under `C:\Program Files\Blender Foundation\Blender *\blender.exe`.
5. Use `Tools > Blender Bridge > Create Session` to launch the global Blender worker.
6. Add a [UBlenderBridgeComponent](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeComponent.h) to an actor.
7. Create a [UBlenderBridgeSource](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeSource.h) asset and set `Blend File`.
   Tip: you can also drag a `.blend` file into the Content Browser and the plugin will auto-create a configured `BlenderBridgeSource` asset for it.
8. Assign that source asset to the component.
9. If the source exposes multiple GN targets, choose one in `Target`.
10. Click `Refresh Input Schema` if needed and wait for the schema to populate.
11. Edit reflected values in `Input Values`.
12. Click `Request Cook`.
13. Confirm the preview mesh updates in the Unreal viewport.

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

Supported reflected socket types today:

- `float`
- `int`
- `bool`
- `vector3`
- `color`
- `string`
- `enum`

Practical note:

- `vector3` is the most useful next-step test because it maps naturally to nodes like `Transform Geometry`.
- `color` and `string` values are transported and applied, but whether they create a visible change depends entirely on the GN graph you build.

## Shared Session Notes

- Session controls are global and live under `Tools > Blender Bridge`.
- Components no longer own Blender worker processes.
- Multiple actors can point at the same source and target while keeping separate `Input Values` in Unreal.
- Multiple sources can stay resident in one Blender worker process.
- The current behavior is editor-only. Packaged/runtime bridge execution is intentionally unsupported in this first shared-session pass.

## Intentional Gaps

- No in-place shared-memory remap without worker restart yet.
- No richer editor customization beyond the current `.blend` import factory and grouped input UI yet.
- No packaged-runtime bridge execution yet.
- No custom render proxy or RDG path yet.

## Syncing To A UE Project

If you are developing the plugin from this repo and testing it from a separate Unreal project, use the PowerShell helpers in [scripts/Sync-BlenderBridgeToProject.ps1](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\scripts\Sync-BlenderBridgeToProject.ps1) and [scripts/Watch-BlenderBridgeToProject.ps1](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\scripts\Watch-BlenderBridgeToProject.ps1).

Manual one-shot sync:

```powershell
powershell -ExecutionPolicy Bypass -File "C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\scripts\Sync-BlenderBridgeToProject.ps1"
```

Continuous watch-and-sync:

```powershell
powershell -ExecutionPolicy Bypass -File "C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\scripts\Watch-BlenderBridgeToProject.ps1"
```

These scripts mirror the source plugin into the Unreal project plugin folder while ignoring generated folders such as `Binaries`, `Intermediate`, and `__pycache__`.

## Recommended Reading

- Project status and next milestones: [PLAN.md](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\PLAN.md)
- Manual test guide: [TESTING.md](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\TESTING.md)
- Local path reference: [PATHS.md](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\PATHS.md)
