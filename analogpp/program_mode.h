#ifndef __PROGRAM_MODE_H__
#define __PROGRAM_MODE_H__

#include <Arduino.h>
#include <inttypes.h>

void cycle_on_off(uint8_t * colors, long in_time, long on_time, long out_time, long off_time);
void cycle_between(uint8_t * colors, long hold_time, long fade_time);
void gradient_fade(uint8_t * gradient, long time_base, long delay_time);
void flicker(uint8_t w0, uint8_t r0, uint8_t g0, uint8_t b0);
void run_program(uint8_t p);

#endif
