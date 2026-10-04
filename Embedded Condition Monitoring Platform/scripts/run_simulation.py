#!/usr/bin/env python3
"""Run the deterministic host simulation and save a summary."""
import argparse, json, pathlib, subprocess, sys, time
ROOT=pathlib.Path(__file__).resolve().parents[1]
SCENARIOS=["NORMAL","IMBALANCE","MISALIGNMENT","BEARING_FAULT","OVERLOAD"]
def main():
 p=argparse.ArgumentParser();p.add_argument('--scenario',choices=SCENARIOS,default='NORMAL');p.add_argument('--no-plot',action='store_true');a=p.parse_args();exe=ROOT/'build'/'condition_monitor';exe.parent.mkdir(exist_ok=True)
 sources=[ROOT/'firmware/Core/Src/main.c',ROOT/'firmware/Middleware/dsp/dsp.c',ROOT/'firmware/Middleware/filters/filters.c',ROOT/'firmware/Middleware/buffers/dma_buffer.c',ROOT/'firmware/Middleware/fault_detection/monitor.c',ROOT/'firmware/Middleware/communication/protocol.c',ROOT/'firmware/Simulation/sensor_sim.c',ROOT/'firmware/Drivers/vibration/vibration_sensor.c',ROOT/'firmware/Drivers/temperature/temperature_sensor.c',ROOT/'firmware/Drivers/current/current_sensor.c',ROOT/'firmware/Drivers/microphone/microphone.c']
 includes=['firmware/Core/Inc','firmware/Middleware/dsp','firmware/Middleware/filters','firmware/Middleware/buffers','firmware/Middleware/fault_detection','firmware/Middleware/communication','firmware/Drivers/common','firmware/Drivers/vibration','firmware/Drivers/temperature','firmware/Drivers/current','firmware/Drivers/microphone','firmware/Simulation']
 subprocess.run(['gcc','-std=c11','-O2','-Wall','-Wextra',*[f'-I{item}' for item in includes],*map(str,sources),'-lm','-o',str(exe)],cwd=ROOT,check=True)
 out=subprocess.check_output([str(exe),a.scenario],text=True);print(out,end='');(ROOT/'results').mkdir(exist_ok=True);(ROOT/'results'/'summary.txt').write_text(out)
 if not a.no_plot:
  try:
   import numpy as np
   import matplotlib.pyplot as plt
   fs=10000;n=2048;t=np.arange(n)/fs;freq={'NORMAL':30,'IMBALANCE':30,'MISALIGNMENT':30,'BEARING_FAULT':1800,'OVERLOAD':30}[a.scenario];amp=.12 if a.scenario=='NORMAL' else .6 if a.scenario=='IMBALANCE' else .4
   x=amp*np.sin(2*np.pi*freq*t)
   fig,ax=plt.subplots(2,1,figsize=(9,5));ax[0].plot(t[:400],x[:400]);ax[0].set(xlabel='Time (s)',ylabel='Acceleration (g)',title=a.scenario+' simulated vibration');sp=np.abs(np.fft.rfft(x*np.hanning(n)));hz=np.fft.rfftfreq(n,1/fs);ax[1].plot(hz,sp);ax[1].set(xlabel='Frequency (Hz)',ylabel='Magnitude');fig.tight_layout();fig.savefig(ROOT/'results'/'simulation.png',dpi=140);plt.close(fig)
  except ImportError: print('Plot skipped: install requirements.txt for matplotlib.')
if __name__=='__main__':main()
