# Blender Bridge Implementation Plan

## Summary

This project is now past the pure architecture stage and into an implemented editor-first vertical slice.

The bridge currently does all of the following:

- Unreal launches a headless Blender worker.
- Unreal and Blender communicate over a Windows named pipe.
- Mesh payloads move through double-buffered named shared memory.
- Unreal reads the cooked mesh and displays it with `UProceduralMeshComponent`.
- A `BlenderBridgeSource` asset can point at a real `.blend` file.
- The worker can cook one named object or merge all mesh-compatible scene objects.
- One resolvable Geometry Nodes modifier can expose and accept `float`, `int`, `bool`, `vector3`, `color`, and `string` inputs.

This is still a min-copy bridge, not a true zero-copy renderer. That is expected and acceptable for the current phase.

## Current Status

### Implemented

- Windows-only named pipe control plane.
- Double-buffered shared-memory mesh contract.
- Unreal-owned Blender worker launch and shutdown.
- Async startup, message sending, and stop flow to avoid editor hangs.
- Hello-world triangle mode.
- Real `.blend` loading and evaluated mesh extraction.
- Triangulated render-vertex packing with UV and normal support.
- Basic Blender-to-Unreal coordinate conversion and world-transform application.
- Multi-object merge when `Object Name` is blank.
- `BlenderBridgeSource` asset workflow.
- GN input reflection and round-trip updates for:
  - `float`
  - `int`
  - `bool`
  - `vector3`
  - `color`
  - `string`
- Sync scripts for mirroring the plugin into a separate Unreal project.

### Working Baseline

- UE 5.7
- Blender 4.4
- Editor viewport workflow
- One preview mesh section through `UProceduralMeshComponent`

### Not Yet Implemented

- Shared-memory resize/remap recovery.
- Geometry Nodes enum support.
- Material-slot streaming.
- Collision generation.
- Geometry Nodes instance-aware scene output beyond realized merged mesh behavior.
- Custom editor module and richer UI customization.
- Custom render proxy / RDG upload path.
- Packaged runtime support.
- Automated end-to-end tests.

## Current Architecture

### Unreal Side

- Plugin root: `Plugins/BlenderBridge`
- Runtime module: `BlenderBridgeRuntime`
- Main user-facing types:
  - `UBlenderBridgeComponent`
  - `UBlenderBridgeSource`
  - `FBridgeInputSchema`
  - `FBridgeSharedHeader`
  - `FBridgeMeshSlotHeader`

Responsibilities:

- Create shared memory.
- Launch Blender with session arguments.
- Connect to the named pipe.
- Receive schema and cook completion messages.
- Store live values in `FInstancedPropertyBag`.
- Read cooked mesh data and build a preview mesh.

### Blender Side

- Worker script: `Plugins/BlenderBridge/Scripts/bridge_server.py`

Responsibilities:

- Open the named pipe server and shared memory mapping.
- Load the requested `.blend`.
- Resolve the target object or mergeable object set.
- Resolve one Geometry Nodes modifier for schema reflection when possible.
- Apply incoming modifier values.
- Evaluate objects, triangulate them, and pack render vertices.
- Write the inactive shared-memory slot and publish `CookComplete`.

## Verified Workflow

The current workflow that has already been exercised looks like this:

1. Create or open a UE 5.7 C++ project.
2. Copy or sync `Plugins/BlenderBridge` into the project's `Plugins` folder.
3. Rebuild the project/plugin.
4. Add `UBlenderBridgeComponent` to an actor.
5. Point `Blender Executable Path` to Blender 4.4.
6. Assign a `BlenderBridgeSource` asset.
7. Click `Start Bridge`.
8. Wait for `Bridge ready...`.
9. Edit reflected inputs in `Input Values`.
10. Click `Request Cook`.
11. Confirm the preview mesh updates in the viewport.

This flow now works for both the hello-world path and the real-mesh Geometry Nodes path.

## Recommended Next Milestones

### Next

- Add enum support for GN sockets where Unreal can present safe editable values.
- Add better modifier-resolution diagnostics when multiple GN modifiers exist.
- Add optional scene/output modes:
  - one named object
  - merged scene mesh
  - realized GN result

### After That

- Implement shared-memory resize/remap flow.
- Add better editor UX around source selection and session status.
- Support more mesh sections and material slots.
- Decide whether a custom render path is worth the complexity after profiling.

### Later

- Collision.
- Packaged runtime.
- Automated tests.
- Custom render proxy / RDG path.

## Test Focus

The highest-value tests for the next phase are:

- GN schema reflection for mixed socket types.
- Repeated cook stability while editing values rapidly.
- Multi-object merge correctness.
- Large mesh behavior once resize/remap exists.
- Modifier-resolution correctness when `Modifier Name` is provided versus blank.

## Assumptions

- Windows only.
- Local machine IPC only.
- Editor-first workflow.
- One preview mesh section for now.
- Min-copy is the current goal; true zero-copy rendering is deferred.
