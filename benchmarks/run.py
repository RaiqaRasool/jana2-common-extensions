"""Run the initial comparison inside Docker using the installed stack."""
import argparse, csv, datetime, json, os, re, shlex, statistics, subprocess, sys, time
from pathlib import Path

INPUTS = ('rsidis_production_28268.dat.0', 'mollerpol_test_614.evio.0')

CASES = {"parsing": "evio_parser,evio_common_modules", "translation_disabled": "evio_parser,evio_common_modules,detector_translation"}

def parse_physics_events(log, case):
    log = re.sub(r"\x1b\[[0-9;]*m", "", log)
    stages = {}
    current = None
    for line in log.splitlines():
        name = re.search(r"Arrow name:\s+(\S+)", line)
        if name:
            current = name.group(1)
        count = re.search(r"Events completed:\s+([\d,]+)", line)
        if count and current:
            if current in stages:
                raise ValueError(f'Duplicate stage metrics: {current}')
            stages[current] = int(count.group(1).replace(',', ''))
    if 'BlockSource' not in stages or 'PhysicsEventUnfold' not in stages:
        raise ValueError('Missing block-source or unfolder metrics')
    blocks = stages['BlockSource']
    unfolded = stages['PhysicsEventUnfold']
    children = unfolded - blocks
    if children <= 0:
        raise ValueError('Nonpositive physics-event count')
    if case == 'translation_disabled':
        if 'PhysicsEventTap' not in stages:
            raise ValueError('Missing downstream physics-stage metrics')
        if stages['PhysicsEventTap'] != children:
            raise ValueError('Downstream physics count disagrees with unfolder minus blocks')
        children = stages['PhysicsEventTap']
    return children, blocks, unfolded

def report(rows):
    lines = ["# Core pipeline comparison", "", "End-to-end rate = physics events / whole-command wall seconds, including startup and shutdown. Counts come from stage diagnostics; JANA final counts and rates are not used.", "", "| Case | Successful runs | Event counts | Median kHz | Min–max kHz | Median wall seconds | Change vs parsing |", "|---|---:|---|---:|---|---:|---:|"]
    rates = [r['rate_hz'] for r in rows if r['case'] == 'parsing' and r['ok']]
    base = statistics.median(rates) if rates else None
    for case in CASES:
        runs = [r for r in rows if r['case'] == case and r['ok']]
        if not runs:
            lines.append(f"| {case} | 0/3 | — | — | — | — | — |")
            continue
        rates = [r['rate_hz']/1000 for r in runs]
        median = statistics.median(rates)
        counts = ', '.join(str(n) for n in sorted({r['events'] for r in runs}))
        delta = f"{(median*1000/base-1)*100:+.1f}%" if base else '—'
        lines.append(f"| {case} | {len(runs)}/3 | {counts} | {median:.3f} | {min(rates):.3f}–{max(rates):.3f} | {statistics.median(r['wall_seconds'] for r in runs):.3f} | {delta} |")
    warnings = []
    if len({r['events'] for r in rows if r['events'] is not None}) > 1:
        warnings.append('EVENT COUNT MISMATCH: counts should agree; rates remain visible. Interpret differences with caution.')
    if any(not r['ok'] for r in rows):
        warnings.append('FAILED RUN OR INVALID STAGE METRICS: medians include successful runs only; inspect logs.')
    if any(r['case'] == 'translation_disabled' and not r['no_catalogs_warning'] for r in rows):
        warnings.append('NO-CATALOG CONDITION UNCONFIRMED: expected warning missing; inspect logs.')
    lines += ['', *('🔴 '+w for w in warnings), '', '## Individual measured runs', '', '| Case | Run | Physics events | End-to-end kHz | Blocks | Unfolder outputs | Wall seconds | Exit | Log |', '|---|---:|---:|---:|---:|---:|---:|---:|---|']
    for r in rows:
        rate = f"{r['rate_hz']/1000:.3f}" if r['rate_hz'] is not None else '—'
        lines.append(f"| {r['case']} | {r['run']} | {r['events']} | {rate} | {r['block_events']} | {r['unfolder_outputs']} | {r['wall_seconds']:.3f} | {r['exit_code']} | [{r['log']}]({r['log']}) |")
    return '\n'.join(lines)+'\n', warnings

def run_input(root, source, out, wrapper):
    home = os.environ['JCE_HOME']
    out.mkdir(parents=True)
    env = dict(os.environ, JCE_CONFIG_DIR='')
    commands = {}
    for case, plugins in CASES.items():
        defaults = out/f'{case}.db'
        defaults.write_text(plugins+'\n')
        commands[case] = [str(wrapper), f'-PDEFAULT_PLUGINS:FILE={defaults}', '-Pnthreads=1', str(source)]
    metadata = dict(commands=commands, input=str(source), input_bytes=source.stat().st_size, threads=1, repetitions=3, warmups=1, rate_metric='physics events / whole-command wall seconds', JCE_HOME=home, JCE_CONFIG_DIR='', environment={k: env.get(k,'') for k in ('JANA_HOME','JANA_PLUGIN_PATH','LD_LIBRARY_PATH')})
    (out/'metadata.json').write_text(json.dumps(metadata, indent=2)+'\n')
    rows = []
    for iteration in range(4):
        for case, command in commands.items():
            label = str(iteration) if iteration else 'warmup'
            logpath = out/f'{case}-{label}.log'
            print(f'{case} {label}: {shlex.join(command)}', flush=True)
            start = time.perf_counter()
            with logpath.open('w') as stream:
                result = subprocess.run(command, cwd=root, env=env, stdout=stream, stderr=subprocess.STDOUT)
            duration = time.perf_counter()-start
            log = logpath.read_text(errors='replace')
            error = ''
            try:
                count, blocks, unfolded = parse_physics_events(log, case)
            except ValueError as exc:
                count = blocks = unfolded = None
                error = str(exc)
                print(f'{case} {label}: {error}', flush=True)
            rate = count / duration if count is not None and duration > 0 else None
            if not iteration:
                if result.returncode:
                    print(f'Warm-up failed; continuing; see {logpath}', flush=True)
                continue
            rows.append(dict(case=case, run=iteration, events=count, rate_hz=rate, block_events=blocks, unfolder_outputs=unfolded, count_error=error, wall_seconds=duration, exit_code=result.returncode, ok=result.returncode==0 and count is not None and rate is not None, no_catalogs_warning='No detector mapping catalogs are registered' in log, log=logpath.name))
            with (out/'results.csv').open('w', newline='') as stream:
                writer = csv.DictWriter(stream, fieldnames=list(rows[0]))
                writer.writeheader(); writer.writerows(rows)
    text, warnings = report(rows)
    (out/'report.md').write_text(text)
    print(text)
    if sys.stdout.isatty():
        for warning in warnings:
            print('\033[31m'+warning+'\033[0m')
    print(f'Saved results: {out}')
    return rows, text, warnings

def select_inputs(root, paths):
    sources = [Path(path).resolve() for path in paths] if paths else [root.parent/'data_files'/name for name in INPUTS]
    names = [source.name for source in sources]
    if len(names) != len(set(names)):
        raise ValueError('Input filenames must be distinct because each gets a named results folder.')
    missing = [str(source) for source in sources if not source.is_file()]
    if missing:
        raise ValueError('Missing input files: ' + ', '.join(missing) + '\nSupply input paths: python3 benchmarks/run.py /workspace/path/to/file.evio [more files ...]')
    return sources

def main():
    parser = argparse.ArgumentParser(description='Compare end-to-end physics-event rates. Input data is not bundled or downloaded.')
    parser.add_argument('inputs', nargs='*', help='Container input paths; defaults to the two reference files under /workspace/data_files/')
    args = parser.parse_args()
    if not Path('/.dockerenv').exists():
        sys.exit('Run inside the Docker dev container; host execution is disabled.')
    root = Path(__file__).resolve().parent.parent
    home = os.environ.get('JCE_HOME')
    if not home:
        sys.exit('Set JCE_HOME to the existing container installation.')
    wrapper = Path(home)/'scripts/jce.sh'
    try:
        sources = select_inputs(root, args.inputs)
    except ValueError as exc:
        parser.error(str(exc))
    for path in [wrapper]:
        if not path.is_file():
            sys.exit(f'Missing prerequisite: {path}')
    out = root/'benchmarks/results'/datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S.%fZ')
    sections = ['# Standard input comparisons', '', 'Each input has its own baseline; rates are not pooled across files.', '']
    failed = False
    for source in sources:
        rows, text, warnings = run_input(root, source, out/source.name, wrapper)
        failed |= any(not row['ok'] for row in rows)
        # Prefix relative log links for the combined report.
        for row in rows:
            text = text.replace(f"]({row['log']})", f"]({source.name}/{row['log']})")
        sections += [f'## {source.name}', '', text.replace('# Core pipeline comparison', '### Core pipeline comparison', 1).replace('## Individual measured runs', '#### Individual measured runs')]
        (out/'report.md').write_text('\n'.join(sections))
    print('\n'.join(sections))
    print(f'Combined report: {out / "report.md"}')
    return int(failed)

if __name__ == '__main__':
    sys.exit(main())
