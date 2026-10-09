# EP-004: sequential RNTuple experiment

## Change

Add opt-in `ROOT_FORMAT=rntuple`; default `ttree` preserves EP-002 behavior.
Both modes fill in the sequential JANA callback and use the same detector hit
loops and buffers. RNTuple fields bind to those buffers directly, avoiding a
second event/vector copy. All four datasets and `h_integral` share one ROOT file.
RNTuple writers finalize before that file closes. The compression algorithm and
level are copied from the TFile, matching the TTree control (ZLIB level 1).
`ROOT_IMT_THREADS` remains available in both modes.

This is a format experiment, not yet worker-parallel filling. RNTuple and TTree
have different storage/reader APIs. The names `waveform_tree`, `pulse_tree`,
`caen1190_tree`, and `m_tree` are retained, but the first four objects are RNTuples
in the new mode. `m_tree`'s 20 helicity leaves become individual uint32 fields;
other payload names/types and fill rules stay the same. Empty waveform/CAEN
entries, zero-integral pulse filtering, last-hit pedestal scalars, per-hit
helicity entries, and the adaptive integral histogram remain unchanged.

Read an RNTuple with `ROOT::Experimental::RNTupleReader::Open(name, path)` on
ROOT 6.34, then `GetView<T>(field)`; do not cast these datasets to TTree.
Enable with `-PROOT_FORMAT=rntuple`; `-PROOT_IMT_THREADS=4` selects the measured
compression-thread hint. ROOT 6.34 exposes this API under Experimental.

## Validation and benchmark

[Benchmark report](../records/EP-004-sequential-rntuple-2026-10-09.md).
Implementation and report are committed together; resolve the revision with
`git log -- benchmarks/optimizations/EP-004-sequential-rntuple.md`.

Small-input exact ordered comparison passed for all four datasets and every
histogram bin/error/edge. `benchmarks/compare_ttree_rntuple.C` compares vectors,
unsigned scalar leaves and signed nhits directly across the two reader APIs.
The full-file runner alternates order, uses one warmup plus three measured runs
per format/input, and checks physics/block/unfolded counts, dataset counts,
histograms, clean ROOT opens and absence of text output. Full payload validation
and the benchmark outcome are recorded in the linked report.

## Reusable pattern

Keep a validated output backend as a same-binary control. Bind the new backend
to existing event buffers and match compression before measuring the storage
format. Validate decoded values across formats rather than file bytes. Establish
this sequential reference before changing worker ownership or output order.

The next parallel step needs an explicit file/layout decision: ROOT 6.34's
RNTupleParallelWriter::Append documentation prohibits accessing the supplied
TFile while fill contexts are active. Four independent parallel writers sharing
one TFile are therefore not assumed safe.

## Sources

Installed ROOT 6.34 headers: RNTupleWriter.hxx, RNTupleModel.hxx, REntry.hxx,
RNTupleWriteOptions.hxx and RNTupleParallelWriter.hxx. ROOT API overview:
https://root.cern/doc/master/classROOT_1_1RNTupleParallelWriter.html
