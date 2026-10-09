# ROOT-only sequential baseline — 2026-10-09

One worker, one warm-up and three measured runs per full input. ROOT-only, unchanged default compression; mounted workspace output, no fsync. Physics events / whole-command wall time including startup and shutdown. Input hashing and validation excluded. linux/amd64 emulation on ARM.

| Input | Physics events | Median wall s | Median kHz | Min–max kHz | ROOT bytes |
|---|---:|---:|---:|---|---:|
| rsidis_production_28268.dat.0 | 83565 | 38.169 | 2.189 | 2.158–2.190 | 532845804 |
| mollerpol_test_614.evio.0 | 3173350 | 52.736 | 60.174 | 59.178–60.274 | 562522821 |

## Individual runs

| Input | Run | Wall s | kHz | Valid | Log |
|---|---|---:|---:|---|---|
| rsidis_production_28268.dat.0 | warmup | 37.705 | 2.216 | True | [log](../results/20261009T163223.239569Z-root-only/rsidis_production_28268.dat.0/processor-warmup.log) |
| rsidis_production_28268.dat.0 | 1 | 38.726 | 2.158 | True | [log](../results/20261009T163223.239569Z-root-only/rsidis_production_28268.dat.0/processor-1.log) |
| rsidis_production_28268.dat.0 | 2 | 38.169 | 2.189 | True | [log](../results/20261009T163223.239569Z-root-only/rsidis_production_28268.dat.0/processor-2.log) |
| rsidis_production_28268.dat.0 | 3 | 38.151 | 2.190 | True | [log](../results/20261009T163223.239569Z-root-only/rsidis_production_28268.dat.0/processor-3.log) |
| mollerpol_test_614.evio.0 | warmup | 52.599 | 60.332 | True | [log](../results/20261009T163223.239569Z-root-only/mollerpol_test_614.evio.0/processor-warmup.log) |
| mollerpol_test_614.evio.0 | 1 | 52.736 | 60.174 | True | [log](../results/20261009T163223.239569Z-root-only/mollerpol_test_614.evio.0/processor-1.log) |
| mollerpol_test_614.evio.0 | 2 | 53.624 | 59.178 | True | [log](../results/20261009T163223.239569Z-root-only/mollerpol_test_614.evio.0/processor-2.log) |
| mollerpol_test_614.evio.0 | 3 | 52.648 | 60.274 | True | [log](../results/20261009T163223.239569Z-root-only/mollerpol_test_614.evio.0/processor-3.log) |

## Optimization reference

This is the baseline for subsequent optimization. Preserve hit types, trees, branches, fill rules, compression, input hashes, and output scope when comparing. The older ROOT-plus-text record is historical and does not isolate optimization gains.

Rebuilt and installed the current processor before measurement. ROOT 6.34.00 on the separate Ubuntu 24.04 amd64 image, C++20 JCE with the accepted prebuilt ROOT C++17 warning. No translation, dump, or experiment plugins; JCE_CONFIG_DIR cleared. One worker, sequential ProcessSequential writing. Outputs overwritten each run; final files remain local.

Source commit: `aa403f4484f13a8e2e8c32772937699cb268540c`. Installed plugin SHA-256: `cdc0409d8c131dd7ff33e13c62a04a71960f84d64743d958d7b78fdfa3a7b07c`.

All eight runs (including warm-ups) passed: downstream physics counts equal unfolder minus source counts, waveform and CAEN1190 entry counts match physics counts, all five ROOT objects exist, ROOT opens without recovery, and no text files are produced. Saved schemas and object entry counts agree across all four runs per input.

## Reproduction

```sh
docker compose -f docker/compose.root.yaml run --rm dev-root python3 benchmarks/run_processor.py
```

Per-input defaults contain exactly `evio_parser,evio_common_modules,evio_processor`. Exact commands and metadata are retained with the logs.

### mollerpol_test_614.evio.0

Input bytes: 898415628; SHA-256: `774463627754a9b45180d83472a2ce70dd5735f73bac612966b87d7054f6179d`.

```json
{
  "waveform_tree": {
    "class": "TTree",
    "entries": 3173350,
    "branches": [
      "slot",
      "chan",
      "waveform",
      "rocid"
    ]
  },
  "pulse_tree": {
    "class": "TTree",
    "entries": 3173350,
    "branches": [
      "integral_sum",
      "pedestal_sum",
      "coarse_time",
      "fine_time",
      "pulse_peak",
      "pedestal_quality",
      "nhits",
      "chan",
      "slot",
      "rocid"
    ]
  },
  "caen1190_tree": {
    "class": "TTree",
    "entries": 3173350,
    "branches": [
      "rocid",
      "slot",
      "chan",
      "measurement",
      "opt",
      "flags",
      "trig_time",
      "hdr_chip_id",
      "hdr_event_id",
      "hdr_bunch_id",
      "trl_status"
    ]
  },
  "h_integral": {
    "class": "TH1I",
    "entries": 16280373,
    "branches": []
  },
  "m_tree": {
    "class": "TTree",
    "entries": 3173350,
    "branches": [
      "heldec"
    ]
  }
}
```

### rsidis_production_28268.dat.0

Input bytes: 1058422544; SHA-256: `7d0ade12b6dc80218bcc68faeaeb0b95df9f7b967e060040da8b58ea9a27b065`.

```json
{
  "waveform_tree": {
    "class": "TTree",
    "entries": 83565,
    "branches": [
      "slot",
      "chan",
      "waveform",
      "rocid"
    ]
  },
  "caen1190_tree": {
    "class": "TTree",
    "entries": 83565,
    "branches": [
      "rocid",
      "slot",
      "chan",
      "measurement",
      "opt",
      "flags",
      "trig_time",
      "hdr_chip_id",
      "hdr_event_id",
      "hdr_bunch_id",
      "trl_status"
    ]
  },
  "pulse_tree": {
    "class": "TTree",
    "entries": 83565,
    "branches": [
      "integral_sum",
      "pedestal_sum",
      "coarse_time",
      "fine_time",
      "pulse_peak",
      "pedestal_quality",
      "nhits",
      "chan",
      "slot",
      "rocid"
    ]
  },
  "h_integral": {
    "class": "TH1I",
    "entries": 4991770,
    "branches": []
  },
  "m_tree": {
    "class": "TTree",
    "entries": 0,
    "branches": [
      "heldec"
    ]
  }
}
```

Raw logs, measurements CSV, commands, schemas, environment metadata, and final ROOT files: `benchmarks/results/20261009T163223.239569Z-root-only/` (Git-ignored local artifacts).
