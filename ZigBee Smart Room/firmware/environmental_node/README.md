# Environmental sensor end device

Endpoint 1, Home Automation profile 0x0104. Servers: Basic, Identify, Temperature Measurement (0x0402), Relative Humidity Measurement (0x0405), Illuminance Measurement (0x0400). The SHT31-DIS and VEML6030 are polled every five seconds on I2C1. Measurements are validated, converted to ZCL units, and written to cluster attributes. Reporting defaults are configured in `zigbee/stm32wb_device.c`; retry occurs on subsequent sample periods after bus or CRC errors.
