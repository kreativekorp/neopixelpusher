#ifndef __NPP_BLINK_PATTERNS_H__
#define __NPP_BLINK_PATTERNS_H__

#include <Arduino.h>

#define NPP_BLINK_PATTERNS 18
#define NPP_BLINK_PATTERNS_VERSION "BP18A"

void npp_load_blink_pattern(uint8_t i, uint8_t * ms, uint8_t * frames, uint8_t * channels, uint8_t * ch0, uint8_t * ch1, uint8_t * ch2, uint8_t * ch3);

#endif
