import time

try:
    import numpy as np
except ImportError as exc:
    raise RuntimeError("geometry_nodes_bridge_server.py requires NumPy in the Blender Python environment") from exc

try:
    import bpy  # type: ignore
    from mathutils import Matrix, Quaternion, Vector  # type: ignore
except ImportError:
    bpy = None
    Matrix = None
    Quaternion = None
    Vector = None


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
BRIDGE_ROOT_COLLECTION_NAME = "GeometryNodesBridgeSources"


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


def elapsed_ms(start_time: float, end_time: float | None = None) -> float:
    final_time = time.perf_counter() if end_time is None else end_time
    return (final_time - start_time) * 1000.0


def make_extract_timing_breakdown() -> dict[str, float]:
    return {
        "geometry_access_ms": 0.0,
        "build_parts_ms": 0.0,
        "mesh_prepare_ms": 0.0,
        "mesh_read_ms": 0.0,
        "mesh_expand_ms": 0.0,
        "transform_ms": 0.0,
        "merge_ms": 0.0,
    }
