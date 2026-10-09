# EP-002: ROOT internal parallel compression

## Change and intended scope

Add opt-in `ROOT_IMT_THREADS` (default 0) to enable ROOT's implicit multithreading
before creating the trees. Default 0 leaves the current ROOT state unchanged.
Filling remains in `ProcessSequential`; ROOT handles supported branch-compression
work internally. No worker-local trees or merger are introduced, and no schema,
fill-rule, entry-order, algorithm, or compression-level change is intended.

ROOT's pool is process-global and its requested size is a hint. The plugin logs
the actual size; initialization fails if IMT cannot be enabled. It does not
tear down a pool shared with other plugins. Avoid oversubscribing JANA plus ROOT.

Implementation and this record are committed together; resolve their revision
with `git log -- benchmarks/optimizations/EP-002-root-compression.md`.
References: [EP-000](../records/evio-processor-root-only-2026-10-09.md) and
[EP-001](EP-001-waveform-copy.md). The benchmark uses a fresh same-binary IMT-off
control, not only the historical timings.

## Correctness

Build passed. On complete small input `mollerpol_test_548.evio.0`, IMT=4
matched the pre-change file entry-by-entry: 1,031 waveform/CAEN1190/pulse entries,
1,017 helicity entries, and all histogram bins, errors, and edges. Local files:
`benchmarks/results/waveform-copy-check/after.root` and `imt4.root`.
Use `benchmarks/compare_processor_root.py` to repeat this comparison.
Full inputs additionally check physics counts, schemas, object entry counts,
histogram bins/edges, clean ROOT open, and absence of text output.

## Benchmark and outcome

[Benchmark report](../records/EP-002-root-compression-2026-10-09.md):
+23.14% SIDIS and +5.11% Møller throughput in an alternating same-binary control
comparison under amd64 emulation. One JANA worker, IMT off versus four ROOT
threads, one warm-up and three measured full-file runs per case/input.
All 16 runs passed validation. Complete final files also matched every ordered
tree value and histogram bin/error/edge on both full inputs using
`compare_processor_root_fast.C`. A deliberately altered histogram was detected.
Keep the option disabled by default; gains and pool sizing are workload-specific.

## Reusable pattern

Before restructuring event ownership or output ordering, test parallelism already
provided by the output library. Make global thread-pool changes explicit and
opt-in, log the effective configuration, and compare against a same-binary
control. Parallelism is useful only if measured gains exceed scheduling overhead.

## Sources

- [ROOT multithreading manual](https://root.cern/manual/multi_threading/): supported implicit parallel operations and global pool configuration.
- Installed ROOT 6.34.00 headers/runtime and small-input output comparison.
