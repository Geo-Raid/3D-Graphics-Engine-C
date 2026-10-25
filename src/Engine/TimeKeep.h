#ifndef TIMEKEEP_H
#define TIMEKEEP_H

#include "TimeKeep.c"

extern void init_timer(double target_fps);
extern void cleanup_timer(void);
extern void cap_frame_rate(void);

#endif