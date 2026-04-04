import argparse
import ast
import ctypes
import json
import mmap
import os
import sys

try:
    import numpy as np
except ImportError as exc:
    raise RuntimeError("bridge_server.py requires NumPy in the Blender Python environment") from exc

try:
    import bpy  # type: ignore
except ImportError:
    bpy = None


PIPE_ACCESS_DUPLEX = 0x00000003
PIPE_TYPE_BYTE = 0x00000000
PIPE_READMODE_BYTE = 0x00000000
PIPE_WAIT = 0x00000000
PIPE_UNLIMITED_INSTANCES = 255
INVALID_HANDLE_VALUE = ctypes.c_void_p(-1).value
SUPPORTED_OBJECT_TYPES = {"MESH", "CURVE", "SURFACE", "FONT", "META"}
SUPPORTED_GN_SOCKET_TYPE_PREFIXES = (
    ("NodeSocketFloat", "float"),
    ("NodeSocketInt", "integer"),
    ("NodeSocketBool", "boolean"),
    ("NodeSocketVector", "vector3"),
    ("NodeSocketColor", "color"),
    ("NodeSocketString", "string"),
)

ERROR_PIPE_CONNECTED = 535

kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
kernel32.CreateNamedPipeW.restype = ctypes.c_void_p
kernel32.ConnectNamedPipe.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
kernel32.ConnectNamedPipe.restype = ctypes.c_int
kernel32.WriteFile.argtypes = [
    ctypes.c_void_p,
    ctypes.c_void_p,
    ctypes.c_uint32,
    ctypes.POINTER(ctypes.c_uint32),
    ctypes.c_void_p,
]
kernel32.WriteFile.restype = ctypes.c_int
kernel32.ReadFile.argtypes = [
    ctypes.c_void_p,
    ctypes.c_void_p,
    ctypes.c_uint32,
    ctypes.POINTER(ctypes.c_uint32),
    ctypes.c_void_p,
]
kernel32.ReadFile.restype = ctypes.c_int
kernel32.FlushFileBuffers.argtypes = [ctypes.c_void_p]
kernel32.FlushFileBuffers.restype = ctypes.c_int
kernel32.DisconnectNamedPipe.argtypes = [ctypes.c_void_p]
kernel32.DisconnectNamedPipe.restype = ctypes.c_int
kernel32.CloseHandle.argtypes = [ctypes.c_void_p]
kernel32.CloseHandle.restype = ctypes.c_int


def get_supported_gn_socket_type(socket_type: str) -> str | None:
    for socket_prefix, bridge_type in SUPPORTED_GN_SOCKET_TYPE_PREFIXES:
        if socket_type == socket_prefix or socket_type.startswith(socket_prefix):
            return bridge_type

    return None


def coerce_numeric_sequence(value, expected_length: int, component_names: tuple[str, ...]) -> tuple[float, ...]:
    if isinstance(value, dict):
        coerced_components = []
        for component_name in component_names:
            if component_name in value:
                coerced_components.append(float(value[component_name]))
                continue

            uppercase_component_name = component_name.upper()
            if uppercase_component_name in value:
                coerced_components.append(float(value[uppercase_component_name]))
                continue

            raise RuntimeError(f"Missing component '{component_name}' in Geometry Nodes input payload")

        return tuple(coerced_components)

    if isinstance(value, (list, tuple)):
        if len(value) != expected_length:
            raise RuntimeError(
                f"Expected {expected_length} values for Geometry Nodes input, got {len(value)}"
            )
        return tuple(float(component) for component in value)

    raise RuntimeError(
        f"Expected a {expected_length}-component sequence for Geometry Nodes input, got {type(value).__name__}"
    )


def to_float_list(value, expected_length: int, fallback: float = 0.0) -> list[float]:
    try:
        components = list(value)
    except TypeError:
        return [fallback] * expected_length

    if len(components) < expected_length:
        components.extend([fallback] * (expected_length - len(components)))

    return [float(component) for component in components[:expected_length]]


class BridgeMeshSlotHeader(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("sequence", ctypes.c_uint64),
        ("slot_index", ctypes.c_uint32),
        ("status", ctypes.c_uint32),
        ("vertex_count", ctypes.c_uint32),
        ("index_count", ctypes.c_uint32),
        ("positions_offset", ctypes.c_uint32),
        ("positions_bytes", ctypes.c_uint32),
        ("normals_offset", ctypes.c_uint32),
        ("normals_bytes", ctypes.c_uint32),
        ("uv0_offset", ctypes.c_uint32),
        ("uv0_bytes", ctypes.c_uint32),
        ("indices_offset", ctypes.c_uint32),
        ("indices_bytes", ctypes.c_uint32),
        ("bounds_min", ctypes.c_float * 3),
        ("bounds_max", ctypes.c_float * 3),
        ("reserved", ctypes.c_uint32 * 8),
    ]


class BridgeSharedHeader(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("magic", ctypes.c_uint32),
        ("version", ctypes.c_uint32),
        ("header_bytes", ctypes.c_uint64),
        ("total_bytes", ctypes.c_uint64),
        ("slot_stride", ctypes.c_uint64),
        ("slot_count", ctypes.c_uint32),
        ("active_slot", ctypes.c_uint32),
        ("reserved", ctypes.c_uint64 * 4),
        ("slots", BridgeMeshSlotHeader * 2),
    ]


assert ctypes.sizeof(BridgeMeshSlotHeader) == 112
assert ctypes.sizeof(BridgeSharedHeader) == 296


class NamedPipeServer:
    def __init__(self, pipe_name: str) -> None:
        self.pipe_name = pipe_name
        self.handle = None
        self._read_buffer = ""

    @property
    def platform_path(self) -> str:
        return rf"\\.\pipe\{self.pipe_name}"

    def connect(self) -> None:
        self.handle = kernel32.CreateNamedPipeW(
            ctypes.c_wchar_p(self.platform_path),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            65536,
            65536,
            0,
            None,
        )
        if self.handle in (None, INVALID_HANDLE_VALUE):
            raise ctypes.WinError(ctypes.get_last_error())

        connected = kernel32.ConnectNamedPipe(self.handle, None)
        if not connected:
            error_code = ctypes.get_last_error()
            if error_code != ERROR_PIPE_CONNECTED:
                raise ctypes.WinError(error_code)

    def close(self) -> None:
        if self.handle not in (None, INVALID_HANDLE_VALUE):
            kernel32.FlushFileBuffers(self.handle)
            kernel32.DisconnectNamedPipe(self.handle)
            kernel32.CloseHandle(self.handle)
            self.handle = None

    def write_json(self, payload: dict) -> None:
        self.write_line(json.dumps(payload))

    def write_line(self, text: str) -> None:
        if self.handle is None:
            raise RuntimeError("Pipe is not connected")

        encoded = (text + "\n").encode("utf-8")
        buffer = ctypes.create_string_buffer(encoded)
        bytes_written = ctypes.c_uint32(0)
        success = kernel32.WriteFile(
            self.handle,
            buffer,
            len(encoded),
            ctypes.byref(bytes_written),
            None,
        )
        if not success:
            raise ctypes.WinError(ctypes.get_last_error())

    def read_json(self) -> dict:
        line = self.read_line()
        normalized_line = line.replace("\x00", "").strip()

        try:
            return json.loads(normalized_line)
        except json.JSONDecodeError as json_error:
            try:
                fallback_value = ast.literal_eval(normalized_line)
            except (SyntaxError, ValueError):
                raise RuntimeError(
                    f"Failed to parse control message. Raw line: {normalized_line!r}. JSON error: {json_error}"
                ) from json_error

            if not isinstance(fallback_value, dict):
                raise RuntimeError(
                    f"Control message parsed as a non-dict literal. Raw line: {normalized_line!r}"
                ) from json_error

            return fallback_value

    def read_line(self) -> str:
        while True:
            newline_index = self._read_buffer.find("\n")
            if newline_index >= 0:
                line = self._read_buffer[:newline_index]
                self._read_buffer = self._read_buffer[newline_index + 1 :]
                return line.strip()

            chunk = ctypes.create_string_buffer(4096)
            bytes_read = ctypes.c_uint32(0)
            success = kernel32.ReadFile(
                self.handle,
                chunk,
                len(chunk),
                ctypes.byref(bytes_read),
                None,
            )
            if not success or bytes_read.value == 0:
                raise RuntimeError("Pipe closed while waiting for data")

            self._read_buffer += chunk.raw[: bytes_read.value].decode("utf-8")


class BridgeWorker:
    def __init__(self, args: argparse.Namespace) -> None:
        self.args = args
        self.pipe = NamedPipeServer(args.pipe_name)
        self.shared_memory = mmap.mmap(-1, args.shared_memory_bytes, tagname=args.shared_memory_name)
        self.values = {"seed": 100.0}
        self.loaded_blend_file = args.blend_file or ""
        self.object_name = args.object_name or ""
        self.modifier_name = args.modifier_name or ""

    def send(self, message_type: str, **payload: object) -> None:
        payload["message_type"] = message_type
        self.pipe.write_json(payload)

    def load_asset(self, payload: dict) -> None:
        self.args.hello_world = bool(payload.get("hello_world", self.args.hello_world))
        blend_file = payload.get("blend_file") or self.loaded_blend_file or ""
        self.loaded_blend_file = str(blend_file)
        self.object_name = str(payload.get("object_name") or self.object_name)
        self.modifier_name = str(payload.get("modifier_name") or self.modifier_name)

        if self.args.hello_world or bpy is None:
            return

        if not blend_file:
            raise RuntimeError("Blend file path is required when hello world mode is disabled")

        if not os.path.exists(str(blend_file)):
            raise RuntimeError(f"Blend file not found: {blend_file}")

        bpy.ops.wm.open_mainfile(filepath=str(blend_file))

    def describe_inputs(self) -> None:
        if not self.args.hello_world:
            schema, target_object, modifier = self.build_geometry_nodes_schema()
            self.send(
                "DescribeInputs",
                schema=schema,
                object_name=getattr(target_object, "name", ""),
                modifier_name=getattr(modifier, "name", ""),
            )
            return

        self.send(
            "DescribeInputs",
            schema=[
                {
                    "id": "seed",
                    "label": "Seed",
                    "type": "float",
                    "default": 100.0,
                    "min": 10.0,
                    "max": 500.0,
                }
            ],
        )

    def set_inputs(self, payload: dict) -> None:
        values = payload.get("values", {})
        if isinstance(values, dict):
            self.values.update(values)
            if not self.args.hello_world:
                self.apply_geometry_nodes_inputs()

    def resolve_target_objects(self) -> list:
        if bpy is None:
            raise RuntimeError("bpy is not available in the bridge worker")

        candidate_name = self.object_name.strip()
        if candidate_name:
            target_object = bpy.data.objects.get(candidate_name)
            if target_object is None:
                raise RuntimeError(f"Could not find Blender object '{candidate_name}'")
            if target_object.type not in SUPPORTED_OBJECT_TYPES:
                raise RuntimeError(f"Blender object '{target_object.name}' has unsupported type '{target_object.type}'")
            return [target_object]

        scene_objects = list(bpy.context.scene.objects) if bpy.context.scene is not None else list(bpy.data.objects)
        target_objects = [obj for obj in scene_objects if obj.type in SUPPORTED_OBJECT_TYPES and not obj.hide_render]
        if not target_objects:
            raise RuntimeError("Could not find any mesh-compatible objects in the open blend file")

        return target_objects

    def describe_cooked_targets(self, object_names: list[str]) -> str:
        if not object_names:
            return "NoObjects"

        if len(object_names) == 1:
            return object_names[0]

        preview = ", ".join(object_names[:3])
        if len(object_names) > 3:
            preview = f"{preview}, +{len(object_names) - 3} more"
        return f"{len(object_names)} objects ({preview})"

    def resolve_geometry_nodes_modifier(self):
        target_objects = self.resolve_target_objects()
        requested_modifier_name = self.modifier_name.strip()

        candidates = []
        for target_object in target_objects:
            for modifier in target_object.modifiers:
                if modifier.type != "NODES" or modifier.node_group is None:
                    continue

                if requested_modifier_name and modifier.name != requested_modifier_name:
                    continue

                candidates.append((target_object, modifier))
                if self.object_name.strip() and not requested_modifier_name:
                    break

        if requested_modifier_name:
            if not candidates:
                raise RuntimeError(
                    f"Could not find Geometry Nodes modifier '{requested_modifier_name}'"
                    + (f" on object '{self.object_name}'" if self.object_name.strip() else "")
                )
            return candidates[0]

        if self.object_name.strip():
            return candidates[0] if candidates else (None, None)

        if len(candidates) == 1:
            return candidates[0]

        return (None, None)

    def build_geometry_nodes_schema(self):
        target_object, modifier = self.resolve_geometry_nodes_modifier()
        if modifier is None or modifier.node_group is None:
            return [], target_object, modifier

        schema = []
        for item in modifier.node_group.interface.items_tree:
            if getattr(item, "item_type", "") != "SOCKET":
                continue
            if getattr(item, "in_out", "") != "INPUT":
                continue

            input_type = get_supported_gn_socket_type(getattr(item, "socket_type", ""))
            if input_type is None:
                continue

            entry = {
                "id": item.identifier,
                "label": item.name,
                "type": input_type,
            }

            if input_type == "float":
                entry["default"] = float(modifier.get(item.identifier, getattr(item, "default_value", 0.0)))
                entry["min"] = float(getattr(item, "min_value", -100000.0))
                entry["max"] = float(getattr(item, "max_value", 100000.0))
            elif input_type == "integer":
                entry["default"] = int(modifier.get(item.identifier, getattr(item, "default_value", 0)))
                entry["min"] = int(getattr(item, "min_value", -2147483648))
                entry["max"] = int(getattr(item, "max_value", 2147483647))
            elif input_type == "boolean":
                entry["default_bool"] = bool(modifier.get(item.identifier, getattr(item, "default_value", False)))
            elif input_type == "vector3":
                entry["default_vector"] = to_float_list(
                    modifier.get(item.identifier, getattr(item, "default_value", (0.0, 0.0, 0.0))),
                    3,
                )
            elif input_type == "color":
                entry["default_color"] = to_float_list(
                    modifier.get(item.identifier, getattr(item, "default_value", (0.0, 0.0, 0.0, 1.0))),
                    4,
                )
            elif input_type == "string":
                entry["default_string"] = str(modifier.get(item.identifier, getattr(item, "default_value", "")))

            schema.append(entry)

        return schema, target_object, modifier

    def apply_geometry_nodes_inputs(self) -> None:
        target_object, modifier = self.resolve_geometry_nodes_modifier()
        if modifier is None or modifier.node_group is None:
            return

        did_apply_value = False
        for item in modifier.node_group.interface.items_tree:
            if getattr(item, "item_type", "") != "SOCKET":
                continue
            if getattr(item, "in_out", "") != "INPUT":
                continue

            input_type = get_supported_gn_socket_type(getattr(item, "socket_type", ""))
            if input_type is None:
                continue

            identifier = item.identifier
            if identifier not in self.values:
                continue

            use_attribute_key = f"{identifier}_use_attribute"
            attribute_name_key = f"{identifier}_attribute_name"
            if use_attribute_key in modifier.keys():
                modifier[use_attribute_key] = False
            if attribute_name_key in modifier.keys():
                modifier[attribute_name_key] = ""

            if input_type == "float":
                modifier[identifier] = float(self.values[identifier])
            elif input_type == "integer":
                modifier[identifier] = int(self.values[identifier])
            elif input_type == "boolean":
                modifier[identifier] = bool(self.values[identifier])
            elif input_type == "vector3":
                modifier[identifier] = coerce_numeric_sequence(self.values[identifier], 3, ("x", "y", "z"))
            elif input_type == "color":
                modifier[identifier] = coerce_numeric_sequence(self.values[identifier], 4, ("r", "g", "b", "a"))
            elif input_type == "string":
                modifier[identifier] = str(self.values[identifier])
            did_apply_value = True

        if did_apply_value and target_object is not None:
            target_object.update_tag()

    def extract_render_buffers_for_object(self, target_object, depsgraph):
        evaluated_object = target_object.evaluated_get(depsgraph)
        evaluated_mesh = evaluated_object.to_mesh(preserve_all_data_layers=True, depsgraph=depsgraph)

        if evaluated_mesh is None:
            raise RuntimeError(f"Failed to evaluate Blender object '{target_object.name}' into a mesh")

        try:
            evaluated_mesh.transform(evaluated_object.matrix_world)
            evaluated_mesh.calc_loop_triangles()
            if hasattr(evaluated_mesh, "calc_normals_split"):
                evaluated_mesh.calc_normals_split()

            triangle_count = len(evaluated_mesh.loop_triangles)
            if triangle_count == 0:
                return None

            vertex_coords = np.empty(len(evaluated_mesh.vertices) * 3, dtype=np.float32)
            evaluated_mesh.vertices.foreach_get("co", vertex_coords)
            vertex_coords = vertex_coords.reshape(-1, 3)

            triangle_vertex_indices = np.empty(triangle_count * 3, dtype=np.int32)
            evaluated_mesh.loop_triangles.foreach_get("vertices", triangle_vertex_indices)
            triangle_vertex_indices = triangle_vertex_indices.reshape(-1, 3)

            triangle_loop_indices = np.empty(triangle_count * 3, dtype=np.int32)
            evaluated_mesh.loop_triangles.foreach_get("loops", triangle_loop_indices)
            triangle_loop_indices = triangle_loop_indices.reshape(-1, 3)

            positions = vertex_coords[triangle_vertex_indices].astype(np.float32, copy=True)

            try:
                loop_normals = np.empty(len(evaluated_mesh.loops) * 3, dtype=np.float32)
                evaluated_mesh.loops.foreach_get("normal", loop_normals)
                loop_normals = loop_normals.reshape(-1, 3)
                normals = loop_normals[triangle_loop_indices].astype(np.float32, copy=True)
            except (AttributeError, RuntimeError):
                vertex_normals = np.empty(len(evaluated_mesh.vertices) * 3, dtype=np.float32)
                evaluated_mesh.vertices.foreach_get("normal", vertex_normals)
                vertex_normals = vertex_normals.reshape(-1, 3)
                normals = vertex_normals[triangle_vertex_indices].astype(np.float32, copy=True)

            if evaluated_mesh.uv_layers.active is not None and len(evaluated_mesh.uv_layers.active.data) > 0:
                loop_uv = np.empty(len(evaluated_mesh.uv_layers.active.data) * 2, dtype=np.float32)
                evaluated_mesh.uv_layers.active.data.foreach_get("uv", loop_uv)
                loop_uv = loop_uv.reshape(-1, 2)
                uv0 = loop_uv[triangle_loop_indices].astype(np.float32, copy=True)
            else:
                uv0 = np.zeros((triangle_count, 3, 2), dtype=np.float32)

            positions *= 100.0
            positions[:, :, 1] *= -1.0
            normals[:, :, 1] *= -1.0

            render_vertex_count = triangle_count * 3
            packed_positions = np.ascontiguousarray(positions.reshape(render_vertex_count, 3), dtype=np.float32)
            packed_normals = np.ascontiguousarray(normals.reshape(render_vertex_count, 3), dtype=np.float32)
            packed_uv0 = np.ascontiguousarray(uv0.reshape(render_vertex_count, 2), dtype=np.float32)
            return packed_positions, packed_normals, packed_uv0
        finally:
            evaluated_object.to_mesh_clear()

    def write_mesh_to_shared_memory(
        self,
        positions: np.ndarray,
        normals: np.ndarray,
        uv0: np.ndarray,
        indices: np.ndarray,
    ) -> int | None:
        positions = np.ascontiguousarray(positions, dtype=np.float32)
        normals = np.ascontiguousarray(normals, dtype=np.float32)
        uv0 = np.ascontiguousarray(uv0, dtype=np.float32)
        indices = np.ascontiguousarray(indices, dtype=np.uint32)

        if positions.ndim != 2 or positions.shape[1] != 3:
            raise RuntimeError("Positions buffer must be shaped as [vertex_count, 3]")
        if normals.shape != positions.shape:
            raise RuntimeError("Normals buffer must match positions")
        if uv0.ndim != 2 or uv0.shape[0] != positions.shape[0] or uv0.shape[1] != 2:
            raise RuntimeError("UV0 buffer must be shaped as [vertex_count, 2]")
        if indices.ndim != 1:
            raise RuntimeError("Index buffer must be a flat array")

        header = BridgeSharedHeader.from_buffer(self.shared_memory)
        slot_index = 1 if header.active_slot == 0 else 0
        slot = header.slots[slot_index]

        positions_offset = 0
        normals_offset = positions_offset + positions.nbytes
        uv0_offset = normals_offset + normals.nbytes
        indices_offset = uv0_offset + uv0.nbytes
        required_bytes = indices_offset + indices.nbytes

        if required_bytes > header.slot_stride:
            self.send("ResizeRequired", required_bytes=int(required_bytes))
            return None

        slot.status = 0

        slot_buffer = memoryview(self.shared_memory)[
            header.header_bytes + slot_index * header.slot_stride : header.header_bytes + (slot_index + 1) * header.slot_stride
        ]

        np.frombuffer(slot_buffer, dtype=np.float32, count=positions.size, offset=positions_offset)[:] = positions.reshape(-1)
        np.frombuffer(slot_buffer, dtype=np.float32, count=normals.size, offset=normals_offset)[:] = normals.reshape(-1)
        np.frombuffer(slot_buffer, dtype=np.float32, count=uv0.size, offset=uv0_offset)[:] = uv0.reshape(-1)
        np.frombuffer(slot_buffer, dtype=np.uint32, count=indices.size, offset=indices_offset)[:] = indices

        slot.sequence += 1
        slot.slot_index = slot_index
        slot.vertex_count = positions.shape[0]
        slot.index_count = indices.shape[0]
        slot.positions_offset = positions_offset
        slot.positions_bytes = positions.nbytes
        slot.normals_offset = normals_offset
        slot.normals_bytes = normals.nbytes
        slot.uv0_offset = uv0_offset
        slot.uv0_bytes = uv0.nbytes
        slot.indices_offset = indices_offset
        slot.indices_bytes = indices.nbytes

        bounds_min = positions.min(axis=0)
        bounds_max = positions.max(axis=0)
        slot.bounds_min[:] = [float(bounds_min[0]), float(bounds_min[1]), float(bounds_min[2])]
        slot.bounds_max[:] = [float(bounds_max[0]), float(bounds_max[1]), float(bounds_max[2])]
        slot.status = 1

        header.active_slot = slot_index
        return int(slot_index)

    def cook_hello_world(self) -> None:
        seed_value = float(self.values.get("seed", 100.0))
        size = max(seed_value, 10.0)

        positions = np.asarray(
            [
                [0.0, 0.0, 0.0],
                [size, 0.0, 0.0],
                [0.0, size, 0.0],
            ],
            dtype=np.float32,
        )
        normals = np.asarray(
            [
                [0.0, 0.0, 1.0],
                [0.0, 0.0, 1.0],
                [0.0, 0.0, 1.0],
            ],
            dtype=np.float32,
        )
        uv0 = np.asarray(
            [
                [0.0, 0.0],
                [1.0, 0.0],
                [0.0, 1.0],
            ],
            dtype=np.float32,
        )
        indices = np.asarray([0, 1, 2], dtype=np.uint32)

        slot_index = self.write_mesh_to_shared_memory(positions, normals, uv0, indices)
        if slot_index is None:
            return

        self.send(
            "CookComplete",
            slot=slot_index,
            sequence=int(BridgeSharedHeader.from_buffer(self.shared_memory).slots[slot_index].sequence),
            vertex_count=int(positions.shape[0]),
            index_count=int(indices.shape[0]),
            object_name=self.object_name or "HelloWorldTriangle",
        )

    def cook_real_mesh(self) -> None:
        if bpy is None:
            raise RuntimeError("Real mesh cooking requires Blender's bpy module")

        self.apply_geometry_nodes_inputs()
        bpy.context.view_layer.update()
        depsgraph = bpy.context.evaluated_depsgraph_get()
        target_objects = self.resolve_target_objects()
        packed_positions_parts = []
        packed_normals_parts = []
        packed_uv0_parts = []
        cooked_object_names = []

        for target_object in target_objects:
            extracted_buffers = self.extract_render_buffers_for_object(target_object, depsgraph)
            if extracted_buffers is None:
                continue

            packed_positions, packed_normals, packed_uv0 = extracted_buffers
            if packed_positions.size == 0:
                continue

            packed_positions_parts.append(packed_positions)
            packed_normals_parts.append(packed_normals)
            packed_uv0_parts.append(packed_uv0)
            cooked_object_names.append(target_object.name)

        if not packed_positions_parts:
            if self.object_name.strip():
                raise RuntimeError(f"Blender object '{self.object_name}' produced no triangles")
            raise RuntimeError("No mesh-compatible objects produced triangles in the open blend file")

        merged_positions = np.concatenate(packed_positions_parts, axis=0)
        merged_normals = np.concatenate(packed_normals_parts, axis=0)
        merged_uv0 = np.concatenate(packed_uv0_parts, axis=0)
        merged_indices = np.arange(merged_positions.shape[0], dtype=np.uint32)

        slot_index = self.write_mesh_to_shared_memory(merged_positions, merged_normals, merged_uv0, merged_indices)
        if slot_index is None:
            return

        cooked_target_label = self.describe_cooked_targets(cooked_object_names)
        self.send(
            "CookComplete",
            slot=slot_index,
            sequence=int(BridgeSharedHeader.from_buffer(self.shared_memory).slots[slot_index].sequence),
            vertex_count=int(merged_positions.shape[0]),
            index_count=int(merged_indices.shape[0]),
            object_name=cooked_target_label,
            object_count=len(cooked_object_names),
        )

    def cook(self) -> None:
        if self.args.hello_world:
            self.cook_hello_world()
            return

        self.cook_real_mesh()

    def run(self) -> int:
        self.pipe.connect()
        self.send("Hello", worker="Blender", mode="hello_world" if self.args.hello_world else "mesh_object")

        try:
            while True:
                message = self.pipe.read_json()
                message_type = message.get("message_type", "")

                if message_type == "Hello":
                    continue

                if message_type == "LoadAsset":
                    self.load_asset(message)
                    continue

                if message_type == "DescribeInputs":
                    self.describe_inputs()
                    continue

                if message_type == "SetInputs":
                    self.set_inputs(message)
                    continue

                if message_type == "Cook":
                    self.cook()
                    continue

                if message_type == "Shutdown":
                    return 0

                self.send("Error", message=f"Unsupported message_type: {message_type}")
        except Exception as exc:  # pylint: disable=broad-except
            self.send("Error", message=str(exc))
            return 1
        finally:
            self.pipe.close()
            self.shared_memory.close()


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pipe-name", required=True)
    parser.add_argument("--shared-memory-name", required=True)
    parser.add_argument("--shared-memory-bytes", required=True, type=int)
    parser.add_argument("--blend-file", default="")
    parser.add_argument("--object-name", default="")
    parser.add_argument("--modifier-name", default="")
    parser.add_argument("--hello-world", action="store_true")
    return parser.parse_args(argv)


def extract_bridge_argv(argv: list[str]) -> list[str]:
    if "--" in argv:
        separator_index = argv.index("--")
        return argv[separator_index + 1 :]
    return argv


def main(argv: list[str]) -> int:
    args = parse_args(extract_bridge_argv(argv))
    worker = BridgeWorker(args)
    return worker.run()


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
