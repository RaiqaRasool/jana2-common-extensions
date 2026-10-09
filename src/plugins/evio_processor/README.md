# evio_processor Plugin

The `evio_processor` consumes the typed hit objects produced by `evio_parser` and writes them to a ROOT file as TTrees and histograms. Debugging CSV output is provided by `evio_parser_dump` and `detector_translation_dump`.

This plugin is optional because it introduces the project's ROOT dependency.
Enable it at configure time with `-DJCE_BUILD_EVIO_PROCESSOR=ON`; the default
JCE build does not require ROOT or install this plugin.

The processor consumes common hardware hit types only. Compton-specific
inputs and `compton_tree` output are excluded; those belong in a downstream
experiment plugin.

The plugin operates at the **physics event level** — it receives individual `JEvent`s that have already been unfolded by `JEventUnfolder_EVIO` and contain fully decoded detector hits.

---

## Table of Contents

- [Architecture](#architecture)
- [Plugin Initialization](#plugin-initialization)
- [Output Files](#output-files)
- [Data Flow](#data-flow)
- [Configuration Parameters](#configuration-parameters)
- [Example Usage](#example-usage)

---

## Architecture

```
evio_parser plugin
    └─ JEventUnfolder_EVIO
            │  (physics-level JEvents with typed hits)
            ▼
evio_processor plugin
    └─ JEventProcessor_EVIO
            ├─ ProcessSequential(event)
            │       ├─ reads FADC250WaveformHit objects
            │       ├─ reads FADC250PulseHit objects
            │       ├─ reads HelicityDecoderData objects
            │       ├─ fills ROOT TTrees
            │       └─ fills ROOT histograms
            └─ Finish()
                    └─ writes and closes ROOT file
```

### `JEventProcessor_EVIO`

The single processor class in this plugin. It:

- Uses `CallbackStyle::ExpertMode` and `ProcessSequential` to serialize tree filling and preserve existing entry order. ROOT can optionally compress branch baskets in parallel.
- Declares all hit inputs as **optional** with `SetOptional(true)`. This means the processor will not throw an error if a given hit type is absent from an event (e.g. an event with no waveform data).
- Copies waveform samples directly from input hits into the ROOT branch buffers.
- Uses JANA2's typed `Input<T>` mechanism to retrieve hits from the `JEvent` by type.

---

## Plugin Initialization

`InitPlugin.cc` registers `JEventProcessor_EVIO` with the JANA2 application:

```cpp
extern "C" {
    void InitPlugin(JApplication* app) {
        InitJANAPlugin(app);
        app->Add(new JEventProcessor_EVIO());
    }
}
```

No additional services or parsers are registered by this plugin — it relies entirely on objects inserted into `JEvent` by `evio_parser`.

## Output Files

### ROOT file (`evio_processor.root` by default)

| Object | Class | Description |
|---|---|---|
| `waveform_tree` | `TTree` | FADC250 raw waveform samples per event |
| `pulse_tree` | `TTree` | FADC250 pulse analysis data per event |
| `caen1190_tree` | `TTree` | CAEN1190 TDC data per event |
| `m_tree` | `TTree` | Helicity decoder data |
| `h_integral` | `TH1I` | Distribution of `FADC250PulseHit::integral_sum` values |

---

## Data Flow

This plugin is the **consumer end** of the pipeline:

```
evio_parser                             evio_processor
──────────────────────────────────      ──────────────────────────────
ModuleParser_FADC::parse()
  → EventHits_FADC::insertIntoEvent()
      → event.Insert(waveforms)    →→→  m_waveform_hits_in()   → waveform_tree
      → event.Insert(pulses)       →→→  m_pulse_hits_in()       → pulse_tree, h_integral
ModuleParser_HelicityDecoder::parse()
  → event.Insert(helicity)         →→→  m_heldec_data_in()      → m_tree
...
```

All hit types consumed by this processor are declared in `JEventProcessor_EVIO.h` as `Input<T>` members. JANA2 resolves them by type at event processing time.

---

## Configuration Parameters

All parameters are set on the JANA2 command line with `-P<name>=<value>`.

| Parameter | Default | `is_shared` | Description |
|---|---|---|---|
| `ROOT_OUT_FILENAME` | `evio_processor.root` | yes | Path/name of the ROOT output file |
| `ROOT_FORMAT` | `ttree` | yes | `ttree` or `rntuple`; RNTuple uses a different reading API |
| `ROOT_IMT_THREADS` | `0` | yes | ROOT implicit-MT thread hint for parallel compression; 0 leaves ROOT unchanged |

---

## Example Usage

Using the JCE wrapper ([`jce.sh`](../../../scripts/jce.sh) or [`jce.csh`](../../../scripts/jce.csh); see [Basic usage](../../../README.md#basic-usage)):

```tcsh
"${JCE_HOME}/scripts/jce.sh" -Pplugins=evio_processor data.evio
```

Produces only `evio_processor.root` in the current directory.

### Custom ROOT output filename

```tcsh
"${JCE_HOME}/scripts/jce.sh" -Pplugins=evio_processor -PROOT_OUT_FILENAME=run_042.root data.evio
```

### With filtering and custom mapping

```tcsh
"${JCE_HOME}/scripts/jce.sh" -Pplugins=evio_processor -PFILTER:ENABLE=1 -PFILTER:FILE="${JCE_HOME}/config/filter.db" -PBANKMAP:FILE="${JCE_HOME}/config/mapping.db" -PROOT_OUT_FILENAME=run_042_filtered.root data.evio
```

## Internal parallel compression

Set `-PROOT_IMT_THREADS=4` to enable ROOT's internal multithreading before
creating the output trees. Tree filling stays sequential; existing schemas,
entry order, fill rules, compression algorithm, and compression level stay the
same. ROOT schedules compression work across branches where supported. This is
not worker-local tree writing or `TBufferMerger`.

The default, 0, leaves ROOT's global state unchanged. A positive value requires
a ROOT build with `imt` support; otherwise initialization fails. ROOT's pool is
process-global, the count is a hint, and an already-enabled pool may be reused.
The plugin logs the actual pool size and does not disable a pool other plugins
may use. Account for both JANA workers and ROOT threads when sizing a job.

Benchmark this option on the target machine; small baskets and emulation may
make scheduling overhead exceed any benefit. See EP-002 under
`benchmarks/optimizations/` for correctness checks and measured results.

## Sequential RNTuple experiment

Set `-PROOT_FORMAT=rntuple` to store the same detector payloads as four RNTuples
in the same `.root` file. Filling remains sequential. Dataset names and event
selection rules are retained; `m_tree` exposes its 20 helicity values as separate
uint32 fields instead of a composite `heldec` branch. `h_integral` remains TH1I.
Compression uses the TFile's algorithm and level, matching the TTree mode.

RNTuples are not TTrees: read them using
`ROOT::Experimental::RNTupleReader::Open("waveform_tree", "evio_processor.root")`
and typed `GetView<T>("field")` calls in ROOT 6.34. Existing TTree casts and branch
reading code require adaptation. Default `ROOT_FORMAT=ttree` retains the existing
output contract. See EP-004 in `benchmarks/optimizations/` for validation and the
matched-compression benchmark before choosing a backend.
