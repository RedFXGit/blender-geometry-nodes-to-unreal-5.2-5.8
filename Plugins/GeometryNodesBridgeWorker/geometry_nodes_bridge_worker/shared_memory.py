import ctypes


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


class BridgeInstanceBakeSlotHeader(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("sequence", ctypes.c_uint64),
        ("slot_index", ctypes.c_uint32),
        ("status", ctypes.c_uint32),
        ("payload_bytes", ctypes.c_uint32),
        ("prototype_count", ctypes.c_uint32),
        ("placement_count", ctypes.c_uint32),
        ("true_instance_count", ctypes.c_uint32),
        ("reserved", ctypes.c_uint32 * 20),
    ]


class BridgeInstanceBakeSharedHeader(ctypes.Structure):
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
        ("slots", BridgeInstanceBakeSlotHeader * 2),
    ]


class BridgeInstanceBakePayloadHeader(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("magic", ctypes.c_uint32),
        ("version", ctypes.c_uint32),
        ("prototype_count", ctypes.c_uint32),
        ("material_slot_count", ctypes.c_uint32),
        ("section_count", ctypes.c_uint32),
        ("placement_count", ctypes.c_uint32),
        ("prototype_records_offset", ctypes.c_uint32),
        ("material_slot_records_offset", ctypes.c_uint32),
        ("section_records_offset", ctypes.c_uint32),
        ("placement_records_offset", ctypes.c_uint32),
        ("string_table_offset", ctypes.c_uint32),
        ("string_table_bytes", ctypes.c_uint32),
        ("payload_bytes", ctypes.c_uint32),
        ("reserved", ctypes.c_uint32 * 3),
    ]


class BridgeInstanceBakePrototypeRecord(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("prototype_key_offset", ctypes.c_uint32),
        ("prototype_key_length", ctypes.c_uint32),
        ("first_material_slot_index", ctypes.c_uint32),
        ("material_slot_count", ctypes.c_uint32),
        ("first_section_index", ctypes.c_uint32),
        ("section_count", ctypes.c_uint32),
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
        ("reserved", ctypes.c_uint32 * 6),
    ]


class BridgeInstanceBakeMaterialSlotRecord(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("slot_id_offset", ctypes.c_uint32),
        ("slot_id_length", ctypes.c_uint32),
        ("slot_name_offset", ctypes.c_uint32),
        ("slot_name_length", ctypes.c_uint32),
        ("material_name_offset", ctypes.c_uint32),
        ("material_name_length", ctypes.c_uint32),
        ("section_index", ctypes.c_int32),
        ("reserved", ctypes.c_uint32),
    ]


class BridgeInstanceBakeSectionRecord(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("section_index", ctypes.c_int32),
        ("first_vertex", ctypes.c_int32),
        ("vertex_count", ctypes.c_int32),
        ("first_index", ctypes.c_int32),
        ("index_count", ctypes.c_int32),
        ("slot_id_offset", ctypes.c_uint32),
        ("slot_id_length", ctypes.c_uint32),
        ("slot_name_offset", ctypes.c_uint32),
        ("slot_name_length", ctypes.c_uint32),
        ("material_name_offset", ctypes.c_uint32),
        ("material_name_length", ctypes.c_uint32),
        ("reserved", ctypes.c_uint32),
    ]


class BridgeInstanceBakePlacementRecord(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("prototype_index", ctypes.c_uint32),
        ("reserved", ctypes.c_uint32),
        ("translation", ctypes.c_float * 3),
        ("rotation", ctypes.c_float * 4),
        ("scale3d", ctypes.c_float * 3),
    ]


assert ctypes.sizeof(BridgeMeshSlotHeader) == 112
assert ctypes.sizeof(BridgeSharedHeader) == 296
assert ctypes.sizeof(BridgeInstanceBakeSlotHeader) == 112
assert ctypes.sizeof(BridgeInstanceBakeSharedHeader) == 296
assert ctypes.sizeof(BridgeInstanceBakePayloadHeader) == 64
assert ctypes.sizeof(BridgeInstanceBakePrototypeRecord) == 112
assert ctypes.sizeof(BridgeInstanceBakeMaterialSlotRecord) == 32
assert ctypes.sizeof(BridgeInstanceBakeSectionRecord) == 48
assert ctypes.sizeof(BridgeInstanceBakePlacementRecord) == 48
