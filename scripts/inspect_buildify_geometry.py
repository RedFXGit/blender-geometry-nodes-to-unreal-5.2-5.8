import bpy


TARGET_OBJECT_NAME = "building_base"
TARGET_MODIFIER_NAME = "GeometryNodes"


def mesh_bounds(mesh):
    if mesh is None or len(mesh.vertices) == 0:
        return None

    xs = [vertex.co.x for vertex in mesh.vertices]
    ys = [vertex.co.y for vertex in mesh.vertices]
    zs = [vertex.co.z for vertex in mesh.vertices]
    return {
        "min": (min(xs), min(ys), min(zs)),
        "max": (max(xs), max(ys), max(zs)),
    }


def main():
    obj = bpy.data.objects.get(TARGET_OBJECT_NAME)
    if obj is None:
        print(f"Missing object: {TARGET_OBJECT_NAME}")
        return

    modifier = obj.modifiers.get(TARGET_MODIFIER_NAME)
    print("object_name", obj.name)
    print("object_type", obj.type)
    print("location", tuple(obj.location))
    print("rotation_euler", tuple(obj.rotation_euler))
    print("scale", tuple(obj.scale))
    print("matrix_world_translation", tuple(obj.matrix_world.to_translation()))

    if obj.data is not None and obj.type == "MESH":
        print("mesh_bounds", mesh_bounds(obj.data))

    if modifier is None:
        print(f"Missing modifier: {TARGET_MODIFIER_NAME}")
        return

    print("modifier_name", modifier.name)
    print("modifier_keys", list(modifier.keys()))
    print("interface_items")
    for item in modifier.node_group.interface.items_tree:
        if getattr(item, "item_type", "") != "SOCKET":
            continue
        print(
            "  socket",
            {
                "name": getattr(item, "name", ""),
                "identifier": getattr(item, "identifier", ""),
                "in_out": getattr(item, "in_out", ""),
                "socket_type": getattr(item, "socket_type", ""),
            },
        )

    print("group_input_outputs")
    for node in modifier.node_group.nodes:
        if getattr(node, "bl_idname", "") != "NodeGroupInput":
            continue
        for output in node.outputs:
            print(
                "  output",
                {
                    "name": getattr(output, "name", ""),
                    "identifier": getattr(output, "identifier", ""),
                    "bl_idname": getattr(output, "bl_idname", ""),
                    "type": getattr(output, "type", ""),
                    "link_count": len(output.links),
                },
            )

    bpy.context.view_layer.update()
    depsgraph = bpy.context.evaluated_depsgraph_get()
    evaluated_object = obj.evaluated_get(depsgraph)
    evaluated_mesh = evaluated_object.to_mesh(preserve_all_data_layers=True, depsgraph=depsgraph)
    if evaluated_mesh is not None:
        try:
            print("evaluated_mesh_bounds_local", mesh_bounds(evaluated_mesh))
            world_corners = [evaluated_object.matrix_world @ vertex.co for vertex in evaluated_mesh.vertices]
            if world_corners:
                xs = [corner.x for corner in world_corners]
                ys = [corner.y for corner in world_corners]
                zs = [corner.z for corner in world_corners]
                print("evaluated_mesh_bounds_world", {"min": (min(xs), min(ys), min(zs)), "max": (max(xs), max(ys), max(zs))})
        finally:
            evaluated_object.to_mesh_clear()

    # Simulate a centered owner-geometry override like a UE plane at actor-local origin.
    original_vertices = [vertex.co.copy() for vertex in obj.data.vertices]
    original_faces = [tuple(p.vertices) for p in obj.data.polygons]
    try:
        obj.data.clear_geometry()
        obj.data.from_pydata(
            [
                (-5.0, -5.0, 0.0),
                (5.0, -5.0, 0.0),
                (5.0, 5.0, 0.0),
                (-5.0, 5.0, 0.0),
            ],
            [],
            [(0, 1, 2), (0, 2, 3)],
        )
        obj.data.update()
        bpy.context.view_layer.update()
        depsgraph = bpy.context.evaluated_depsgraph_get()
        evaluated_object = obj.evaluated_get(depsgraph)
        evaluated_mesh = evaluated_object.to_mesh(preserve_all_data_layers=True, depsgraph=depsgraph)
        if evaluated_mesh is not None:
            try:
                print("simulated_override_mesh_bounds_local", mesh_bounds(evaluated_mesh))
                world_corners = [evaluated_object.matrix_world @ vertex.co for vertex in evaluated_mesh.vertices]
                if world_corners:
                    xs = [corner.x for corner in world_corners]
                    ys = [corner.y for corner in world_corners]
                    zs = [corner.z for corner in world_corners]
                    print(
                        "simulated_override_mesh_bounds_world",
                        {"min": (min(xs), min(ys), min(zs)), "max": (max(xs), max(ys), max(zs))},
                    )
            finally:
                evaluated_object.to_mesh_clear()
    finally:
        obj.data.clear_geometry()
        obj.data.from_pydata([tuple(vertex) for vertex in original_vertices], [], original_faces)
        obj.data.update()


if __name__ == "__main__":
    main()
