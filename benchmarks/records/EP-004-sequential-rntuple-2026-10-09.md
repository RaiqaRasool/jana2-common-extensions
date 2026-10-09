# EP-004: Sequential RNTuple vs EP-002 TTree

One JANA worker; four ROOT IMT threads in both modes; TTree vs sequential RNTuple, alternating pair order; one warm-up and three measured full-file runs per case. Compression matched at ZLIB level 1; mounted workspace, no fsync. End-to-end physics count / whole-command wall time. Validation/hashing excluded. amd64 emulation on ARM.

| Input | Format | Median s | Median kHz | Min–max kHz | Change vs TTree |
|---|---:|---:|---:|---|---:|
| rsidis_production_28268.dat.0 | ttree | 34.143 | 2.447 | 2.338–2.474 | +0.00% |
| rsidis_production_28268.dat.0 | rntuple | 18.284 | 4.570 | 4.532–5.478 | +86.74% |
| mollerpol_test_614.evio.0 | ttree | 52.785 | 60.119 | 59.932–61.217 | +0.00% |
| mollerpol_test_614.evio.0 | rntuple | 23.541 | 134.802 | 134.777–135.466 | +124.23% |

## Individual runs

| Input | Format | Run | Wall s | kHz | ROOT bytes | Log |
|---|---:|---|---:|---:|---:|---|
| rsidis_production_28268.dat.0 | ttree | warmup | 35.218 | 2.373 | 532844538 | [log](../results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/imtttree-warmup.log) |
| rsidis_production_28268.dat.0 | rntuple | warmup | 15.361 | 5.440 | 377305447 | [log](../results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/imtrntuple-warmup.log) |
| rsidis_production_28268.dat.0 | rntuple | 1 | 15.256 | 5.478 | 377305447 | [log](../results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/imtrntuple-1.log) |
| rsidis_production_28268.dat.0 | ttree | 1 | 33.771 | 2.474 | 532847443 | [log](../results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/imtttree-1.log) |
| rsidis_production_28268.dat.0 | ttree | 2 | 34.143 | 2.447 | 532836993 | [log](../results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/imtttree-2.log) |
| rsidis_production_28268.dat.0 | rntuple | 2 | 18.284 | 4.570 | 377305447 | [log](../results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/imtrntuple-2.log) |
| rsidis_production_28268.dat.0 | rntuple | 3 | 18.441 | 4.532 | 377305447 | [log](../results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/imtrntuple-3.log) |
| rsidis_production_28268.dat.0 | ttree | 3 | 35.735 | 2.338 | 532835692 | [log](../results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/imtttree-3.log) |
| mollerpol_test_614.evio.0 | ttree | warmup | 53.963 | 58.806 | 562524773 | [log](../results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/imtttree-warmup.log) |
| mollerpol_test_614.evio.0 | rntuple | warmup | 24.186 | 131.207 | 216708980 | [log](../results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/imtrntuple-warmup.log) |
| mollerpol_test_614.evio.0 | rntuple | 1 | 23.541 | 134.802 | 216708980 | [log](../results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/imtrntuple-1.log) |
| mollerpol_test_614.evio.0 | ttree | 1 | 52.949 | 59.932 | 562526394 | [log](../results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/imtttree-1.log) |
| mollerpol_test_614.evio.0 | ttree | 2 | 51.838 | 61.217 | 562525448 | [log](../results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/imtttree-2.log) |
| mollerpol_test_614.evio.0 | rntuple | 2 | 23.545 | 134.777 | 216708980 | [log](../results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/imtrntuple-2.log) |
| mollerpol_test_614.evio.0 | rntuple | 3 | 23.425 | 135.466 | 216708980 | [log](../results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/imtrntuple-3.log) |
| mollerpol_test_614.evio.0 | ttree | 3 | 52.785 | 60.119 | 562523233 | [log](../results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/imtttree-3.log) |

## Optimization and correctness

Change: [EP-004 sequential RNTuple](../optimizations/EP-004-sequential-rntuple.md).
The implementation and record are committed together; resolve their revision
with `git log -- benchmarks/records/EP-004-sequential-rntuple-2026-10-09.md`.

All 16 runs passed physics/block/unfolded counts, dataset counts, clean ROOT-open
and exact histogram bin/error/edge checks. All logs confirmed four ROOT IMT
threads. The final complete files also matched EVERY ordered payload value in
all four datasets, including all 20 helicity values, using
`benchmarks/compare_ttree_rntuple.C`. All histogram entries/bins/errors/edges
matched. A small positive control passed; the comparator rejected a copy with
its first pulse pedestal_sum deliberately incremented, reporting
`pulse_tree/pedestal_sum entry 0`. The small fixture had 1,031 waveform/pulse/CAEN
rows and 1,017 helicity rows. SIDIS's four dataset counts were 83,565/83,565/
83,565/0; Møller's were 3,173,350 in each dataset.

Compression is matched at ZLIB level 1 (TFile setting 101, passed explicitly to
RNTupleWriteOptions). RNTuple uses different column encodings, page/cluster
layout and reader APIs; this comparison measures that backend change, not
worker-parallel filling. No detector types or hit values were removed.
One JANA worker fills both modes sequentially. The existing TTree default is
retained; RNTuple is opt-in with `ROOT_FORMAT=rntuple`.

Median throughput increased 86.74% on SIDIS and 124.23% on Møller against the
fresh same-binary EP-002 control. Corresponding wall time reductions were
46.45% and 55.40%. RNTuple output sizes were 377,305,447 and 216,708,980 bytes,
roughly 29.2% and 61.5% smaller than TTree. SIDIS RNTuple measured runs ranged
15.256–18.441 seconds; the spread warrants cautious interpretation. Results are
under amd64 emulation on ARM, with mounted storage and no explicit fsync;
these are not native-Linux performance or disk-durability guarantees. Rates
use physics tap counts divided by entire command wall time, not JANA's final
rate. Validation and hashing are outside the timed region.

## Provenance

Parent revision: `14ace43eb71503f8699044643de4ee6431d32ec1`; implementation was uncommitted during timing. Installed plugin SHA-256: `0853daef200b94b71957b4581c18482b2062226cd99ea0162e2812e170ff1f30`. ROOT 6.34.00; Linux-6.10.14-linuxkit-x86_64-with-glibc2.39; 12 visible CPUs.

Benchmarked source SHA-256:

```json
{
  "src/plugins/evio_processor/InitPlugin.cc": "d671b53644eb8aa08af1e07cc2c20573c94eff95172ce822fc176c09596b2b1e",
  "src/plugins/evio_processor/CMakeLists.txt": "bd0d132f55e62eb9bb4a72e1e229448a203b019b7ae31b9f7570716412c8adaf",
  "src/plugins/evio_processor/JEventProcessor_EVIO.h": "871835cdac8e76281255297ca6208daaf9d486269d4a5687704e3c710a2d1edb",
  "src/plugins/evio_processor/SequentialRootNtuple.h": "37c9e88242aedf6a45a174d78f513aa6bf1c99f59a88f274bf89395c06a3c45d",
  "src/plugins/evio_processor/JEventProcessor_EVIO.cc": "71a62002bee08ddc7b17bb1e59efe4070dfc5df8c00aa497cf2547068849d33b"
}
```

### rsidis_production_28268.dat.0

Input bytes: 1058422544; SHA-256: `7d0ade12b6dc80218bcc68faeaeb0b95df9f7b967e060040da8b58ea9a27b065`. Exact timed commands:

```json
{
  "ttree": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_FORMAT=ttree",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/ttree.root",
    "/workspace/data_files/rsidis_production_28268.dat.0"
  ],
  "rntuple": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_FORMAT=rntuple",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T185828.544172Z-EP-004/rsidis_production_28268.dat.0/rntuple.root",
    "/workspace/data_files/rsidis_production_28268.dat.0"
  ]
}
```

### mollerpol_test_614.evio.0

Input bytes: 898415628; SHA-256: `774463627754a9b45180d83472a2ce70dd5735f73bac612966b87d7054f6179d`. Exact timed commands:

```json
{
  "ttree": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_FORMAT=ttree",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/ttree.root",
    "/workspace/data_files/mollerpol_test_614.evio.0"
  ],
  "rntuple": [
    "/workspace/jana2-common-extensions/jce-root-stack/scripts/jce.sh",
    "-PDEFAULT_PLUGINS:FILE=/workspace/jana2-common-extensions/benchmarks/results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/plugins.db",
    "-Pnthreads=1",
    "-PROOT_IMT_THREADS=4",
    "-PROOT_FORMAT=rntuple",
    "-PROOT_OUT_FILENAME=/workspace/jana2-common-extensions/benchmarks/results/20261009T185828.544172Z-EP-004/mollerpol_test_614.evio.0/rntuple.root",
    "/workspace/data_files/mollerpol_test_614.evio.0"
  ]
}
```

Captured environment:

```json
{
  "JCE_HOME": "/workspace/jana2-common-extensions/jce-root-stack",
  "JANA_HOME": "/workspace/jana2-common-extensions/jce-root-stack",
  "JANA_PLUGIN_PATH": "/workspace/jana2-common-extensions/jce-root-stack/lib/plugins",
  "LD_LIBRARY_PATH": "/workspace/jana2-common-extensions/jce-root-stack/lib:/opt/root/lib",
  "JCE_CONFIG_DIR": ""
}
```

Local ignored evidence: `benchmarks/results/20261009T185828.544172Z-EP-004/` (CSV, metadata, logs, output ROOT files and full comparison logs). Small/negative controls: `benchmarks/results/EP-004-small/`.

Reproduction within the ROOT service: `python3 benchmarks/run_rntuple.py --deadline <UTC-Unix-deadline>`, followed by `root -l -b -q 'benchmarks/compare_ttree_rntuple.C("<ttree.root>","<rntuple.root>")'` for each input. The current runner uses clearer format log names; captured commands and raw run logs above are those actually timed.
