"""Small matplotlib viewer for generated scenario CSVs."""
import csv,pathlib,sys
import matplotlib.pyplot as plt
p=pathlib.Path(__file__).resolve().parents[2]/'simulation'/'generated_data'/(sys.argv[1].lower()+'.csv')
with p.open() as f: rows=list(csv.DictReader(f))
t=[float(r['time_s']) for r in rows];v=[float(r['vibration_g']) for r in rows];temp=[float(r['temperature_c']) for r in rows];cur=[float(r['current_a']) for r in rows]
fig,ax=plt.subplots(3,1,figsize=(9,7));ax[0].plot(t,v);ax[0].set_ylabel('Vibration (g)');ax[1].plot(t,temp);ax[1].set_ylabel('Temperature (C)');ax[2].plot(t,cur);ax[2].set_ylabel('Current (A)');ax[2].set_xlabel('Time (s)');fig.suptitle(sys.argv[1]+' simulated data');fig.tight_layout();plt.show()
