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
  pinMode(MODE_PIN2, INPUT_PULLUP);
  pinMode(MODE_PIN3, INPUT_PULLUP);
  pinMode(MODE_PIN4, INPUT_PULLUP);
  pinMode(MODE_PIN5, INPUT_PULLUP);
}

uint8_t read_digital(uint8_t * i) {
  uint8_t i0 = 0;
  if (!digitalRead(MODE_PIN2)) i0 |= 8;
  if (!digitalRead(MODE_PIN3)) i0 |= 4;
  if (!digitalRead(MODE_PIN4)) i0 |= 2;
  if (!digitalRead(MODE_PIN5)) i0 |= 1;
  if (*i == i0) return 0;
  *i = i0; return 1;
}

void init_analog() {
  pinMode(MODE_PIN2, INPUT);
  pinMode(MODE_PIN3, INPUT);
  pinMode(MODE_PIN4, INPUT);
  pinMode(MODE_PIN5, INPUT);
}

static uint8_t eq(uint16_t a, uint16_t b, uint16_t e) {
  return ((a < b) ? (b - a) : (a - b)) <= e;
}

uint8_t read_analog(uint16_t * w, uint16_t * r, uint16_t * g, uint16_t * b) {
  uint16_t w0 = analogRead(MODE_PIN2);
  uint16_t r0 = analogRead(MODE_PIN3);
  uint16_t g0 = analogRead(MODE_PIN4);
  uint16_t b0 = analogRead(MODE_PIN5);
  if (eq(*w,w0,1) && eq(*r,r0,1) && eq(*g,g0,1) && eq(*b,b0,1)) return 0;
  *w = w0; *r = r0; *g = g0; *b = b0; return 1;
}

void set_color(uint8_t w, uint8_t r, uint8_t g, uint8_t b) {
  Serial.print("W="); Serial.print(w); Serial.print(", ");
  Serial.print("R="); Serial.print(r); Serial.print(", ");
  Serial.print("G="); Serial.print(g); Serial.print(", ");
  Serial.print("B="); Serial.print(b); Serial.println();
  analogWrite(W_PIN, 255 - w);
  analogWrite(R_PIN, 255 - r);
  analogWrite(G_PIN, 255 - g);
  analogWrite(B_PIN, 255 - b);
}
