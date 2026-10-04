#ifndef FILTERS_H
#define FILTERS_H
typedef struct { float previous_input, previous_output; float highpass_hz, sample_rate_hz; } DcBlocker;
void dc_blocker_init(DcBlocker *filter,float cutoff_hz,float sample_rate_hz);
float dc_blocker_process(DcBlocker *filter,float input);
#endif
