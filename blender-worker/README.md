# Blender Worker Staging

This folder is the phase-1 staging area for distributing the Blender-side worker separately from the Unreal plugin without changing the current editor workflow.

Current source of truth for the worker code remains:

- `Plugins/GeometryNodesBridge/Scripts/geometry_nodes_bridge_server.py`
- `Plugins/GeometryNodesBridge/Scripts/geometry_nodes_bridge_worker`

To stage a standalone worker package, run:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\Stage-GeometryNodesBridgeWorker.ps1
```

That script copies the worker into `blender-worker/staging/GeometryNodesBridgeWorker`.

After staging, point the Unreal plugin setting `Bridge Worker Script Path` at the staged `geometry_nodes_bridge_server.py`.

Leaving `Bridge Worker Script Path` empty keeps the current bundled plugin worker as the fallback.
