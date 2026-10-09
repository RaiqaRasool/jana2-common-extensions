# EP-006: worker-local RNTuple fill contexts — parked

## Change

Add opt-in `ROOT_RNTUPLE_PARALLEL=1`, valid only with
`ROOT_FORMAT=rntuple_event`. The default remains serial and the previous output
modes remain available. The parallel writer uses exactly the EP-005 event model,
compression, identity fields and histogram. It permits reordered entries.

One RNTupleParallelWriter coordinates the single events dataset. Each JANA
worker lazily creates a fill context, its own EventRootRecord and a bare REntry
bound directly to the record's buffers. Models use CreateBare. A mutex protects
context registration only; normal event filling uses a thread-local weak cache
and ROOT's internal synchronization. No mutable event buffers are shared.

The ExpertMode ProcessParallel callback collects/serializes event data. The
original adaptive histogram still fills in ProcessSequential and is detached
from TFile while contexts are active. Finish flushes every context, destroys
all contexts, finalizes the parallel writer, then writes/deletes the histogram
and closes the file. Weak caches cannot keep a context alive after Finish or
reuse stale ownership if a processor address is recycled.

Enable with `-PROOT_FORMAT=rntuple_event -PROOT_RNTUPLE_PARALLEL=1 -Pnthreads=4`.
`ROOT_IMT_THREADS` configures ROOT's process-global pool hint; four is
configured throughout the benchmark. ROOT 6.34's parallel-writer compression
strategy differs from the serial writer's use of that pool. Account for both JANA workers and ROOT threads.
Reading and event identity follow EP-005; order across worker contexts is not
guaranteed. ROOT 6.34 exposes this writer under ROOT::Experimental.

## Verification and measurements

[Benchmark report](../records/EP-006-parallel-rntuple-2026-10-09.md).
Implementation is parked on `wip/ep-006-parallel-rntuple`. The user stopped
the experiment before full validation; it is not promoted to main.

The small fixture passed exact event-keyed comparisons for 1/2/4 JANA workers:
all 49 fields, identities, and histogram entries/bins/errors/edges match the
EP-005 serial output. There are 1,031 distinct events and 1,017 helicity hits.
`compare_event_rntuple.C` sorts exact identities, rejects duplicates, builds a
row mapping, then compares decoded values directly without payload hashes.
It reads in target order to retain locality across merged worker clusters.

The full benchmark compares serial filling with 1 and 4 JANA workers and
parallel filling with 1, 2 and 4 workers. The serial-4 control isolates the
writer change at the same JANA worker count; comparison with serial-1 includes
parallelism in parser/pipeline stages too. Four ROOT IMT threads and ZLIB level
1 are fixed, with one warmup and three measured full-file runs per case/input.
The user stopped after 38 of 40 timing runs. Full event-keyed comparisons
of the large output files were NOT run. The partial outcomes are recorded
in the report and are not a fully validated performance baseline.

## Reusable pattern

Prefer the output library's native parallel API. Own buffers and bound entries
per worker, let the library coordinate storage, and make lifetime ordering
explicit. Keep other ROOT objects out of the file while contexts fill. Retain
both a same-layout sequential control and a same-worker-count control; avoid
attributing all pipeline scaling to the writer. Validate by stable identity,
not incidental entry order, and report workloads that regress.

## API evidence

Installed ROOT 6.34 RNTupleParallelWriter.hxx documents thread-safe context
creation, cloned models, indeterminate cross-context order, and destruction of
all contexts before the writer. Its Append call states the supplied TFile must
not be accessed while contexts fill. RNTupleFillContext.hxx provides
CreateBareEntry through GetModel, Fill and FlushCluster. See also
https://root.cern/doc/master/classROOT_1_1RNTupleParallelWriter.html.

## Version-specific compression caveat

The installed ROOT 6.34 RNTupleWriteOptions::EImplicitMT has only kOff and
kDefault, with no explicit kOn. ROOT 6.38 release notes list #18398, allowing IMT
with RNTupleParallelWriter; newer API documentation provides explicit kOn and
notes that parallel writers default to compression within fill contexts.
Thus four globally enabled IMT threads do not establish identical compression
thread scheduling across the two 6.34 writer APIs. Algorithm and level remain
matched, but this measures the complete native writing strategy, not isolated
parallel Fill overhead with identical compression execution. No ROOT upgrade
or unsupported API workaround is included in this change.

Reference: https://root.cern/doc/v638/release-notes.html

## Final decision

Stop optimizing on this setup. Keep the validated sequential RNTuple backend
(EP-004), with EP-005 retained as a separately validated event-layout reference.
The parallel experiment builds and small 1/2/4-worker checks passed, but partial
full-file timings did not beat the best sequential configuration on either
input. SIDIS favored four JANA workers with serial filling. Møller favored one
JANA worker with serial filling. These are observations on amd64 emulation;
they do not establish native-Linux scaling or a universal limitation of RNTuple.

No further optimization, ROOT upgrade, or benchmark is scheduled. Revisit only
for an observed throughput requirement or a different runtime/hardware.
