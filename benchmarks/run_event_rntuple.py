"""Matched event-layout or parallel-fill benchmark in the isolated ROOT service."""
import argparse
import csv
import datetime
import json
import os
from pathlib import Path
import platform
import statistics
import subprocess
import time
from run import parse_physics_events, select_inputs
from run_processor import digest


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--stage',choices=['layout','parallel'],required=True)
    parser.add_argument('--deadline',type=float,required=True)
    args=parser.parse_args()
    assert Path('/.dockerenv').exists(), 'Run in the ROOT Docker service'
    import ROOT
    root=Path(__file__).resolve().parent.parent
    home=Path(os.environ['JCE_HOME'])
    optimization='EP-005' if args.stage=='layout' else 'EP-006'
    cases=[('four-dataset','rntuple',1,False),('event','rntuple_event',1,False)] if args.stage=='layout' else [('sequential','rntuple_event',1,False),('parallel-1','rntuple_event',1,True),('parallel-2','rntuple_event',2,True),('serial-4','rntuple_event',4,False),('parallel-4','rntuple_event',4,True)]
    out=root/'benchmarks/results'/(datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S.%fZ')+'-'+optimization)
    out.mkdir(parents=True)
    env=dict(os.environ,JCE_CONFIG_DIR='')
    meta=dict(optimization=optimization,stage=args.stage,parent_commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),plugin_sha256=digest(home/'lib/plugins/evio_processor.so'),source_sha256={str(p.relative_to(root)):digest(p) for p in (root/'src/plugins/evio_processor').iterdir() if p.suffix in ('.h','.cc') or p.name=='CMakeLists.txt'},root_version=ROOT.gROOT.GetVersion(),platform=platform.platform(),cpus=os.cpu_count(),cases=cases,root_imt_threads=4,compression='ZLIB 1',warmups=1,repetitions=3,deadline=args.deadline,environment={k:env.get(k,'') for k in ('JCE_HOME','JANA_HOME','JANA_PLUGIN_PATH','LD_LIBRARY_PATH','JCE_CONFIG_DIR')})
    (out/'metadata.json').write_text(json.dumps(meta,indent=2)+'\n')
    rows=[]
    def save():
        if rows:
            with (out/'results.csv').open('w',newline='') as f:
                writer=csv.DictWriter(f,fieldnames=list(rows[0]));writer.writeheader();writer.writerows(rows)
    for source in select_inputs(root,[]):
        folder=out/source.name;folder.mkdir()
        defaults=folder/'plugins.db';defaults.write_text('evio_parser,evio_common_modules,evio_processor\n')
        commands={name:[str(home/'scripts/jce.sh'),f'-PDEFAULT_PLUGINS:FILE={defaults}',f'-Pnthreads={workers}',f'-PROOT_FORMAT={storage}','-PROOT_IMT_THREADS=4',*([f'-PROOT_RNTUPLE_PARALLEL={int(parallel)}'] if args.stage=='parallel' else []),f'-PROOT_OUT_FILENAME={folder/(name+".root")}',str(source)] for name,storage,workers,parallel in cases}
        (folder/'metadata.json').write_text(json.dumps(dict(input=str(source),bytes=source.stat().st_size,sha256=digest(source),commands=commands),indent=2)+'\n')
        reference_hist=None;reference_counts=None
        for iteration in range(4):
            order=cases if iteration%2==0 else list(reversed(cases))
            for name,storage,workers,parallel in order:
                label=str(iteration) if iteration else 'warmup'
                print(f'{source.name}: {name}, {label}',flush=True)
                remaining=args.deadline-time.time()-60
                if remaining<=0:raise SystemExit(f'Deadline reached: {out}')
                log=folder/f'{name}-{label}.log';output=folder/(name+'.root')
                start=time.perf_counter()
                try:
                    with log.open('w') as f: result=subprocess.run(commands[name],cwd=folder,env=env,stdout=f,stderr=subprocess.STDOUT,timeout=remaining)
                except subprocess.TimeoutExpired:
                    save();raise SystemExit(f'Deadline stopped run: {out}')
                wall=time.perf_counter()-start
                row=dict(input=source.name,case=name,jana_workers=workers,parallel=parallel,run=label,wall_seconds=wall,events=0,blocks=0,unfolded=0,rate_hz=0,root_bytes=output.stat().st_size if output.exists() else 0,ok=False,error='',log=str(log.relative_to(out)))
                try:
                    assert result.returncode==0,f'Exit {result.returncode}'
                    content=log.read_text(errors='replace')
                    assert 'ROOT implicit multithreading enabled for branch compression: 4 threads' in content,'ROOT pool not confirmed'
                    events,blocks,unfolded=parse_physics_events(content,'translation_disabled')
                    row.update(events=events,blocks=blocks,unfolded=unfolded,rate_hz=events/wall)
                    counts=(events,blocks,unfolded)
                    if reference_counts is None:reference_counts=counts
                    assert counts==reference_counts,'Physics counts changed'
                    handle=ROOT.TFile.Open(str(output))
                    assert handle and not handle.IsZombie() and not handle.TestBit(ROOT.TFile.kRecovered),'Invalid/recovered ROOT file'
                    try:
                        names=('waveform_tree','pulse_tree','caen1190_tree','m_tree') if storage=='rntuple' else ('events',)
                        entries={n:int(ROOT.Experimental.RNTupleReader.Open(n,str(output)).GetNEntries()) for n in names}
                        assert entries[names[0]]==events,'Dataset count mismatch'
                        if storage=='rntuple':assert entries['caen1190_tree']==events
                        hist=handle.Get('h_integral');assert hist,'Missing histogram'
                        histogram=dict(entries=hist.GetEntries(),bins=[(hist.GetBinContent(i),hist.GetBinError(i)) for i in range(hist.GetNcells())],edges=[hist.GetXaxis().GetBinLowEdge(i) for i in range(1,hist.GetNbinsX()+2)])
                        if reference_hist is None:reference_hist=histogram
                        assert histogram==reference_hist,'Histogram mismatch'
                        (folder/f'validation-{name}-{label}.json').write_text(json.dumps(dict(datasets=entries,histogram=histogram),indent=2)+'\n')
                    finally:handle.Close()
                    assert not list(folder.glob('*.txt')),'Unexpected text output'
                    row['ok']=True
                except Exception as exc:row['error']=str(exc)
                rows.append(row);save();print(json.dumps(row),flush=True)
                if not row['ok']:raise SystemExit(f'Validation failed; stop for investigation: {out}')
    lines=[f'# {optimization}: {args.stage} benchmark','','Four ROOT IMT threads and ZLIB level 1 in all cases. Alternating forward/reverse case order, one warmup plus three measured full-file runs per case/input. Physics tap counts / entire command wall time; validation/hashing excluded. amd64 emulation on ARM, mounted workspace, no explicit fsync.','','| Input | Case | Median s | Median kHz | Min–max kHz | Change vs first case |','|---|---|---:|---:|---|---:|']
    for source in select_inputs(root,[]):
        selected=[r for r in rows if r['input']==source.name and r['run']!='warmup']
        base=statistics.median(r['rate_hz'] for r in selected if r['case']==cases[0][0])
        for name,*_ in cases:
            runs=[r for r in selected if r['case']==name];rates=[r['rate_hz']/1000 for r in runs]
            lines.append(f"| {source.name} | {name} | {statistics.median(r['wall_seconds'] for r in runs):.3f} | {statistics.median(rates):.3f} | {min(rates):.3f}–{max(rates):.3f} | {(statistics.median(rates)*1000/base-1)*100:+.2f}% |")
    lines+=['','## Individual runs','','| Input | Case | Run | Wall s | kHz | ROOT bytes | Log |','|---|---|---|---:|---:|---:|---|']
    for r in rows:lines.append(f"| {r['input']} | {r['case']} | {r['run']} | {r['wall_seconds']:.3f} | {r['rate_hz']/1000:.3f} | {r['root_bytes']} | [log]({r['log']}) |")
    (out/'report.md').write_text('\n'.join(lines)+'\n')
    print('\n'.join(lines),flush=True);print(f'Results: {out}',flush=True)

if __name__=='__main__':main()
