# Geometry Nodes Bridge Smoke Suite

- Run label: smoke_validation_4
- Status: **passed**

## Functional

- GeometryNodesBridge.E2E.HouseCookAndStaticBake: **pass**
- GeometryNodesBridge.E2E.CurveOwnerSplineRecook: **pass**
- GeometryNodesBridge.E2E.InstancesInstancedBakeSharing: **pass**

## Performance

| Scenario | Median (ms) | Threshold (ms) | Status |
| --- | ---: | ---: | --- |
| GeometryNodesBridge.PerfSmoke.HousePreview | 127.686 | 139.090 | pass |
| GeometryNodesBridge.PerfSmoke.CurvePreview | 141.949 | 205.385 | pass |
| GeometryNodesBridge.PerfSmoke.InstancesPreview | 28.597 | 31.521 | pass |

