# Smart fan end device

Endpoint 1, Home Automation profile 0x0104. Servers: Basic, Identify, On/Off (0x0006), and Fan Control (0x0202). The supported speed modes are off, low, medium, and high; these map to 0%, 35%, 65%, and 100% PWM. The board starts with PWM disabled. See `zigbee/stm32wb_device.c` and `platform/board_io.c`.
