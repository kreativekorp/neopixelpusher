#include <Arduino.h>
#include <avr/pgmspace.h>
#include "npp_color_patterns.h"
#include "npp_color.h"

static const uint8_t npp_color_pattern_crystal          [] PROGMEM = {  1, CnWHT };
static const uint8_t npp_color_pattern_warm_white       [] PROGMEM = {  1, CnWWT };
static const uint8_t npp_color_pattern_icicle           [] PROGMEM = {  1, CnAZR };
static const uint8_t npp_color_pattern_multicolor_mini  [] PROGMEM = {  5, CnRED, CnBLU, CnGRN, CnMAG, CnGLD };
static const uint8_t npp_color_pattern_multicolor_solid [] PROGMEM = {  5, CnRED, CnBLU, CnWHT, CnORG, CnGRN };
static const uint8_t npp_color_pattern_multicolor_trans [] PROGMEM = {  5, CnRED, CnBLU, CnGRN, CnYEL, CnMAG };
static const uint8_t npp_color_pattern_rainbow          [] PROGMEM = {  8, CnRED, CnORG, CnYEL, CnGRN, CnCYN, CnBLU, CnVIO, CnMAG };
static const uint8_t npp_color_pattern_pastel           [] PROGMEM = {  8, CnCRL, CnCRG, CnLEM, CnLIM, CnSKY, CnFRS, CnLVN, CnPNK };
static const uint8_t npp_color_pattern_candy_cane       [] PROGMEM = {  2, CnRED, CnWHT };
static const uint8_t npp_color_pattern_noel             [] PROGMEM = {  2, CnRED, CnGRN };
static const uint8_t npp_color_pattern_cabana           [] PROGMEM = {  2, CnBLU, CnWHT };
static const uint8_t npp_color_pattern_surf_and_turf    [] PROGMEM = {  2, CnBLU, CnGRN };
static const uint8_t npp_color_pattern_autumn           [] PROGMEM = {  4, CnBRN, CnRED, CnORG, CnYEL };
static const uint8_t npp_color_pattern_pumpkin          [] PROGMEM = {  1, CnORG };
static const uint8_t npp_color_pattern_summer           [] PROGMEM = {  2, CnYEL, CnLEM };
static const uint8_t npp_color_pattern_independence     [] PROGMEM = {  3, CnRED, CnWHT, CnBLU };
static const uint8_t npp_color_pattern_spring           [] PROGMEM = {  6, CnWHT, CnYEL, CnCHR, CnGRN, CnLIM, CnLEM };
static const uint8_t npp_color_pattern_true_love        [] PROGMEM = {  6, CnWHT, CnPNK, CnMAG, CnRSE, CnRED, CnCRL };
static const uint8_t npp_color_pattern_winter           [] PROGMEM = {  6, CnWHT, CnCYN, CnAZR, CnBLU, CnFRS, CnSKY };
static const uint8_t npp_color_pattern_tammy_day        [] PROGMEM = {  2, CnVIO, CnPUR };
static const uint8_t npp_color_pattern_rainbow_deluxe   [] PROGMEM = { 16, CnRSE, CnRED, CnSCL, CnORG, CnGLD, CnYEL, CnCHR, CnGRN, CnAQU, CnCYN, CnAZR, CnBLU, CnIND, CnVIO, CnPUR, CnMAG };
static const uint8_t npp_color_pattern_beckies_colors   [] PROGMEM = {  5, CnLVN, CnPNK, CnSKY, CnLEM, CnWHT };
static const uint8_t npp_color_pattern_primaries        [] PROGMEM = {  4, CnRED, CnYEL, CnBLU, CnGRN };
static const uint8_t npp_color_pattern_powell_station   [] PROGMEM = {  3, CnORG, CnMAG, CnYEL };
static const uint8_t npp_color_pattern_festive_bow_tree [] PROGMEM = {  2, CnMAG, CnYEL };
static const uint8_t npp_color_pattern_red_alert        [] PROGMEM = {  1, CnRED };
static const uint8_t npp_color_pattern_construction     [] PROGMEM = {  1, CnGLD };
static const uint8_t npp_color_pattern_clover           [] PROGMEM = {  1, CnGRN };
static const uint8_t npp_color_pattern_blue_light       [] PROGMEM = {  1, CnBLU };
static const uint8_t npp_color_pattern_law_and_order    [] PROGMEM = {  2, CnRED, CnBLU };
static const uint8_t npp_color_pattern_barber_pole      [] PROGMEM = {  4, CnRED, CnWHT, CnBLU, CnWHT };
static const uint8_t npp_color_pattern_genova           [] PROGMEM = {  3, CnRED, CnWHT, CnGRN };
static const uint8_t npp_color_pattern_mosswood_1       [] PROGMEM = { 25, CnGRN, CnRED, CnRED, CnRED, CnWHT, CnORG, CnRED, CnBLU, CnRED, CnYEL, CnBLU, CnRED, CnBLU, CnWHT, CnRED, CnRED, CnRED, CnGRN, CnGRN, CnBLU, CnGRN, CnYEL, CnRED, CnRED, CnBLU };
static const uint8_t npp_color_pattern_mosswood_2       [] PROGMEM = { 25, CnRED, CnRED, CnBLU, CnRED, CnAQU, CnORG, CnRED, CnGRN, CnBLU, CnGRN, CnWHT, CnRED, CnRED, CnBLU, CnWHT, CnBLU, CnYEL, CnRED, CnRED, CnRED, CnGRN, CnORG, CnBLU, CnBLU, CnWHT };
static const uint8_t npp_color_pattern_magic            [] PROGMEM = {  3, CnPUR, CnIND, CnMAG };
static const uint8_t npp_color_pattern_laughter         [] PROGMEM = {  3, CnPNK, CnMAG, CnCYN };
static const uint8_t npp_color_pattern_loyalty          [] PROGMEM = {  6, CnRED, CnORG, CnYEL, CnGRN, CnBLU, CnVIO };
static const uint8_t npp_color_pattern_honesty          [] PROGMEM = {  3, CnORG, CnLEM, CnGRN };
static const uint8_t npp_color_pattern_generosity       [] PROGMEM = {  3, CnWHT, CnVIO, CnAZR };
static const uint8_t npp_color_pattern_kindness         [] PROGMEM = {  3, CnLEM, CnPNK, CnAQU };
static const uint8_t npp_color_pattern_harmony          [] PROGMEM = {  6, CnPNK, CnCRG, CnLEM, CnWHT, CnSKY, CnLVN };
static const uint8_t npp_color_pattern_u_n_owen         [] PROGMEM = {  7, CnCYN, CnAZR, CnPUR, CnRSE, CnORG, CnYEL, CnGRN };

static const uint8_t * const npp_color_patterns[NPP_COLOR_PATTERNS] PROGMEM = {
	npp_color_pattern_crystal,
	npp_color_pattern_warm_white,
	npp_color_pattern_icicle,
	npp_color_pattern_multicolor_mini,
	npp_color_pattern_multicolor_solid,
	npp_color_pattern_multicolor_trans,
	npp_color_pattern_rainbow,
	npp_color_pattern_pastel,
	npp_color_pattern_candy_cane,
	npp_color_pattern_noel,
	npp_color_pattern_cabana,
	npp_color_pattern_surf_and_turf,
	npp_color_pattern_autumn,
	npp_color_pattern_pumpkin,
	npp_color_pattern_summer,
	npp_color_pattern_independence,
	npp_color_pattern_spring,
	npp_color_pattern_true_love,
	npp_color_pattern_winter,
	npp_color_pattern_tammy_day,
	npp_color_pattern_rainbow_deluxe,
	npp_color_pattern_beckies_colors,
	npp_color_pattern_primaries,
	npp_color_pattern_powell_station,
	npp_color_pattern_festive_bow_tree,
	npp_color_pattern_red_alert,
	npp_color_pattern_construction,
	npp_color_pattern_clover,
	npp_color_pattern_blue_light,
	npp_color_pattern_law_and_order,
	npp_color_pattern_barber_pole,
	npp_color_pattern_genova,
	npp_color_pattern_mosswood_1,
	npp_color_pattern_mosswood_2,
	npp_color_pattern_magic,
	npp_color_pattern_laughter,
	npp_color_pattern_loyalty,
	npp_color_pattern_honesty,
	npp_color_pattern_generosity,
	npp_color_pattern_kindness,
	npp_color_pattern_harmony,
	npp_color_pattern_u_n_owen,
};

void npp_load_color_pattern(uint8_t i, uint8_t * n, uint8_t * r, uint8_t * g, uint8_t * b) {
	uint8_t * p = (uint8_t *)pgm_read_word(&npp_color_patterns[i]);
	for (*n = i = pgm_read_byte(p++); i > 0; i--) {
		*r++ = pgm_read_byte(p++);
		*g++ = pgm_read_byte(p++);
		*b++ = pgm_read_byte(p++);
	}
}
