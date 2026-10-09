# Sequential evio_processor baseline — 2026-10-09

One worker; one warm-up and three measured full-file runs per input. Both ROOT and text output enabled. Physics events / whole-command wall time, including startup and shutdown. Warm-ups and validation are excluded from timing summaries. amd64 emulation on ARM; mounted-workspace output, no fsync.

| Input | Successful measured runs | Physics events | Median wall s | Median kHz | Min–max kHz | ROOT bytes (last run) | Text bytes (last run) |
|---|---:|---:|---:|---:|---|---:|---:|
| rsidis_production_28268.dat.0 | 3/3 | 83565 | 90.804 | 0.920 | 0.906–0.927 | 532845608 | 4486507424 |
| mollerpol_test_614.evio.0 | 3/3 | 3173350 | 85.314 | 37.196 | 34.237–37.422 | 562524680 | 2141503116 |

## Individual runs (including warm-up)

| Input | Run | Wall s | kHz | Physics events | Valid | Log |
|---|---|---:|---:|---:|---|---|
| rsidis_production_28268.dat.0 | warmup | 88.784 | 0.941 | 83565 | True  | [log](../results/20261009T160810.608195Z-evio-processor/rsidis_production_28268.dat.0/processor-warmup.log) |
| rsidis_production_28268.dat.0 | 1 | 92.205 | 0.906 | 83565 | True  | [log](../results/20261009T160810.608195Z-evio-processor/rsidis_production_28268.dat.0/processor-1.log) |
| rsidis_production_28268.dat.0 | 2 | 90.165 | 0.927 | 83565 | True  | [log](../results/20261009T160810.608195Z-evio-processor/rsidis_production_28268.dat.0/processor-2.log) |
| rsidis_production_28268.dat.0 | 3 | 90.804 | 0.920 | 83565 | True  | [log](../results/20261009T160810.608195Z-evio-processor/rsidis_production_28268.dat.0/processor-3.log) |
| mollerpol_test_614.evio.0 | warmup | 95.413 | 33.259 | 3173350 | True  | [log](../results/20261009T160810.608195Z-evio-processor/mollerpol_test_614.evio.0/processor-warmup.log) |
| mollerpol_test_614.evio.0 | 1 | 92.687 | 34.237 | 3173350 | True  | [log](../results/20261009T160810.608195Z-evio-processor/mollerpol_test_614.evio.0/processor-1.log) |
| mollerpol_test_614.evio.0 | 2 | 84.799 | 37.422 | 3173350 | True  | [log](../results/20261009T160810.608195Z-evio-processor/mollerpol_test_614.evio.0/processor-2.log) |
| mollerpol_test_614.evio.0 | 3 | 85.314 | 37.196 | 3173350 | True  | [log](../results/20261009T160810.608195Z-evio-processor/mollerpol_test_614.evio.0/processor-3.log) |

## Environment and interpretation

- ROOT 6.34.00, Ubuntu 24.04, GCC 13.2, RelWithDebInfo, JCE C++20.
- `docker/compose.root.yaml` / `dev-root`, `linux/amd64` emulation on an ARM host; container sees 12 CPUs.
- Sequential `ProcessSequential` writer, one JANA worker. Both ROOT and text output enabled; no translation, dump, or experiment plugins.
- Installed stack rebuilt before timing; all 12 CTest checks passed.
- Each input runs once for warm-up followed by three measured runs. Files are overwritten between runs; final ROOT/text files remain local. ROOT default compression is unchanged.
- Whole-command wall time includes startup and output close. Input hashing and post-run validation are outside the timed interval. No fsync; this measures buffered output, not durable disk throughput.
- Physics counts come from PhysicsEventTap and must equal PhysicsEventUnfold minus BlockSource. Both waveform_tree and caen1190_tree must have that many entries. pulse_tree, m_tree, and h_integral must exist; ROOT must open cleanly without recovery, and text must be nonempty. All checks passed on every run.
- The prebuilt ROOT C++17/C++20 warning is accepted and unchanged.
- Compare future writer versions with the same input hashes, architecture/emulation, worker count, compression, output settings, and mount. These timings are not comparable directly to previous native-container parser timings.
- Møller measured times varied from 84.799 to 92.687 seconds; use repeated runs and the median when assessing improvements.

Captured source commit: `b7171c55cd1f5bfcd6e5a2aaa4eee3cdb3204cdd`. Installed evio_processor SHA-256: `35088a0750cb30004462f8c46877a4337545d89b717c512559085221cd36b35f`. Repository history changed during measurement; the captured plugin hash identifies the binary used for all runs.

## Inputs and exact commands

Commands execute inside the ROOT service with `JCE_CONFIG_DIR` cleared. Each per-input `plugins.db` contains exactly `evio_parser,evio_common_modules,evio_processor`.

### rsidis_production_28268.dat.0

Input bytes: 1058422544; SHA-256: `7d0ade12b6dc80218bcc68faeaeb0b95df9f7b967e060040da8b58ea9a27b065`.

```sh
/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh \
  -PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T160810.608195Z-evio-processor/rsidis_production_28268.dat.0/plugins.db \
  -Pnthreads=1 \
  -PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T160810.608195Z-evio-processor/rsidis_production_28268.dat.0/output.root \
  -PTXT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T160810.608195Z-evio-processor/rsidis_production_28268.dat.0/output.txt \
  /workspace/data_files/rsidis_production_28268.dat.0
```

### mollerpol_test_614.evio.0

Input bytes: 898415628; SHA-256: `774463627754a9b45180d83472a2ce70dd5735f73bac612966b87d7054f6179d`.

```sh
/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh \
  -PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T160810.608195Z-evio-processor/mollerpol_test_614.evio.0/plugins.db \
  -Pnthreads=1 \
  -PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T160810.608195Z-evio-processor/mollerpol_test_614.evio.0/output.root \
  -PTXT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T160810.608195Z-evio-processor/mollerpol_test_614.evio.0/output.txt \
  /workspace/data_files/mollerpol_test_614.evio.0
```

## Saved artifacts

Full logs, CSV (including warm-ups), exact commands, environment metadata, and
final ROOT/text output are in `benchmarks/results/20261009T160810.608195Z-evio-processor/`.
Raw artifacts are local and Git-ignored; this record preserves the measurements
for version control. Final output totals about 7.72 GB across both inputs.
