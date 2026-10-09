# EP-005: one event-oriented RNTuple

## Change and contract

Add opt-in `ROOT_FORMAT=rntuple_event`, retaining TTree as default and the
EP-004 four-dataset `rntuple` mode as a control. One `events` RNTuple stores one
row per physics event alongside the original `h_integral` histogram. Sequential
filling and ZLIB level 1 compression are retained. Four ROOT IMT threads are
used in the benchmark, matching the control.

Fields have waveform_, pulse_, caen1190_, and helicity_ prefixes. The waveform
and CAEN vectors retain every old value and hit/sample order. Pulse vectors
retain nonzero-integral hits and last-hit pedestal scalars; events with no
qualifying pulses now have an empty vector and pulse_nhits=0 instead of an
omitted pulse row. Each helicity field becomes a vector with one item per hit,
so an event with zero/multiple hits remains a single event row. No detector
payload is removed. This layout requires adapted readers.

Add source_file/run_number/block_number/event_number identification to each
row. The block number comes from the parent Block JEvent, the event number from
the physics JEvent. Source identity is its resource path; repeated use of the
same resource needs external disambiguation. Standard fixtures use one input
source. IDs and reordered entries were approved by the user before this work.

EventRootRecord owns the event-local vectors and exposes their field bindings;
its Load method retrieves optional typed hits directly from the supplied event.
This provides the same record for subsequent worker-local fill contexts, without
shared mutable branch buffers. This step still uses RNTupleWriter sequentially.

## Validation and outcome

[Benchmark report](../records/EP-005-event-rntuple-layout-2026-10-09.md).
Implementation and record are committed together; find the revision with
`git log -- benchmarks/optimizations/EP-005-event-rntuple-layout.md`.

`compare_rntuple_layout.C` compares every vector/scalar across the four old
RNTuples and the new event dataset, accounts for omitted pulse rows, flattens
helicity vectors back to old hit rows, compares histograms exactly, and rejects
duplicate block/event IDs within the single-source fixtures. The small input
passed with 1,031 unique event IDs and 1,017 helicity hits. Full-file validation
and timing outcomes are in the linked report.

## Reusable pattern

Establish one event record with explicit identity before parallel filling.
Validate a layout change separately from concurrency. Bind buffers directly,
represent optional hit collections as vectors, and retain the prior backend as
an independently readable control. Use one parallel writer for one dataset;
do not assume independent writers sharing a TFile synchronize with one another.

Installed ROOT 6.34 RNTupleParallelWriter.hxx documents that Append's supplied
TFile must not be accessed while fill contexts are active. Keeping the histogram
writes until after dataset finalization avoids this overlap in the next step.
