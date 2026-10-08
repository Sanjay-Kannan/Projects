import sys, unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "host" / "coordinator"))
from room_coordinator.control import Policy, RoomController

class ControllerTests(unittest.TestCase):
    def setUp(self): self.c = RoomController()
    def test_temperature_hysteresis(self):
        self.assertEqual(self.c.update_temperature(27.0), "off")
        self.assertEqual(self.c.update_temperature(28.0), "low")
        self.assertEqual(self.c.update_temperature(27.0), "low")
        self.assertEqual(self.c.update_temperature(26.0), "off")
    def test_light_hysteresis(self):
        self.assertTrue(self.c.update_illuminance(99.0))
        self.assertTrue(self.c.update_illuminance(120.0))
        self.assertFalse(self.c.update_illuminance(141.0))
    def test_reject_out_of_range(self):
        self.assertIsNone(self.c.update_temperature(200.0))
        self.assertIsNone(self.c.update_illuminance(-1.0))
    def test_bad_hysteresis_rejected(self):
        with self.assertRaises(ValueError): RoomController(Policy(26, 28, 140, 100))

if __name__ == "__main__": unittest.main()
