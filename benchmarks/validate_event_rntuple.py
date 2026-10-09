"""Run exact full-output comparisons after timing, with a shared hard deadline."""
import argparse
import json
import os
from pathlib import Path
import signal
import subprocess
import time


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('results',type=Path)
    parser.add_argument('--deadline',type=float,required=True)
    args=parser.parse_args()
    root=Path(__file__).resolve().parent.parent
    out=args.results.resolve()
    meta=json.loads((out/'metadata.json').read_text())
    layout=meta['stage']=='layout'
    for folder in sorted(out.iterdir()):
        if not folder.is_dir() or not (folder/'metadata.json').exists():continue
        before=folder/('four-dataset.root' if layout else 'sequential.root')
        targets=['event'] if layout else [case[0] for case in meta['cases'][1:]]
        macro='compare_rntuple_layout' if layout else 'compare_event_rntuple'
        for target in targets:
            remaining=args.deadline-time.time()-30
            if remaining<=0:raise SystemExit('Deadline reached before comparison')
            print(f'{folder.name}: exact comparison {target}',flush=True)
            expression=f'benchmarks/{macro}.C({json.dumps(str(before))},{json.dumps(str(folder/(target+".root")))})'
            with (folder/(target+'-full-compare.log')).open('w') as log:
                process=subprocess.Popen(['root','-l','-b','-q',expression],cwd=root,stdout=log,stderr=subprocess.STDOUT,start_new_session=True)
                try:code=process.wait(timeout=remaining)
                except subprocess.TimeoutExpired:
                    os.killpg(process.pid,signal.SIGKILL);process.wait()
                    raise SystemExit('Deadline stopped full-output comparison')
            if code:raise SystemExit(f'Comparison failed ({code}): {folder}/{target}-full-compare.log')
    (out/'full-comparison-passed.txt').write_text('Every final output passed the exact full-payload comparison.\n')
    print('All complete output comparisons passed',flush=True)

if __name__=='__main__':main()
