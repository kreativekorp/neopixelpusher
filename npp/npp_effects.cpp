#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <EEPROM.h>
#include "npp_blink_patterns.h"
#include "npp_color_patterns.h"
#include "npp_color.h"
#include "npp_eeprom.h"
#include "npp_effects.h"

extern Adafruit_NeoPixel strip;

static int eestart;
static uint16_t strip_length;
static uint8_t effect_randomize;
static uint8_t effect_mode;
static uint8_t color_preset;
static uint8_t color_count;
static uint8_t r[NPP_EFFECT_MAX_COLORS_RAM];
static uint8_t g[NPP_EFFECT_MAX_COLORS_RAM];
static uint8_t b[NPP_EFFECT_MAX_COLORS_RAM];
static uint8_t blink_preset;
static uint8_t blink_msperframe;
static uint8_t blink_frames;
static uint8_t blink_channels;
static uint8_t ch0[NPP_EFFECT_MAX_FRAMES_RAM];
static uint8_t ch1[NPP_EFFECT_MAX_FRAMES_RAM];
static uint8_t ch2[NPP_EFFECT_MAX_FRAMES_RAM];
static uint8_t ch3[NPP_EFFECT_MAX_FRAMES_RAM];
static uint16_t speed_adjust;
static uint32_t randomize_interval;
static uint32_t randomize_time;
static uint32_t frame_time;
static uint32_t frame;
static uint8_t frame_changed;

/* Private functions used by command processor for parsing and formatting numbers. */

static char fmt_digit(uint8_t v) {
	if (v < 10) return '0' + v;
	else return 'A' + v - 10;
}

static uint8_t parse_digit(char d) {
	if (d >= '0' && d <= '9') return d - '0';
	if (d >= 'A' && d <= 'Z') return d - 'A' + 10;
	if (d >= 'a' && d <= 'z') return d - 'a' + 10;
	return -1;
}

static char * fmt_hex(char * buf, uint8_t v) {
	*buf++ = fmt_digit(v >> 4);
	*buf++ = fmt_digit(v & 15);
	return buf;
}

static char * fmt_int(char * buf, uint32_t v) {
	uint8_t tmp[11]; uint8_t p = 0;
	do { tmp[p++] = v % 10; v /= 10; } while (v);
	while (p) *buf++ = '0' + tmp[--p];
	return buf;
}

static uint8_t parse_int(char * buf, uint32_t * v) {
	uint8_t init = 0, d;
	while (*buf) {
		if ((d = parse_digit(*buf++)) < 10) {
			if (init) {
				*v = (*v) * 10 + d;
			} else {
				*v = d;
				init = 1;
			}
		}
	}
	return init;
}

static char * fmt_float(char * buf, float v) {
	uint8_t i = 0;
	uint32_t iv = v;
	buf = fmt_int(buf, iv);
	v -= iv;
	*buf++ = '.';
	do {
		v *= 10;
		iv = v;
		*buf++ = '0' + iv;
		v -= iv;
		i++;
	}
	while (v && i < 3);
	return buf;
}

static uint8_t parse_float(char * buf, float * v) {
	uint32_t iv = 0, ev = 1;
	uint8_t init = 0, dp = 0, d;
	while (*buf) {
		if ((*buf) == '.') {
			init = dp = 1;
		} else if ((d = parse_digit(*buf)) < 10) {
			if (init) {
				iv = iv * 10 + d;
				if (dp) ev *= 10;
			} else {
				iv = d;
				init = 1;
			}
		}
		buf++;
	}
	if (init) *v = (float)iv / (float)ev;
	return init;
}

/* Private functions used by command processor for changing effects settings. */

static void npp_effect_set_color_preset(uint8_t i) {
	npp_load_color_pattern(color_preset = i, &color_count, r, g, b);
	EEPROM_update(eestart + 2, color_preset);
	EEPROM_update(eestart + 3, color_count);
}

static uint8_t npp_effect_set_color_pattern(char * buf) {
	uint8_t i = 0, p = 0, d, rv, gv, bv;
	while ((*buf) && i < NPP_EFFECT_MAX_COLORS_RAM) {
		if ((*buf) == 'X') {
			i = color_count;
			p = 0;
		} else if ((d = parse_digit(*buf)) < 16) {
			switch (p) {
				case 0: rv = d << 4; break;
				case 1: rv |= d; break;
				case 2: gv = d << 4; break;
				case 3: gv |= d; break;
				case 4: bv = d << 4; break;
				case 5: bv |= d; break;
			}
			p++;
			if (p >= 6) {
				r[i] = rv;
				g[i] = gv;
				b[i] = bv;
				i++;
				p = 0;
			}
		}
		buf++;
	}
	if (i) {
		EEPROM_update(eestart + 2, color_preset = 254);
		EEPROM_update(eestart + 3, color_count = i);
		int a = eestart + NPP_EFFECT_EEPROM_ADDR_COLORS;
		for (i = 0; i < NPP_EFFECT_MAX_COLORS_EEPROM; i++) EEPROM_update(a++, r[i]);
		for (i = 0; i < NPP_EFFECT_MAX_COLORS_EEPROM; i++) EEPROM_update(a++, g[i]);
		for (i = 0; i < NPP_EFFECT_MAX_COLORS_EEPROM; i++) EEPROM_update(a++, b[i]);
		return 1;
	}
	return 0;
}

static void npp_effect_set_blink_preset(uint8_t i) {
	effect_mode = NPP_EFFECTS + i;
	npp_load_blink_pattern(blink_preset = i, &blink_msperframe, &blink_frames, &blink_channels, ch0, ch1, ch2, ch3);
	frame_time = millis();
	frame = 0;
	frame_changed = 1;
	if (effect_randomize) return;
	EEPROM_update(eestart + 1, effect_mode);
	EEPROM_update(eestart + 4, blink_preset);
	EEPROM_update(eestart + 5, blink_msperframe);
	EEPROM_update(eestart + 6, blink_frames);
	EEPROM_update(eestart + 7, blink_channels);
}

static uint8_t npp_effect_set_blink_pattern(char * buf) {
	uint8_t i = 0, p = 253, d, ms, cc, c0, c1, c2, c3;
	while ((*buf) && i < NPP_EFFECT_MAX_FRAMES_RAM) {
		if ((*buf) == 'X') {
			i = blink_frames;
			p = 0;
			ms = blink_msperframe;
			cc = blink_channels;
		} else if ((d = parse_digit(*buf)) < 16) {
			switch (p) {
				case 253: ms  = d << 4; break;
				case 254: ms |= d     ; break;
				case 255: cc  = d     ; break;
				case 0: c0  = d << 4; break;
				case 1: c0 |= d     ; break;
				case 2: c1  = d << 4; break;
				case 3: c1 |= d     ; break;
				case 4: c2  = d << 4; break;
				case 5: c2 |= d     ; break;
				case 6: c3  = d << 4; break;
				case 7: c3 |= d     ; break;
			}
			p++;
			if (p < 253 && p >= (cc << 1)) {
				ch0[i] = c0;
				ch1[i] = c1;
				ch2[i] = c2;
				ch3[i] = c3;
				i++;
				p = 0;
			}
		}
		buf++;
	}
	if (i) {
		EEPROM_update(eestart + 1, effect_mode = 254);
		EEPROM_update(eestart + 4, blink_preset = 254);
		EEPROM_update(eestart + 5, blink_msperframe = ms);
		EEPROM_update(eestart + 6, blink_frames = i);
		EEPROM_update(eestart + 7, blink_channels = cc);
		int a = eestart + NPP_EFFECT_EEPROM_ADDR_FRAMES;
		for (i = 0; i < NPP_EFFECT_MAX_FRAMES_EEPROM; i++) EEPROM_update(a++, ch0[i]);
		for (i = 0; i < NPP_EFFECT_MAX_FRAMES_EEPROM; i++) EEPROM_update(a++, ch1[i]);
		for (i = 0; i < NPP_EFFECT_MAX_FRAMES_EEPROM; i++) EEPROM_update(a++, ch2[i]);
		for (i = 0; i < NPP_EFFECT_MAX_FRAMES_EEPROM; i++) EEPROM_update(a++, ch3[i]);
		return 1;
	}
	return 0;
}

static void npp_effect_set_speed_adjust(uint16_t s) {
	EEPROM_update16(eestart + 8, speed_adjust = s);
}

static void npp_effect_set_mode(uint8_t m) {
	if (m >= NPP_EFFECTS) {
		npp_effect_set_blink_preset(m - NPP_EFFECTS);
	} else {
		effect_mode = m;
		frame_time = millis();
		frame = 0;
		frame_changed = 1;
		if (effect_randomize) return;
		EEPROM_update(eestart + 1, m);
	}
}

static void npp_effect_set_randomize(uint8_t r) {
	EEPROM_update(eestart, effect_randomize = r);
	if (effect_randomize) {
		randomize_time = millis();
		npp_effect_set_mode(random(NPP_EFFECTS + NPP_BLINK_PATTERNS));
	}
}

static void npp_effect_set_randomize_interval(uint32_t ms) {
	if (ms) EEPROM_update32(eestart + 10, randomize_interval = ms);
	npp_effect_set_randomize(ms != 0);
}

/* Private functions used by effects. */

static uint8_t frame_advance(uint16_t ms, uint16_t mod) {
	uint32_t m = millis();
	uint32_t e = (((m - frame_time) * speed_adjust) >> 8);
	if (e < ms) return 0;
	frame_time = m;
	frame += e / ms;
	if (mod && frame >= mod) frame = 0;
	frame_changed = 1;
	return 1;
}

static uint8_t frame_advanced(void) {
	uint8_t ret = frame_changed;
	frame_changed = 0;
	return ret;
}

static void scroll_helper(uint8_t ci) {
	for (uint16_t si = 0; si < strip_length; si++) {
		strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
		ci++; if (ci >= color_count) ci = 0;
	}
	strip.show();
}

static void cycle_helper(uint8_t r, uint8_t g, uint8_t b) {
	uint32_t c = strip.Color(r, g, b);
	for (uint16_t si = 0; si < strip_length; si++) {
		strip.setPixelColor(si, c);
	}
	strip.show();
}

static void fade_helper(uint8_t m, uint8_t gc) {
	uint8_t ci = 0, rv, gv, bv;
	for (uint16_t si = 0; si < strip_length; si++) {
		rv = color_multiply(r[ci], m, gc);
		gv = color_multiply(g[ci], m, gc);
		bv = color_multiply(b[ci], m, gc);
		strip.setPixelColor(si, strip.Color(rv, gv, bv));
		ci++; if (ci >= color_count) ci = 0;
	}
	strip.show();
}

static void walk_helper(uint16_t i) {
	uint8_t ci = 0;
	uint16_t i1 = i >> 1;
	uint16_t i2 = i1 + (i & 1);
	if (i2 >= strip_length) i2 = 0;
	for (uint16_t si = 0; si < strip_length; si++) {
		if (si == i1 || si == i2) {
			strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
		} else {
			strip.setPixelColor(si, 0);
		}
		ci++; if (ci >= color_count) ci = 0;
	}
	strip.show();
}

static uint32_t running_helper(uint8_t ph, uint8_t ci) {
	uint8_t rv, gv, bv;
	switch (ph) {
		case 0:
			rv = 0;
			gv = 0;
			bv = 0;
			break;
		case 1:
		case 5:
			rv = r[ci] >> 2;
			gv = g[ci] >> 2;
			bv = b[ci] >> 2;
			break;
		case 2:
		case 4:
			rv = (r[ci] >> 2) + (r[ci] >> 1);
			gv = (g[ci] >> 2) + (g[ci] >> 1);
			bv = (b[ci] >> 2) + (b[ci] >> 1);
			break;
		default:
			rv = r[ci];
			gv = g[ci];
			bv = b[ci];
			break;
	}
	return strip.Color(rv, gv, bv);
}

static void cylon_helper(uint16_t f, uint8_t eyeSize) {
	uint16_t si;
	uint8_t ci;

	for (si = 0; si < strip_length; si++) strip.setPixelColor(si, 0);
	si = f; ci = f % color_count;

	strip.setPixelColor(si, strip.Color(r[ci] / 10, g[ci] / 10, b[ci] / 10));
	si++; ci++; if (ci >= color_count) ci = 0;

	while (eyeSize) {
		strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
		si++; ci++; if (ci >= color_count) ci = 0;
		eyeSize--;
	}

	strip.setPixelColor(si, strip.Color(r[ci] / 10, g[ci] / 10, b[ci] / 10));
	strip.show();
}

static void kitt_helper(uint16_t f, uint8_t eyeSize) {
	uint16_t n = strip_length - eyeSize - 2;
	uint16_t h = n >> 1;
	uint16_t hl = strip_length >> 1;
	uint16_t si1, si2;
	uint8_t ci1, ci2;

	for (si1 = 0; si1 < strip_length; si1++) strip.setPixelColor(si1, 0);
	si1 = (f < h) ? f : (n - f);
	si2 = strip_length - si1 - 1;
	ci1 = si1 % color_count;
	ci2 = si2 % color_count;

	strip.setPixelColor(si1, strip.Color(r[ci1] / 10, g[ci1] / 10, b[ci1] / 10));
	strip.setPixelColor(si2, strip.Color(r[ci2] / 10, g[ci2] / 10, b[ci2] / 10));
	si1++;
	si2--;
	if (si2 < hl) { strip.show(); return; }
	ci1++; if (ci1 >= color_count) ci1 = 0;
	ci2--; if (ci2 >= color_count) ci2 = color_count - 1;

	while (eyeSize) {
		strip.setPixelColor(si1, strip.Color(r[ci1], g[ci1], b[ci1]));
		strip.setPixelColor(si2, strip.Color(r[ci2], g[ci2], b[ci2]));
		si1++;
		si2--;
		if (si2 < hl) { strip.show(); return; }
		ci1++; if (ci1 >= color_count) ci1 = 0;
		ci2--; if (ci2 >= color_count) ci2 = color_count - 1;
		eyeSize--;
	}

	strip.setPixelColor(si1, strip.Color(r[ci1] / 10, g[ci1] / 10, b[ci1] / 10));
	strip.setPixelColor(si2, strip.Color(r[ci2] / 10, g[ci2] / 10, b[ci2] / 10));
	strip.show();
}

#define MAX_BOUNCING_BALLS (NPP_EFFECT_MAX_FRAMES_RAM * 4 / 16)

#define BALL_GRAVITY               (-9.81)
#define BALL_START_HEIGHT          (    1)
#define BALL_IMPACT_VELOCITY_START ( 4.43)

typedef struct npp_effect_bouncing_ball {
	float height;
	float impactVelocity;
	float dampening;
	uint32_t timeSinceBounce;
} npp_effect_bouncing_ball;

static npp_effect_bouncing_ball * balls = (npp_effect_bouncing_ball *)(void *)ch0;

static void bouncing_ball_helper(uint8_t ballCount, uint8_t multiColor) {
	float timeSinceBounce;
	uint16_t si;
	uint8_t ci;
	uint8_t i;

	if (ballCount > MAX_BOUNCING_BALLS) ballCount = MAX_BOUNCING_BALLS;

	if (!frame) {
		frame = 1;
		for (i = 0; i < ballCount; i++) {
			balls[i].height = BALL_START_HEIGHT;
			balls[i].impactVelocity = BALL_IMPACT_VELOCITY_START;
			balls[i].dampening = 0.90f - (float)i / ((float)ballCount * (float)ballCount);
			balls[i].timeSinceBounce = millis();
		}
	}

	for (si = 0; si < strip_length; si++) strip.setPixelColor(si, 0);
	for (i = 0; i < ballCount; i++) {
		timeSinceBounce = (millis() - balls[i].timeSinceBounce) / (float)1000;
		balls[i].height = (0.5 * BALL_GRAVITY * timeSinceBounce + balls[i].impactVelocity) * timeSinceBounce;
		if (balls[i].height < 0) {
			balls[i].height = 0;
			balls[i].impactVelocity *= balls[i].dampening;
			if (balls[i].impactVelocity < 0.01) {
				balls[i].impactVelocity = BALL_IMPACT_VELOCITY_START;
			}
			balls[i].timeSinceBounce = millis();
		}
		si = round(balls[i].height * (strip_length - 1) / BALL_START_HEIGHT);
		ci = (multiColor ? i : si) % color_count;
		strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
	}
	strip.show();
}

/* Private functions for the effects themselves. */

static void npp_effect_steady(void) {
	scroll_helper(0);
}

static void npp_effect_scroll_left(void) {
	scroll_helper(frame);
	frame_advance(50, color_count);
}

static void npp_effect_scroll_right(void) {
	scroll_helper(color_count - frame - 1);
	frame_advance(50, color_count);
}

static void npp_effect_blink(void) {
	if (frame) scroll_helper(0);
	else cycle_helper(0, 0, 0);
	frame_advance(1000, 2);
}

static void npp_effect_cycle(void) {
	cycle_helper(r[frame], g[frame], b[frame]);
	frame_advance(1000, color_count);
}

static void npp_effect_cycle_on_off(void) {
	if (frame & 1) {
		uint8_t ci = frame >> 1;
		cycle_helper(r[ci], g[ci], b[ci]);
	} else {
		cycle_helper(0, 0, 0);
	}
	frame_advance(1000, color_count << 1);
}

static void npp_effect_color_wipe(void) {
	uint8_t ci = frame / strip_length;
	uint16_t si = frame % strip_length;
	uint32_t c = strip.Color(r[ci], g[ci], b[ci]);
	while (si < strip_length) strip.setPixelColor(si--, c);
	strip.show();
	frame_advance(50, strip_length * color_count);
}

static void npp_effect_color_wipe_on_off(void) {
	uint8_t i = frame / strip_length;
	uint8_t ci = i >> 1;
	uint16_t si = frame % strip_length;
	uint32_t c = (i & 1) ? 0 : strip.Color(r[ci], g[ci], b[ci]);
	while (si < strip_length) strip.setPixelColor(si--, c);
	strip.show();
	frame_advance(50, strip_length * color_count * 2);
}

static void npp_effect_fast_fade_in_out(void) {
	uint8_t m = (frame & 0x100) ? (~frame) : frame;
	fade_helper(m, 1);
	frame_advance(3, 512);
}

static void npp_effect_fast_fade_cycle(void) {
	uint8_t c1 = frame >> 8;
	uint8_t c2 = c1 + 1; if (c2 >= color_count) c2 = 0;
	uint8_t sf = frame;
	uint8_t rv = color_blend(r[c1], r[c2], sf, 256);
	uint8_t gv = color_blend(g[c1], g[c2], sf, 256);
	uint8_t bv = color_blend(b[c1], b[c2], sf, 256);
	cycle_helper(rv, gv, bv);
	frame_advance(3, ((uint16_t)color_count) << 8);
}

static void npp_effect_fast_fade_cycle_in_out(void) {
	uint8_t ci = frame >> 9;
	uint8_t m = (frame & 0x100) ? (~frame) : frame;
	uint8_t rv = color_multiply(r[ci], m, 1);
	uint8_t gv = color_multiply(g[ci], m, 1);
	uint8_t bv = color_multiply(b[ci], m, 1);
	cycle_helper(rv, gv, bv);
	frame_advance(3, ((uint16_t)color_count) << 9);
}

static void npp_effect_slow_fade_in_out(void) {
	uint8_t m = (frame >> 8) & 0x03;
	switch (m) {
		case 0: m = frame; break;
		case 1: m = 255; break;
		case 2: m = ~frame; break;
		case 3: m = 0; break;
	}
	fade_helper(m, 1);
	frame_advance(5, 1024);
}

static void npp_effect_slow_fade_cycle(void) {
	uint8_t c1 = frame >> 9;
	uint8_t c2 = c1 + 1; if (c2 >= color_count) c2 = 0;
	uint8_t sf = (frame & 0x100) ? 255 : frame;
	uint8_t rv = color_blend(r[c1], r[c2], sf, 256);
	uint8_t gv = color_blend(g[c1], g[c2], sf, 256);
	uint8_t bv = color_blend(b[c1], b[c2], sf, 256);
	cycle_helper(rv, gv, bv);
	frame_advance(5, ((uint16_t)color_count) << 9);
}

static void npp_effect_slow_fade_cycle_in_out(void) {
	uint8_t ci = frame >> 10;
	uint8_t m = (frame >> 8) & 0x03;
	switch (m) {
		case 0: m = frame; break;
		case 1: m = 255; break;
		case 2: m = ~frame; break;
		case 3: m = 0; break;
	}
	uint8_t rv = color_multiply(r[ci], m, 1);
	uint8_t gv = color_multiply(g[ci], m, 1);
	uint8_t bv = color_multiply(b[ci], m, 1);
	cycle_helper(rv, gv, bv);
	frame_advance(5, ((uint16_t)color_count) << 10);
}

static void npp_effect_walk_left(void) {
	uint16_t n = strip_length << 1;
	uint16_t sf = n - frame - 1;
	walk_helper(sf);
	frame_advance(50, n);
}

static void npp_effect_walk_right(void) {
	walk_helper(frame);
	frame_advance(50, strip_length << 1);
}

static void npp_effect_walk_left_right(void) {
	uint16_t n = strip_length << 1;
	uint16_t sf = (frame < n) ? (n - frame - 1) : (frame - n);
	walk_helper(sf);
	frame_advance(50, n << 1);
}

static void npp_effect_hop_left(void) {
	uint16_t n = strip_length;
	uint16_t sf = n - frame - 1;
	walk_helper(sf << 1);
	frame_advance(50, n);
}

static void npp_effect_hop_right(void) {
	walk_helper(frame << 1);
	frame_advance(50, strip_length);
}

static void npp_effect_oscillate(void) {
	uint16_t n = strip_length;
	uint16_t sf = (frame < n) ? (n - frame - 1) : (frame - n);
	walk_helper(sf << 1);
	frame_advance(50, n << 1);
}

static void npp_effect_running_lights_a_left(void) {
	uint8_t ph = frame % 6, ci = 0;
	for (uint16_t si = 0; si < strip_length; si++) {
		strip.setPixelColor(si, running_helper(ph, ci));
		ph++; if (ph >= 6) ph = 0;
		ci++; if (ci >= color_count) ci = 0;
	}
	strip.show();
	frame_advance(50, 6);
}

static void npp_effect_running_lights_a_right(void) {
	uint8_t ph = 5 - (frame % 6), ci = 0;
	for (uint16_t si = 0; si < strip_length; si++) {
		strip.setPixelColor(si, running_helper(ph, ci));
		ph++; if (ph >= 6) ph = 0;
		ci++; if (ci >= color_count) ci = 0;
	}
	strip.show();
	frame_advance(50, 6);
}

static void npp_effect_running_lights_b_left(void) {
	uint8_t ph = frame % 6, ci = frame / 6;
	for (uint16_t si = 0; si < strip_length; si++) {
		strip.setPixelColor(si, running_helper(ph, ci));
		ph++;
		if (ph >= 6) {
			ph = 0;
			ci++;
			if (ci >= color_count) {
				ci = 0;
			}
		}
	}
	strip.show();
	frame_advance(50, color_count * 6);
}

static void npp_effect_running_lights_b_right(void) {
	uint8_t ph = 5 - (frame % 6), ci = color_count - (frame / 6) - 1;
	for (uint16_t si = 0; si < strip_length; si++) {
		strip.setPixelColor(si, running_helper(ph, ci));
		ph++;
		if (ph >= 6) {
			ph = 0;
			ci++;
			if (ci >= color_count) {
				ci = 0;
			}
		}
	}
	strip.show();
	frame_advance(50, color_count * 6);
}

static void npp_effect_twinkle(void) {
	if (frame_advanced()) {
		if (!frame) cycle_helper(0, 0, 0);
		uint16_t si = random(strip_length);
		uint8_t ci = si % color_count;
		strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
		strip.show();
	}
	frame_advance(100, (strip_length + 5) / 6);
}

static void npp_effect_twinkle_random(void) {
	if (frame_advanced()) {
		if (!frame) cycle_helper(0, 0, 0);
		uint16_t si = random(strip_length);
		uint8_t ci = random(color_count);
		strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
		strip.show();
	}
	frame_advance(100, (strip_length + 5) / 6);
}

static void npp_effect_twinkle_endless(void) {
	if (frame_advanced()) {
		uint16_t si = random(strip_length);
		uint8_t ci = random(color_count);
		strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
		strip.show();
	}
	frame_advance(100, 0);
}

#define MOODLIGHT_MAX_LEN (NPP_EFFECT_MAX_FRAMES_RAM << 1)

static void npp_effect_moodlight(void) {
	uint8_t i, rv, gv, bv;
	uint16_t si, n = strip_length;
	if (n > MOODLIGHT_MAX_LEN) n = MOODLIGHT_MAX_LEN;

	if (!frame) {
		frame = 256;
		for (si = 0; si < n; si++) {
			ch0[si] = random(color_count);
			ch2[si] = random(color_count);
		}
	}

	if (frame >= 512) {
		frame = 256;
		for (si = 0; si < n; si++) {
			ch0[si] = ch2[si];
			ch2[si] = random(color_count);
		}
	}

	i = frame;
	for (si = 0; si < n; si++) {
		rv = color_blend(r[ch0[si]], r[ch2[si]], i, 256);
		gv = color_blend(g[ch0[si]], g[ch2[si]], i, 256);
		bv = color_blend(b[ch0[si]], b[ch2[si]], i, 256);
		strip.setPixelColor(si, strip.Color(rv, gv, bv));
	}
	strip.show();

	frame_advance(3, 0);
}

static void npp_effect_sparkle(void) {
	uint16_t si = random(strip_length);
	uint8_t ci = si % color_count;
	cycle_helper(0, 0, 0);
	strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
	strip.show();
}

static void npp_effect_sparkle_random(void) {
	uint16_t si = random(strip_length);
	uint8_t ci = random(color_count);
	cycle_helper(0, 0, 0);
	strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
	strip.show();
}

static void npp_effect_snow_sparkle(void) {
	if (frame_advanced()) {
		fade_helper(16, 0);
		if (!frame) {
			uint16_t si = random(strip_length);
			uint8_t ci = si % color_count;
			strip.setPixelColor(si, strip.Color(r[ci], g[ci], b[ci]));
			strip.show();
		}
	}
	frame_advance(20, 11);
}

static void npp_effect_static(void) {
	uint16_t si;
	uint8_t ci;
	uint32_t c;
	for (si = 0; si < strip_length; si++) {
		ci = random(color_count + 1);
		c = (ci == color_count) ? 0 : strip.Color(r[ci], g[ci], b[ci]);
		strip.setPixelColor(si, c);
	}
	strip.show();
}

static void npp_effect_tail_chase_left(void) {
	uint32_t n = strip_length - 1;
	uint32_t i = frame;
	uint8_t ci = 0, m, rv, gv, bv;
	for (uint16_t si = 0; si < strip_length; si++) {
		m = ~(uint8_t)(255 * i / n);
		rv = color_multiply(r[ci], m, 1);
		gv = color_multiply(g[ci], m, 1);
		bv = color_multiply(b[ci], m, 1);
		strip.setPixelColor(si, strip.Color(rv, gv, bv));
		ci++; if (ci >= color_count) ci = 0;
		i++; if (i > n) i = 0;
	}
	strip.show();
	frame_advance(10, strip_length);
}

static void npp_effect_tail_chase_right(void) {
	uint32_t n = strip_length - 1;
	uint32_t i = frame;
	uint8_t ci = 0, m, rv, gv, bv;
	for (uint16_t si = 0; si < strip_length; si++) {
		m = ~(uint8_t)(255 * i / n);
		rv = color_multiply(r[ci], m, 1);
		gv = color_multiply(g[ci], m, 1);
		bv = color_multiply(b[ci], m, 1);
		strip.setPixelColor(si, strip.Color(rv, gv, bv));
		ci++; if (ci >= color_count) ci = 0;
		i--; if (i > n) i = n;
	}
	strip.show();
	frame_advance(10, strip_length);
}

static void npp_effect_cylon(void) {
	uint8_t eyeSize = (strip_length < 12) ? 1 : 4;
	uint16_t n1 = strip_length - eyeSize - 2;
	uint16_t n2 = n1 + 5;
	uint16_t n3 = n2 + n1;
	uint16_t n4 = n2 + n2;
	uint16_t f = (frame < n1) ? frame : (frame < n2) ? n1 : (frame < n3) ? (n3 - frame) : 0;
	cylon_helper(f, eyeSize);
	frame_advance(10, n4);
}

static void npp_effect_new_kitt(void) {
	uint8_t eyeSize = (strip_length < 20) ? 1 : 8;
	uint16_t n1 = strip_length - eyeSize - 2;
	uint16_t n2 = n1 + 5;
	uint16_t f = frame % n2;
	uint8_t s = frame / n2;
	switch (s) {
		case 0: case 4: cylon_helper((f < n1) ? (n1 - f) : 0, eyeSize); break;
		case 1: case 3: cylon_helper((f < n1) ? f : n1, eyeSize); break;
		case 2: case 5: kitt_helper((f < n1) ? f : 0, eyeSize); break;
	}
	frame_advance(10, n2 * 6);
}

static void npp_effect_bouncing_balls(void) {
	bouncing_ball_helper(3, 0);
}

static void npp_effect_multicolor_bouncing_balls(void) {
	bouncing_ball_helper(color_count, 1);
}

#define FIRE_MAX_LEN (NPP_EFFECT_MAX_FRAMES_RAM * 4)

#define FIRE_COOLING   55
#define FIRE_SPARKING 120
#define FIRE_SPEED     15

static void npp_effect_fire(void) {
	if (frame_advanced()) {
		uint16_t i, n = strip_length, cooldown;
		if (n > FIRE_MAX_LEN) n = FIRE_MAX_LEN;

		// Step 1. Cool down every cell a little.
		for (i = 0; i < n; i++) {
			cooldown = random(FIRE_COOLING * 10 / n + 2);
			if (cooldown > ch0[i]) {
				ch0[i] = 0;
			} else {
				ch0[i] -= cooldown;
			}
		}

		// Step 2. Heat from each cell drifts up and diffuses a little.
		for (i = n - 1; i >= 2; i--) {
			ch0[i] = (ch0[i - 1] + ch0[i - 2] + ch0[i - 2]) / 3;
		}

		// Step 3. Randomly ignite new sparks near the bottom.
		if (random(255) < FIRE_SPARKING) {
			i = random(7);
			ch0[i] += random(160, 255);
		}

		// Step 4. Convert heat to LED colors.
		for (i = 0; i < n; i++) {
			if (ch0[i] < 85) {
				strip.setPixelColor(i, strip.Color(ch0[i] * 3, 0, 0));
			} else if (ch0[i] < 170) {
				strip.setPixelColor(i, strip.Color(255, (ch0[i] - 85) * 3, 0));
			} else {
				strip.setPixelColor(i, strip.Color(255, 255, (ch0[i] - 170) * 3));
			}
		}

		strip.show();
	}
	frame_advance(FIRE_SPEED, 0);
}

static void npp_effect_meteor_rain(void) {
	if (frame_advanced()) {
		uint8_t meteorSize = (strip_length + 5) / 6;
		uint16_t i;
		union {
			uint32_t c;
			uint8_t cv[4];
		};
		uint16_t si;
		uint8_t ci;
		for (i = 0; i < strip_length; i++) {
			if (random(2)) {
				c = strip.getPixelColor(i);
				cv[0] = (cv[0] <= 10) ? 0 : (cv[0] - (cv[0] >> 2));
				cv[1] = (cv[1] <= 10) ? 0 : (cv[1] - (cv[1] >> 2));
				cv[2] = (cv[2] <= 10) ? 0 : (cv[2] - (cv[2] >> 2));
				cv[3] = (cv[3] <= 10) ? 0 : (cv[3] - (cv[3] >> 2));
				strip.setPixelColor(i, c);
			}
		}
		for (i = 0, si = frame; i < meteorSize; i++, si--) {
			if (si < strip_length) {
				ci = si % color_count;
				c = strip.Color(r[ci], g[ci], b[ci]);
				strip.setPixelColor(si, c);
			}
		}
		strip.show();
	}
	frame_advance(30, strip_length << 1);
}

static void npp_effect_mmxl(void) {
	uint8_t lv, rv, gv, bv, ci = 0, bi = 0;
	for (uint16_t si = 0; si < strip_length; si++) {
		switch (bi) {
			case 0: lv = ch0[frame]; break;
			case 1: lv = ch1[frame]; break;
			case 2: lv = ch2[frame]; break;
			case 3: lv = ch3[frame]; break;
		}
		rv = color_multiply(r[ci], lv, 1);
		gv = color_multiply(g[ci], lv, 1);
		bv = color_multiply(b[ci], lv, 1);
		strip.setPixelColor(si, strip.Color(rv, gv, bv));
		ci++; if (ci >= color_count) ci = 0;
		bi++; if (bi >= blink_channels) bi = 0;
	}
	strip.show();
	frame_advance(blink_msperframe, blink_frames);
}

/* Public interface. */

int npp_effect_setup(int eeaddr) {
	eestart = eeaddr;
	strip_length = strip.numPixels();

	effect_randomize = EEPROM.read(eestart);
	if (effect_randomize == 255) effect_randomize = 0;

	effect_mode = EEPROM.read(eestart + 1);
	if (effect_mode == 255) effect_mode = 0;

	color_preset = EEPROM.read(eestart + 2);
	if (color_preset == 255) {
		npp_load_color_pattern(color_preset = 6, &color_count, r, g, b);
	} else if (color_preset < NPP_COLOR_PATTERNS) {
		npp_load_color_pattern(color_preset, &color_count, r, g, b);
	} else {
		uint8_t i;
		int a = eestart + NPP_EFFECT_EEPROM_ADDR_COLORS;
		color_count = EEPROM.read(eestart + 3);
		for (i = 0; i < NPP_EFFECT_MAX_COLORS_EEPROM; i++) r[i] = EEPROM.read(a++);
		for (i = 0; i < NPP_EFFECT_MAX_COLORS_EEPROM; i++) g[i] = EEPROM.read(a++);
		for (i = 0; i < NPP_EFFECT_MAX_COLORS_EEPROM; i++) b[i] = EEPROM.read(a++);
	}

	blink_preset = EEPROM.read(eestart + 4);
	if (blink_preset == 255) {
		npp_load_blink_pattern(blink_preset = 1, &blink_msperframe, &blink_frames, &blink_channels, ch0, ch1, ch2, ch3);
	} else if (blink_preset < NPP_BLINK_PATTERNS) {
		npp_load_blink_pattern(blink_preset, &blink_msperframe, &blink_frames, &blink_channels, ch0, ch1, ch2, ch3);
	} else {
		uint8_t i;
		int a = eestart + NPP_EFFECT_EEPROM_ADDR_FRAMES;
		blink_msperframe = EEPROM.read(eestart + 5);
		blink_frames = EEPROM.read(eestart + 6);
		blink_channels = EEPROM.read(eestart + 7);
		for (i = 0; i < NPP_EFFECT_MAX_FRAMES_EEPROM; i++) ch0[i] = EEPROM.read(a++);
		for (i = 0; i < NPP_EFFECT_MAX_FRAMES_EEPROM; i++) ch1[i] = EEPROM.read(a++);
		for (i = 0; i < NPP_EFFECT_MAX_FRAMES_EEPROM; i++) ch2[i] = EEPROM.read(a++);
		for (i = 0; i < NPP_EFFECT_MAX_FRAMES_EEPROM; i++) ch3[i] = EEPROM.read(a++);
	}

	speed_adjust = EEPROM_read16(eestart + 8);
	if (speed_adjust == (uint16_t)(-1)) speed_adjust = 0x0100;

	randomize_interval = EEPROM_read32(eestart + 10);
	if (randomize_interval == (uint32_t)(-1)) randomize_interval = 60000;

	randomize_time = millis();
	frame_time = millis();
	frame = 0;
	frame_changed = 1;
	return eestart + NPP_EFFECT_EEPROM_ADDR_END;
}

void npp_effect_loop(void) {
	if (effect_randomize) {
		uint32_t m = millis();
		if (m - randomize_time >= randomize_interval) {
			randomize_time = m;
			npp_effect_set_mode(random(NPP_EFFECTS + NPP_BLINK_PATTERNS));
		}
	}
	switch (effect_mode) {
		case  0: npp_effect_steady                    (); break;
		case  1: npp_effect_scroll_left               (); break;
		case  2: npp_effect_scroll_right              (); break;
		case  3: npp_effect_blink                     (); break;
		case  4: npp_effect_cycle                     (); break;
		case  5: npp_effect_cycle_on_off              (); break;
		case  6: npp_effect_color_wipe                (); break;
		case  7: npp_effect_color_wipe_on_off         (); break;
		case  8: npp_effect_fast_fade_in_out          (); break;
		case  9: npp_effect_fast_fade_cycle           (); break;
		case 10: npp_effect_fast_fade_cycle_in_out    (); break;
		case 11: npp_effect_slow_fade_in_out          (); break;
		case 12: npp_effect_slow_fade_cycle           (); break;
		case 13: npp_effect_slow_fade_cycle_in_out    (); break;
		case 14: npp_effect_walk_left                 (); break;
		case 15: npp_effect_walk_right                (); break;
		case 16: npp_effect_walk_left_right           (); break;
		case 17: npp_effect_hop_left                  (); break;
		case 18: npp_effect_hop_right                 (); break;
		case 19: npp_effect_oscillate                 (); break;
		case 20: npp_effect_running_lights_a_left     (); break;
		case 21: npp_effect_running_lights_a_right    (); break;
		case 22: npp_effect_running_lights_b_left     (); break;
		case 23: npp_effect_running_lights_b_right    (); break;
		case 24: npp_effect_twinkle                   (); break;
		case 25: npp_effect_twinkle_random            (); break;
		case 26: npp_effect_twinkle_endless           (); break;
		case 27: npp_effect_moodlight                 (); break;
		case 28: npp_effect_sparkle                   (); break;
		case 29: npp_effect_sparkle_random            (); break;
		case 30: npp_effect_snow_sparkle              (); break;
		case 31: npp_effect_static                    (); break;
		case 32: npp_effect_tail_chase_left           (); break;
		case 33: npp_effect_tail_chase_right          (); break;
		case 34: npp_effect_cylon                     (); break;
		case 35: npp_effect_new_kitt                  (); break;
		case 36: npp_effect_bouncing_balls            (); break;
		case 37: npp_effect_multicolor_bouncing_balls (); break;
		case 38: npp_effect_fire                      (); break;
		case 39: npp_effect_meteor_rain               (); break;
		default: npp_effect_mmxl                      (); break;
	}
}

char * npp_effect_command(char * buf) {
	char * ret = buf;
	uint32_t v;
	float fv;
	switch (buf[0]) {
		case 'R':
			if (parse_int(&buf[1], &v)) {
				if (v < 2) npp_effect_set_randomize(v);
				else npp_effect_set_randomize_interval(v);
			}
			buf = fmt_int(buf, effect_randomize);
			*buf++ = ' ';
			buf = fmt_int(buf, randomize_interval);
			*buf = 0;
			return ret;
		case 'M':
			if (parse_int(&buf[1], &v)) npp_effect_set_mode(v % (NPP_EFFECTS + NPP_BLINK_PATTERNS));
			buf = fmt_int(buf, effect_mode);
			*buf = 0;
			return ret;
		case 'C':
			if (buf[1] == 'P') {
				if (parse_int(&buf[2], &v)) npp_effect_set_color_preset(v % NPP_COLOR_PATTERNS);
				buf = fmt_int(buf, color_preset);
			} else if (buf[1] == 'S' || buf[1] == 'X') {
				npp_effect_set_color_pattern(&buf[1]);
				buf = fmt_int(buf, color_count);
			} else if (buf[1] == 'T') {
				if (parse_int(&buf[2], &v) && v < color_count) {
					int ci = v;
					buf = fmt_hex(buf, r[ci]);
					buf = fmt_hex(buf, g[ci]);
					buf = fmt_hex(buf, b[ci]);
				} else {
					buf = fmt_int(buf, color_count);
				}
			} else {
				return 0;
			}
			*buf = 0;
			return ret;
		case 'B':
			if (buf[1] == 'P') {
				if (parse_int(&buf[2], &v)) npp_effect_set_blink_preset(v % NPP_BLINK_PATTERNS);
				buf = fmt_int(buf, blink_preset);
			} else if (buf[1] == 'S' || buf[1] == 'X') {
				npp_effect_set_blink_pattern(&buf[1]);
				buf = fmt_hex(buf, blink_msperframe);
				*buf++ = '0' + blink_channels;
				*buf++ = ' ';
				buf = fmt_int(buf, blink_frames);
			} else if (buf[1] == 'T') {
				if (parse_int(&buf[2], &v) && v < blink_frames) {
					int ci = v;
					if (blink_channels > 0) buf = fmt_hex(buf, ch0[ci]);
					if (blink_channels > 1) buf = fmt_hex(buf, ch1[ci]);
					if (blink_channels > 2) buf = fmt_hex(buf, ch2[ci]);
					if (blink_channels > 3) buf = fmt_hex(buf, ch3[ci]);
				} else {
					buf = fmt_hex(buf, blink_msperframe);
					*buf++ = '0' + blink_channels;
					*buf++ = ' ';
					buf = fmt_int(buf, blink_frames);
				}
			} else {
				return 0;
			}
			*buf = 0;
			return ret;
		case 'S':
			if (parse_float(&buf[1], &fv)) npp_effect_set_speed_adjust(fv * 256);
			buf = fmt_float(buf, (float)speed_adjust / (float)256);
			*buf = 0;
			return ret;
	}
	return 0;
}
