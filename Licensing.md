# Licensing

This repository uses a split-license model so the Unreal plugin side and the Blender worker side are not presented as one single repo-wide license bucket.

## Path Mapping

MIT

- `Plugins/GeometryNodesBridge/**`
- `scripts/**`
- `README.md`
- `AGENTS.md`
- `Licensing.md`
- `.gitignore`
- `.gitattributes`

GPL-3.0-or-later

- `Plugins/GeometryNodesBridgeWorker/**`

## Why The Split Exists

The Blender worker uses Blender's Python API and is intended to be treated separately from the Unreal plugin side.

This repo keeps the current development workflow intact:

- the worker source of truth lives at `Plugins/GeometryNodesBridgeWorker`
- the Unreal plugin points at that worker through `Bridge Worker Script Path`

## Development Notes

- `Bridge Worker Script Path` is required.
- During development, point it at `Plugins/GeometryNodesBridgeWorker/geometry_nodes_bridge_server.py` in this repo.
- There is no plugin-side worker fallback in the current setup.

## Notes

- This file documents the repo's intended path-level licensing layout; it is not legal advice.
- If a file or descendant directory includes a more specific license notice, that more specific notice controls.
