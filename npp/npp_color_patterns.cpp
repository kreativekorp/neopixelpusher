#include <Arduino.h>
#include <avr/pgmspace.h>
#include "npp_color_patterns.h"
#include "npp_color.h"

static const uint8_t npp_color_palette[] PROGMEM = {
	CnBLK, CnDGY, CnMGY, CnLGY, CnWHT, CnWWT, CnTAN, CnBRN,
	CnMRN, CnUMB, CnOLV, CnFGR, CnTEL, CnNVY, CnEGP, CnPLM,
	CnRED, CnSCL, CnORG, CnGLD, CnYEL, CnCHR, CnGRN, CnAQU,
	CnCYN, CnAZR, CnBLU, CnIND, CnVIO, CnPUR, CnMAG, CnRSE,
	CnCRL, CnCRG, CnLEM, CnLIM, CnSKY, CnFRS, CnLVN, CnPNK,
};

#define CiBLK   0
#define CiDGY   3
#define CiMGY   6
#define CiLGY   9
#define CiWHT  12
#define CiWWT  15
#define CiTAN  18
#define CiBRN  21
#define CiMRN  24
#define CiUMB  27
#define CiOLV  30
#define CiFGR  33
#define CiTEL  36
#define CiNVY  39
#define CiEGP  42
#define CiPLM  45
#define CiRED  48
#define CiSCL  51
#define CiORG  54
#define CiGLD  57
#define CiYEL  60
#define CiCHR  63
#define CiGRN  66
#define CiAQU  69
#define CiCYN  72
#define CiAZR  75
#define CiBLU  78
#define CiIND  81
#define CiVIO  84
#define CiPUR  87
#define CiMAG  90
#define CiRSE  93
#define CiCRL  96
#define CiCRG  99
#define CiLEM 102
#define CiLIM 105
#define CiSKY 108
#define CiFRS 111
#define CiLVN 114
#define CiPNK 117

static const uint8_t npp_color_pattern_crystal          [] PROGMEM = {  1, CiWHT };
static const uint8_t npp_color_pattern_warm_white       [] PROGMEM = {  1, CiWWT };
static const uint8_t npp_color_pattern_icicle           [] PROGMEM = {  1, CiAZR };
static const uint8_t npp_color_pattern_multicolor_mini  [] PROGMEM = {  5, CiRED, CiBLU, CiGRN, CiMAG, CiGLD };
static const uint8_t npp_color_pattern_multicolor_solid [] PROGMEM = {  5, CiRED, CiBLU, CiWHT, CiORG, CiGRN };
static const uint8_t npp_color_pattern_multicolor_trans [] PROGMEM = {  5, CiRED, CiBLU, CiGRN, CiYEL, CiMAG };
static const uint8_t npp_color_pattern_rainbow          [] PROGMEM = {  8, CiRED, CiORG, CiYEL, CiGRN, CiCYN, CiBLU, CiVIO, CiMAG };
static const uint8_t npp_color_pattern_pastel           [] PROGMEM = {  8, CiCRL, CiCRG, CiLEM, CiLIM, CiSKY, CiFRS, CiLVN, CiPNK };
static const uint8_t npp_color_pattern_candy_cane       [] PROGMEM = {  2, CiRED, CiWHT };
static const uint8_t npp_color_pattern_noel             [] PROGMEM = {  2, CiRED, CiGRN };
static const uint8_t npp_color_pattern_cabana           [] PROGMEM = {  2, CiBLU, CiWHT };
static const uint8_t npp_color_pattern_surf_and_turf    [] PROGMEM = {  2, CiBLU, CiGRN };
static const uint8_t npp_color_pattern_autumn           [] PROGMEM = {  4, CiBRN, CiRED, CiORG, CiYEL };
static const uint8_t npp_color_pattern_pumpkin          [] PROGMEM = {  1, CiORG };
static const uint8_t npp_color_pattern_summer           [] PROGMEM = {  2, CiYEL, CiLEM };
static const uint8_t npp_color_pattern_independence     [] PROGMEM = {  3, CiRED, CiWHT, CiBLU };
static const uint8_t npp_color_pattern_spring           [] PROGMEM = {  6, CiWHT, CiYEL, CiCHR, CiGRN, CiLIM, CiLEM };
static const uint8_t npp_color_pattern_true_love        [] PROGMEM = {  6, CiWHT, CiPNK, CiMAG, CiRSE, CiRED, CiCRL };
static const uint8_t npp_color_pattern_winter           [] PROGMEM = {  6, CiWHT, CiCYN, CiAZR, CiBLU, CiFRS, CiSKY };
static const uint8_t npp_color_pattern_tammy_day        [] PROGMEM = {  2, CiVIO, CiPUR };
static const uint8_t npp_color_pattern_rainbow_deluxe   [] PROGMEM = { 16, CiRSE, CiRED, CiSCL, CiORG, CiGLD, CiYEL, CiCHR, CiGRN, CiAQU, CiCYN, CiAZR, CiBLU, CiIND, CiVIO, CiPUR, CiMAG };
static const uint8_t npp_color_pattern_beckies_colors   [] PROGMEM = {  5, CiLVN, CiPNK, CiSKY, CiLEM, CiWHT };
static const uint8_t npp_color_pattern_primaries        [] PROGMEM = {  4, CiRED, CiYEL, CiBLU, CiGRN };
static const uint8_t npp_color_pattern_powell_station   [] PROGMEM = {  3, CiORG, CiMAG, CiYEL };
static const uint8_t npp_color_pattern_festive_bow_tree [] PROGMEM = {  2, CiMAG, CiYEL };
static const uint8_t npp_color_pattern_red_alert        [] PROGMEM = {  1, CiRED };
static const uint8_t npp_color_pattern_construction     [] PROGMEM = {  1, CiGLD };
static const uint8_t npp_color_pattern_clover           [] PROGMEM = {  1, CiGRN };
static const uint8_t npp_color_pattern_blue_light       [] PROGMEM = {  1, CiBLU };
static const uint8_t npp_color_pattern_law_and_order    [] PROGMEM = {  2, CiRED, CiBLU };
static const uint8_t npp_color_pattern_barber_pole      [] PROGMEM = {  4, CiRED, CiWHT, CiBLU, CiWHT };
static const uint8_t npp_color_pattern_genova           [] PROGMEM = {  3, CiRED, CiWHT, CiGRN };
static const uint8_t npp_color_pattern_mosswood_1       [] PROGMEM = { 25, CiGRN, CiRED, CiRED, CiRED, CiWHT, CiORG, CiRED, CiBLU, CiRED, CiYEL, CiBLU, CiRED, CiBLU, CiWHT, CiRED, CiRED, CiRED, CiGRN, CiGRN, CiBLU, CiGRN, CiYEL, CiRED, CiRED, CiBLU };
static const uint8_t npp_color_pattern_mosswood_2       [] PROGMEM = { 25, CiRED, CiRED, CiBLU, CiRED, CiAQU, CiORG, CiRED, CiGRN, CiBLU, CiGRN, CiWHT, CiRED, CiRED, CiBLU, CiWHT, CiBLU, CiYEL, CiRED, CiRED, CiRED, CiGRN, CiORG, CiBLU, CiBLU, CiWHT };
static const uint8_t npp_color_pattern_magic            [] PROGMEM = {  3, CiPUR, CiIND, CiMAG };
static const uint8_t npp_color_pattern_laughter         [] PROGMEM = {  3, CiPNK, CiMAG, CiCYN };
static const uint8_t npp_color_pattern_loyalty          [] PROGMEM = {  6, CiRED, CiORG, CiYEL, CiGRN, CiBLU, CiVIO };
static const uint8_t npp_color_pattern_honesty          [] PROGMEM = {  3, CiORG, CiLEM, CiGRN };
static const uint8_t npp_color_pattern_generosity       [] PROGMEM = {  3, CiWHT, CiVIO, CiAZR };
static const uint8_t npp_color_pattern_kindness         [] PROGMEM = {  3, CiLEM, CiPNK, CiAQU };
static const uint8_t npp_color_pattern_harmony          [] PROGMEM = {  6, CiPNK, CiCRG, CiLEM, CiWHT, CiSKY, CiLVN };
static const uint8_t npp_color_pattern_u_n_owen         [] PROGMEM = {  7, CiCYN, CiAZR, CiPUR, CiRSE, CiORG, CiYEL, CiGRN };

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
		uint8_t q = pgm_read_byte(p++);
		*r++ = pgm_read_byte(&npp_color_palette[q++]);
		*g++ = pgm_read_byte(&npp_color_palette[q++]);
		*b++ = pgm_read_byte(&npp_color_palette[q++]);
	}
}
