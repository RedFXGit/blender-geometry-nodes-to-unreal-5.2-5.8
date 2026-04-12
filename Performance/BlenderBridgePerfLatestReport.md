# Blender Bridge Perf Run

- Pass label: area_4_shm_write
- Run started (UTC): 2026-04-10T03:11:34.9543376Z
- Status: **accepted**

## Acceptance

- house_preview primary metric improved by 12.64 ms (9.58%).
- mean_worker_shared_write_ms dropped from 6.82 ms to 5.99 ms.
- Guardrail scenario curve_preview stayed within the no-regress bar (-57.00%%).
- Guardrail scenario instances_preview stayed within the no-regress bar (-57.57%%).
- Secondary guardrail scenario instances_bake regressed by 17.43%%; reported for visibility but not used as a hard acceptance blocker for this preview-focused pass.

## Scenario Summary

| Scenario | Metric | Mean (ms) | Delta vs baseline (ms) | Delta vs previous (ms) |
| --- | --- | ---: | ---: | ---: |
| house_preview | total_to_visible_ms | 119.270 | -350.686 | -12.638 |
| curve_preview | total_to_visible_ms | 242.242 | -321.057 | -3.461 |
| instances_preview | total_to_visible_ms | 22.111 | -30.002 | 2.813 |
| instances_bake | total_to_finish_ms | 102.470 | 15.209 | -0.738 |

