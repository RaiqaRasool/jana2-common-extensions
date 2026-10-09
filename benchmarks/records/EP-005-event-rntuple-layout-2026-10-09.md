# EP-005: layout benchmark

Four ROOT IMT threads and ZLIB level 1 in all cases. Alternating forward/reverse case order, one warmup plus three measured full-file runs per case/input. Physics tap counts / entire command wall time; validation/hashing excluded. amd64 emulation on ARM, mounted workspace, no explicit fsync.

| Input | Case | Median s | Median kHz | Min–max kHz | Change vs first case |
|---|---|---:|---:|---|---:|
| rsidis_production_28268.dat.0 | four-dataset | 15.761 | 5.302 | 5.026–6.114 | +0.00% |
| rsidis_production_28268.dat.0 | event | 14.709 | 5.681 | 5.665–6.124 | +7.15% |
| mollerpol_test_614.evio.0 | four-dataset | 22.119 | 143.465 | 137.281–143.653 | +0.00% |
| mollerpol_test_614.evio.0 | event | 25.985 | 122.123 | 119.609–122.209 | -14.88% |

## Individual runs

| Input | Case | Run | Wall s | kHz | ROOT bytes | Log |
|---|---|---|---:|---:|---:|---|
| rsidis_production_28268.dat.0 | four-dataset | warmup | 13.848 | 6.034 | 377305486 | [log](../results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/four-dataset-warmup.log) |
| rsidis_production_28268.dat.0 | event | warmup | 13.009 | 6.424 | 377440488 | [log](../results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/event-warmup.log) |
| rsidis_production_28268.dat.0 | event | 1 | 13.645 | 6.124 | 377440488 | [log](../results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/event-1.log) |
| rsidis_production_28268.dat.0 | four-dataset | 1 | 15.761 | 5.302 | 377305486 | [log](../results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/four-dataset-1.log) |
| rsidis_production_28268.dat.0 | four-dataset | 2 | 16.625 | 5.026 | 377305486 | [log](../results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/four-dataset-2.log) |
| rsidis_production_28268.dat.0 | event | 2 | 14.709 | 5.681 | 377440488 | [log](../results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/event-2.log) |
| rsidis_production_28268.dat.0 | event | 3 | 14.752 | 5.665 | 377440488 | [log](../results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/event-3.log) |
| rsidis_production_28268.dat.0 | four-dataset | 3 | 13.668 | 6.114 | 377305486 | [log](../results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/four-dataset-3.log) |
| mollerpol_test_614.evio.0 | four-dataset | warmup | 22.176 | 143.099 | 216709024 | [log](../results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/four-dataset-warmup.log) |
| mollerpol_test_614.evio.0 | event | warmup | 25.939 | 122.341 | 218354535 | [log](../results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/event-warmup.log) |
| mollerpol_test_614.evio.0 | event | 1 | 26.531 | 119.609 | 218354535 | [log](../results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/event-1.log) |
| mollerpol_test_614.evio.0 | four-dataset | 1 | 22.119 | 143.465 | 216709024 | [log](../results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/four-dataset-1.log) |
| mollerpol_test_614.evio.0 | four-dataset | 2 | 22.090 | 143.653 | 216709024 | [log](../results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/four-dataset-2.log) |
| mollerpol_test_614.evio.0 | event | 2 | 25.985 | 122.123 | 218354535 | [log](../results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/event-2.log) |
| mollerpol_test_614.evio.0 | event | 3 | 25.967 | 122.209 | 218354535 | [log](../results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/event-3.log) |
| mollerpol_test_614.evio.0 | four-dataset | 3 | 23.116 | 137.281 | 216709024 | [log](../results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/four-dataset-3.log) |

## Optimization and validation

Change: [EP-005 event-rntuple-layout](../optimizations/EP-005-event-rntuple-layout.md). Implementation and record are committed together; resolve with `git log -- benchmarks/records/EP-005-event-rntuple-layout-2026-10-09.md`.

All 16 runs passed physics/block/unfolded counts, clean ROOT opens, dataset counts and exact histogram entry/bin/error/edge checks. Complete final outputs additionally passed `compare_rntuple_layout.C`: every detector value matches the previous four-dataset backend, accounting for omitted pulse rows and per-event helicity vectors. The single-source fixtures have unique block/event IDs. Small-fixture comparison passed for 1,031 events and 1,017 helicity hits. This establishes a new layout reference; changes in timing include representation and event-ID overhead, not parallel filling.

rsidis_production_28268.dat.0: event median 5.681 kHz (+7.15% vs four-dataset); four-dataset median 5.302 kHz (+0.00% vs four-dataset).

mollerpol_test_614.evio.0: event median 122.123 kHz (-14.88% vs four-dataset); four-dataset median 143.465 kHz (+0.00% vs four-dataset).

Only comparisons within this alternating session support the percentages above; historical sessions show host/emulation variability. ZLIB level 1 and four ROOT IMT threads are fixed. Rates include parsing, filling, compression, writing and finalization. Mounted storage has no explicit fsync. These are amd64-emulated measurements, not native Linux results.

## Provenance

Parent revision: `51f3d0b3faa17512a909943196bbe58bdd6288e0`. Installed plugin SHA-256: `7b8ba63e166136d853f53d4453c67afdfce34b96dc65b75ed9c6bd18505a6c68`. ROOT 6.34.00; Linux-6.10.14-linuxkit-x86_64-with-glibc2.39; 12 visible CPUs. Source and commands were uncommitted during timing.

Source SHA-256:

```json
{
  "src/plugins/evio_processor/InitPlugin.cc": "d671b53644eb8aa08af1e07cc2c20573c94eff95172ce822fc176c09596b2b1e",
  "src/plugins/evio_processor/CMakeLists.txt": "13617e570d586c745ba492dd081fb78580d6feec925cbc9ca7b1c42d599d81c6",
  "src/plugins/evio_processor/EventRootNtuple.h": "ea626b5a1231535262a25e41dac057143cb6cf8119382b4d0b287356317832f6",
  "src/plugins/evio_processor/JEventProcessor_EVIO.h": "79929fc99686f217a48358c8e665849921653e3cbf548c59264729ccc23ebba4",
  "src/plugins/evio_processor/SequentialRootNtuple.h": "37c9e88242aedf6a45a174d78f513aa6bf1c99f59a88f274bf89395c06a3c45d",
  "src/plugins/evio_processor/JEventProcessor_EVIO.cc": "ef34310b0bbab3e65409b7a69ba272b271fea3a96eb80c377f7f43c1c78cf34a",
  "src/plugins/evio_processor/EventRootNtuple.cc": "5109b90bcaa354b59dfef3ab5a1f8673934f84e8d6b185fbbb3f1f68ce1dc17f"
}
```

### rsidis_production_28268.dat.0

Input bytes: 1058422544; SHA-256: `7d0ade12b6dc80218bcc68faeaeb0b95df9f7b967e060040da8b58ea9a27b065`. Exact timed commands:

```json
{
  "four-dataset": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_FORMAT=rntuple",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/four-dataset.root",
    "/workspace/data_files/rsidis_production_28268.dat.0"
  ],
  "event": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_FORMAT=rntuple_event",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T192027.700741Z-EP-005/rsidis_production_28268.dat.0/event.root",
    "/workspace/data_files/rsidis_production_28268.dat.0"
  ]
}
```

### mollerpol_test_614.evio.0

Input bytes: 898415628; SHA-256: `774463627754a9b45180d83472a2ce70dd5735f73bac612966b87d7054f6179d`. Exact timed commands:

```json
{
  "four-dataset": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_FORMAT=rntuple",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/four-dataset.root",
    "/workspace/data_files/mollerpol_test_614.evio.0"
  ],
  "event": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_FORMAT=rntuple_event",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T192027.700741Z-EP-005/mollerpol_test_614.evio.0/event.root",
    "/workspace/data_files/mollerpol_test_614.evio.0"
  ]
}
```

Environment:

```json
{
  "JCE_HOME": "/workspace/jana2-common-extensions/jce-root-stack",
  "JANA_HOME": "/workspace/jana2-common-extensions/jce-root-stack",
  "JANA_PLUGIN_PATH": "/workspace/jana2-common-extensions/jce-root-stack/lib/plugins",
  "LD_LIBRARY_PATH": "/workspace/jana2-common-extensions/jce-root-stack/lib:/opt/root/lib",
  "JCE_CONFIG_DIR": ""
}
```

Local ignored evidence: `benchmarks/results/20261009T192027.700741Z-EP-005/` contains CSV, logs, metadata, per-run validation snapshots, ROOT files and full-comparison logs. Reproduce in the ROOT Docker service with `python3 benchmarks/run_event_rntuple.py --stage layout --deadline <UTC-Unix-deadline>`, then use the corresponding comparator on each final output.
