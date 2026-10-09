# EP-001: Remove the intermediate waveform copy

## Change and rationale

Plugin: `evio_processor`. Implementation is committed together with this record and its benchmark report.
Resolve the revision with `git log -- benchmarks/optimizations/EP-001-waveform-copy.md`.
Reference: [EP-000 ROOT-only sequential baseline](../records/evio-processor-root-only-2026-10-09.md), recorded in commit `aca63cd`.

Previously, each hit's waveform was copied into `WaveformTreeRow::waveform`, then
copied again into the ROOT branch vector. Append directly from the immutable hit
waveform and read slot/channel/ROC directly from the hit. Remove the temporary
row structure and its initialization.

Expected effect: one fewer waveform-sized copy per hit and no temporary waveform
buffer. This is a hypothesis about cost reduction, not a measured throughput gain.
No new dependencies, threading, compression changes, or allocation policy changes.

Implementation: `src/plugins/evio_processor/JEventProcessor_EVIO.cc` and `.h`.

## Output invariants and correctness

Preserve all tree names, branch names/types, flattened waveform sample order,
per-sample slot/channel/ROC values, fill rules, histogram contents, and sequential
processing. No input data or output scope is removed.

The targeted plugin build passed. A before/after run on the complete small input
`mollerpol_test_548.evio.0` used one worker and ROOT-only output. The comparison
script checked branch schemas and every entry value: 1,031 waveform, CAEN1190,
and pulse entries, plus 1,017 helicity entries. All histogram bins, errors, and
axis edges matched. This is small-input validation, not exhaustive coverage of
every possible hit combination or a performance measurement.

```sh
docker compose -f docker/compose.root.yaml run --rm dev-root \
  python3 benchmarks/compare_processor_root.py \
  benchmarks/results/waveform-copy-check/before.root \
  benchmarks/results/waveform-copy-check/after.root
```

Local reference files and generation logs: `benchmarks/results/waveform-copy-check/`.
The existing ROOT standard mismatch warning remains accepted.

## Associated benchmarks

[Full standard-input benchmark](../records/EP-001-waveform-copy-2026-10-09.md).
All measured runs passed validation. Throughput changed by -4.88% on SIDIS
and -3.56% on Møller relative to EP-000 in separate sessions under emulation.
No speedup demonstrated; session differences prevent causal attribution.

## Reusable plugin pattern

When input ownership guarantees read access during a callback, copy directly
from input storage into the final output buffer. Avoid temporary payload copies
that add no transformation. Preserve lifetime boundaries: this change still owns
the branch buffer and does not retain pointers into an event after processing.

## Outcome

Correctness checks passed. Full benchmark shows no throughput gain. Retain the
simpler direct-copy path without claiming improved throughput. EP-000 stays the
optimization reference; this measurement is an additional checkpoint.
