#ifndef __NPP_COLOR_PATTERNS_H__
#define __NPP_COLOR_PATTERNS_H__

#include <Arduino.h>

#define NPP_COLOR_PATTERNS 42
#define NPP_COLOR_PATTERNS_VERSION "CP42A"

void npp_load_color_pattern(uint8_t i, uint8_t * n, uint8_t * r, uint8_t * g, uint8_t * b);

#endif
