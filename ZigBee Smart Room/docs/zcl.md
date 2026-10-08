# ZCL mapping

Cluster IDs below follow the Zigbee Cluster Library and ST STM32WB cluster support documentation. All clusters are server clusters on endpoint 1, profile 0x0104. The selected CubeWB release determines exact API names and cluster-template availability.

| Device | Cluster | ID | Key data/behavior |
|---|---|---:|---|
| Sensor | Basic | 0x0000 | Manufacturer/model/software identity |
| Sensor | Identify | 0x0003 | IdentifyTime |
| Sensor | Illuminance Measurement | 0x0400 | MeasuredValue, MinMeasuredValue, MaxMeasuredValue |
| Sensor | Temperature Measurement | 0x0402 | MeasuredValue, MinMeasuredValue, MaxMeasuredValue, Tolerance |
| Sensor | Relative Humidity Measurement | 0x0405 | MeasuredValue, MinMeasuredValue, MaxMeasuredValue, Tolerance |
| Light | On/Off | 0x0006 | OnOff and On/Off commands |
| Light | Level Control | 0x0008 | CurrentLevel and move/step/transition commands |
| Fan | Fan Control | 0x0202 | FanMode, FanModeSequence, PercentSetting, PercentCurrent |

For measurement clusters, measured temperature and relative humidity use signed/unsigned hundredths respectively. Illuminance uses ZCL logarithmic encoding; use the CubeWB cluster helper if available rather than writing raw lux directly. 0xffff indicates invalid measured value. Reporting intervals and change thresholds must be appropriate to sensor noise and powered/sleeping behavior. Verify permissions and supported commands against the precise CubeWB release's cluster template and CSA spec.
