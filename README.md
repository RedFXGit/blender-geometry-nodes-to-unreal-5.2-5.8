# Blender Bridge Vertical Slice

This repository contains a working Unreal-to-Blender editor bridge prototype.

Today it can:

- launch Blender headless from Unreal
- exchange control messages over a Windows named pipe
- stream cooked mesh data through double-buffered shared memory
- display the result in Unreal with `UProceduralMeshComponent`
- cook either a hello-world triangle or a real `.blend` file
- reflect and drive one Geometry Nodes modifier with supported input types

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
- Unreal can launch Blender headless, request a cook, and consume the resulting payload.
- A minimal property-bag-backed input schema can drive the cook.
- A real Blender object, or all mergeable scene objects, can be loaded from a `.blend` file and streamed into Unreal as triangle-expanded geometry.
- World transforms and basic Blender-to-Unreal coordinate conversion are applied during extraction.
- A single Geometry Nodes modifier can expose and accept `float`, `int`, `bool`, `vector3`, `color`, and `string` inputs in real-mesh mode.
- Startup, cook requests, and stop requests all run asynchronously enough to keep the editor usable during the current workflow.

## Unreal Usage

1. Copy or sync `Plugins/BlenderBridge` into an Unreal C++ project.
2. Enable the `BlenderBridge` plugin and ensure `ProceduralMeshComponent` is available.
3. Add a [UBlenderBridgeComponent](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeComponent.h) to an actor.
4. Point `Blender Executable Path` at Blender, for example `C:\Program Files\Blender Foundation\Blender 4.4\blender.exe`.
5. For hello world, leave `Source` empty or enable `Hello World Mode` on a [UBlenderBridgeSource](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeSource.h) asset.
6. For a real Blender test, create a `BlenderBridgeSource` asset and set:
   - `Hello World Mode` off
   - `Blend File`
   - `Object Name` if you want one object only
   - `Modifier Name` if you want to reflect a specific Geometry Nodes modifier
7. Leave `Object Name` blank if you want the worker to merge all mesh-compatible scene objects into one preview mesh.
8. Click `Start Bridge` and wait for `Bridge ready...`.
9. Edit reflected values in `Input Values` if a schema appears.
10. Click `Request Cook`.
11. Confirm the preview mesh updates in the Unreal viewport.

## Geometry Nodes Notes

The current GN path works best when:

- one object clearly owns the target `NODES` modifier
- `Modifier Name` is set when a file has multiple GN modifiers
- the graph produces a realized mesh-like output

Supported reflected socket types today:

- `float`
- `int`
- `bool`
- `vector3`
- `color`
- `string`

Practical note:

- `vector3` is the most useful next-step test because it maps naturally to nodes like `Transform Geometry`.
- `color` and `string` values are transported and applied, but whether they create a visible change depends entirely on the GN graph you build.

## Intentional Gaps

- No Geometry Nodes extraction from instanced scene output yet.
- No enum support yet.
- No shared-memory resize/remap flow yet.
- No editor customization module yet.
- No packaged-runtime support yet.
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
