# Blender Bridge Vertical Slice

This repository contains a working Unreal-to-Blender editor bridge prototype.

Today it can:

- launch Blender headless from Unreal
- exchange control messages over a Windows named pipe
- stream cooked mesh data through double-buffered shared memory
- grow the shared-memory budget automatically when a cook exceeds the current slot size
- display the result in Unreal with `UProceduralMeshComponent`
- cook a real `.blend` file
- import a `.blend` file into the Content Browser as a configured `BlenderBridgeSource` asset
- reflect and drive one Geometry Nodes modifier with supported input types, including enums and Blender interface panel groupings

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
- A real Blender object, including evaluated Geometry Nodes instance output, or all mergeable scene objects can be loaded from a `.blend` file and streamed into Unreal as triangle-expanded geometry.
- World transforms and basic Blender-to-Unreal coordinate conversion are applied during extraction.
- A single Geometry Nodes modifier can expose and accept `float`, `int`, `bool`, `vector3`, `color`, `string`, and `enum` inputs in real-mesh mode.
- Reflected GN inputs preserve Blender interface panel hierarchy in the Unreal details panel.
- Startup, cook requests, and stop requests all run asynchronously enough to keep the editor usable during the current workflow.
- Oversized cooks can trigger an automatic worker restart with a larger shared-memory mapping and then retry.

## Unreal Usage

1. Copy or sync `Plugins/BlenderBridge` into an Unreal C++ project.
2. Enable the `BlenderBridge` plugin and ensure `ProceduralMeshComponent` is available.
3. Add a [UBlenderBridgeComponent](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeComponent.h) to an actor.
4. Point `Blender Executable Path` at Blender, for example `C:\Program Files\Blender Foundation\Blender 4.4\blender.exe`.
5. Create a [UBlenderBridgeSource](C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeSource.h) asset and set `Blend File`.
   Tip: you can also drag a `.blend` file into the Content Browser and the plugin will auto-create a configured `BlenderBridgeSource` asset for it.
6. Assign that source asset to the component.
7. Click `Start Bridge` and wait for `Bridge ready...`.
8. Edit reflected values in `Input Values` if a schema appears.
9. Click `Request Cook`.
10. Confirm the preview mesh updates in the Unreal viewport.

## Geometry Nodes Notes

The current GN path works best when:

- one object clearly owns the target `NODES` modifier
- the saved `.blend` has an obvious active object or only one likely GN candidate when multiple modifiers exist
- the graph produces a realized mesh-like output

Auto-resolution behavior today:

- if a Geometry Nodes modifier is selected, the worker cooks that object's evaluated GeometrySet, including nested instance references
- if no Geometry Nodes modifier is selected, the worker cooks all visible mesh-compatible scene objects into one preview mesh
- if exactly one Geometry Nodes modifier candidate exists, it is selected automatically
- if multiple candidates exist, the worker prefers the saved active object when possible and otherwise falls back to the first candidate while logging what it chose

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

## Intentional Gaps

- No in-place shared-memory remap without worker restart yet.
- No richer editor customization beyond the current `.blend` import factory yet.
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
