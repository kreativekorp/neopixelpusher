#include <Arduino.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "analogpp_lib.h"
#include "npp_color.h"
#include "program_mode.h"

static uint8_t digital_colors[] = {
  255, CnBLK,   0, CnWHT,   0, CnYEL,   0, CnGLD,
  0,   CnORG,   0, CnRED,   0, CnRSE,   0, CnMAG,
  0,   CnPUR,   0, CnVIO,   0, CnBLU,   0, CnAZR,
  0,   CnCYN,   0, CnAQU,   0, CnGRN,   0, CnCHR,
};

void digital_mode() {
  uint8_t i = -1;
  Serial.println("Digital Mode");
  init_digital();
  for (;;) {
    if (read_digital(&i)) {
      uint8_t p = i << 2;
      uint8_t w = digital_colors[p++];
      uint8_t r = digital_colors[p++];
      uint8_t g = digital_colors[p++];
      uint8_t b = digital_colors[p++];
      set_color(w, r, g, b);
    }
  }
}

void analog_mode() {
  uint16_t w = -1;
  uint16_t r = -1;
  uint16_t g = -1;
  uint16_t b = -1;
  Serial.println("Analog Mode");
  init_analog();
  for (;;) {
    if (read_analog(&w, &r, &g, &b)) {
      uint8_t w0 = color_correct(w >> 2);
      uint8_t r0 = color_correct(r >> 2);
      uint8_t g0 = color_correct(g >> 2);
      uint8_t b0 = color_correct(b >> 2);
      set_color(w0, r0, g0, b0);
    }
  }
}

void program_mode() {
  uint8_t p = -1;
  Serial.print("Program Mode ");
  init_digital();
  read_digital(&p);
  Serial.println(p);
  run_program(p);
}

void serial_mode() {
  uint8_t buf[256];
  uint8_t p = 0;
  Serial.println("Serial Mode");
  for (;;) {
    int ch = Serial.read();
    if (ch > 0) {
      if (ch == '\r' || ch == '\n') {
        buf[p] = 0; p = 0;
        while (buf[p] && buf[p] <= 32) p++;
        if (buf[p]) {
          uint8_t cmd = buf[p++];
          uint8_t w = 0, r = 0, g = 0, b = 0;
          while (buf[p] && (buf[p] < '0' || buf[p] > '9')) p++;
          while (buf[p] >= '0' && buf[p] <= '9') w = w * 10 + (buf[p++] - '0');
          while (buf[p] && (buf[p] < '0' || buf[p] > '9')) p++;
          while (buf[p] >= '0' && buf[p] <= '9') r = r * 10 + (buf[p++] - '0');
          while (buf[p] && (buf[p] < '0' || buf[p] > '9')) p++;
          while (buf[p] >= '0' && buf[p] <= '9') g = g * 10 + (buf[p++] - '0');
          while (buf[p] && (buf[p] < '0' || buf[p] > '9')) p++;
          while (buf[p] >= '0' && buf[p] <= '9') b = b * 10 + (buf[p++] - '0');
          switch (cmd) {
            case 'D': case 'd':
              p = w << 2;
              w = digital_colors[p++];
              r = digital_colors[p++];
              g = digital_colors[p++];
              b = digital_colors[p++];
              set_color(w, r, g, b);
              break;
            case 'C': case 'c':
              w = color_correct(w);
              r = color_correct(r);
              g = color_correct(g);
              b = color_correct(b);
            case 'A': case 'a':
              set_color(w, r, g, b);
              break;
            case 'G': case 'g':
              w = color_correct(w);
              r = color_correct(r);
              g = color_correct(g);
              b = color_correct(b);
            case 'E': case 'e':
              Serial.print("W="); Serial.print(w); Serial.print(", ");
              Serial.print("R="); Serial.print(r); Serial.print(", ");
              Serial.print("G="); Serial.print(g); Serial.print(", ");
              Serial.print("B="); Serial.print(b); Serial.println();
              break;
            case 'M': case 'm':
              Serial.println(color_multiply(w, r, g));
              break;
            case 'B': case 'b':
              Serial.println(color_blend(w, r, g, b));
              break;
            case 'P': case 'p':
              run_program(w);
              break;
            case 'F': case 'f':
              flicker(w, r, g, b);
              break;
          }
        }
        p = 0;
      } else if (p < 255) {
        buf[p] = ch;
        p++;
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("AnalogPixelPusher v1.0");
  init_mode();
  switch (read_mode()) {
    case 0: digital_mode(); break;
    case 1: analog_mode();  break;
    case 2: program_mode(); break;
    case 3: serial_mode();  break;
  }
}

void loop() {}
