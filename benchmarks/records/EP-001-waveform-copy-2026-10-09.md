# EP-001: waveform-copy benchmark — 2026-10-09

One worker, one warm-up and three measured runs per full input. ROOT-only, unchanged default compression; mounted workspace output, no fsync. Physics events / whole-command wall time including startup and shutdown. Input hashing and validation excluded. linux/amd64 emulation on ARM.

| Input | Physics events | Median wall s | Median kHz | Min–max kHz | ROOT bytes |
|---|---:|---:|---:|---|---:|
| rsidis_production_28268.dat.0 | 83565 | 40.128 | 2.082 | 2.080–2.107 | 532845804 |
| mollerpol_test_614.evio.0 | 3173350 | 54.681 | 58.034 | 57.298–58.688 | 562522821 |

## Individual runs

| Input | Run | Wall s | kHz | Valid | Log |
|---|---|---:|---:|---|---|
| rsidis_production_28268.dat.0 | warmup | 39.153 | 2.134 | True | [log](../results/20261009T164618.050287Z-root-only/rsidis_production_28268.dat.0/processor-warmup.log) |
| rsidis_production_28268.dat.0 | 1 | 40.182 | 2.080 | True | [log](../results/20261009T164618.050287Z-root-only/rsidis_production_28268.dat.0/processor-1.log) |
| rsidis_production_28268.dat.0 | 2 | 39.666 | 2.107 | True | [log](../results/20261009T164618.050287Z-root-only/rsidis_production_28268.dat.0/processor-2.log) |
| rsidis_production_28268.dat.0 | 3 | 40.128 | 2.082 | True | [log](../results/20261009T164618.050287Z-root-only/rsidis_production_28268.dat.0/processor-3.log) |
| mollerpol_test_614.evio.0 | warmup | 54.627 | 58.091 | True | [log](../results/20261009T164618.050287Z-root-only/mollerpol_test_614.evio.0/processor-warmup.log) |
| mollerpol_test_614.evio.0 | 1 | 54.681 | 58.034 | True | [log](../results/20261009T164618.050287Z-root-only/mollerpol_test_614.evio.0/processor-1.log) |
| mollerpol_test_614.evio.0 | 2 | 55.383 | 57.298 | True | [log](../results/20261009T164618.050287Z-root-only/mollerpol_test_614.evio.0/processor-2.log) |
| mollerpol_test_614.evio.0 | 3 | 54.071 | 58.688 | True | [log](../results/20261009T164618.050287Z-root-only/mollerpol_test_614.evio.0/processor-3.log) |

## Change and reference

Optimization: [EP-001](../optimizations/EP-001-waveform-copy.md), direct waveform append without a temporary copy. Reference: [EP-000](evio-processor-root-only-2026-10-09.md). This report is committed together with the implementation; resolve its revision with `git log -- benchmarks/records/EP-001-waveform-copy-2026-10-09.md`.

| Input | EP-000 median s | EP-001 median s | Throughput change |
|---|---:|---:|---:|
| mollerpol_test_614.evio.0 | 52.736 | 54.681 | -3.56% |
| rsidis_production_28268.dat.0 | 38.169 | 40.128 | -4.88% |

## Interpretation

No end-to-end speedup demonstrated. Measured throughput is lower on both inputs. These are separate benchmark sessions under Docker amd64 emulation, without an interleaved control or isolated host load, so the difference does not establish a causal regression. Keep the simpler direct-copy path as a reduction in redundant work, not as a proven throughput improvement. EP-000 remains the reference; use this result as an additional checkpoint.

All eight runs passed validation. Input hashes, schemas, per-object entry counts, and ROOT output sizes match EP-000. Small-input before/after comparison also matched every tree value and histogram bin. Full-input validation checks schemas/counts, not every payload value. ROOT default compression and one-worker sequential writing are unchanged.

## Provenance and reproduction

Parent source revision captured at run start: `aca63cd8de18afe89b333332dcbdf0e0a7f1c747` (implementation was uncommitted). Installed plugin SHA-256: `5f269232ced1ef1de3c4bffb8614f12abf12d9823822a88bbe87b83f11907e03`. ROOT 6.34.00; Linux-6.10.14-linuxkit-x86_64-with-glibc2.39; 12 visible CPUs.

Source JEventProcessor_EVIO.cc SHA-256: `85621d7da426526b7a3bcd7cfaef5f6ae8297821dff3d2334017811d4b032552`.

Source JEventProcessor_EVIO.h SHA-256: `8d61f0488682e567dda37c7f0475b9f7987e1b1c5ecbd061a3d5557f36f86118`.

```sh
docker compose -f docker/compose.root.yaml run --rm dev-root python3 benchmarks/run_processor.py
```

### mollerpol_test_614.evio.0

Input bytes: 898415628; SHA-256: `774463627754a9b45180d83472a2ce70dd5735f73bac612966b87d7054f6179d`.

Exact container command:

```json
[
  "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
  "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T164618.050287Z-root-only/mollerpol_test_614.evio.0/plugins.db",
  "-Pnthreads=1",
  "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T164618.050287Z-root-only/mollerpol_test_614.evio.0/output.root",
  "/workspace/data_files/mollerpol_test_614.evio.0"
]
```

### rsidis_production_28268.dat.0

Input bytes: 1058422544; SHA-256: `7d0ade12b6dc80218bcc68faeaeb0b95df9f7b967e060040da8b58ea9a27b065`.

Exact container command:

```json
[
  "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
  "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T164618.050287Z-root-only/rsidis_production_28268.dat.0/plugins.db",
  "-Pnthreads=1",
  "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T164618.050287Z-root-only/rsidis_production_28268.dat.0/output.root",
  "/workspace/data_files/rsidis_production_28268.dat.0"
]
```

Raw measurements, environment metadata, per-run schemas, logs, and final ROOT files: `benchmarks/results/20261009T164618.050287Z-root-only/` (local ignored artifacts). JCE_CONFIG_DIR cleared; plugins exactly evio_parser,evio_common_modules,evio_processor.
