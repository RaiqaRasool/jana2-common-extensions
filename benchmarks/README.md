# ROOT writing review checkpoints

These branches contain only evio_processor changes and ROOT benchmark support
relative to origin/main at 56500d8. The existing project history is unchanged;
evio_parser_dump implementation and commits are excluded from these branches.

- review/ep-004-rntuple: sequential four-dataset RNTuple (original 51f3d0b).
- review/ep-005-event-rntuple: one event-oriented dataset (original f63c36d).
- review/ep-006-parallel-rntuple: parked parallel experiment (original 82caac5).

The review branches build incrementally. Plugin source at each checkpoint is
identical to its original checkpoint. Benchmarks were measured on those original
revisions, not rerun on these review branches; retain their recorded source and
binary hashes, runtime settings, and validation limits when interpreting rates.

Load only evio_parser, evio_common_modules, and evio_processor for ROOT timing.
ROOT_FORMAT defaults to ttree. Select rntuple for EP-004's opt-in backend; later
checkpoints add rntuple_event and ROOT_RNTUPLE_PARALLEL respectively. See the
plugin README and numbered optimization/benchmark records.

Median is the middle of three measured full-file rates. SIDIS and Møller name
the two inputs. All recorded timings use linux/amd64 emulation. EP-004 is the
recommended simple backend. EP-006 is unfinished: large-file payload validation
was not performed, and its provisional rates did not improve the best serial
configuration. Its presence is for review, not a recommendation to enable it.
