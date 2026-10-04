#!/usr/bin/env python3
"""Generate reproducible CSV datasets for each operating scenario."""
import csv, math, pathlib, random
ROOT=pathlib.Path(__file__).resolve().parents[1];OUT=ROOT/'simulation'/'generated_data';OUT.mkdir(parents=True,exist_ok=True)
for name in ['NORMAL','IMBALANCE','MISALIGNMENT','BEARING_FAULT','OVERLOAD']:
 rng=random.Random(2026);fs=1000;n=2000;rpm=1800;f0=rpm/60
 with (OUT/(name.lower()+'.csv')).open('w',newline='') as fp:
  w=csv.writer(fp);w.writerow(['time_s','vibration_g','temperature_c','current_a','acoustic'])
  for i in range(n):
   t=i/fs;amp={'NORMAL':.12,'IMBALANCE':.6,'MISALIGNMENT':.4,'BEARING_FAULT':.18,'OVERLOAD':.45}[name];v=amp*math.sin(2*math.pi*f0*t)+rng.gauss(0,.015)
   if name=='MISALIGNMENT':v+=.3*math.sin(4*math.pi*f0*t)
   if name=='BEARING_FAULT':v+=.35*math.sin(2*math.pi*180*t)
   temp=42+(18 if name=='OVERLOAD' else 0)*min(t/2,1);cur=.9+(2.2 if name=='OVERLOAD' else 0)+.1*math.sin(2*math.pi*2*t)
   w.writerow([f'{t:.4f}',f'{v:.6f}',f'{temp:.3f}',f'{cur:.4f}',f'{abs(v)*.25:.6f}'])
print(f'Generated five deterministic datasets in {OUT}')
