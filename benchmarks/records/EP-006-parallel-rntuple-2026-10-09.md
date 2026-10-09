# EP-006: stopped parallel RNTuple experiment

**Stopped at the user’s request on 2026-10-09. Not a validated performance baseline.**

## Final conclusion

The substantial validated improvement is EP-004 sequential RNTuple: +86.74% SIDIS and +124.23% Møller against its fresh TTree controls. Retain that simple backend. EP-005 separately validates an event-oriented layout, with +7.15% SIDIS and -14.88% Møller in its own session. Further parallel-writing complexity is not justified by the observations on this setup.

EP-006 completed 38 of 40 timing runs, all passing physics/block/unfolded counts, clean file open, dataset counts, and exact histogram checks. The third Møller parallel-1 run was interrupted; the final sequential run was not started. No full-file event-keyed payload comparisons or negative-control test were run. The build and small fixture’s exact 1/2/4-worker event-keyed comparisons passed for all 49 fields. The large-file timing observations below remain provisional.

[Optimization and parked implementation](../optimizations/EP-006-parallel-rntuple.md). The code is saved on `wip/ep-006-parallel-rntuple`; main retains the last fully validated implementation at EP-005.

| Input | Case | Measured runs completed | Median kHz |
|---|---|---:|---:|
| rsidis_production_28268.dat.0 | sequential | 3/3 | 5.682 |
| rsidis_production_28268.dat.0 | parallel-1 | 3/3 | 2.489 |
| rsidis_production_28268.dat.0 | parallel-2 | 3/3 | 4.254 |
| rsidis_production_28268.dat.0 | serial-4 | 3/3 | 6.963 |
| rsidis_production_28268.dat.0 | parallel-4 | 3/3 | 6.236 |
| mollerpol_test_614.evio.0 | sequential | 2/3 | 118.684 |
| mollerpol_test_614.evio.0 | parallel-1 | 2/3 | 81.713 |
| mollerpol_test_614.evio.0 | parallel-2 | 3/3 | 114.795 |
| mollerpol_test_614.evio.0 | serial-4 | 3/3 | 56.181 |
| mollerpol_test_614.evio.0 | parallel-4 | 3/3 | 75.451 |

SIDIS’s four-worker parallel writer was about 10.4% slower than its four-worker serial control. Møller’s parallel writer improved over the four-worker serial control, but all parallel cases trailed the one-worker serial observations. More threads did not improve the best overall rate. These are whole-pipeline rates, not isolated writer microbenchmarks.

## Conditions and limitations

Alternating forward/reverse case order, one warmup plus three planned measured runs per case/input. ZLIB level 1, global ROOT IMT pool configured to four threads, JANA workers varied by case. ROOT 6.34’s parallel writer lacks the newer explicit IMT-on option: compression scheduling differs between writer APIs despite identical algorithm/level and global pool settings. See the linked optimization record. Physics tap counts divided by entire command wall time; validation/hashing excluded. linux/amd64 emulation on ARM, mounted storage, no explicit fsync.

## Completed runs

| Input | Case | Run | Wall s | kHz | ROOT bytes | Log |
|---|---|---|---:|---:|---:|---|
| rsidis_production_28268.dat.0 | sequential | warmup | 17.679 | 4.727 | 377440533 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/sequential-warmup.log) |
| rsidis_production_28268.dat.0 | parallel-1 | warmup | 36.919 | 2.263 | 377440313 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-1-warmup.log) |
| rsidis_production_28268.dat.0 | parallel-2 | warmup | 19.693 | 4.243 | 377698779 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-2-warmup.log) |
| rsidis_production_28268.dat.0 | serial-4 | warmup | 12.043 | 6.939 | 377579552 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/serial-4-warmup.log) |
| rsidis_production_28268.dat.0 | parallel-4 | warmup | 12.310 | 6.788 | 378057710 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-4-warmup.log) |
| rsidis_production_28268.dat.0 | parallel-4 | 1 | 13.120 | 6.369 | 377995500 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-4-1.log) |
| rsidis_production_28268.dat.0 | serial-4 | 1 | 11.430 | 7.311 | 377589862 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/serial-4-1.log) |
| rsidis_production_28268.dat.0 | parallel-2 | 1 | 19.645 | 4.254 | 377576913 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-2-1.log) |
| rsidis_production_28268.dat.0 | parallel-1 | 1 | 33.568 | 2.489 | 377440313 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-1-1.log) |
| rsidis_production_28268.dat.0 | sequential | 1 | 14.708 | 5.682 | 377440533 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/sequential-1.log) |
| rsidis_production_28268.dat.0 | sequential | 2 | 14.472 | 5.774 | 377440533 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/sequential-2.log) |
| rsidis_production_28268.dat.0 | parallel-1 | 2 | 33.562 | 2.490 | 377440313 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-1-2.log) |
| rsidis_production_28268.dat.0 | parallel-2 | 2 | 23.411 | 3.569 | 377655460 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-2-2.log) |
| rsidis_production_28268.dat.0 | serial-4 | 2 | 15.853 | 5.271 | 377585048 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/serial-4-2.log) |
| rsidis_production_28268.dat.0 | parallel-4 | 2 | 14.957 | 5.587 | 378050532 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-4-2.log) |
| rsidis_production_28268.dat.0 | parallel-4 | 3 | 13.401 | 6.236 | 378058090 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-4-3.log) |
| rsidis_production_28268.dat.0 | serial-4 | 3 | 12.002 | 6.963 | 377542614 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/serial-4-3.log) |
| rsidis_production_28268.dat.0 | parallel-2 | 3 | 19.173 | 4.358 | 377745942 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-2-3.log) |
| rsidis_production_28268.dat.0 | parallel-1 | 3 | 34.016 | 2.457 | 377440313 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-1-3.log) |
| rsidis_production_28268.dat.0 | sequential | 3 | 14.982 | 5.578 | 377440533 | [log](../results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/sequential-3.log) |
| mollerpol_test_614.evio.0 | sequential | warmup | 27.052 | 117.305 | 218354551 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/sequential-warmup.log) |
| mollerpol_test_614.evio.0 | parallel-1 | warmup | 40.145 | 79.047 | 218354309 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-1-warmup.log) |
| mollerpol_test_614.evio.0 | parallel-2 | warmup | 28.763 | 110.328 | 221041384 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-2-warmup.log) |
| mollerpol_test_614.evio.0 | serial-4 | warmup | 55.949 | 56.718 | 219323355 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/serial-4-warmup.log) |
| mollerpol_test_614.evio.0 | parallel-4 | warmup | 40.639 | 78.087 | 223921517 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-4-warmup.log) |
| mollerpol_test_614.evio.0 | parallel-4 | 1 | 41.177 | 77.067 | 223925541 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-4-1.log) |
| mollerpol_test_614.evio.0 | serial-4 | 1 | 56.485 | 56.181 | 219307982 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/serial-4-1.log) |
| mollerpol_test_614.evio.0 | parallel-2 | 1 | 27.532 | 115.261 | 221073786 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-2-1.log) |
| mollerpol_test_614.evio.0 | parallel-1 | 1 | 38.548 | 82.321 | 218354309 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-1-1.log) |
| mollerpol_test_614.evio.0 | sequential | 1 | 26.513 | 119.689 | 218354551 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/sequential-1.log) |
| mollerpol_test_614.evio.0 | sequential | 2 | 26.966 | 117.679 | 218354551 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/sequential-2.log) |
| mollerpol_test_614.evio.0 | parallel-1 | 2 | 39.126 | 81.105 | 218354309 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-1-2.log) |
| mollerpol_test_614.evio.0 | parallel-2 | 2 | 27.644 | 114.795 | 221045465 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-2-2.log) |
| mollerpol_test_614.evio.0 | serial-4 | 2 | 55.524 | 57.153 | 219302258 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/serial-4-2.log) |
| mollerpol_test_614.evio.0 | parallel-4 | 2 | 42.161 | 75.267 | 223935627 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-4-2.log) |
| mollerpol_test_614.evio.0 | parallel-4 | 3 | 42.059 | 75.451 | 223934360 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-4-3.log) |
| mollerpol_test_614.evio.0 | serial-4 | 3 | 57.045 | 55.629 | 219313976 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/serial-4-3.log) |
| mollerpol_test_614.evio.0 | parallel-2 | 3 | 28.495 | 111.365 | 221213331 | [log](../results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-2-3.log) |

## Provenance

Parent revision, exact source hashes, installed binary hash and environment:

```json
{
  "optimization": "EP-006",
  "stage": "parallel",
  "parent_commit": "f63c36ddaab30aaa4d6c5cda6757f1832e29e8fb",
  "plugin_sha256": "efe5ea5a2d5517213804caef5b2a26907ba58168b6c9d60bcc7fc834d12afd87",
  "source_sha256": {
    "src/plugins/evio_processor/InitPlugin.cc": "d671b53644eb8aa08af1e07cc2c20573c94eff95172ce822fc176c09596b2b1e",
    "src/plugins/evio_processor/CMakeLists.txt": "13617e570d586c745ba492dd081fb78580d6feec925cbc9ca7b1c42d599d81c6",
    "src/plugins/evio_processor/EventRootNtuple.h": "c753f399e4083c7fe3a2e392e54117ef4dbd167b32da19ac94c997d8aeefea78",
    "src/plugins/evio_processor/JEventProcessor_EVIO.h": "94d491142c6cfedf5cc9f6974314fc38f4ec575c8b0966580b04bd7ea150257f",
    "src/plugins/evio_processor/SequentialRootNtuple.h": "37c9e88242aedf6a45a174d78f513aa6bf1c99f59a88f274bf89395c06a3c45d",
    "src/plugins/evio_processor/JEventProcessor_EVIO.cc": "6fc12f3aaba9c1dc1cdc22ee73a7498e05ca742f883b172e0c4241a28fb690ae",
    "src/plugins/evio_processor/EventRootNtuple.cc": "2d116a98abc03e2d4f2508a4321d776ccb713a5755b4160cdffc4c229cd8b48f"
  },
  "root_version": "6.34.00",
  "platform": "Linux-6.10.14-linuxkit-x86_64-with-glibc2.39",
  "cpus": 12,
  "cases": [
    [
      "sequential",
      "rntuple_event",
      1,
      false
    ],
    [
      "parallel-1",
      "rntuple_event",
      1,
      true
    ],
    [
      "parallel-2",
      "rntuple_event",
      2,
      true
    ],
    [
      "serial-4",
      "rntuple_event",
      4,
      false
    ],
    [
      "parallel-4",
      "rntuple_event",
      4,
      true
    ]
  ],
  "root_imt_threads": 4,
  "compression": "ZLIB 1",
  "warmups": 1,
  "repetitions": 3,
  "deadline": 1791576124.0,
  "environment": {
    "JCE_HOME": "/workspace/jana2-common-extensions/jce-root-stack",
    "JANA_HOME": "/workspace/jana2-common-extensions/jce-root-stack",
    "JANA_PLUGIN_PATH": "/workspace/jana2-common-extensions/jce-root-stack/lib/plugins",
    "LD_LIBRARY_PATH": "/workspace/jana2-common-extensions/jce-root-stack/lib:/opt/root/lib",
    "JCE_CONFIG_DIR": ""
  }
}
```

### mollerpol_test_614.evio.0

Input hash and exact timed commands:

```json
{
  "input": "/workspace/data_files/mollerpol_test_614.evio.0",
  "bytes": 898415628,
  "sha256": "774463627754a9b45180d83472a2ce70dd5735f73bac612966b87d7054f6179d",
  "commands": {
    "sequential": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/plugins.db",
      "-Pnthreads=1",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=0",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/sequential.root",
      "/workspace/data_files/mollerpol_test_614.evio.0"
    ],
    "parallel-1": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/plugins.db",
      "-Pnthreads=1",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=1",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-1.root",
      "/workspace/data_files/mollerpol_test_614.evio.0"
    ],
    "parallel-2": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/plugins.db",
      "-Pnthreads=2",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=1",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-2.root",
      "/workspace/data_files/mollerpol_test_614.evio.0"
    ],
    "serial-4": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/plugins.db",
      "-Pnthreads=4",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=0",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/serial-4.root",
      "/workspace/data_files/mollerpol_test_614.evio.0"
    ],
    "parallel-4": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/plugins.db",
      "-Pnthreads=4",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=1",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/mollerpol_test_614.evio.0/parallel-4.root",
      "/workspace/data_files/mollerpol_test_614.evio.0"
    ]
  }
}
```

### rsidis_production_28268.dat.0

Input hash and exact timed commands:

```json
{
  "input": "/workspace/data_files/rsidis_production_28268.dat.0",
  "bytes": 1058422544,
  "sha256": "7d0ade12b6dc80218bcc68faeaeb0b95df9f7b967e060040da8b58ea9a27b065",
  "commands": {
    "sequential": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/plugins.db",
      "-Pnthreads=1",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=0",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/sequential.root",
      "/workspace/data_files/rsidis_production_28268.dat.0"
    ],
    "parallel-1": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/plugins.db",
      "-Pnthreads=1",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=1",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-1.root",
      "/workspace/data_files/rsidis_production_28268.dat.0"
    ],
    "parallel-2": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/plugins.db",
      "-Pnthreads=2",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=1",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-2.root",
      "/workspace/data_files/rsidis_production_28268.dat.0"
    ],
    "serial-4": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/plugins.db",
      "-Pnthreads=4",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=0",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/serial-4.root",
      "/workspace/data_files/rsidis_production_28268.dat.0"
    ],
    "parallel-4": [
      "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
      "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/plugins.db",
      "-Pnthreads=4",
      "-PROOT_FORMAT=rntuple_event",
      "-PROOT_IMT_THREADS=4",
      "-PROOT_RNTUPLE_PARALLEL=1",
      "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T193157.403532Z-EP-006/rsidis_production_28268.dat.0/parallel-4.root",
      "/workspace/data_files/rsidis_production_28268.dat.0"
    ]
  }
}
```

Local ignored evidence: `benchmarks/results/20261009T193157.403532Z-EP-006/`; small checks: `benchmarks/results/EP-006-small/`. The interrupted container has stopped. No benchmark will resume automatically.

The installed build artifacts may still contain the parked experimental plugin. Rebuild/install main before using its baseline; switching Git branches does not change generated binaries.
