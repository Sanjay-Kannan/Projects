import pathlib, subprocess, sys, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class PipelineTests(unittest.TestCase):
 @classmethod
 def setUpClass(cls): subprocess.run([sys.executable,str(ROOT/'scripts'/'run_simulation.py'),'--no-plot'],check=True,capture_output=True,text=True)
 def run_case(self,name): return subprocess.check_output([str(ROOT/'build'/'condition_monitor'),name],text=True)
 def test_all_faults_detected(self):
  for s in ['NORMAL','IMBALANCE','MISALIGNMENT','BEARING_FAULT','OVERLOAD']:
   with self.subTest(scenario=s): self.assertIn('fault='+s,self.run_case(s))
 def test_fft_peak_is_shaft_rate(self): self.assertIn('dominant_hz=29.',self.run_case('IMBALANCE'))
 def test_sensor_driver_lifecycle_and_ranges(self):
  exe=ROOT/'build'/'test_drivers'
  src=[ROOT/'tests'/'test_drivers.c',ROOT/'firmware'/'Simulation'/'sensor_sim.c']
  for sensor in ['vibration','temperature','current','microphone']:
   src.append(ROOT/'firmware'/'Drivers'/sensor/({'microphone':'microphone.c'}.get(sensor,sensor+'_sensor.c')))
  includes=['firmware/Drivers/common','firmware/Drivers/vibration','firmware/Drivers/temperature','firmware/Drivers/current','firmware/Drivers/microphone','firmware/Simulation']
  subprocess.run(['gcc','-std=c11','-Wall','-Wextra',*[f'-I{item}' for item in includes],*map(str,src),'-lm','-o',str(exe)],cwd=ROOT,check=True)
  subprocess.run([str(exe)],check=True)
 def test_buffers_filters_and_protocol(self):
  exe=ROOT/'build'/'test_middleware'
  src=[ROOT/'tests'/'test_middleware.c',ROOT/'firmware'/'Middleware'/'buffers'/'dma_buffer.c',ROOT/'firmware'/'Middleware'/'filters'/'filters.c',ROOT/'firmware'/'Middleware'/'communication'/'protocol.c',ROOT/'firmware'/'Middleware'/'dsp'/'dsp.c']
  includes=['firmware/Middleware/buffers','firmware/Middleware/filters','firmware/Middleware/communication','firmware/Middleware/dsp']
  subprocess.run(['gcc','-std=c11','-Wall','-Wextra',*[f'-I{item}' for item in includes],*map(str,src),'-lm','-o',str(exe)],cwd=ROOT,check=True)
  subprocess.run([str(exe)],check=True)
if __name__=='__main__':unittest.main()
