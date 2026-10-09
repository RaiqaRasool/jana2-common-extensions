# EP-002: ROOT internal parallel compression

One JANA worker; ROOT IMT disabled vs four threads, alternating pair order; one warm-up and three measured full-file runs per case. Default compression settings unchanged; mounted workspace, no fsync. End-to-end physics count / whole-command wall time. Validation/hashing excluded. amd64 emulation on ARM.

| Input | ROOT threads | Median s | Median kHz | Min–max kHz | Change vs disabled |
|---|---:|---:|---:|---|---:|
| rsidis_production_28268.dat.0 | 0 | 37.674 | 2.218 | 2.215–2.218 | +0.00% |
| rsidis_production_28268.dat.0 | 4 | 30.595 | 2.731 | 2.685–2.774 | +23.14% |
| mollerpol_test_614.evio.0 | 0 | 52.154 | 60.846 | 60.826–60.862 | +0.00% |
| mollerpol_test_614.evio.0 | 4 | 49.617 | 63.957 | 63.955–64.136 | +5.11% |

## Individual runs

| Input | ROOT threads | Run | Wall s | kHz | ROOT bytes | Log |
|---|---:|---|---:|---:|---:|---|
| rsidis_production_28268.dat.0 | 0 | warmup | 37.643 | 2.220 | 532846718 | [log](../results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/imt0-warmup.log) |
| rsidis_production_28268.dat.0 | 4 | warmup | 30.575 | 2.733 | 532833186 | [log](../results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/imt4-warmup.log) |
| rsidis_production_28268.dat.0 | 4 | 1 | 30.595 | 2.731 | 532845461 | [log](../results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/imt4-1.log) |
| rsidis_production_28268.dat.0 | 0 | 1 | 37.672 | 2.218 | 532846718 | [log](../results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/imt0-1.log) |
| rsidis_production_28268.dat.0 | 0 | 2 | 37.722 | 2.215 | 532846718 | [log](../results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/imt0-2.log) |
| rsidis_production_28268.dat.0 | 4 | 2 | 30.124 | 2.774 | 532832810 | [log](../results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/imt4-2.log) |
| rsidis_production_28268.dat.0 | 4 | 3 | 31.125 | 2.685 | 532847300 | [log](../results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/imt4-3.log) |
| rsidis_production_28268.dat.0 | 0 | 3 | 37.674 | 2.218 | 532846718 | [log](../results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/imt0-3.log) |
| mollerpol_test_614.evio.0 | 0 | warmup | 52.557 | 60.379 | 562523799 | [log](../results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/imt0-warmup.log) |
| mollerpol_test_614.evio.0 | 4 | warmup | 49.256 | 64.426 | 562524565 | [log](../results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/imt4-warmup.log) |
| mollerpol_test_614.evio.0 | 4 | 1 | 49.618 | 63.955 | 562525151 | [log](../results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/imt4-1.log) |
| mollerpol_test_614.evio.0 | 0 | 1 | 52.140 | 60.862 | 562523799 | [log](../results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/imt0-1.log) |
| mollerpol_test_614.evio.0 | 0 | 2 | 52.154 | 60.846 | 562523799 | [log](../results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/imt0-2.log) |
| mollerpol_test_614.evio.0 | 4 | 2 | 49.617 | 63.957 | 562522979 | [log](../results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/imt4-2.log) |
| mollerpol_test_614.evio.0 | 4 | 3 | 49.478 | 64.136 | 562523233 | [log](../results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/imt4-3.log) |
| mollerpol_test_614.evio.0 | 0 | 3 | 52.171 | 60.826 | 562523799 | [log](../results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/imt0-3.log) |

## Optimization and validation

Change: [EP-002 internal parallel compression](../optimizations/EP-002-root-compression.md). Implementation is committed together with this record; resolve its revision with `git log -- benchmarks/records/EP-002-root-compression-2026-10-09.md`.

All 16 runs passed counts/schema/histogram checks. Complete final off/four-thread files also matched every ordered tree value and histogram bin/error/edge in both inputs using `compare_processor_root_fast.C`. The comparator passed a small positive control and detected a deliberately altered histogram. Binary ROOT file sizes can differ slightly because compressed basket layout/metadata differ; payloads and entry order match.

SIDIS: +23.14% throughput (18.79% less wall time). Møller: +5.11% throughput (4.86% less wall time). These results use amd64 emulation, not native Linux. The feature stays opt-in: benefits and best pool size depend on the workload and hardware. Default 0 leaves ROOT state unchanged. The pool is process-global; an existing pool may be reused. No worker-local tree merging is implemented in this optimization.

## Provenance

Captured parent revision: `bc4def394026a855abbfe8aeed29eeb9ad30b1ca`; implementation was uncommitted during timing. Installed plugin SHA-256: `1e8e24e5640e0c1ba951c166f594c08b18d0c5aa34ce132077e7fb097baa7640`. ROOT 6.34.00; Linux-6.10.14-linuxkit-x86_64-with-glibc2.39; 12 visible CPUs.

Source JEventProcessor_EVIO.cc SHA-256: `a35a75b244c62dd56bf8d9570f1837378b75be9df1b04a4331a536e94fb1e15f`.

Source JEventProcessor_EVIO.h SHA-256: `ea9a680b39475636f2d291d620bc5f6296f595eb90a0658f80831c56ed50c533`.


### rsidis_production_28268.dat.0

Input bytes: 1058422544; SHA-256: `7d0ade12b6dc80218bcc68faeaeb0b95df9f7b967e060040da8b58ea9a27b065`. Exact commands:

```json
{
  "0": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_IMT_THREADS=0",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/0.root",
    "/workspace/data_files/rsidis_production_28268.dat.0"
  ],
  "4": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T170310.218149Z-EP-002/rsidis_production_28268.dat.0/4.root",
    "/workspace/data_files/rsidis_production_28268.dat.0"
  ]
}
```

### mollerpol_test_614.evio.0

Input bytes: 898415628; SHA-256: `774463627754a9b45180d83472a2ce70dd5735f73bac612966b87d7054f6179d`. Exact commands:

```json
{
  "0": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_IMT_THREADS=0",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/0.root",
    "/workspace/data_files/mollerpol_test_614.evio.0"
  ],
  "4": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T170310.218149Z-EP-002/mollerpol_test_614.evio.0/4.root",
    "/workspace/data_files/mollerpol_test_614.evio.0"
  ]
}
```

Local raw evidence: `benchmarks/results/20261009T170310.218149Z-EP-002/`. Warm-ups are retained in CSV but excluded from summaries. Full comparison completed on resume after the previous time cap stopped the validation.
