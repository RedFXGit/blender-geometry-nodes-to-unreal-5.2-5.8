# Testing The Bridge Vertical Slice

This bridge is testable today as a manual smoke test. The current slice covers:

- a shared editor-global Blender session
- multiple loaded `.blend` sources in one Blender worker
- per-actor target selection, schema reflection, and recook for supported socket types
- curve-owner spline import and live recook from an embedded Unreal spline component
- live procedural preview in Unreal
- baked static-mesh or instanced-HISM output with generated materials, shared prototype meshes, and baked actor management

## What Is Testable Today

- Unreal Editor can host the plugin runtime and editor modules.
- Unreal Editor can launch one shared Blender worker process from the global session menu.
- The worker can keep multiple `.blend` files loaded at once without reopening a mainfile for every actor switch.
- The worker can discover Geometry Nodes targets from a loaded `.blend`, reflect their schema, and cook one selected target at a time.
- The worker can duplicate a curve-owner GN target per Unreal actor, import the authored Blender Bezier curve into Unreal, and recook from Unreal spline edits.
- The worker can apply supported Geometry Nodes values and recook the selected preview mesh.
- Unreal can read the shared-memory slot and render the result with `UProceduralMeshComponent`.
- Unreal can import a `.blend` file into the Content Browser as a configured `BlenderBridgeSource` asset.
- Unreal shows that asset in the Content Browser as `Blender Bridge Source`.
- Unreal can drag a `BlenderBridgeSource` into the level and spawn a configured `BlenderBridgeActor`.
- Unreal can bake the latest cooked result either to one static mesh asset or to shared prototype meshes plus a baked HISM actor.
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
5. Open `Edit > Project Settings > Plugins > Blender Bridge` and confirm `Automatically Start Session On Editor Launch`, `Blender Executable Path`, and `Bake Root Content Path`.
   Note: if the Blender path is blank, the plugin should auto-detect the newest standard install.
6. If auto-start is disabled, use `Tools > Blender Bridge > Create Session`.
7. Drag a `.blend` file into the Content Browser and confirm Unreal creates a `BlenderBridgeSource` asset for it.
8. Drag that `BlenderBridgeSource` asset into the level and confirm Unreal spawns a `BlenderBridgeActor` with `Source` already assigned.
9. If the source exposes multiple GN targets, pick one in `Target`.
10. Click `Refresh Input Schema` if the schema does not appear automatically.
11. Click `Request Cook`.
12. Confirm the selected Blender target appears as a triangulated procedural preview mesh.
13. Leave `Bake Output Mode` at `StaticMesh` and click `Bake Output`.
14. Confirm Unreal creates a baked static mesh asset, a baked actor, and generated placeholder materials under the bake root.

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
2. Ensure the global session is running, either from auto-start or `Tools > Blender Bridge > Create Session`.
3. If needed, choose the correct `Target`.
4. Confirm the reflected schema contains the vector input.
5. Change `X`, `Y`, or `Z` in `Input Values`.
6. Click `Request Cook`.
7. Confirm the preview mesh moves.

## Curve-Owner Spline Test

Use `C:\Users\Lassiter\Downloads\Blender_Geo\Curve.blend` as the anchor example for this workflow.

In Blender:

1. Confirm `Blenderesse_ElectricLineGenerator_V01` is a `CURVE` object with a `GeometryNodes` modifier.
2. Confirm the curve uses one Bezier spline and the GN graph consumes `Group Input -> Geometry`.
3. Confirm the `Collision` collection input is authored in Blender and saved with the file.

In Unreal:

1. Import or point a `BlenderBridgeSource` at `C:\Users\Lassiter\Downloads\Blender_Geo\Curve.blend`.
2. Drag the source into the level and select `Blenderesse_ElectricLineGenerator_V01|GeometryNodes` as the target if needed.
3. Confirm the actor contains a `Bridge Spline Component`.
4. Click `Refresh Input Schema` and confirm the target reports ready status for curve-owner spline editing.
5. Confirm the embedded Unreal spline matches the authored Blender curve on first load.
6. Move one or more spline points in the viewport and wait for the debounced recook.
7. Confirm the preview mesh updates and the Blender-authored `Collision` collection behavior still affects the result.
8. Change a reflected numeric input such as `Spacing` and confirm the cook still responds while using the edited spline.
9. Duplicate the actor, edit the duplicate's spline differently, and confirm both actors keep distinct results.
10. Set `Bake Output Mode` to `InstancedHierarchy` and bake one actor.
11. Confirm repeated spline-generated elements are baked as HISM instances instead of one merged mesh when the worker can recover instances.

## Bake Validation Test

After the procedural preview is working:

1. Set `Bake Output Mode` to `StaticMesh` and click `Bake Output`.
2. Confirm `Baked Static Mesh` and `Baked Actor` populate on the component.
3. If `Hide Preview After Bake` is enabled, confirm the live preview hides and the baked actor stays visible.
4. Click `Select Baked Output` and confirm Unreal selects the baked actor or syncs the baked asset in the Content Browser.
5. Reopen the baked static mesh asset and confirm its UV dropdown shows only `UV Channel 0` for single-UV test content.
6. Click `Delete Baked Output` and confirm the baked actor, baked mesh, and generated bake materials are removed.

## Instanced Bake Validation Test

After the procedural preview is working on a target with repeated instances:

1. Set `Bake Output Mode` to `InstancedHierarchy`.
2. Click `Bake Output`.
3. Confirm the component stores a `Baked Actor` and one or more `Baked Prototype Meshes`.
4. Confirm the baked actor contains one `UHierarchicalInstancedStaticMeshComponent` per recovered prototype.
5. Confirm repeated geometry appears through instance counts on those HISM components instead of one merged baked mesh asset.
6. Duplicate the source actor or add another actor that points at the same `.blend` source and target, then bake again.
7. Confirm reused prototypes point at the same shared prototype mesh assets under the bake root's source-scoped `_SharedPrototypes` folder.
8. Click `Select Baked Output` and confirm Unreal selects the baked actor or syncs the shared prototype mesh assets if the actor is unavailable.
9. Click `Delete Baked Output` and confirm only unreferenced shared prototype meshes are deleted.

## Additional GN Type Checks

These are useful follow-up checks after the vector test:

- `float`: drive a scale or translation amount and confirm geometry changes.
- `int`: drive a count-based or stepped GN parameter and confirm topology or placement changes.
- `bool`: switch a branch in the graph and confirm a visible mesh change.
- `color`: confirm the schema appears and cooks succeed; visible output depends on the graph.
- `string`: confirm the schema appears and cooks succeed; visible output depends on the graph.
- `enum`: create a menu input, confirm Unreal presents a constrained selection, change it, and confirm the graph responds.

## Material Slot Checks

- Confirm `Cooked Material Slots` populates after a successful cook.
- Confirm `Material Overrides` mirrors discovered slot ids.
- Assign an override material and confirm it applies to the preview mesh.
- Bake again in both `StaticMesh` and `InstancedHierarchy` mode and confirm overridden slots use the override while non-overridden slots receive generated placeholder materials.

## Expected Success Signals

- `Connection State` moves to `Connected` after the global session is created.
- `Last Status Message` shows progress like loading sources, reflecting schema, cooking, and baking output.
- Importing a `.blend` into the Content Browser creates a `BlenderBridgeSource` with `Blend File` already populated.
- The Content Browser labels that asset type as `Blender Bridge Source` instead of generic `Data Asset`.
- Dragging that source into the level creates a `BlenderBridgeActor` with `Source` already assigned on its bridge component.
- In real-mesh mode, `Target` auto-selects when there is exactly one GN target and stays unresolved when there are multiple.
- `Input Schema` populates with supported float, int, bool, vector3, color, string, and enum inputs for the selected target.
- Curve-owner targets import their authored Blender spline into the actor's embedded Unreal spline component on load.
- A visible Blender target mesh appears in the level after `Request Cook`.
- Changing a supported GN input and requesting another cook changes the preview mesh when the graph uses that input visibly.
- Editing the embedded Unreal spline on a curve-owner target triggers a debounced cook and updates only that actor's preview.
- Baking creates a baked static mesh asset and baked actor under the configured bake root.
- Instanced bake creates a baked actor with HISM components plus shared prototype mesh assets under the configured bake root.
- Duplicate actors from the same `.blend` source reuse identical shared prototype mesh assets instead of duplicating them.
- Single-UV bake content stays single-channel in the final baked static mesh asset.
- If the mesh exceeds the current slot size, the bridge logs the resize, restarts the global session automatically, restores loaded sources, and retries the cook.

## Likely Failure Points

- No Unreal host project: this repo contains a plugin, not a full `.uproject`.
- Blender executable path invalid and auto-detect failed.
- Blender Python missing NumPy: the worker expects NumPy inside Blender's Python environment.
- Source missing `Blend File`: the component will reject source load.
- Multiple Geometry Nodes targets: the component requires a manual `Target` selection.
- Renamed or removed GN targets: a previously saved `Target` can become unresolved after reload.
- C++ changes require a UE rebuild; Blender worker script changes require restarting the global session.
- Unreal Editor left open during plugin sync/build can prevent the test project from picking up the newest plugin DLL cleanly.
