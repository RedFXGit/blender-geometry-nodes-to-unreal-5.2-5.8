import argparse
import ast
import ctypes
import hashlib
import json
import math
import mmap
import os
import sys

try:
    import numpy as np
except ImportError as exc:
    raise RuntimeError("bridge_server.py requires NumPy in the Blender Python environment") from exc

try:
    import bpy  # type: ignore
    from mathutils import Vector  # type: ignore
except ImportError:
    bpy = None
    Vector = None


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
    ("NodeSocketMenu", "enum"),
)
MAX_GEOMETRYSET_RECURSION_DEPTH = 16
BRIDGE_ROOT_COLLECTION_NAME = "BlenderBridgeSources"

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


def get_numeric_sequence_length(value) -> int | None:
    if value is None or isinstance(value, (str, bytes)):
        return None

    try:
        return len(value)
    except TypeError:
        return None


def coerce_numeric_sequence(
    value,
    expected_length: int,
    component_names: tuple[str, ...],
    fallback_components=None,
) -> list[float]:
    fallback_values = to_float_list(fallback_components or (), expected_length, 0.0)

    if isinstance(value, dict):
        coerced_components = []
        for index in range(expected_length):
            if index < len(component_names):
                component_name = component_names[index]
                if component_name in value:
                    coerced_components.append(float(value[component_name]))
                    continue

                uppercase_component_name = component_name.upper()
                if uppercase_component_name in value:
                    coerced_components.append(float(value[uppercase_component_name]))
                    continue

            coerced_components.append(fallback_values[index])

        return coerced_components

    if isinstance(value, (list, tuple)):
        coerced_components = [float(component) for component in value[:expected_length]]
        if len(coerced_components) < expected_length:
            coerced_components.extend(fallback_values[len(coerced_components) : expected_length])
        return coerced_components

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


def normalize_enum_items(raw_items) -> list[dict[str, object]]:
    options: list[dict[str, object]] = []

    for index, raw_item in enumerate(raw_items or []):
        option_id = ""
        option_label = ""
        option_value = index

        if isinstance(raw_item, dict):
            option_id = str(raw_item.get("identifier") or raw_item.get("id") or "")
            option_label = str(raw_item.get("name") or raw_item.get("label") or "")
            option_value = int(raw_item.get("value", index))
        elif isinstance(raw_item, (list, tuple)):
            if len(raw_item) > 0 and raw_item[0] is not None:
                option_id = str(raw_item[0])
            if len(raw_item) > 1 and raw_item[1] is not None:
                option_label = str(raw_item[1])
            if len(raw_item) > 4 and raw_item[4] is not None:
                option_value = int(raw_item[4])
        elif raw_item is not None:
            option_id = str(raw_item)

        if not option_id:
            option_id = option_label or f"Option_{index}"
        if not option_label:
            option_label = option_id

        options.append({"id": option_id, "label": option_label, "value": option_value})

    return options


def get_interface_item_category_path(item) -> str:
    category_parts: list[str] = []
    current_parent = getattr(item, "parent", None)

    while current_parent is not None:
        parent_name = str(getattr(current_parent, "name", "") or "").strip()
        if parent_name:
            category_parts.append(parent_name)
        current_parent = getattr(current_parent, "parent", None)

    category_parts.reverse()
    return "|".join(category_parts)


def convert_curve_vector_to_unreal(value) -> list[float]:
    return [
        float(value[0]) * 100.0,
        float(value[1]) * -100.0,
        float(value[2]) * 100.0,
    ]


def convert_curve_vector_from_unreal(value) -> object:
    if Vector is None:
        raise RuntimeError("mathutils.Vector is unavailable")

    components = to_float_list(value, 3, 0.0)
    return Vector(
        (
            components[0] / 100.0,
            components[1] / -100.0,
            components[2] / 100.0,
        )
    )


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


class BridgeLoadedSource:
    def __init__(
        self,
        source_handle: str,
        blend_file: str,
        collection_name: str,
        object_names: list[str],
        targets: list[dict[str, object]],
    ) -> None:
        self.source_handle = source_handle
        self.blend_file = blend_file
        self.collection_name = collection_name
        self.object_names = list(object_names)
        self.targets = list(targets)
        self.targets_by_id = {str(target["id"]): target for target in targets}


class BridgeComponentState:
    def __init__(
        self,
        source_handle: str,
        component_handle: str,
        target_id: str,
        object_name: str,
    ) -> None:
        self.source_handle = source_handle
        self.component_handle = component_handle
        self.target_id = target_id
        self.object_name = object_name


class BridgeWorker:
    def __init__(self, args: argparse.Namespace) -> None:
        self.args = args
        self.pipe = NamedPipeServer(args.pipe_name)
        self.shared_memory = mmap.mmap(-1, args.shared_memory_bytes, tagname=args.shared_memory_name)
        self.loaded_sources: dict[str, BridgeLoadedSource] = {}
        self.component_states: dict[tuple[str, str], BridgeComponentState] = {}

    def send(self, message_type: str, **payload: object) -> None:
        payload["message_type"] = message_type
        self.pipe.write_json(payload)

    def send_for_request(
        self,
        message_type: str,
        request_id: str = "",
        source_handle: str = "",
        target_id: str = "",
        **payload: object,
    ) -> None:
        if request_id:
            payload["request_id"] = request_id
        if source_handle:
            payload["source_handle"] = source_handle
        if target_id:
            payload["target_id"] = target_id
        self.send(message_type, **payload)

    def get_root_collection(self):
        if bpy is None or bpy.context.scene is None:
            raise RuntimeError("A Blender scene is required for the bridge worker")

        root_collection = bpy.data.collections.get(BRIDGE_ROOT_COLLECTION_NAME)
        if root_collection is None:
            root_collection = bpy.data.collections.new(BRIDGE_ROOT_COLLECTION_NAME)

        if bpy.context.scene.collection.children.get(root_collection.name) is None:
            bpy.context.scene.collection.children.link(root_collection)

        return root_collection

    def make_source_collection_name(self, source_handle: str) -> str:
        digest = hashlib.sha1(source_handle.encode("utf-8")).hexdigest()[:12]
        return f"BridgeSource_{digest}"

    def make_component_state_key(self, source_handle: str, component_handle: str) -> tuple[str, str]:
        return (source_handle, component_handle)

    def make_component_object_name(self, source_handle: str, component_handle: str, target_name: str) -> str:
        digest = hashlib.sha1(f"{source_handle}|{component_handle}|{target_name}".encode("utf-8")).hexdigest()[:12]
        return f"BridgeComponent_{digest}_{target_name}"

    def release_component_state(self, component_handle: str, source_handle: str = "") -> None:
        if bpy is None or not component_handle:
            return

        keys_to_release = [
            state_key
            for state_key, state in self.component_states.items()
            if state.component_handle == component_handle and (not source_handle or state.source_handle == source_handle)
        ]
        if not keys_to_release and source_handle:
            keys_to_release = [
                state_key
                for state_key, state in self.component_states.items()
                if state.component_handle == component_handle
            ]

        for state_key in keys_to_release:
            state = self.component_states.pop(state_key, None)
            if state is None:
                continue

            bridge_object = bpy.data.objects.get(state.object_name)
            if bridge_object is not None:
                bpy.data.objects.remove(bridge_object, do_unlink=True)

    def unload_source(self, source_handle: str) -> None:
        source_state = self.loaded_sources.pop(source_handle, None)
        if source_state is None or bpy is None:
            return

        component_handles = [
            state.component_handle
            for state in self.component_states.values()
            if state.source_handle == source_handle
        ]
        for component_handle in component_handles:
            self.release_component_state(component_handle, source_handle)

        for object_name in reversed(source_state.object_names):
            bridge_object = bpy.data.objects.get(object_name)
            if bridge_object is not None:
                bpy.data.objects.remove(bridge_object, do_unlink=True)

        source_collection = bpy.data.collections.get(source_state.collection_name)
        if source_collection is not None:
            bpy.data.collections.remove(source_collection, do_unlink=True)

    def collect_geometry_nodes_candidates(self, target_objects: list) -> list[tuple[object, object]]:
        candidates = []
        for target_object in target_objects:
            if target_object is None or getattr(target_object, "type", "") not in SUPPORTED_OBJECT_TYPES:
                continue

            for modifier in target_object.modifiers:
                if modifier.type == "NODES" and modifier.node_group is not None:
                    candidates.append((target_object, modifier))
        return candidates

    def load_source_objects(self, blend_file: str, source_handle: str) -> BridgeLoadedSource:
        if bpy is None:
            raise RuntimeError("Real mesh cooking requires Blender's bpy module")
        if not blend_file:
            raise RuntimeError("Blend file path is required")
        if not os.path.exists(blend_file):
            raise RuntimeError(f"Blend file not found: {blend_file}")

        collection_name = self.make_source_collection_name(source_handle)
        self.unload_source(source_handle)

        with bpy.data.libraries.load(blend_file, link=False) as (data_from, data_to):
            data_to.objects = list(data_from.objects)

        loaded_objects = [bridge_object for bridge_object in data_to.objects if bridge_object is not None]
        root_collection = self.get_root_collection()
        source_collection = bpy.data.collections.new(collection_name)
        root_collection.children.link(source_collection)

        for bridge_object in loaded_objects:
            if source_collection.objects.get(bridge_object.name) is None:
                source_collection.objects.link(bridge_object)

        targets = self.build_target_descriptors(loaded_objects)
        source_state = BridgeLoadedSource(
            source_handle=source_handle,
            blend_file=blend_file,
            collection_name=collection_name,
            object_names=[bridge_object.name for bridge_object in loaded_objects],
            targets=targets,
        )
        self.loaded_sources[source_handle] = source_state
        return source_state

    def build_target_descriptors(self, target_objects: list) -> list[dict[str, object]]:
        descriptors: list[dict[str, object]] = []
        for target_object, modifier in self.collect_geometry_nodes_candidates(target_objects):
            target_id = f"{target_object.name}|{modifier.name}"
            descriptors.append(
                {
                    "id": target_id,
                    "label": f"{target_object.name} / {modifier.name}",
                    "object_name": target_object.name,
                    "modifier_name": modifier.name,
                    "object_type": str(getattr(target_object, "type", "") or ""),
                }
            )

        descriptors.sort(key=lambda entry: (str(entry["object_name"]), str(entry["modifier_name"])))
        return descriptors

    def ensure_source_loaded(self, payload: dict) -> None:
        request_id = str(payload.get("request_id") or "")
        source_handle = str(payload.get("source_handle") or "").strip()
        blend_file = str(payload.get("blend_file") or "").strip()
        force_reload = bool(payload.get("force_reload", False))

        if not source_handle:
            raise RuntimeError("source_handle is required")

        source_state = self.loaded_sources.get(source_handle)
        if source_state is not None and not force_reload:
            if blend_file and blend_file != source_state.blend_file:
                force_reload = True
            else:
                self.send_for_request(
                    "SourceLoaded",
                    request_id=request_id,
                    source_handle=source_handle,
                    blend_file=source_state.blend_file,
                    targets=source_state.targets,
                )
                return

        if not blend_file:
            if source_state is not None:
                blend_file = source_state.blend_file
            else:
                raise RuntimeError("blend_file is required")

        source_state = self.load_source_objects(blend_file, source_handle)
        self.send_for_request(
            "SourceLoaded",
            request_id=request_id,
            source_handle=source_handle,
            blend_file=source_state.blend_file,
            targets=source_state.targets,
        )

    def resolve_source_state(self, source_handle: str) -> BridgeLoadedSource:
        if not source_handle:
            raise RuntimeError("source_handle is required")

        source_state = self.loaded_sources.get(source_handle)
        if source_state is None:
            raise RuntimeError(f"Source is not loaded: {source_handle}")

        return source_state

    def resolve_target(self, source_handle: str, target_id: str):
        source_state = self.resolve_source_state(source_handle)
        if not target_id:
            raise RuntimeError("target_id is required")

        target_descriptor = source_state.targets_by_id.get(target_id)
        if target_descriptor is None:
            raise RuntimeError(f"Target '{target_id}' was not found in loaded source '{source_handle}'")

        target_object = bpy.data.objects.get(str(target_descriptor["object_name"]))
        if target_object is None:
            raise RuntimeError(f"Object '{target_descriptor['object_name']}' is no longer available")

        modifier_name = str(target_descriptor["modifier_name"])
        modifier = target_object.modifiers.get(modifier_name)
        if modifier is None or modifier.type != "NODES" or modifier.node_group is None:
            raise RuntimeError(
                f"Geometry Nodes modifier '{modifier_name}' on '{target_object.name}' is no longer available"
            )

        return source_state, target_descriptor, target_object, modifier

    def does_modifier_use_owner_geometry(self, modifier) -> bool:
        if modifier is None or modifier.node_group is None:
            return False

        for node in modifier.node_group.nodes:
            if getattr(node, "bl_idname", "") != "NodeGroupInput":
                continue

            for output in node.outputs:
                if getattr(output, "name", "") == "Geometry" and len(output.links) > 0:
                    return True

        return False

    def export_curve_owner_snapshot(self, target_object) -> dict[str, object]:
        curve_data = getattr(target_object, "data", None)
        if curve_data is None or len(curve_data.splines) != 1:
            return {"curve_type": "None", "closed": False, "points": []}

        spline = curve_data.splines[0]
        if getattr(spline, "type", "") != "BEZIER":
            return {"curve_type": "Unsupported", "closed": bool(getattr(spline, "use_cyclic_u", False)), "points": []}

        points: list[dict[str, object]] = []
        for bezier_point in spline.bezier_points:
            position = convert_curve_vector_to_unreal(bezier_point.co)
            arrive_tangent = convert_curve_vector_to_unreal((bezier_point.co - bezier_point.handle_left) * 3.0)
            leave_tangent = convert_curve_vector_to_unreal((bezier_point.handle_right - bezier_point.co) * 3.0)
            radius = float(getattr(bezier_point, "radius", 1.0) or 1.0)
            points.append(
                {
                    "position": position,
                    "arrive_tangent": arrive_tangent,
                    "leave_tangent": leave_tangent,
                    "roll_degrees": float(math.degrees(float(getattr(bezier_point, "tilt", 0.0) or 0.0))),
                    "scale": [radius, radius, radius],
                    "point_type": "Curve",
                }
            )

        return {
            "curve_type": "Bezier",
            "closed": bool(getattr(spline, "use_cyclic_u", False)),
            "points": points,
        }

    def build_target_capabilities(self, target_object, modifier) -> dict[str, object]:
        capabilities: dict[str, object] = {
            "target_kind": "standard",
            "supports_curve_owner": False,
            "curve_owner_message": "",
            "owner_curve": {
                "curve_type": "None",
                "closed": False,
                "points": [],
            },
        }

        if target_object is None or getattr(target_object, "type", "") != "CURVE":
            return capabilities

        capabilities["target_kind"] = "curve_owner"

        if not self.does_modifier_use_owner_geometry(modifier):
            capabilities["curve_owner_message"] = "Curve-owner targets must consume the modifier Geometry input."
            return capabilities

        curve_data = getattr(target_object, "data", None)
        if curve_data is None:
            capabilities["curve_owner_message"] = "Curve-owner target is missing curve data."
            return capabilities

        if len(curve_data.splines) != 1:
            capabilities["curve_owner_message"] = "Curve-owner v1 requires exactly one spline on the target curve object."
            return capabilities

        spline = curve_data.splines[0]
        if getattr(spline, "type", "") != "BEZIER":
            capabilities["curve_owner_message"] = (
                f"Curve-owner v1 only supports Bezier splines; found {getattr(spline, 'type', 'Unknown')}."
            )
            return capabilities

        capabilities["supports_curve_owner"] = True
        capabilities["curve_owner_message"] = "Curve-owner target is compatible."
        capabilities["owner_curve"] = self.export_curve_owner_snapshot(target_object)
        return capabilities

    def resolve_component_target(self, source_state: BridgeLoadedSource, target_descriptor: dict[str, object], target_object, component_handle: str):
        if not component_handle:
            raise RuntimeError("component_handle is required for curve-owner targets")

        state_key = self.make_component_state_key(source_state.source_handle, component_handle)
        existing_state = self.component_states.get(state_key)
        if existing_state is not None and existing_state.target_id == str(target_descriptor["id"]):
            existing_object = bpy.data.objects.get(existing_state.object_name)
            if existing_object is not None:
                existing_modifier = existing_object.modifiers.get(str(target_descriptor["modifier_name"]))
                if existing_modifier is not None and existing_modifier.type == "NODES" and existing_modifier.node_group is not None:
                    return existing_object, existing_modifier

            self.release_component_state(component_handle, source_state.source_handle)

        duplicate_object = target_object.copy()
        if getattr(target_object, "data", None) is not None:
            duplicate_object.data = target_object.data.copy()
        duplicate_object.name = self.make_component_object_name(
            source_state.source_handle,
            component_handle,
            str(target_descriptor["object_name"]),
        )

        source_collection = bpy.data.collections.get(source_state.collection_name)
        if source_collection is None:
            raise RuntimeError(f"Loaded source collection '{source_state.collection_name}' is no longer available")
        if source_collection.objects.get(duplicate_object.name) is None:
            source_collection.objects.link(duplicate_object)

        duplicate_modifier = duplicate_object.modifiers.get(str(target_descriptor["modifier_name"]))
        if duplicate_modifier is None or duplicate_modifier.type != "NODES" or duplicate_modifier.node_group is None:
            raise RuntimeError("Failed to duplicate the curve-owner target modifier for component-local cooking")

        self.component_states[state_key] = BridgeComponentState(
            source_handle=source_state.source_handle,
            component_handle=component_handle,
            target_id=str(target_descriptor["id"]),
            object_name=duplicate_object.name,
        )
        return duplicate_object, duplicate_modifier

    def apply_curve_owner_payload(self, target_object, curve_payload: dict) -> None:
        if bpy is None:
            raise RuntimeError("Curve-owner payload application requires Blender's bpy module")

        curve_data = getattr(target_object, "data", None)
        if curve_data is None:
            raise RuntimeError("Curve-owner target does not have curve data")

        points = curve_payload.get("points", [])
        if not isinstance(points, list) or not points:
            raise RuntimeError("Curve-owner payload must include at least one spline point")

        while len(curve_data.splines) > 0:
            curve_data.splines.remove(curve_data.splines[0])

        spline = curve_data.splines.new(type="BEZIER")
        if len(points) > 1:
            spline.bezier_points.add(len(points) - 1)

        spline.use_cyclic_u = bool(curve_payload.get("closed", False))
        curve_data.dimensions = "3D"

        for point_index, point_payload in enumerate(points):
            bezier_point = spline.bezier_points[point_index]
            point_position = convert_curve_vector_from_unreal(point_payload.get("position", (0.0, 0.0, 0.0)))
            arrive_tangent = convert_curve_vector_from_unreal(point_payload.get("arrive_tangent", (0.0, 0.0, 0.0)))
            leave_tangent = convert_curve_vector_from_unreal(point_payload.get("leave_tangent", (0.0, 0.0, 0.0)))
            point_type = str(point_payload.get("point_type", "Curve") or "Curve").strip().lower()

            bezier_point.co = point_position
            bezier_point.handle_left = point_position - (arrive_tangent / 3.0)
            bezier_point.handle_right = point_position + (leave_tangent / 3.0)

            handle_type = "FREE"
            if point_type in {"linear", "constant"}:
                handle_type = "VECTOR"
            bezier_point.handle_left_type = handle_type
            bezier_point.handle_right_type = handle_type

            bezier_point.tilt = math.radians(float(point_payload.get("roll_degrees", 0.0) or 0.0))

            scale_values = to_float_list(point_payload.get("scale", (1.0, 1.0, 1.0)), 3, 1.0)
            bezier_point.radius = float(sum(scale_values) / len(scale_values))

        target_object.update_tag()

    def build_geometry_nodes_schema_for_target(self, modifier) -> list[dict[str, object]]:
        if modifier is None or modifier.node_group is None:
            return []

        schema: list[dict[str, object]] = []
        for item in modifier.node_group.interface.items_tree:
            if getattr(item, "item_type", "") != "SOCKET":
                continue
            if getattr(item, "in_out", "") != "INPUT":
                continue

            input_type = get_supported_gn_socket_type(getattr(item, "socket_type", ""))
            if input_type is None:
                continue

            entry: dict[str, object] = {
                "id": item.identifier,
                "label": item.name,
                "type": input_type,
            }

            category_path = get_interface_item_category_path(item)
            if category_path:
                entry["category"] = category_path

            description = str(getattr(item, "description", "") or "").strip()
            if description:
                entry["description"] = description

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
            elif input_type == "enum":
                enum_options = self.get_modifier_enum_options(modifier, item.identifier)
                if not enum_options:
                    continue

                valid_values = {int(option["value"]) for option in enum_options}
                current_value = int(modifier.get(item.identifier, enum_options[0]["value"]))
                if current_value not in valid_values:
                    current_value = int(enum_options[0]["value"])

                entry["default"] = current_value
                entry["enum_options"] = enum_options

            schema.append(entry)

        return schema

    def describe_target(self, payload: dict) -> None:
        request_id = str(payload.get("request_id") or "")
        source_handle = str(payload.get("source_handle") or "").strip()
        target_id = str(payload.get("target_id") or "").strip()
        _source_state, _target_descriptor, target_object, modifier = self.resolve_target(source_handle, target_id)
        capabilities = self.build_target_capabilities(target_object, modifier)

        self.send_for_request(
            "TargetDescription",
            request_id=request_id,
            source_handle=source_handle,
            target_id=target_id,
            schema=self.build_geometry_nodes_schema_for_target(modifier),
            object_name=target_object.name,
            modifier_name=modifier.name,
            **capabilities,
        )

    def get_modifier_enum_options(self, modifier, identifier: str) -> list[dict[str, object]]:
        try:
            ui_metadata = modifier.id_properties_ui(identifier).as_dict()
        except (AttributeError, KeyError, TypeError):
            return []

        return normalize_enum_items(ui_metadata.get("items"))

    def coerce_enum_value(self, modifier, identifier: str, raw_value) -> int:
        enum_options = self.get_modifier_enum_options(modifier, identifier)
        if not enum_options:
            raise RuntimeError(f"Geometry Nodes enum input '{identifier}' does not expose any options")

        valid_values = {int(option["value"]) for option in enum_options}

        if isinstance(raw_value, str):
            for option in enum_options:
                if raw_value == option["id"] or raw_value == option["label"]:
                    return int(option["value"])
            raise RuntimeError(f"Unknown enum option '{raw_value}' for Geometry Nodes input '{identifier}'")

        coerced_value = int(raw_value)
        if coerced_value not in valid_values:
            raise RuntimeError(f"Invalid enum value '{coerced_value}' for Geometry Nodes input '{identifier}'")

        return coerced_value

    def apply_geometry_nodes_inputs(self, target_object, modifier, values: dict[str, object]) -> None:
        if modifier is None or modifier.node_group is None or not isinstance(values, dict):
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
            if identifier not in values:
                continue

            use_attribute_key = f"{identifier}_use_attribute"
            attribute_name_key = f"{identifier}_attribute_name"
            if use_attribute_key in modifier.keys():
                modifier[use_attribute_key] = False
            if attribute_name_key in modifier.keys():
                modifier[attribute_name_key] = ""

            if input_type == "float":
                modifier[identifier] = float(values[identifier])
            elif input_type == "integer":
                modifier[identifier] = int(values[identifier])
            elif input_type == "boolean":
                modifier[identifier] = bool(values[identifier])
            elif input_type == "vector3":
                existing_value = modifier.get(identifier, None)
                target_length = get_numeric_sequence_length(existing_value) or 3
                modifier[identifier] = coerce_numeric_sequence(
                    values[identifier],
                    target_length,
                    ("x", "y", "z"),
                    existing_value,
                )
            elif input_type == "color":
                existing_value = modifier.get(identifier, None)
                target_length = get_numeric_sequence_length(existing_value) or 4
                modifier[identifier] = coerce_numeric_sequence(
                    values[identifier],
                    target_length,
                    ("r", "g", "b", "a"),
                    existing_value,
                )
            elif input_type == "string":
                modifier[identifier] = str(values[identifier])
            elif input_type == "enum":
                modifier[identifier] = self.coerce_enum_value(modifier, identifier, values[identifier])

            did_apply_value = True

        if did_apply_value and target_object is not None:
            target_object.update_tag()

    def get_cache_key(self, value):
        try:
            return (type(value).__name__, int(value.as_pointer()))
        except (AttributeError, TypeError, ValueError):
            return None

    def make_material_slot_id(self, slot_name: str, material_name: str, slot_index: int) -> str:
        slot_name = str(slot_name or "").strip()
        material_name = str(material_name or "").strip()
        if slot_name and material_name and slot_name != material_name:
            return f"{slot_name}|{material_name}"
        if slot_name:
            return slot_name
        if material_name:
            return material_name
        return f"MaterialSlot_{slot_index}"

    def build_material_slot_descriptors_for_mesh(self, mesh) -> list[dict[str, object]]:
        material_slots: list[dict[str, object]] = []
        mesh_materials = list(getattr(mesh, "materials", []) or [])
        for slot_index, material in enumerate(mesh_materials):
            material_name = ""
            if material is not None:
                material_name = str(getattr(material, "name", "") or "").strip()

            slot_name = material_name or f"MaterialSlot_{slot_index}"
            material_slots.append(
                {
                    "slot_id": self.make_material_slot_id(slot_name, material_name, slot_index),
                    "slot_name": slot_name,
                    "material_name": material_name or slot_name,
                }
            )

        if not material_slots:
            material_slots.append(
                {
                    "slot_id": "Default",
                    "slot_name": "Default",
                    "material_name": "Default",
                }
            )

        return material_slots

    def extract_local_render_buffers_for_mesh(self, mesh):
        mesh.calc_loop_triangles()
        if hasattr(mesh, "calc_normals_split"):
            mesh.calc_normals_split()

        triangle_count = len(mesh.loop_triangles)
        if triangle_count == 0:
            return None

        vertex_coords = np.empty(len(mesh.vertices) * 3, dtype=np.float32)
        mesh.vertices.foreach_get("co", vertex_coords)
        vertex_coords = vertex_coords.reshape(-1, 3)

        triangle_vertex_indices = np.empty(triangle_count * 3, dtype=np.int32)
        mesh.loop_triangles.foreach_get("vertices", triangle_vertex_indices)
        triangle_vertex_indices = triangle_vertex_indices.reshape(-1, 3)

        triangle_loop_indices = np.empty(triangle_count * 3, dtype=np.int32)
        mesh.loop_triangles.foreach_get("loops", triangle_loop_indices)
        triangle_loop_indices = triangle_loop_indices.reshape(-1, 3)

        material_slots = self.build_material_slot_descriptors_for_mesh(mesh)
        triangle_material_indices = np.empty(triangle_count, dtype=np.int32)
        mesh.loop_triangles.foreach_get("material_index", triangle_material_indices)
        if len(material_slots) == 1:
            triangle_material_indices.fill(0)
        else:
            triangle_material_indices = np.clip(triangle_material_indices, 0, len(material_slots) - 1)

        positions = vertex_coords[triangle_vertex_indices].astype(np.float32, copy=True)

        try:
            loop_normals = np.empty(len(mesh.loops) * 3, dtype=np.float32)
            mesh.loops.foreach_get("normal", loop_normals)
            loop_normals = loop_normals.reshape(-1, 3)
            normals = loop_normals[triangle_loop_indices].astype(np.float32, copy=True)
        except (AttributeError, RuntimeError):
            vertex_normals = np.empty(len(mesh.vertices) * 3, dtype=np.float32)
            mesh.vertices.foreach_get("normal", vertex_normals)
            vertex_normals = vertex_normals.reshape(-1, 3)
            normals = vertex_normals[triangle_vertex_indices].astype(np.float32, copy=True)

        if mesh.uv_layers.active is not None and len(mesh.uv_layers.active.data) > 0:
            loop_uv = np.empty(len(mesh.uv_layers.active.data) * 2, dtype=np.float32)
            mesh.uv_layers.active.data.foreach_get("uv", loop_uv)
            loop_uv = loop_uv.reshape(-1, 2)
            uv0 = loop_uv[triangle_loop_indices].astype(np.float32, copy=True)
        else:
            uv0 = np.zeros((triangle_count, 3, 2), dtype=np.float32)

        render_vertex_count = triangle_count * 3
        packed_positions = np.ascontiguousarray(positions.reshape(render_vertex_count, 3), dtype=np.float32)
        packed_normals = np.ascontiguousarray(normals.reshape(render_vertex_count, 3), dtype=np.float32)
        packed_uv0 = np.ascontiguousarray(uv0.reshape(render_vertex_count, 2), dtype=np.float32)
        return (
            packed_positions,
            packed_normals,
            packed_uv0,
            np.ascontiguousarray(triangle_material_indices, dtype=np.int32),
            material_slots,
        )

    def transform_render_buffers(self, positions, normals, uv0, transform_matrix, convert_to_unreal: bool):
        transformed_positions = np.array(positions, dtype=np.float32, copy=True)
        transformed_normals = np.array(normals, dtype=np.float32, copy=True)
        transformed_uv0 = np.array(uv0, dtype=np.float32, copy=True)

        if transform_matrix is not None:
            linear_matrix = np.array(transform_matrix.to_3x3(), dtype=np.float32)
            translation = np.array(transform_matrix.to_translation(), dtype=np.float32)
            transformed_positions = transformed_positions @ linear_matrix.T
            transformed_positions += translation

            normal_matrix = np.array(transform_matrix.to_3x3().inverted_safe().transposed(), dtype=np.float32)
            transformed_normals = transformed_normals @ normal_matrix.T
            normal_lengths = np.linalg.norm(transformed_normals, axis=1, keepdims=True)
            normal_lengths = np.maximum(normal_lengths, 1e-8)
            transformed_normals = transformed_normals / normal_lengths

        if convert_to_unreal:
            transformed_positions *= 100.0
            transformed_positions[:, 1] *= -1.0
            transformed_normals[:, 1] *= -1.0

        return (
            np.ascontiguousarray(transformed_positions, dtype=np.float32),
            np.ascontiguousarray(transformed_normals, dtype=np.float32),
            np.ascontiguousarray(transformed_uv0, dtype=np.float32),
        )

    def transform_render_part(self, render_part, transform_matrix, convert_to_unreal: bool):
        transformed_positions, transformed_normals, transformed_uv0 = self.transform_render_buffers(
            render_part[0],
            render_part[1],
            render_part[2],
            transform_matrix,
            convert_to_unreal,
        )
        return (
            transformed_positions,
            transformed_normals,
            transformed_uv0,
            np.array(render_part[3], dtype=np.int32, copy=True),
            [dict(slot) for slot in render_part[4]],
        )

    def merge_render_parts(self, render_parts):
        ordered_slot_keys: list[tuple[str, str, str]] = []
        slot_metadata_by_key: dict[tuple[str, str, str], dict[str, object]] = {}
        slot_chunks: dict[tuple[str, str, str], list[tuple[np.ndarray, np.ndarray, np.ndarray]]] = {}

        for render_part in render_parts:
            positions, normals, uv0, triangle_material_indices, material_slots = render_part
            if positions.size == 0:
                continue

            triangle_positions = positions.reshape(-1, 3, 3)
            triangle_normals = normals.reshape(-1, 3, 3)
            triangle_uv0 = uv0.reshape(-1, 3, 2)
            normalized_material_indices = np.array(triangle_material_indices, dtype=np.int32, copy=False)

            if normalized_material_indices.shape[0] != triangle_positions.shape[0]:
                raise RuntimeError("Triangle material index count does not match extracted triangle buffers")

            for slot_index, slot in enumerate(material_slots):
                slot_name = str(slot.get("slot_name") or "").strip()
                material_name = str(slot.get("material_name") or "").strip()
                slot_id = str(slot.get("slot_id") or self.make_material_slot_id(slot_name, material_name, slot_index))
                slot_key = (slot_id, slot_name, material_name)
                if slot_key not in slot_metadata_by_key:
                    ordered_slot_keys.append(slot_key)
                    slot_metadata_by_key[slot_key] = {
                        "slot_id": slot_id,
                        "slot_name": slot_name or slot_id,
                        "material_name": material_name or slot_name or slot_id,
                    }

                triangle_mask = normalized_material_indices == slot_index
                if not np.any(triangle_mask):
                    continue

                slot_chunks.setdefault(slot_key, []).append(
                    (
                        np.ascontiguousarray(triangle_positions[triangle_mask].reshape(-1, 3), dtype=np.float32),
                        np.ascontiguousarray(triangle_normals[triangle_mask].reshape(-1, 3), dtype=np.float32),
                        np.ascontiguousarray(triangle_uv0[triangle_mask].reshape(-1, 2), dtype=np.float32),
                    )
                )

        if not ordered_slot_keys:
            return None

        merged_positions_parts = []
        merged_normals_parts = []
        merged_uv0_parts = []
        merged_indices_parts = []
        material_slot_descriptors: list[dict[str, object]] = []
        section_descriptors: list[dict[str, object]] = []

        vertex_offset = 0
        index_offset = 0
        section_index = 0

        for slot_key in ordered_slot_keys:
            chunks = slot_chunks.get(slot_key)
            if not chunks:
                continue

            section_positions = np.concatenate([chunk[0] for chunk in chunks], axis=0)
            section_normals = np.concatenate([chunk[1] for chunk in chunks], axis=0)
            section_uv0 = np.concatenate([chunk[2] for chunk in chunks], axis=0)
            section_vertex_count = int(section_positions.shape[0])
            section_index_count = section_vertex_count

            merged_positions_parts.append(section_positions)
            merged_normals_parts.append(section_normals)
            merged_uv0_parts.append(section_uv0)
            merged_indices_parts.append(np.arange(vertex_offset, vertex_offset + section_vertex_count, dtype=np.uint32))

            slot_metadata = dict(slot_metadata_by_key[slot_key])
            slot_metadata["section_index"] = section_index
            material_slot_descriptors.append(slot_metadata)
            section_descriptors.append(
                {
                    "section_index": section_index,
                    "slot_id": slot_metadata["slot_id"],
                    "slot_name": slot_metadata["slot_name"],
                    "material_name": slot_metadata["material_name"],
                    "first_vertex": vertex_offset,
                    "vertex_count": section_vertex_count,
                    "first_index": index_offset,
                    "index_count": section_index_count,
                }
            )

            vertex_offset += section_vertex_count
            index_offset += section_index_count
            section_index += 1

        if not merged_positions_parts:
            return None

        return (
            np.concatenate(merged_positions_parts, axis=0),
            np.concatenate(merged_normals_parts, axis=0),
            np.concatenate(merged_uv0_parts, axis=0),
            np.concatenate(merged_indices_parts, axis=0),
            material_slot_descriptors,
            section_descriptors,
        )

    def build_local_render_parts_from_reference(self, reference, depsgraph, reference_cache: dict, recursion_depth: int):
        cache_key = self.get_cache_key(reference)
        if cache_key is not None and cache_key in reference_cache:
            return reference_cache[cache_key]

        render_parts = []
        if reference is not None:
            if bpy is not None and isinstance(reference, bpy.types.Object):
                if reference.type in SUPPORTED_OBJECT_TYPES:
                    evaluated_reference = reference.evaluated_get(depsgraph)
                    geometry_getter = getattr(evaluated_reference, "evaluated_geometry", None)
                    if callable(geometry_getter):
                        render_parts = self.build_local_render_parts_from_geometry(
                            geometry_getter(),
                            depsgraph,
                            reference_cache,
                            recursion_depth,
                        )
                    else:
                        evaluated_mesh = evaluated_reference.to_mesh(
                            preserve_all_data_layers=True,
                            depsgraph=depsgraph,
                        )
                        if evaluated_mesh is not None:
                            try:
                                extracted_buffers = self.extract_local_render_buffers_for_mesh(evaluated_mesh)
                                if extracted_buffers is not None:
                                    render_parts = [extracted_buffers]
                            finally:
                                evaluated_reference.to_mesh_clear()
            elif hasattr(reference, "mesh") and hasattr(reference, "instance_references"):
                render_parts = self.build_local_render_parts_from_geometry(
                    reference,
                    depsgraph,
                    reference_cache,
                    recursion_depth,
                )

        if cache_key is not None:
            reference_cache[cache_key] = render_parts
        return render_parts

    def build_local_render_parts_from_geometry(self, geometry, depsgraph, reference_cache: dict, recursion_depth: int):
        if recursion_depth > MAX_GEOMETRYSET_RECURSION_DEPTH:
            raise RuntimeError("Geometry Nodes instance recursion exceeded the supported depth")

        render_parts = []
        mesh = getattr(geometry, "mesh", None)
        if mesh is not None:
            extracted_buffers = self.extract_local_render_buffers_for_mesh(mesh)
            if extracted_buffers is not None:
                render_parts.append(extracted_buffers)

        instances_pointcloud = None
        try:
            instances_pointcloud = geometry.instances_pointcloud()
        except AttributeError:
            instances_pointcloud = None

        if instances_pointcloud is None:
            return render_parts

        try:
            transform_data = instances_pointcloud.attributes["instance_transform"].data
            reference_index_data = instances_pointcloud.attributes[".reference_index"].data
        except (AttributeError, KeyError, TypeError):
            return render_parts

        references = geometry.instance_references()
        if not references:
            return render_parts

        for point_index in range(len(instances_pointcloud.points)):
            reference_index = int(reference_index_data[point_index].value)
            if reference_index < 0 or reference_index >= len(references):
                continue

            instance_transform = transform_data[point_index].value.copy()
            reference_parts = self.build_local_render_parts_from_reference(
                references[reference_index],
                depsgraph,
                reference_cache,
                recursion_depth + 1,
            )
            for reference_part in reference_parts:
                render_parts.append(self.transform_render_part(reference_part, instance_transform, convert_to_unreal=False))

        return render_parts

    def extract_render_buffers_for_geometry_nodes_target(self, target_object, depsgraph):
        evaluated_object = target_object.evaluated_get(depsgraph)
        geometry_getter = getattr(evaluated_object, "evaluated_geometry", None)
        if not callable(geometry_getter):
            return None, None

        geometry = geometry_getter()
        instance_count = 0
        try:
            instances_pointcloud = geometry.instances_pointcloud()
            if instances_pointcloud is not None:
                instance_count = len(instances_pointcloud.points)
        except AttributeError:
            instance_count = 0

        local_render_parts = self.build_local_render_parts_from_geometry(geometry, depsgraph, {}, 0)
        if not local_render_parts:
            return None, None

        world_matrix = target_object.matrix_world.copy()
        world_render_parts = [self.transform_render_part(render_part, world_matrix, convert_to_unreal=True) for render_part in local_render_parts]
        merged_buffers = self.merge_render_parts(world_render_parts)
        if merged_buffers is None:
            return None, None

        cooked_target_label = target_object.name
        if instance_count > 0:
            cooked_target_label = f"{target_object.name} ({instance_count} GN instances)"

        return merged_buffers, cooked_target_label

    def extract_render_buffers_for_object(self, target_object, depsgraph):
        evaluated_object = target_object.evaluated_get(depsgraph)
        evaluated_mesh = evaluated_object.to_mesh(preserve_all_data_layers=True, depsgraph=depsgraph)

        if evaluated_mesh is None:
            raise RuntimeError(f"Failed to evaluate Blender object '{target_object.name}' into a mesh")

        try:
            extracted_buffers = self.extract_local_render_buffers_for_mesh(evaluated_mesh)
            if extracted_buffers is None:
                return None

            transformed_part = self.transform_render_part(
                extracted_buffers,
                evaluated_object.matrix_world.copy(),
                convert_to_unreal=True,
            )
            return self.merge_render_parts([transformed_part])
        finally:
            evaluated_object.to_mesh_clear()

    def write_mesh_to_shared_memory(
        self,
        positions: np.ndarray,
        normals: np.ndarray,
        uv0: np.ndarray,
        indices: np.ndarray,
        request_id: str = "",
        source_handle: str = "",
        target_id: str = "",
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
            self.send_for_request(
                "ResizeRequired",
                request_id=request_id,
                source_handle=source_handle,
                target_id=target_id,
                required_bytes=int(required_bytes),
            )
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

    def cook_target(self, payload: dict) -> None:
        if bpy is None:
            raise RuntimeError("Real mesh cooking requires Blender's bpy module")

        request_id = str(payload.get("request_id") or "")
        source_handle = str(payload.get("source_handle") or "").strip()
        target_id = str(payload.get("target_id") or "").strip()
        component_handle = str(payload.get("component_handle") or "").strip()
        values = payload.get("values", {})
        if not isinstance(values, dict):
            values = {}
        curve_payload = payload.get("owner_curve", {})
        if not isinstance(curve_payload, dict):
            curve_payload = {}

        source_state, target_descriptor, target_object, modifier = self.resolve_target(source_handle, target_id)
        capabilities = self.build_target_capabilities(target_object, modifier)
        if bool(capabilities.get("supports_curve_owner", False)):
            working_object, working_modifier = self.resolve_component_target(
                source_state,
                target_descriptor,
                target_object,
                component_handle,
            )
            self.apply_curve_owner_payload(working_object, curve_payload)
            self.apply_geometry_nodes_inputs(working_object, working_modifier, values)
            target_object = working_object
            modifier = working_modifier
        else:
            self.apply_geometry_nodes_inputs(target_object, modifier, values)

        bpy.context.view_layer.update()
        depsgraph = bpy.context.evaluated_depsgraph_get()
        extracted_buffers, cooked_target_label = self.extract_render_buffers_for_geometry_nodes_target(target_object, depsgraph)
        if extracted_buffers is None:
            extracted_buffers = self.extract_render_buffers_for_object(target_object, depsgraph)
            cooked_target_label = str(target_descriptor["object_name"])
        else:
            cooked_target_label = str(target_descriptor["object_name"])

        if extracted_buffers is None or extracted_buffers[0].size == 0:
            raise RuntimeError(f"Target '{target_id}' did not produce any triangles")

        merged_positions = extracted_buffers[0]
        merged_normals = extracted_buffers[1]
        merged_uv0 = extracted_buffers[2]
        merged_indices = extracted_buffers[3]
        material_slots = extracted_buffers[4]
        section_descriptors = extracted_buffers[5]

        slot_index = self.write_mesh_to_shared_memory(
            merged_positions,
            merged_normals,
            merged_uv0,
            merged_indices,
            request_id=request_id,
            source_handle=source_handle,
            target_id=target_id,
        )
        if slot_index is None:
            return

        self.send_for_request(
            "CookComplete",
            request_id=request_id,
            source_handle=source_handle,
            target_id=target_id,
            slot=slot_index,
            sequence=int(BridgeSharedHeader.from_buffer(self.shared_memory).slots[slot_index].sequence),
            vertex_count=int(merged_positions.shape[0]),
            index_count=int(merged_indices.shape[0]),
            object_name=cooked_target_label,
            object_count=1,
            material_slots=material_slots,
            sections=section_descriptors,
        )

    def handle_message(self, message: dict) -> bool:
        message_type = str(message.get("message_type", ""))
        if message_type == "Hello":
            return True
        if message_type == "EnsureSourceLoaded":
            self.ensure_source_loaded(message)
            return True
        if message_type == "DescribeTarget":
            self.describe_target(message)
            return True
        if message_type == "CookTarget":
            self.cook_target(message)
            return True
        if message_type == "ReleaseComponentState":
            self.release_component_state(
                str(message.get("component_handle") or "").strip(),
                str(message.get("source_handle") or "").strip(),
            )
            return True
        if message_type == "Shutdown":
            return False

        raise RuntimeError(f"Unsupported message_type: {message_type}")

    def run(self) -> int:
        self.pipe.connect()
        self.send("Hello", worker="Blender", mode="mesh_auto")

        try:
            while True:
                message = self.pipe.read_json()
                try:
                    should_continue = self.handle_message(message)
                except Exception as exc:  # pylint: disable=broad-except
                    self.send_for_request(
                        "Error",
                        request_id=str(message.get("request_id") or ""),
                        source_handle=str(message.get("source_handle") or ""),
                        target_id=str(message.get("target_id") or ""),
                        message=str(exc),
                    )
                    continue

                if not should_continue:
                    return 0
        except Exception as exc:  # pylint: disable=broad-except
            self.send("Error", message=str(exc))
            return 1
        finally:
            for source_handle in list(self.loaded_sources.keys()):
                self.unload_source(source_handle)
            self.pipe.close()
            self.shared_memory.close()


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pipe-name", required=True)
    parser.add_argument("--shared-memory-name", required=True)
    parser.add_argument("--shared-memory-bytes", required=True, type=int)
    parser.add_argument("--blend-file", default="")
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
