"""Benchmark the ROOT-only processor in the separate ROOT Docker service."""
import csv
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import statistics
import subprocess
import sys
import time
from run import parse_physics_events, select_inputs


def digest(path):
    h = hashlib.sha256()
    with path.open('rb') as f:
        for chunk in iter(lambda: f.read(8*1024*1024), b''):
            h.update(chunk)
    return h.hexdigest()


def main():
    if not Path('/.dockerenv').exists():
        sys.exit('Run in docker/compose.root.yaml dev-root')
    import ROOT
    root = Path(__file__).resolve().parent.parent
    home = Path(os.environ['JCE_HOME'])
    sources = select_inputs(root, sys.argv[1:])
    out = root/'benchmarks/results'/(datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S.%fZ')+'-root-only')
    out.mkdir(parents=True)
    env = dict(os.environ, JCE_CONFIG_DIR='')
    meta = dict(commit=subprocess.check_output(['git','rev-parse','HEAD'], cwd=root, text=True).strip(),
                status=subprocess.check_output(['git','status','--short'], cwd=root, text=True),
                platform=platform.platform(), cpu_count=os.cpu_count(), root_version=ROOT.gROOT.GetVersion(),
                plugin_sha256=digest(home/'lib/plugins/evio_processor.so'), threads=1, warmups=1, repetitions=3,
                environment={k:env.get(k,'') for k in ('JCE_HOME','JANA_HOME','JANA_PLUGIN_PATH','LD_LIBRARY_PATH','JCE_CONFIG_DIR')})
    (out/'metadata.json').write_text(json.dumps(meta, indent=2)+'\n')
    rows = []
    for source in sources:
        folder = out/source.name
        folder.mkdir()
        defaults = folder/'plugins.db'
        defaults.write_text('evio_parser,evio_common_modules,evio_processor\n')
        output = folder/'output.root'
        command = [str(home/'scripts/jce.sh'), f'-PDEFAULT_PLUGINS:FILE={defaults}', '-Pnthreads=1', f'-PROOT_OUT_FILENAME={output}', str(source)]
        (folder/'metadata.json').write_text(json.dumps(dict(input=str(source), bytes=source.stat().st_size, sha256=digest(source), command=command), indent=2)+'\n')
        for iteration in range(4):
            label = str(iteration) if iteration else 'warmup'
            log = folder/f'processor-{label}.log'
            print(f'{source.name}: {label}', flush=True)
            start = time.perf_counter()
            with log.open('w') as f:
                result = subprocess.run(command, cwd=folder, env=env, stdout=f, stderr=subprocess.STDOUT)
            wall = time.perf_counter()-start
            row = dict(input=source.name, run=label, wall_seconds=wall, events=None, blocks=None, unfolded=None, rate_hz=None, root_bytes=output.stat().st_size if output.exists() else 0, exit_code=result.returncode, ok=False, error='', log=str(log.relative_to(out)))
            try:
                if result.returncode:
                    raise ValueError(f'Exit {result.returncode}')
                events, blocks, unfolded = parse_physics_events(log.read_text(errors='replace'), 'translation_disabled')
                row.update(events=events, blocks=blocks, unfolded=unfolded, rate_hz=events/wall)
                handle = ROOT.TFile.Open(str(output))
                if not handle or handle.IsZombie():
                    raise ValueError('Invalid ROOT output')
                try:
                    assert not handle.TestBit(ROOT.TFile.kRecovered), 'ROOT recovered'
                    for name in ('waveform_tree','caen1190_tree'):
                        tree = handle.Get(name)
                        assert tree and tree.GetEntries()==events, f'{name} count mismatch'
                    for name in ('pulse_tree','m_tree','h_integral'):
                        assert handle.Get(name), f'Missing {name}'
                    schema = {key.GetName(): {'class': key.GetClassName(), 'entries': int(handle.Get(key.GetName()).GetEntries()), 'branches': [b.GetName() for b in handle.Get(key.GetName()).GetListOfBranches()] if key.GetClassName()=='TTree' else []} for key in handle.GetListOfKeys()}
                    (folder/f'schema-{label}.json').write_text(json.dumps(schema, indent=2)+'\n')
                finally:
                    handle.Close()
                assert not list(folder.glob('*.txt')), 'Unexpected text output'
                row['ok'] = True
            except Exception as exc:
                row['error'] = str(exc)
            rows.append(row)
            with (out/'results.csv').open('w', newline='') as f:
                writer = csv.DictWriter(f, fieldnames=list(row)); writer.writeheader(); writer.writerows(rows)
            print(json.dumps(row), flush=True)
    lines = ['# ROOT-only sequential baseline', '', 'One worker, one warm-up and three measured runs per full input. ROOT-only, unchanged default compression; mounted workspace output, no fsync. Physics events / whole-command wall time including startup and shutdown. Input hashing and validation excluded. linux/amd64 emulation on ARM.', '', '| Input | Physics events | Median wall s | Median kHz | Min–max kHz | ROOT bytes |', '|---|---:|---:|---:|---|---:|']
    for source in sources:
        runs = [r for r in rows if r['input']==source.name and r['run']!='warmup' and r['ok']]
        if len(runs)!=3 or len({r['events'] for r in runs})!=1:
            sys.exit(f'Invalid baseline; inspect {out}')
        rates = [r['rate_hz']/1000 for r in runs]
        lines.append(f"| {source.name} | {runs[0]['events']} | {statistics.median(r['wall_seconds'] for r in runs):.3f} | {statistics.median(rates):.3f} | {min(rates):.3f}–{max(rates):.3f} | {runs[-1]['root_bytes']} |")
    lines += ['', '## Individual runs', '', '| Input | Run | Wall s | kHz | Valid | Log |', '|---|---|---:|---:|---|---|']
    for r in rows:
        rate = f"{r['rate_hz']/1000:.3f}" if r['rate_hz'] is not None else '—'
        lines.append(f"| {r['input']} | {r['run']} | {r['wall_seconds']:.3f} | {rate} | {r['ok']} | [log]({r['log']}) |")
    (out/'report.md').write_text('\n'.join(lines)+'\n')
    print('\n'.join(lines), flush=True)
    print(f'Results: {out}', flush=True)
    return int(any(not r['ok'] for r in rows))


if __name__=='__main__':
    sys.exit(main())
