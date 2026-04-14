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
