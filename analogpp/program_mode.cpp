#include <Arduino.h>
#include <inttypes.h>
#include "analogpp_lib.h"
#include "npp_color.h"
#include "program_mode.h"

static uint8_t warm_white[] = {
  1,
  255, CnBLK,
};

static uint8_t rgb_white[] = {
  1,
  0, CnWHT,
};

static uint8_t rainbow[] = {
  8,
  0, CnRED, 0, CnORG, 0, CnYEL, 0, CnGRN,
  0, CnCYN, 0, CnBLU, 0, CnPUR, 0, CnMAG,
};

static uint8_t sunrise_gradient[] = {
  0,   7,   2,   10,  30, /* 64,  40,  74, */
  0,   7,   2,   10,  30, /* 64,  40,  74, */
  0,   32,  8,   11,  30, /* 115, 67,  75, */
  0,   102, 11,  2,   30, /* 179, 77,  37, */
  0,   218, 41,  0,   30, /* 240, 126, 7,  */
  0,   235, 178, 15,  30, /* 247, 222, 85, */
  255, 0,   0,   0,   30,
  255, 0,   0,   0,   0,
};

static uint8_t sunset_gradient[] = {
  255, 0,   0,   0,   30,
  255, 0,   0,   0,   30,
  0,   235, 178, 15,  30, /* 247, 222, 85, */
  0,   218, 41,  0,   30, /* 240, 126, 7,  */
  0,   102, 11,  2,   30, /* 179, 77,  37, */
  0,   32,  8,   11,  30, /* 115, 67,  75, */
  0,   7,   2,   10,  30, /* 64,  40,  74, */
  0,   7,   2,   10,  0,  /* 64,  40,  74, */
};



static void fade_in(
  uint8_t w0, uint8_t r0, uint8_t g0, uint8_t b0,
  long fade_time, long t
) {
  uint8_t m = 255L * t / fade_time;
  uint8_t w = color_multiply(w0, m, 1);
  uint8_t r = color_multiply(r0, m, 1);
  uint8_t g = color_multiply(g0, m, 1);
  uint8_t b = color_multiply(b0, m, 1);
  set_color(w, r, g, b);
}

static void fade_out(
  uint8_t w0, uint8_t r0, uint8_t g0, uint8_t b0,
  long fade_time, long t
) {
  uint8_t m = 255L * (fade_time - t) / fade_time;
  uint8_t w = color_multiply(w0, m, 1);
  uint8_t r = color_multiply(r0, m, 1);
  uint8_t g = color_multiply(g0, m, 1);
  uint8_t b = color_multiply(b0, m, 1);
  set_color(w, r, g, b);
}

static void fade_between(
  uint8_t w0, uint8_t r0, uint8_t g0, uint8_t b0,
  uint8_t w1, uint8_t r1, uint8_t g1, uint8_t b1,
  long fade_time, long t
) {
  uint8_t w = color_blend(w0, w1, t, fade_time);
  uint8_t r = color_blend(r0, r1, t, fade_time);
  uint8_t g = color_blend(g0, g1, t, fade_time);
  uint8_t b = color_blend(b0, b1, t, fade_time);
  set_color(w, r, g, b);
}



static uint8_t cycle_on_off(uint8_t * colors, long in_time, long on_time, long out_time, long off_time, long t) {
  if (t < 0) return 0;
  int n = *colors++;
  for (int i = 0; i < n; i++) {
    uint8_t w = *colors++;
    uint8_t r = *colors++;
    uint8_t g = *colors++;
    uint8_t b = *colors++;
    if (t < in_time) {
      fade_in(w, r, g, b, in_time, t);
      return 1;
    }
    t -= in_time;
    if (t < on_time) {
      set_color(w, r, g, b);
      return 1;
    }
    t -= on_time;
    if (t < out_time) {
      fade_out(w, r, g, b, out_time, t);
      return 1;
    }
    t -= out_time;
    if (t < off_time) {
      set_color(0, 0, 0, 0);
      return 1;
    }
    t -= off_time;
  }
  return 0;
}

static uint8_t * coo_colors;
static long coo_in_time;
static long coo_on_time;
static long coo_out_time;
static long coo_off_time;
static long coo_millis;

void cycle_on_off_setup(uint8_t * colors, long in_time, long on_time, long out_time, long off_time) {
  coo_colors = colors;
  coo_in_time = in_time;
  coo_on_time = on_time;
  coo_out_time = out_time;
  coo_off_time = off_time;
  coo_millis = millis();
}

void cycle_on_off_loop() {
  if (!cycle_on_off(
    coo_colors,
    coo_in_time,
    coo_on_time,
    coo_out_time,
    coo_off_time,
    millis() - coo_millis
  )) coo_millis = millis();
}



static uint8_t cycle_between(uint8_t * colors, long hold_time, long fade_time, long t) {
  if (t < 0) return 0;
  int n = *colors++, i = 0, p = 0;
  uint8_t w0 = colors[p++];
  uint8_t r0 = colors[p++];
  uint8_t g0 = colors[p++];
  uint8_t b0 = colors[p++];
  while (i < n) {
    if (t < hold_time) {
      set_color(w0, r0, g0, b0);
      return 1;
    }
    t -= hold_time;
    i++; if (i >= n) p = 0;
    uint8_t w1 = colors[p++];
    uint8_t r1 = colors[p++];
    uint8_t g1 = colors[p++];
    uint8_t b1 = colors[p++];
    if (t < fade_time) {
      fade_between(w0, r0, g0, b0, w1, r1, g1, b1, fade_time, t);
      return 1;
    }
    t -= fade_time;
    w0 = w1;
    r0 = r1;
    g0 = g1;
    b0 = b1;
  }
  return 0;
}

static uint8_t * cbw_colors;
static long cbw_hold_time;
static long cbw_fade_time;
static long cbw_millis;

void cycle_between_setup(uint8_t * colors, long hold_time, long fade_time) {
  cbw_colors = colors;
  cbw_hold_time = hold_time;
  cbw_fade_time = fade_time;
  cbw_millis = millis();
}

void cycle_between_loop() {
  if (!cycle_between(
    cbw_colors,
    cbw_hold_time,
    cbw_fade_time,
    millis() - cbw_millis
  )) cbw_millis = millis();
}



static uint8_t gradient_fade(uint8_t * gradient, long time_base, long delay_time, long t) {
  if (t < 0) return 0;
  uint8_t w0 = *gradient++;
  uint8_t r0 = *gradient++;
  uint8_t g0 = *gradient++;
  uint8_t b0 = *gradient++;
  while (*gradient) {
    long fade_time = (*gradient++) * time_base;
    uint8_t w1 = *gradient++;
    uint8_t r1 = *gradient++;
    uint8_t g1 = *gradient++;
    uint8_t b1 = *gradient++;
    if (t < fade_time) {
      fade_between(w0, r0, g0, b0, w1, r1, g1, b1, fade_time, t);
      return 1;
    }
    t -= fade_time;
    w0 = w1;
    r0 = r1;
    g0 = g1;
    b0 = b1;
  }
  return 1;
}

static uint8_t * gf_gradient;
static long gf_time_base;
static long gf_delay_time;
static long gf_millis;

void gradient_fade_setup(uint8_t * gradient, long time_base, long delay_time) {
  gf_gradient = gradient;
  gf_time_base = time_base;
  gf_delay_time = delay_time;
  gf_millis = millis();
}

void gradient_fade_loop() {
  if (!gradient_fade(
    gf_gradient,
    gf_time_base,
    gf_delay_time,
    millis() - gf_millis
  )) gf_millis = millis();
}



static uint8_t flicker_w;
static uint8_t flicker_r;
static uint8_t flicker_g;
static uint8_t flicker_b;
static uint32_t flicker_millis;

void flicker_setup(uint8_t w, uint8_t r, uint8_t g, uint8_t b) {
  flicker_w = w;
  flicker_r = r;
  flicker_g = g;
  flicker_b = b;
  flicker_millis = millis() - 72;
}

void flicker_loop() {
  if ((millis() - flicker_millis) >= 72) {
    flicker_millis = millis();
    uint8_t level = random(2) ? 255 : random(64, 256);
    uint8_t w = color_multiply(flicker_w, level, 0);
    uint8_t r = color_multiply(flicker_r, level, 0);
    uint8_t g = color_multiply(flicker_g, level, 0);
    uint8_t b = color_multiply(flicker_b, level, 0);
    set_color(w, r, g, b);
  }
}



#define TB 1000L
#define SRSS_TIME_BASE 60000L
#define SRSS_DELAY_TIME 5000L

static uint8_t pgm_num;

void run_program_setup(uint8_t p) {
  switch ((pgm_num = p)) {
    case 0:  cycle_on_off_setup (warm_white, 0L, TB, 0L, TB); break;
    case 1:  cycle_on_off_setup (rgb_white,  0L, TB, 0L, TB); break;
    case 2:  cycle_between_setup(rainbow,        TB, 0L    ); break;
    case 3:  cycle_on_off_setup (rainbow,    0L, TB, 0L, TB); break;
    case 4:  cycle_on_off_setup (warm_white, TB, 0L, TB, 0L); break;
    case 5:  cycle_on_off_setup (rgb_white,  TB, 0L, TB, 0L); break;
    case 6:  cycle_between_setup(rainbow,        0L, TB    ); break;
    case 7:  cycle_on_off_setup (rainbow,    TB, 0L, TB, 0L); break;
    case 8:  cycle_on_off_setup (warm_white, TB, TB, TB, TB); break;
    case 9:  cycle_on_off_setup (rgb_white,  TB, TB, TB, TB); break;
    case 10: cycle_between_setup(rainbow,        TB, TB    ); break;
    case 11: cycle_on_off_setup (rainbow,    TB, TB, TB, TB); break;
    case 12: flicker_setup      (255, CnBLK); break;
    case 13: flicker_setup      (0,   CnWHT); break;
    case 14: gradient_fade_setup(sunrise_gradient, SRSS_TIME_BASE, SRSS_DELAY_TIME); break;
    case 15: gradient_fade_setup(sunset_gradient,  SRSS_TIME_BASE, SRSS_DELAY_TIME); break;
  }
}

void run_program_loop() {
  switch (pgm_num) {
    case 0:  cycle_on_off_loop();  break;
    case 1:  cycle_on_off_loop();  break;
    case 2:  cycle_between_loop(); break;
    case 3:  cycle_on_off_loop();  break;
    case 4:  cycle_on_off_loop();  break;
    case 5:  cycle_on_off_loop();  break;
    case 6:  cycle_between_loop(); break;
    case 7:  cycle_on_off_loop();  break;
    case 8:  cycle_on_off_loop();  break;
    case 9:  cycle_on_off_loop();  break;
    case 10: cycle_between_loop(); break;
    case 11: cycle_on_off_loop();  break;
    case 12: flicker_loop();       break;
    case 13: flicker_loop();       break;
    case 14: gradient_fade_loop(); break;
    case 15: gradient_fade_loop(); break;
  }
}

#undef TB
#undef SRSS_TIME_BASE
#undef SRSS_DELAY_TIME
