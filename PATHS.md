# Reference Paths

This file captures the most relevant paths discovered so far for the current Blender-to-Unreal bridge implementation.

## Workspace

- Workspace root: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge`
- Plan document: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\PLAN.md`
- README: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\README.md`
- Testing guide: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\TESTING.md`
- This path reference: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\PATHS.md`
- Repo `.gitignore`: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\.gitignore`

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

## Key Implemented Bridge Files

- Plugin descriptor: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\BlenderBridge.uplugin`
- Blender worker script: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Scripts\bridge_server.py`
- Runtime build file: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\BlenderBridgeRuntime.Build.cs`
- Runtime module header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeRuntimeModule.h`
- Runtime module source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeRuntimeModule.cpp`
- Placeable actor header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeActor.h`
- Placeable actor source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeActor.cpp`
- Component header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeComponent.h`
- Component source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeComponent.cpp`
- Session manager header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeSessionManager.h`
- Session manager source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeSessionManager.cpp`
- Settings header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeSettings.h`
- Settings source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeSettings.cpp`
- Source asset header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeSource.h`
- Source asset source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeSource.cpp`
- Protocol header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeProtocol.h`
- Protocol source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeProtocol.cpp`
- Message helpers header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeMessages.h`
- Message helpers source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeMessages.cpp`
- Worker session header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeWorkerSession.h`
- Worker session source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeWorkerSession.cpp`
- Pipe client header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgePipeClient.h`
- Pipe client source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgePipeClient.cpp`
- Shared memory header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Public\BlenderBridgeSharedMemory.h`
- Shared memory source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeRuntime\Private\BlenderBridgeSharedMemory.cpp`
- Editor build file: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\BlenderBridgeEditor.Build.cs`
- Actor factory header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\Public\ActorFactoryBlenderBridgeSource.h`
- Actor factory source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\Private\ActorFactoryBlenderBridgeSource.cpp`
- Asset definition header: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\Private\AssetDefinition_BlenderBridgeSource.h`
- Asset definition source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\Private\AssetDefinition_BlenderBridgeSource.cpp`
- Editor module source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\Private\BlenderBridgeEditorModule.cpp`
- Property bag customization source: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\Plugins\BlenderBridge\Source\BlenderBridgeEditor\Private\BlenderBridgePropertyBagDetails.cpp`

## Sync And Project Paths

- Sync script: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\scripts\Sync-BlenderBridgeToProject.ps1`
- Watch-and-sync script: `C:\Users\Lassiter\Documents\GitHub\blender-unreal-bridge\scripts\Watch-BlenderBridgeToProject.ps1`
- Unreal project root used in testing: `C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal`
- Unreal project plugin copy: `C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal\Plugins\BlenderBridge`
- Unreal project file: `C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal\blender_unreal.uproject`
- Unreal project saved editor config: `C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal\Saved\Config\WindowsEditor`
- Blender Bridge project settings config: `C:\Users\Lassiter\Documents\Unreal Projects\blender_unreal\Saved\Config\WindowsEditor\EditorPerProjectUserSettings.ini`

## Unreal Engine Install

- Epic Games root: `C:\Program Files\Epic Games`
- Unreal Engine install discovered locally: `C:\Program Files\Epic Games\UE_5.7`
- Engine source root: `C:\Program Files\Epic Games\UE_5.7\Engine\Source`
- Engine plugins root: `C:\Program Files\Epic Games\UE_5.7\Engine\Plugins`

## Relevant Unreal Headers

- Property bag API: `C:\Program Files\Epic Games\UE_5.7\Engine\Source\Runtime\CoreUObject\Public\StructUtils\PropertyBag.h`
- Struct utils directory: `C:\Program Files\Epic Games\UE_5.7\Engine\Source\Runtime\CoreUObject\Public\StructUtils`
- Procedural mesh component header: `C:\Program Files\Epic Games\UE_5.7\Engine\Plugins\Runtime\ProceduralMeshComponent\Source\ProceduralMeshComponent\Public\ProceduralMeshComponent.h`

## Local Blender Installs

- Blender Foundation root: `C:\Program Files\Blender Foundation`
- Blender 5.1: `C:\Program Files\Blender Foundation\Blender 5.1\blender.exe`
- Blender 4.4: `C:\Program Files\Blender Foundation\Blender 4.4\blender.exe`
- Blender 3.6: `C:\Program Files\Blender Foundation\Blender 3.6\blender.exe`
- Blender 3.1: `C:\Program Files\Blender Foundation\Blender 3.1\blender.exe`
- Blender 2.82: `C:\Program Files\Blender Foundation\Blender 2.82\blender.exe`

## Current Shared-Session UX Paths

- Unreal project setting location: `Edit > Project Settings > Plugins > Blender Bridge`
- Global session menu location: `Tools > Blender Bridge`
- Asset placement flow: `Import .blend -> BlenderBridgeSource -> drag source into level -> BlenderBridgeActor`
- Auto-detected default Blender path on this machine: `C:\Program Files\Blender Foundation\Blender 5.1\blender.exe`

## Notes

- The Unreal Engine paths above are local-machine discoveries from this session and may differ on another machine.
- `Build\` in the repo is disposable packaging output from Unreal's `BuildPlugin` flow and is not source-of-truth content.
- The shared-session architecture stores `Blender Executable Path` and shared-memory settings globally in editor per-project user settings, not on each component.
- The live source-of-truth implementation is the repo plugin under `Plugins\BlenderBridge`; the Unreal project plugin copy is just the synced test target.
