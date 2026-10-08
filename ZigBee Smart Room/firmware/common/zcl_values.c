#include "zcl_values.h"
#include <math.h>
uint16_t room_lux_to_zcl_illuminance(uint32_t lux) {
    if (lux == 0u) return 0u;
    /* Illuminance MeasuredValue = 10,000*log10(lux)+1; saturate to uint16. */
    double encoded = 10000.0 * log10((double)lux) + 1.0;
    if (encoded >= 65534.0) return 65534u;
    if (encoded < 1.0) return 1u;
    return (uint16_t)floor(encoded + 0.5);
}
