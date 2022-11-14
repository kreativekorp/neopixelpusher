#include <Arduino.h>
#include <inttypes.h>
#include "analogpp_lib.h"

void init_mode() {
  pinMode(MODE_PIN0, INPUT_PULLUP);
  pinMode(MODE_PIN1, INPUT_PULLUP);
}

uint8_t read_mode() {
  uint8_t m = 0;
  if (!digitalRead(MODE_PIN0)) m |= 2;
  if (!digitalRead(MODE_PIN1)) m |= 1;
  return m;
}

void init_digital() {
  pinMode(MODE_DPIN2, INPUT_PULLUP);
  pinMode(MODE_DPIN3, INPUT_PULLUP);
  pinMode(MODE_DPIN4, INPUT_PULLUP);
  pinMode(MODE_DPIN5, INPUT_PULLUP);
}

uint8_t read_digital(uint8_t * i) {
  uint8_t i0 = 0;

#ifdef READ_DIGITAL_REVERSE
  if (!digitalRead(MODE_DPIN2)) i0 |= 1;
  if (!digitalRead(MODE_DPIN3)) i0 |= 2;
  if (!digitalRead(MODE_DPIN4)) i0 |= 4;
  if (!digitalRead(MODE_DPIN5)) i0 |= 8;
#else
  if (!digitalRead(MODE_DPIN2)) i0 |= 8;
  if (!digitalRead(MODE_DPIN3)) i0 |= 4;
  if (!digitalRead(MODE_DPIN4)) i0 |= 2;
  if (!digitalRead(MODE_DPIN5)) i0 |= 1;
#endif

#ifdef READ_DIGITAL_INVERT
  i0 ^= 15;
#endif

#ifdef READ_DIGITAL_DEGRAY
  if (i0 & 8) i0 ^= 4;
  if (i0 & 4) i0 ^= 2;
  if (i0 & 2) i0 ^= 1;
#endif

  if (*i == i0) return 0;
  *i = i0; return 1;
}

void init_analog() {
  pinMode(MODE_APIN2, INPUT);
  pinMode(MODE_APIN3, INPUT);
  pinMode(MODE_APIN4, INPUT);
  pinMode(MODE_APIN5, INPUT);
}

static uint8_t eq(uint16_t a, uint16_t b, uint16_t e) {
  return ((a < b) ? (b - a) : (a - b)) <= e;
}

uint8_t read_analog(uint16_t * w, uint16_t * r, uint16_t * g, uint16_t * b) {
  uint16_t w0 = analogRead(MODE_APIN2);
  uint16_t r0 = analogRead(MODE_APIN3);
  uint16_t g0 = analogRead(MODE_APIN4);
  uint16_t b0 = analogRead(MODE_APIN5);
  if (eq(*w,w0,1) && eq(*r,r0,1) && eq(*g,g0,1) && eq(*b,b0,1)) return 0;
  *w = w0; *r = r0; *g = g0; *b = b0; return 1;
}

static uint8_t unset = 1;
static uint8_t last_w = 0;
static uint8_t last_r = 0;
static uint8_t last_g = 0;
static uint8_t last_b = 0;

void set_color(uint8_t w, uint8_t r, uint8_t g, uint8_t b) {
  if (unset || last_w != w || last_r != r || last_g != g || last_b != b) {
    unset = 0; last_w = w; last_r = r; last_g = g; last_b = b;

#ifdef SET_COLOR_DEBUG
    Serial.print("W="); Serial.print(w); Serial.print(", ");
    Serial.print("R="); Serial.print(r); Serial.print(", ");
    Serial.print("G="); Serial.print(g); Serial.print(", ");
    Serial.print("B="); Serial.print(b); Serial.println();
#endif

#if defined(INVERT_W) || defined(INVERT_WRGB) || defined(INVERT_RGBW)
    analogWrite(W_PIN, 255-w);
#else
    analogWrite(W_PIN, w);
#endif

#if defined(INVERT_R) || defined(INVERT_WRGB) || defined(INVERT_RGBW) || defined(INVERT_RGB)
    analogWrite(R_PIN, 255-r);
#else
    analogWrite(R_PIN, r);
#endif

#if defined(INVERT_G) || defined(INVERT_WRGB) || defined(INVERT_RGBW) || defined(INVERT_RGB)
    analogWrite(G_PIN, 255-g);
#else
    analogWrite(G_PIN, g);
#endif

#if defined(INVERT_B) || defined(INVERT_WRGB) || defined(INVERT_RGBW) || defined(INVERT_RGB)
    analogWrite(B_PIN, 255-b);
#else
    analogWrite(B_PIN, b);
#endif

  }
}
