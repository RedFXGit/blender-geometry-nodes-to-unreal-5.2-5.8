# Reference Paths

This file captures the relevant paths discovered so far for the Blender-to-Unreal bridge scaffold.

## Workspace

- Workspace root: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge`
- Plan document: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\PLAN.md`

## Local Scaffold Paths

- Plugin root: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge`
- Plugin scripts: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Scripts`
- Runtime module root: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime`
- Runtime public headers: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public`
- Runtime private sources: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private`
- Runtime tests: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\Tests`
- Editor module root: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor`
- Editor public headers: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\Public`
- Editor private sources: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\Private`

## Unreal Engine Install

- Epic Games root: `C:\Program Files\Epic Games`
- Unreal Engine install discovered locally: `C:\Program Files\Epic Games\UE_5.7`
- Engine source root: `C:\Program Files\Epic Games\UE_5.7\Engine\Source`
- Engine plugins root: `C:\Program Files\Epic Games\UE_5.7\Engine\Plugins`

## Relevant Unreal Headers

- Property bag API: `C:\Program Files\Epic Games\UE_5.7\Engine\Source\Runtime\CoreUObject\Public\StructUtils\PropertyBag.h`
- Struct utils directory: `C:\Program Files\Epic Games\UE_5.7\Engine\Source\Runtime\CoreUObject\Public\StructUtils`
- Procedural mesh component header: `C:\Program Files\Epic Games\UE_5.7\Engine\Plugins\Runtime\ProceduralMeshComponent\Source\ProceduralMeshComponent\Public\ProceduralMeshComponent.h`

## Expected Implementation Targets

- Unreal plugin descriptor: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\BlenderBridge.uplugin`
- Blender worker script target: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Scripts\bridge_server.py`
- Runtime build file target: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\BlenderBridgeRuntime.Build.cs`
- Editor build file target: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\BlenderBridgeEditor.Build.cs`

## Notes

- The Unreal Engine paths above are local-machine discoveries from this session and may differ on another machine.
- The local scaffold directories already exist but do not contain implementation files yet.
