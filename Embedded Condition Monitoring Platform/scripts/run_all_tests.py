#!/usr/bin/env python3
"""Build firmware host core and execute deterministic system checks."""
import pathlib, subprocess, sys
root=pathlib.Path(__file__).resolve().parents[1]
checks=[('Build', [sys.executable,str(root/'scripts'/'run_simulation.py'),'--no-plot']),('System tests',[sys.executable,'-m','unittest','discover','-s','tests','-v']),('Dataset generation',[sys.executable,str(root/'scripts'/'generate_dataset.py')])]
status=[]
for label,cmd in checks:
 print('\n'+label+':',flush=True);r=subprocess.run(cmd,cwd=root);status.append((label,r.returncode==0))
print('\n==============================\nSYSTEM VERIFICATION SUMMARY\n==============================')
for name,ok in status:print(f'{name:24} {"PASS" if ok else "FAIL"}')
sys.exit(0 if all(ok for _,ok in status) else 1)
