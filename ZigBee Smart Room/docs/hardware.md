# Hardware and wiring

## Selected parts

- 3x ST NUCLEO-WB55RG development boards: environmental, light, fan end devices.
- 1x Sonoff ZBDongle-P USB coordinator (TI CC2652P).
- 1x Sensirion SHT31-DIS sensor breakout and 1x Vishay VEML6030 ambient-light breakout. Both share I2C1 at addresses 0x44 and 0x10.
- 1x LED and 220 ohm series resistor for the dimmable light output.
- 1x 5 V brushless DC fan, AO3400A logic-level N-channel MOSFET, 100 ohm gate resistor, and 100 kOhm gate-to-ground pulldown.
- Regulated 5 V fan supply with current rating above the fan's startup current. MCU board powered by USB; connect supply and MCU grounds.
- 4.7 kOhm I2C pull-ups only if sensor breakout boards do not already include suitable pull-ups to 3.3 V.
- 100 uF decoupling capacitor across fan supply near the driver.

Full BOM: `hardware/BOM.csv`. Use sensor breakouts that do not expose 5 V pull-ups to the STM32.

## NUCLEO-WB55RG pin assignment

The provided CubeMX file assigns I2C1_SCL PB8, I2C1_SDA PB9, TIM3_CH3 PB0 for LED PWM, TIM3_CH4 PB1 for fan PWM, and PA5/LD2 as a status indicator. PWM timer period is 999 counts; with the 64 MHz timer clock and prescaler 63 this is a 1 kHz PWM base. Confirm board revision alternate-function labels before wiring.

```text
3V3 -> SHT31 VIN and VEML6030 VIN; GND -> both sensor grounds
PB8/I2C1_SCL -> both SCL; PB9/I2C1_SDA -> both SDA
PB0/TIM3_CH3 -> 220R -> LED anode; LED cathode -> GND
PB1/TIM3_CH4 -> 100R -> MOSFET gate; gate -> 100k -> GND
MOSFET source -> common GND; drain -> fan negative; fan positive -> external +5V
External 5V supply GND -> NUCLEO GND
```

Do not connect a fan to an MCU pin or the 3.3 V rail. This circuit controls a low-voltage DC fan only; it is not a mains fan controller. A brushless fan has internal commutation electronics; do not add a flyback diode unless the selected motor datasheet requires it.
