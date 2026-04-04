# Testing The Bridge Vertical Slice

This bridge is testable today as a manual smoke test. The current slice covers:

- the original hello-world triangle path
- a real `.blend` object path
- merged multi-object scene preview
- Geometry Nodes schema reflection and recook for supported socket types

## What Is Testable Today

- Unreal can host the plugin runtime module.
- Unreal can launch Blender headless as a worker process.
- The Blender worker can expose one reflected input called `seed` in hello-world mode.
- The worker can write a simple triangle into shared memory.
- The worker can open a real `.blend`, evaluate one target object or all mesh-compatible scene objects, triangulate them, and stream that mesh into Unreal.
- The worker can reflect float, int, bool, vector3, color, and string inputs from one Geometry Nodes modifier in real-mesh mode.
- The worker can apply supported Geometry Nodes values and recook the preview mesh.
- Unreal can read the shared-memory slot and render the result with `UProceduralMeshComponent`.

## What Is Not Yet Testable As A Finished System

- Geometry Nodes input reflection beyond float, int, bool, vector3, color, and string sockets.
- Enum-driven GN UX.
- Shared-memory resize/remap recovery.
- Editor customization polish.
- Packaged runtime behavior.
- Automated end-to-end tests.

## Local Machine Paths Discovered Here

- Unreal Editor: `C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe`
- Blender 4.4: `C:\Program Files\Blender Foundation\Blender 4.4\blender.exe`
- Blender 3.6: `C:\Program Files\Blender Foundation\Blender 3.6\blender.exe`
- Plugin root: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge`

## Fastest Manual Smoke Test

1. Create or open a UE 5.7 C++ project.
2. Copy `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge` into that project's `Plugins` folder.
3. Open the project in Unreal and enable the `BlenderBridge` plugin if Unreal prompts for it.
4. Restart the editor so the plugin compiles and loads.
5. Create an empty actor in a level.
6. Add `BlenderBridgeComponent` to that actor.
7. Set `Blender Executable Path` on the component to `C:\Program Files\Blender Foundation\Blender 4.4\blender.exe`.
8. For hello world, leave `Source` empty or create a `BlenderBridgeSource` asset with `Hello World Mode` enabled.
9. For a real mesh test, create a `BlenderBridgeSource` asset with `Hello World Mode` disabled, set `Blend File`, and optionally set `Object Name`.
   Leave `Object Name` blank to merge all mesh-compatible scene objects.
   Set `Modifier Name` if you want to reflect inputs from a specific Geometry Nodes modifier.
10. Click `Start Bridge` on the component.
11. Wait for `Bridge ready...`.
12. Click `Request Cook`.
13. In hello-world mode, a triangle should appear.
14. In real-mesh mode, the chosen Blender object or merged scene should appear as a triangulated preview mesh.
15. In hello-world mode, change `Input Values.seed` and trigger another cook. The triangle size should change.

## Recommended Geometry Nodes Test

The easiest visible GN test is a vector translation input.

In Blender:

1. Add a `Geometry Nodes` modifier to a mesh object.
2. Build a simple graph:
   - `Group Input`
   - `Transform Geometry`
   - `Group Output`
3. Expose a `Vector` group input.
4. Plug that vector into `Transform Geometry.Translation`.
5. Save the `.blend`.

In Unreal:

1. Point your `BlenderBridgeSource` at that `.blend`.
2. Set `Object Name` to the owning object.
3. Set `Modifier Name` to the GN modifier name if needed.
4. Start the bridge and wait for `Bridge ready...`.
5. Confirm the reflected schema contains the vector input.
6. Change `X`, `Y`, or `Z` in `Input Values`.
7. Click `Request Cook`.
8. Confirm the preview mesh moves.

## Additional GN Type Checks

These are useful follow-up checks after the vector test:

- `float`: drive a scale or translation amount and confirm geometry changes.
- `int`: drive a count-based or stepped GN parameter and confirm topology or placement changes.
- `bool`: switch a branch in the graph and confirm a visible mesh change.
- `color`: confirm the schema appears and cooks succeed; visible output depends on the graph.
- `string`: confirm the schema appears and cooks succeed; visible output depends on the graph.

## Expected Success Signals

- `Connection State` moves to `Connected`.
- `Last Status Message` shows messages like `Bridge ready...` and `Applied bridge mesh from slot 1`.
- In hello-world mode, `Input Schema` populates with one entry named `seed`.
- In real-mesh mode with one resolvable Geometry Nodes modifier, `Input Schema` populates with supported float, int, bool, vector3, color, and string inputs.
- A visible triangle, a specific Blender object, or a merged scene mesh appears in the level after `Request Cook`.
- Changing a supported GN input and requesting another cook changes the preview mesh when the graph uses that input visibly.

## Likely Failure Points

- No Unreal host project: this repo currently contains a plugin, not a full `.uproject`.
- Blender path not set: `blender.exe` was not found on `PATH` during local inspection.
- Blender Python missing NumPy: the worker expects NumPy inside Blender's Python environment.
- Real-mesh source missing `Blend File`: the component will reject startup when hello world mode is disabled and no blend path is set.
- Real-mesh source object or modifier name is wrong: the Blender worker will error if it cannot find the requested target.
- C++ changes require a UE rebuild; Blender worker script changes only require restarting the bridge session.

## If You Want A Lower-Friction Next Step

The most useful next improvement would be enum support, resize/remap handling, and more robust Geometry Nodes scene/output handling.
