# Testing

## Automated host tests

Run `python -m unittest discover -s tests -v`. The suite exercises fan and light hysteresis, invalid measurement ranges, and threshold configuration. It does not emulate the radio or STM32 HAL.

## Physical bring-up

Check I2C addresses and sensor values, PWM duty at the LED and fan outputs, ZCL endpoint/cluster discovery, measurement reports, On/Off/Level/Fan mode commands, node behavior after coordinator interruption, and recovery after a node reset. Physical testing requires the coordinator, three NUCLEO boards, sensors, fan driver, USB host, and RF environment.
