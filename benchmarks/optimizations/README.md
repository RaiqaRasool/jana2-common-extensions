# Plugin optimization records

Keep one numbered record per optimization, alongside the code change. Record
ideas that fail to improve performance too; do not rewrite earlier measurements.

| ID | Plugin | Change | Status | Measurements |
|---|---|---|---|---|
| EP-000 | evio_processor | ROOT-only sequential reference | Baseline | [Reference](../records/evio-processor-root-only-2026-10-09.md) |
| EP-001 | evio_processor | Remove intermediate waveform copy | Validated; no measured speedup | [Record](EP-001-waveform-copy.md), [benchmark](../records/EP-001-waveform-copy-2026-10-09.md) |
| EP-002 | evio_processor | Internal parallel branch compression | Validated; opt-in gains on both inputs | [Record](EP-002-root-compression.md), [benchmark](../records/EP-002-root-compression-2026-10-09.md) |
| EP-003 | evio_processor | Worker-local TTree merger experiment | Parked; correctness mismatch unresolved | `wip/ep-003-worker-local-root` at `8d4979c` |
| EP-004 | evio_processor | Sequential RNTuple with matched compression | Validated; opt-in gains on both inputs | [Record](EP-004-sequential-rntuple.md), [benchmark](../records/EP-004-sequential-rntuple-2026-10-09.md) |
| EP-005 | evio_processor | Single event-oriented RNTuple reference | Sequential layout reference | [Record](EP-005-event-rntuple-layout.md), [benchmark](../records/EP-005-event-rntuple-layout-2026-10-09.md) |

For every subsequent optimization:

1. Assign a stable ID and identify the previous comparable record.
2. Describe the concrete change, expected cost reduction, and applicable plugin pattern.
3. Record output invariants and correctness evidence, including the limits of validation.
4. Link the implementation commit once available. Before commit, explicitly mark it pending.
5. Attach each benchmark report to the ID. Include source revision, installed plugin
   hash, exact commands, input hashes, output settings, architecture/emulation,
   worker count, repetitions, and raw result location.
6. Compare per-input medians and ranges against a compatible reference. Distinguish
   measured effects from expectations. Record regressions and inconclusive results.
7. Update this index with the outcome. Keep historical reports unchanged.

Benchmark reports must identify the optimization ID and link back to its record.
Records must link to every associated report, including reruns and failures.
Local ignored logs and outputs support investigation; committed reports must
retain enough measurements and metadata to remain useful without those files.
Changes to output scope or compression need a new reference, not a claim of
implementation speedup. Retain correctness tests and reproduction commands when
extracting a pattern for future plugins.
