"""EP-002: alternate ROOT compression off/four threads, preserving one JANA worker."""
import argparse
import csv
import datetime
import json
import os
from pathlib import Path
import platform
import statistics
import subprocess
import sys
import time
from run import parse_physics_events, select_inputs
from run_processor import digest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--deadline', type=float, required=True, help='UTC Unix deadline; reserve 60 seconds for shutdown')
    args = parser.parse_args()
    if not Path('/.dockerenv').exists():
        sys.exit('Run in the ROOT Docker service')
    import ROOT
    root = Path(__file__).resolve().parent.parent
    home = Path(os.environ['JCE_HOME'])
    sources = select_inputs(root, [])
    out = root/'benchmarks/results'/(datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S.%fZ')+'-EP-002')
    out.mkdir(parents=True)
    env = dict(os.environ, JCE_CONFIG_DIR='')
    meta = dict(optimization='EP-002', commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),
                status=subprocess.check_output(['git','status','--short'],cwd=root,text=True),
                plugin_sha256=digest(home/'lib/plugins/evio_processor.so'), root_version=ROOT.gROOT.GetVersion(),
                platform=platform.platform(), cpus=os.cpu_count(), jana_threads=1, root_threads=[0,4],
                warmups=1, repetitions=3, deadline=args.deadline, environment={k:env.get(k,'') for k in ('JCE_HOME','JANA_HOME','JANA_PLUGIN_PATH','LD_LIBRARY_PATH','JCE_CONFIG_DIR')})
    (out/'metadata.json').write_text(json.dumps(meta,indent=2)+'\n')
    rows=[]
    def save():
        if rows:
            with (out/'results.csv').open('w',newline='') as f:
                writer=csv.DictWriter(f,fieldnames=list(rows[0]));writer.writeheader();writer.writerows(rows)
    for source in sources:
        folder=out/source.name
        folder.mkdir()
        defaults=folder/'plugins.db'
        defaults.write_text('evio_parser,evio_common_modules,evio_processor\n')
        commands={}
        for threads in (0,4):
            commands[threads]=[str(home/'scripts/jce.sh'),f'-PDEFAULT_PLUGINS:FILE={defaults}','-Pnthreads=1',f'-PROOT_IMT_THREADS={threads}',f'-PROOT_OUT_FILENAME={folder / (str(threads)+".root")}',str(source)]
        (folder/'metadata.json').write_text(json.dumps(dict(input=str(source),bytes=source.stat().st_size,sha256=digest(source),commands=commands),indent=2)+'\n')
        reference=None
        for iteration in range(4):
            label=str(iteration) if iteration else 'warmup'
            # Reverse each pair to limit fixed-order effects.
            for threads in ((0,4) if iteration%2==0 else (4,0)):
                log=folder/f'imt{threads}-{label}.log'
                print(f'{source.name}: IMT {threads}, {label}',flush=True)
                remaining=args.deadline-time.time()-60
                if remaining<=0:
                    sys.exit(f'Deadline reached; partial results: {out}')
                start=time.perf_counter()
                try:
                    with log.open('w') as f:
                        result=subprocess.run(commands[threads],cwd=folder,env=env,stdout=f,stderr=subprocess.STDOUT,timeout=remaining)
                except subprocess.TimeoutExpired:
                    save()
                    sys.exit(f'Deadline stopped the run; partial results: {out}')
                wall=time.perf_counter()-start
                output=folder/f'{threads}.root'
                row=dict(input=source.name,root_threads=threads,run=label,wall_seconds=wall,events=None,blocks=None,unfolded=None,rate_hz=None,root_bytes=output.stat().st_size if output.exists() else 0,exit_code=result.returncode,ok=False,error='',log=str(log.relative_to(out)))
                try:
                    assert result.returncode==0, f'Exit {result.returncode}'
                    content=log.read_text(errors='replace')
                    if threads:
                        assert 'ROOT implicit multithreading enabled for branch compression: 4 threads' in content,'ROOT pool not confirmed'
                    events,blocks,unfolded=parse_physics_events(content,'translation_disabled')
                    row.update(events=events,blocks=blocks,unfolded=unfolded,rate_hz=events/wall)
                    handle=ROOT.TFile.Open(str(output))
                    assert handle and not handle.IsZombie(), 'Invalid ROOT file'
                    try:
                        assert not handle.TestBit(ROOT.TFile.kRecovered), 'ROOT recovered'
                        schema={key.GetName():dict(classname=key.GetClassName(),entries=int(handle.Get(key.GetName()).GetEntries()),branches=[(b.GetName(),b.GetTitle(),b.GetClassName()) for b in handle.Get(key.GetName()).GetListOfBranches()] if key.GetClassName()=='TTree' else []) for key in handle.GetListOfKeys()}
                        assert set(schema)=={'waveform_tree','pulse_tree','caen1190_tree','m_tree','h_integral'}
                        for name in ('waveform_tree','caen1190_tree'):
                            assert schema[name]['entries']==events, f'{name} count mismatch'
                        hist=handle.Get('h_integral')
                        schema['h_integral']['bins']=[(hist.GetBinContent(i),hist.GetBinError(i)) for i in range(hist.GetNcells())]
                        schema['h_integral']['edges']=[hist.GetXaxis().GetBinLowEdge(i) for i in range(1,hist.GetNbinsX()+2)]
                        if reference is None:
                            reference=schema
                        assert schema==reference, 'Schema, count or histogram mismatch'
                        (folder/f'schema-imt{threads}-{label}.json').write_text(json.dumps(schema,indent=2)+'\n')
                    finally:
                        handle.Close()
                    assert not list(folder.glob('*.txt')), 'Unexpected text output'
                    row['ok']=True
                except Exception as exc:
                    row['error']=str(exc)
                rows.append(row);save();print(json.dumps(row),flush=True)
                if not row['ok']:
                    sys.exit(f'Validation failed; stopping for investigation: {out}')
    lines=['# EP-002: ROOT internal parallel compression','','One JANA worker; ROOT IMT disabled vs four threads, alternating pair order; one warm-up and three measured full-file runs per case. Default compression settings unchanged; mounted workspace, no fsync. End-to-end physics count / whole-command wall time. Validation/hashing excluded. amd64 emulation on ARM.','','| Input | ROOT threads | Median s | Median kHz | Min–max kHz | Change vs disabled |','|---|---:|---:|---:|---|---:|']
    for source in sources:
        selected=[r for r in rows if r['input']==source.name and r['run']!='warmup']
        base=statistics.median(r['rate_hz'] for r in selected if r['root_threads']==0)
        for threads in (0,4):
            runs=[r for r in selected if r['root_threads']==threads]
            rates=[r['rate_hz']/1000 for r in runs]
            lines.append(f"| {source.name} | {threads} | {statistics.median(r['wall_seconds'] for r in runs):.3f} | {statistics.median(rates):.3f} | {min(rates):.3f}–{max(rates):.3f} | {(statistics.median(rates)*1000/base-1)*100:+.2f}% |")
    lines+=['','## Individual runs','','| Input | ROOT threads | Run | Wall s | kHz | ROOT bytes | Log |','|---|---:|---|---:|---:|---:|---|']
    for r in rows:
        lines.append(f"| {r['input']} | {r['root_threads']} | {r['run']} | {r['wall_seconds']:.3f} | {r['rate_hz']/1000:.3f} | {r['root_bytes']} | [log]({r['log']}) |")
    (out/'report.md').write_text('\n'.join(lines)+'\n')
    print('\n'.join(lines),flush=True);print(f'Results: {out}',flush=True)


if __name__=='__main__':
    main()
