# Testing The Bridge Vertical Slice

This bridge is testable today as a manual smoke test. The current slice covers:

- a shared editor-global Blender session
- multiple loaded `.blend` sources in one Blender worker
- per-actor target selection, schema reflection, and recook for supported socket types

## What Is Testable Today

- Unreal Editor can host the plugin runtime and editor modules.
- Unreal Editor can launch one shared Blender worker process from the global session menu.
- The worker can keep multiple `.blend` files loaded at once without reopening a mainfile for every actor switch.
- The worker can discover Geometry Nodes targets from a loaded `.blend`, reflect their schema, and cook one selected target at a time.
- The worker can apply supported Geometry Nodes values and recook the selected preview mesh.
- Unreal can read the shared-memory slot and render the result with `UProceduralMeshComponent`.
- Unreal can import a `.blend` file into the Content Browser as a configured `BlenderBridgeSource` asset.
- Unreal can restart the shared session with a larger shared-memory budget when a cook exceeds the current slot size.

## What Is Not Yet Testable As A Finished System

- In-place shared-memory remap without worker restart.
- Packaged runtime bridge behavior.
- Automated end-to-end tests.

## Local Machine Paths Discovered Here

- Unreal Editor: `C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe`
- Blender 5.1: `C:\Program Files\Blender Foundation\Blender 5.1\blender.exe`
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
7. Open `Edit > Project Settings > Plugins > Blender Bridge` and confirm `Blender Executable Path`.
   Note: if blank, the plugin should auto-detect the newest standard Blender install.
8. Use `Tools > Blender Bridge > Create Session`.
9. Drag a `.blend` file into the Content Browser and confirm Unreal creates a `BlenderBridgeSource` asset for it.
10. Assign that source asset to the component.
11. If the source exposes multiple GN targets, pick one in `Target`.
12. Click `Refresh Input Schema` if the schema does not appear automatically.
13. Click `Request Cook`.
14. Confirm the selected Blender target appears as a triangulated preview mesh.

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
2. Create the global session from `Tools > Blender Bridge > Create Session`.
3. If needed, choose the correct `Target`.
4. Confirm the reflected schema contains the vector input.
5. Change `X`, `Y`, or `Z` in `Input Values`.
6. Click `Request Cook`.
7. Confirm the preview mesh moves.

## Additional GN Type Checks

These are useful follow-up checks after the vector test:

- `float`: drive a scale or translation amount and confirm geometry changes.
- `int`: drive a count-based or stepped GN parameter and confirm topology or placement changes.
- `bool`: switch a branch in the graph and confirm a visible mesh change.
- `color`: confirm the schema appears and cooks succeed; visible output depends on the graph.
- `string`: confirm the schema appears and cooks succeed; visible output depends on the graph.
- `enum`: create a menu input, confirm Unreal presents a constrained selection, change it, and confirm the graph responds.

## Expected Success Signals

- `Connection State` moves to `Connected` after the global session is created.
- `Last Status Message` shows messages like `Loading source into the global Blender session.`, `Ready. Received ... bridge inputs.`, and `Applied bridge mesh from slot 1`.
- Importing a `.blend` into the Content Browser creates a `BlenderBridgeSource` with `Blend File` already populated.
- In real-mesh mode, `Target` auto-selects when there is exactly one GN target and stays unresolved when there are multiple.
- `Input Schema` populates with supported float, int, bool, vector3, color, string, and enum inputs for the selected target.
- A visible Blender target mesh appears in the level after `Request Cook`.
- Changing a supported GN input and requesting another cook changes the preview mesh when the graph uses that input visibly.
- Inputs that are grouped into Blender interface panels appear grouped the same way in Unreal details.
- If the mesh exceeds the current slot size, the bridge logs the resize, restarts the global session automatically, restores loaded sources, and retries the cook.

## Likely Failure Points

- No Unreal host project: this repo currently contains a plugin, not a full `.uproject`.
- Blender executable path invalid and auto-detect failed.
- Blender Python missing NumPy: the worker expects NumPy inside Blender's Python environment.
- Source missing `Blend File`: the component will reject source load.
- Multiple Geometry Nodes targets: the component requires a manual `Target` selection.
- Renamed or removed GN targets: a previously saved `Target` can become unresolved after reload.
- C++ changes require a UE rebuild; Blender worker script changes require restarting the global session.

## If You Want A Lower-Friction Next Step

The most useful next improvement would be true multi-instance Blender-side isolation inside the shared session and broader support for non-mesh Geometry Nodes data types like collection-driven inputs.
