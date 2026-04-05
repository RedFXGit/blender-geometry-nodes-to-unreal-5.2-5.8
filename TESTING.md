# Testing The Bridge Vertical Slice

This bridge is testable today as a manual smoke test. The current slice covers:

- a real `.blend` object path
- merged multi-object scene preview
- Geometry Nodes schema reflection and recook for supported socket types

## What Is Testable Today

- Unreal can host the plugin runtime module.
- Unreal can launch Blender headless as a worker process.
- The worker can open a real `.blend`, evaluate one target object or all mesh-compatible scene objects, triangulate them, and stream that mesh into Unreal.
- The worker can reflect float, int, bool, vector3, color, string, and enum inputs from one Geometry Nodes modifier in real-mesh mode.
- The worker can apply supported Geometry Nodes values and recook the preview mesh.
- Unreal can read the shared-memory slot and render the result with `UProceduralMeshComponent`.
- Unreal can import a `.blend` file into the Content Browser as a configured `BlenderBridgeSource` asset.
- Unreal can restart the worker with a larger shared-memory budget when a cook exceeds the current slot size.

## What Is Not Yet Testable As A Finished System

- In-place shared-memory remap without worker restart.
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
8. Drag a `.blend` file into the Content Browser and confirm Unreal creates a `BlenderBridgeSource` asset for it.
9. Assign that source asset to the component.
10. Click `Start Bridge` on the component.
11. Wait for `Bridge ready...`.
12. Click `Request Cook`.
13. Confirm the visible Blender mesh content appears as a triangulated preview mesh.

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
2. Start the bridge and wait for `Bridge ready...`.
3. Confirm the reflected schema contains the vector input.
4. Change `X`, `Y`, or `Z` in `Input Values`.
5. Click `Request Cook`.
6. Confirm the preview mesh moves.

## Additional GN Type Checks

These are useful follow-up checks after the vector test:

- `float`: drive a scale or translation amount and confirm geometry changes.
- `int`: drive a count-based or stepped GN parameter and confirm topology or placement changes.
- `bool`: switch a branch in the graph and confirm a visible mesh change.
- `color`: confirm the schema appears and cooks succeed; visible output depends on the graph.
- `string`: confirm the schema appears and cooks succeed; visible output depends on the graph.
- `enum`: create a menu input, confirm Unreal presents a constrained selection, change it, and confirm the graph responds.

## Expected Success Signals

- `Connection State` moves to `Connected`.
- `Last Status Message` shows messages like `Bridge ready...` and `Applied bridge mesh from slot 1`.
- Importing a `.blend` into the Content Browser creates a `BlenderBridgeSource` with `Blend File` already populated.
- In real-mesh mode with one resolvable Geometry Nodes modifier, `Input Schema` populates with supported float, int, bool, vector3, color, string, and enum inputs.
- A visible Blender object or merged scene mesh appears in the level after `Request Cook`.
- Changing a supported GN input and requesting another cook changes the preview mesh when the graph uses that input visibly.
- Inputs that are grouped into Blender interface panels appear grouped the same way in Unreal details.
- If the mesh exceeds the current slot size, the bridge logs the resize, reconnects automatically, and retries the cook.

## Likely Failure Points

- No Unreal host project: this repo currently contains a plugin, not a full `.uproject`.
- Blender path not set: `blender.exe` was not found on `PATH` during local inspection.
- Blender Python missing NumPy: the worker expects NumPy inside Blender's Python environment.
- Source missing `Blend File`: the component will reject startup.
- Multiple Geometry Nodes candidates: the worker will auto-select a candidate and log what it chose, which may not match your intent in an ambiguous file.
- C++ changes require a UE rebuild; Blender worker script changes only require restarting the bridge session.

## If You Want A Lower-Friction Next Step

The most useful next improvement would be less disruptive shared-memory remap without a worker restart and broader support for non-mesh Geometry Nodes data types like collection-driven inputs.
