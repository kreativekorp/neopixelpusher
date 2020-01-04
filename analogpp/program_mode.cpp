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
  long fade_time, long delay_time
) {
  long t, t0 = millis();
  while ((t = (millis() - t0)) < fade_time) {
    uint8_t m = 255L * t / fade_time;
    uint8_t w = color_multiply(w0, m, 1);
    uint8_t r = color_multiply(r0, m, 1);
    uint8_t g = color_multiply(g0, m, 1);
    uint8_t b = color_multiply(b0, m, 1);
    set_color(w, r, g, b);
    delay(delay_time);
  }
}

static void fade_out(
  uint8_t w0, uint8_t r0, uint8_t g0, uint8_t b0,
  long fade_time, long delay_time
) {
  long t, t0 = millis();
  while ((t = (millis() - t0)) < fade_time) {
    uint8_t m = 255L * (fade_time - t) / fade_time;
    uint8_t w = color_multiply(w0, m, 1);
    uint8_t r = color_multiply(r0, m, 1);
    uint8_t g = color_multiply(g0, m, 1);
    uint8_t b = color_multiply(b0, m, 1);
    set_color(w, r, g, b);
    delay(delay_time);
  }
}

void cycle_on_off(uint8_t * colors, long in_time, long on_time, long out_time, long off_time) {
  int n = *colors++;
  for (;;) {
    for (int i = 0; i < n; i++) {
      int p = i << 2;
      uint8_t w = colors[p++];
      uint8_t r = colors[p++];
      uint8_t g = colors[p++];
      uint8_t b = colors[p++];
      if (in_time) {
        fade_in(w, r, g, b, in_time, 10L);
      }
      if (on_time) {
        set_color(w, r, g, b);
        delay(on_time);
      }
      if (out_time) {
        fade_out(w, r, g, b, out_time, 10L);
      }
      if (off_time) {
        set_color(0, 0, 0, 0);
        delay(off_time);
      }
    }
  }
}



static void fade_between(
  uint8_t w0, uint8_t r0, uint8_t g0, uint8_t b0,
  uint8_t w1, uint8_t r1, uint8_t g1, uint8_t b1,
  long fade_time, long delay_time
) {
  long t, t0 = millis();
  while ((t = (millis() - t0)) < fade_time) {
    uint8_t w = color_blend(w0, w1, t, fade_time);
    uint8_t r = color_blend(r0, r1, t, fade_time);
    uint8_t g = color_blend(g0, g1, t, fade_time);
    uint8_t b = color_blend(b0, b1, t, fade_time);
    set_color(w, r, g, b);
    delay(delay_time);
  }
}

void cycle_between(uint8_t * colors, long hold_time, long fade_time) {
  int n = *colors++, i = 0, p = 0;
  uint8_t w0 = colors[p++];
  uint8_t r0 = colors[p++];
  uint8_t g0 = colors[p++];
  uint8_t b0 = colors[p++];
  for (;;) {
    if (hold_time) {
      set_color(w0, r0, g0, b0);
      delay(hold_time);
    }
    i++; if (i >= n) i = p = 0;
    uint8_t w1 = colors[p++];
    uint8_t r1 = colors[p++];
    uint8_t g1 = colors[p++];
    uint8_t b1 = colors[p++];
    if (fade_time) {
      fade_between(w0, r0, g0, b0, w1, r1, g1, b1, fade_time, 10L);
    }
    w0 = w1;
    r0 = r1;
    g0 = g1;
    b0 = b1;
  }
}

void gradient_fade(uint8_t * gradient, long time_base, long delay_time) {
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
    fade_between(w0, r0, g0, b0, w1, r1, g1, b1, fade_time, delay_time);
    w0 = w1;
    r0 = r1;
    g0 = g1;
    b0 = b1;
  }
}



void flicker(uint8_t w0, uint8_t r0, uint8_t g0, uint8_t b0) {
  for (;;) {
    uint8_t level = random(1) ? 255 : random(64, 256);
    uint8_t w = color_multiply(w0, level, 0);
    uint8_t r = color_multiply(r0, level, 0);
    uint8_t g = color_multiply(g0, level, 0);
    uint8_t b = color_multiply(b0, level, 0);
    set_color(w, r, g, b);
    delay(72);
  }
}



#define TB 1000L
#define SRSS_TIME_BASE 60000L
#define SRSS_DELAY_TIME 5000L

void run_program(uint8_t p) {
  switch (p) {
    // ------------------- colors ---- in  on  out off
    case 0:  cycle_on_off (warm_white, 0L, TB, 0L, TB); break;
    case 1:  cycle_on_off (rgb_white,  0L, TB, 0L, TB); break;
    case 2:  cycle_between(rainbow,        TB, 0L    ); break;
    case 3:  cycle_on_off (rainbow,    0L, TB, 0L, TB); break;
    case 4:  cycle_on_off (warm_white, TB, 0L, TB, 0L); break;
    case 5:  cycle_on_off (rgb_white,  TB, 0L, TB, 0L); break;
    case 6:  cycle_between(rainbow,        0L, TB    ); break;
    case 7:  cycle_on_off (rainbow,    TB, 0L, TB, 0L); break;
    case 8:  cycle_on_off (warm_white, TB, TB, TB, TB); break;
    case 9:  cycle_on_off (rgb_white,  TB, TB, TB, TB); break;
    case 10: cycle_between(rainbow,        TB, TB    ); break;
    case 11: cycle_on_off (rainbow,    TB, TB, TB, TB); break;
    case 12: flicker(255, CnBLK); break;
    case 13: flicker(0,   CnWHT); break;
    case 14: gradient_fade(sunrise_gradient, SRSS_TIME_BASE, SRSS_DELAY_TIME); break;
    case 15: gradient_fade(sunset_gradient,  SRSS_TIME_BASE, SRSS_DELAY_TIME); break;
  }
}

#undef TB
#undef SRSS_TIME_BASE
#undef SRSS_DELAY_TIME
