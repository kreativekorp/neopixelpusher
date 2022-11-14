#ifndef __PROGRAM_MODE_H__
#define __PROGRAM_MODE_H__

#include <Arduino.h>
#include <inttypes.h>

void cycle_on_off_setup(uint8_t * colors, long in_time, long on_time, long out_time, long off_time);
void cycle_on_off_loop();

void cycle_between_setup(uint8_t * colors, long hold_time, long fade_time);
void cycle_between_loop();

void gradient_fade_setup(uint8_t * gradient, long time_base, long delay_time);
void gradient_fade_loop();

void flicker_setup(uint8_t w, uint8_t r, uint8_t g, uint8_t b);
void flicker_loop();

void run_program_setup(uint8_t p);
void run_program_loop();

#endif
