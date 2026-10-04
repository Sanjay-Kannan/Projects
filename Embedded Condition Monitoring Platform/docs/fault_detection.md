# Fault detection

Detection uses interpretable features rather than trained probabilities: vibration RMS and harmonic ratio, high-band energy and acoustic level, plus current and temperature limits. Classification order prioritizes overload, then bearing indicators, harmonic misalignment, and elevated vibration imbalance. Thresholds in this initial host implementation are explicit engineering starting points, not validated machine limits. Severity is an indicator ratio, not a probability.
