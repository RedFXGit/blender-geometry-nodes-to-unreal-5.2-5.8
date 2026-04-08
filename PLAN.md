# Blender Bridge Implementation Plan

## Summary

This project is no longer just an architecture sketch. It now ships an editor-first working slice that covers source import, shared Blender session management, Geometry Nodes schema reflection, live procedural preview, and baked static-mesh output.

The bridge currently does all of the following:

- Unreal launches a shared headless Blender worker.
- Unreal and Blender communicate over a Windows named pipe.
- Mesh payloads move through double-buffered named shared memory.
- Unreal reads the cooked mesh and displays it with `UProceduralMeshComponent`.
- Unreal can grow the shared-memory budget automatically and restart the worker when a cook exceeds the current slot size.
- A `BlenderBridgeSource` asset can point at a real `.blend` file.
- Unreal can import a `.blend` into a configured `BlenderBridgeSource` asset.
- Dragging a source into the level spawns a configured `BlenderBridgeActor`.
- The worker can cook visible mesh-compatible scene objects from the opened `.blend`.
- One selected Geometry Nodes target can expose and accept `float`, `int`, `bool`, `vector3`, `color`, `string`, and `enum` inputs.
- Unreal can bake the latest cooked result to a static mesh asset with generated placeholder materials and a spawned baked actor.

This is still a min-copy bridge, not a true zero-copy renderer. That is expected and acceptable for the current phase.

## Current Status

### Implemented

- Windows-only named pipe control plane.
- Double-buffered shared-memory mesh contract.
- Unreal-owned Blender worker launch and shutdown.
- Async startup, message sending, and stop flow to avoid editor hangs.
- Real `.blend` loading and evaluated mesh extraction.
- Triangulated render-vertex packing with UV and normal support.
- Basic Blender-to-Unreal coordinate conversion and world-transform application.
- Scene-driven target discovery from the source `.blend`.
- `BlenderBridgeSource` asset workflow.
- `.blend` Content Browser import that auto-creates `BlenderBridgeSource`.
- Actor factory that spawns a configured `BlenderBridgeActor` from a source asset.
- Automatic shared-memory growth by restarting the worker with a larger buffer when needed.
- GN input reflection and round-trip updates for:
  - `float`
  - `int`
  - `bool`
  - `vector3`
  - `color`
  - `string`
  - `enum`
- Grouped details-panel presentation for Blender interface panels.
- Material-slot metadata round-tripped from Blender cooks.
- Procedural preview material overrides by slot id.
- Editor bake flow that produces:
  - baked static mesh assets
  - generated placeholder material instances
  - spawned baked static mesh actors
  - baked-output selection and deletion actions
- UV-channel preservation workaround for UE 5.7 baked static meshes so one authored UV channel remains one final render texcoord.
- Sync scripts for mirroring the plugin into a separate Unreal project.

### Working Baseline

- UE 5.7
- Blender 4.4 and newer standard Windows installs
- Editor viewport workflow
- Live procedural preview plus editor bake workflow

### Not Yet Implemented

- In-place shared-memory remap without worker restart.
- Packaged runtime support.
- Custom render proxy / RDG upload path.
- Automated end-to-end tests.

## Current Architecture

### Unreal Side

- Plugin root: `Plugins/BlenderBridge`
- Runtime module: `BlenderBridgeRuntime`
- Editor module: `BlenderBridgeEditor`
- Main user-facing types:
  - `UBlenderBridgeComponent`
  - `ABlenderBridgeActor`
  - `UBlenderBridgeSource`
  - `FBridgeInputSchema`
  - `FBridgeDecodedMesh`
  - `FBridgeMaterialSlotDescriptor`

Responsibilities:

- Create shared memory.
- Launch Blender with session arguments.
- Connect to the named pipe.
- Receive schema and cook completion messages.
- Store live values in `FInstancedPropertyBag`.
- Read cooked mesh data and build a preview mesh.
- Bake cooked output into editor assets and actors.

### Blender Side

- Worker script: `Plugins/BlenderBridge/Scripts/bridge_server.py`

Responsibilities:

- Open the named pipe server and shared memory mapping.
- Load the requested `.blend`.
- Resolve the selected Geometry Nodes target.
- Apply incoming modifier values.
- Evaluate objects, triangulate them, and pack render vertices.
- Emit material-slot and section metadata alongside the cooked mesh.
- Write the inactive shared-memory slot and publish `CookComplete`.

## Verified Workflow

The current workflow that has already been exercised looks like this:

1. Create or open a UE 5.7 C++ project.
2. Copy or sync `Plugins/BlenderBridge` into the project's `Plugins` folder.
3. Rebuild the project/plugin.
4. Open `Edit > Project Settings > Plugins > Blender Bridge` and confirm session settings.
5. Create a `BlenderBridgeSource` asset or import a `.blend`.
6. Drag that source into the level to spawn a `BlenderBridgeActor`.
7. Choose a `Target` if the source exposes multiple GN targets.
8. Click `Refresh Input Schema`.
9. Edit reflected values in `Input Values`.
10. Click `Request Cook`.
11. Confirm the procedural preview updates.
12. Click `Bake To Static Mesh` if you want a persistent editor asset.
13. Confirm baked mesh, generated materials, and baked actor appear under the configured bake root.

## Recommended Next Milestones

### Next

- Reduce any remaining bake-path logging to only long-term useful diagnostics.
- Add clearer failure/status messages around bake output creation and overwrite behavior.
- Add better modifier-resolution diagnostics when multiple GN modifiers exist.

### After That

- Implement shared-memory resize/remap flow without worker restart.
- Add richer editor UX around source selection, session status, and baked-output management.
- Support more mesh sections and richer material handling.
- Decide whether a custom render path is worth the complexity after profiling.

### Later

- Collision generation beyond current baked static mesh defaults.
- Packaged runtime support.
- Automated tests.
- Custom render proxy / RDG path.

## Test Focus

The highest-value tests for the next phase are:

- repeated cook and bake stability while editing values rapidly
- GN schema reflection for mixed socket types, including enums
- multi-target resolution correctness
- material-slot and section correctness across preview and baked output
- large mesh behavior once resize/remap exists

## Assumptions

- Windows only.
- Local machine IPC only.
- Editor-first workflow.
- Min-copy is the current goal; true zero-copy rendering is deferred.
