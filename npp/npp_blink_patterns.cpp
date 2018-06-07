#include <Arduino.h>
#include <avr/pgmspace.h>
#include "npp_blink_patterns.h"

#define BPnTWIFLA   0,   0, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0
#define BPnTWIFLB   0,   0, 255, 255,   0,   0, 255, 255,   0,   0, 255, 255,   0,   0, 255, 255, 255,   0,   0,   0,   0,   0, 255, 255, 255,   0,   0,   0,   0,   0, 255, 255, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255, 255,   0, 255,   0,   0,   0,   0, 255, 255,   0, 255,   0,   0

static const uint8_t npp_blink_pattern_chase_1_of_2    [] PROGMEM = { 200,  2, 2, 255,   0,   0, 255 };
static const uint8_t npp_blink_pattern_chase_1_of_3    [] PROGMEM = { 160,  3, 3, 255,   0,   0,   0, 255,   0,   0,   0, 255 };
static const uint8_t npp_blink_pattern_chase_1_of_4    [] PROGMEM = { 120,  4, 4, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255 };
static const uint8_t npp_blink_pattern_chase_2_of_3    [] PROGMEM = { 160,  3, 3, 255, 255,   0,   0, 255, 255, 255,   0, 255 };
static const uint8_t npp_blink_pattern_chase_2_of_4    [] PROGMEM = { 120,  4, 4, 255, 255,   0,   0,   0, 255, 255,   0,   0,   0, 255, 255, 255,   0,   0, 255 };
static const uint8_t npp_blink_pattern_chase_3_of_4    [] PROGMEM = { 120,  4, 4, 255, 255, 255,   0,   0, 255, 255, 255, 255,   0, 255, 255, 255, 255,   0, 255 };
static const uint8_t npp_blink_pattern_slo_glo_3       [] PROGMEM = { 100, 63, 3,  17,   0, 255,  34,   0, 255,  51,   0, 255,  68,   0, 255,  85,   0, 255, 102,   0, 255, 119,   0, 255, 136,   0, 238, 153,   0, 221, 170,   0, 204, 187,   0, 187, 204,   0, 170, 221,   0, 153, 238,   0, 136, 255,   0, 119, 255,   0, 102, 255,   0,  85, 255,   0,  68, 255,   0,  51, 255,   0,  34, 255,   0,  17, 255,  17,   0, 255,  34,   0, 255,  51,   0, 255,  68,   0, 255,  85,   0, 255, 102,   0, 255, 119,   0, 238, 136,   0, 221, 153,   0, 204, 170,   0, 187, 187,   0, 170, 204,   0, 153, 221,   0, 136, 238,   0, 119, 255,   0, 102, 255,   0,  85, 255,   0,  68, 255,   0,  51, 255,   0,  34, 255,   0,  17, 255,   0,   0, 255,  17,   0, 255,  34,   0, 255,  51,   0, 255,  68,   0, 255,  85,   0, 255, 102,   0, 255, 119,   0, 238, 136,   0, 221, 153,   0, 204, 170,   0, 187, 187,   0, 170, 204,   0, 153, 221,   0, 136, 238,   0, 119, 255,   0, 102, 255,   0,  85, 255,   0,  68, 255,   0,  51, 255,   0,  34, 255,   0,  17, 255 };
static const uint8_t npp_blink_pattern_slo_glo_4       [] PROGMEM = { 100, 84, 4,  17,   0,   0, 255,  34,   0,   0, 255,  51,   0,   0, 255,  68,   0,   0, 255,  85,   0,   0, 255, 102,   0,   0, 255, 119,   0,   0, 255, 136,   0,   0, 238, 153,   0,   0, 221, 170,   0,   0, 204, 187,   0,   0, 187, 204,   0,   0, 170, 221,   0,   0, 153, 238,   0,   0, 136, 255,   0,   0, 119, 255,   0,   0, 102, 255,   0,   0,  85, 255,   0,   0,  68, 255,   0,   0,  51, 255,   0,   0,  34, 255,   0,   0,  17, 255,  17,   0,   0, 255,  34,   0,   0, 255,  51,   0,   0, 255,  68,   0,   0, 255,  85,   0,   0, 255, 102,   0,   0, 255, 119,   0,   0, 238, 136,   0,   0, 221, 153,   0,   0, 204, 170,   0,   0, 187, 187,   0,   0, 170, 204,   0,   0, 153, 221,   0,   0, 136, 238,   0,   0, 119, 255,   0,   0, 102, 255,   0,   0,  85, 255,   0,   0,  68, 255,   0,   0,  51, 255,   0,   0,  34, 255,   0,   0,  17, 255,   0,   0,   0, 255,  17,   0,   0, 255,  34,   0,   0, 255,  51,   0,   0, 255,  68,   0,   0, 255,  85,   0,   0, 255, 102,   0,   0, 255, 119,   0,   0, 238, 136,   0,   0, 221, 153,   0,   0, 204, 170,   0,   0, 187, 187,   0,   0, 170, 204,   0,   0, 153, 221,   0,   0, 136, 238,   0,   0, 119, 255,   0,   0, 102, 255,   0,   0,  85, 255,   0,   0,  68, 255,   0,   0,  51, 255,   0,   0,  34, 255,   0,   0,  17, 255,   0,   0,   0, 255,  17,   0,   0, 255,  34,   0,   0, 255,  51,   0,   0, 255,  68,   0,   0, 255,  85,   0,   0, 255, 102,   0,   0, 255, 119,   0,   0, 238, 136,   0,   0, 221, 153,   0,   0, 204, 170,   0,   0, 187, 187,   0,   0, 170, 204,   0,   0, 153, 221,   0,   0, 136, 238,   0,   0, 119, 255,   0,   0, 102, 255,   0,   0,  85, 255,   0,   0,  68, 255,   0,   0,  51, 255,   0,   0,  34, 255,   0,   0,  17, 255 };
static const uint8_t npp_blink_pattern_waves_3         [] PROGMEM = {  60, 12, 3,   0, 204, 204,  51, 255, 153, 102, 255, 102, 153, 255,  51, 204, 204,   0, 255, 153,  51, 255, 102, 102, 255,  51, 153, 204,   0, 204, 153,  51, 255, 102, 102, 255,  51, 153, 255 };
static const uint8_t npp_blink_pattern_waves_4         [] PROGMEM = {  60, 12, 4,   0, 153, 255, 153,  51, 204, 255, 102, 102, 255, 204,  51, 153, 255, 153,   0, 204, 255, 102,  51, 255, 204,  51, 102, 255, 153,   0, 153, 255, 102,  51, 204, 204,  51, 102, 255, 153,   0, 153, 255, 102,  51, 204, 255,  51, 102, 255, 204 };
static const uint8_t npp_blink_pattern_twinkle_flash_a [] PROGMEM = {  80, 16, 4, BPnTWIFLA };
static const uint8_t npp_blink_pattern_twinkle_flash_b [] PROGMEM = {  80, 16, 4, BPnTWIFLB };
static const uint8_t npp_blink_pattern_twinkle_flash_c [] PROGMEM = {  80, 64, 4, BPnTWIFLA, BPnTWIFLA, BPnTWIFLB, BPnTWIFLB };
static const uint8_t npp_blink_pattern_chasing_flash   [] PROGMEM = {  80, 32, 4, 255, 255,   0,   0,   0, 255, 255,   0,   0,   0, 255, 255, 255,   0,   0, 255, 255, 255,   0,   0,   0, 255, 255,   0,   0,   0, 255, 255, 255,   0,   0, 255, 255, 255,   0,   0,   0, 255, 255,   0,   0,   0, 255, 255, 255,   0,   0, 255, 255, 255,   0,   0,   0, 255, 255,   0,   0,   0, 255, 255, 255,   0,   0, 255,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0 };
static const uint8_t npp_blink_pattern_walk_3          [] PROGMEM = { 160,  6, 3, 255,   0,   0, 255, 255,   0,   0, 255,   0,   0, 255, 255,   0,   0, 255, 255,   0, 255 };
static const uint8_t npp_blink_pattern_walk_4          [] PROGMEM = { 120,  8, 4, 255,   0,   0,   0, 255, 255,   0,   0,   0, 255,   0,   0,   0, 255, 255,   0,   0,   0, 255,   0,   0,   0, 255, 255,   0,   0,   0, 255, 255,   0,   0, 255 };
static const uint8_t npp_blink_pattern_oscillate_3     [] PROGMEM = { 160,  4, 3, 255,   0,   0,   0, 255,   0,   0,   0, 255,   0, 255,   0 };
static const uint8_t npp_blink_pattern_oscillate_4     [] PROGMEM = { 120,  6, 4, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0, 255,   0,   0, 255,   0,   0 };

static const uint8_t * const npp_blink_patterns[NPP_BLINK_PATTERNS] PROGMEM = {
	npp_blink_pattern_chase_1_of_2,
	npp_blink_pattern_chase_1_of_3,
	npp_blink_pattern_chase_1_of_4,
	npp_blink_pattern_chase_2_of_3,
	npp_blink_pattern_chase_2_of_4,
	npp_blink_pattern_chase_3_of_4,
	npp_blink_pattern_slo_glo_3,
	npp_blink_pattern_slo_glo_4,
	npp_blink_pattern_waves_3,
	npp_blink_pattern_waves_4,
	npp_blink_pattern_twinkle_flash_a,
	npp_blink_pattern_twinkle_flash_b,
	npp_blink_pattern_twinkle_flash_c,
	npp_blink_pattern_chasing_flash,
	npp_blink_pattern_walk_3,
	npp_blink_pattern_walk_4,
	npp_blink_pattern_oscillate_3,
	npp_blink_pattern_oscillate_4,
};

void npp_load_blink_pattern(uint8_t i, uint8_t * ms, uint8_t * frames, uint8_t * channels, uint8_t * ch0, uint8_t * ch1, uint8_t * ch2, uint8_t * ch3) {
	uint8_t fc, cc;
	uint8_t * p = (uint8_t *)pgm_read_word(&npp_blink_patterns[i]);
	*ms = pgm_read_byte(p++);
	*frames = fc = pgm_read_byte(p++);
	*channels = cc = pgm_read_byte(p++);
	for (i = 0; i < fc; i++) {
		if (cc > 0) *ch0++ = pgm_read_byte(p++);
		if (cc > 1) *ch1++ = pgm_read_byte(p++);
		if (cc > 2) *ch2++ = pgm_read_byte(p++);
		if (cc > 3) *ch3++ = pgm_read_byte(p++);
	}
}
