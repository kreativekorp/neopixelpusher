#include <Arduino.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "analogpp_lib.h"
#include "npp_color_patterns.h"
#include "npp_color.h"
#include "program_mode.h"

/* ---- DIGITAL MODE - Four switches select one of 16 colors. ---- */

static uint8_t digital_colors[] = {
  255, CnBLK,   0, CnWHT,   0, CnYEL,   0, CnGLD,
  0,   CnORG,   0, CnRED,   0, CnRSE,   0, CnMAG,
  0,   CnPUR,   0, CnVIO,   0, CnBLU,   0, CnAZR,
  0,   CnCYN,   0, CnAQU,   0, CnGRN,   0, CnCHR,
};

static uint8_t digital_i;

void digital_setup() {
  Serial.println("Digital Mode");
  init_digital();
  digital_i = -1;
}

void digital_loop() {
  if (read_digital(&digital_i)) {
    uint8_t p = digital_i << 2;
    uint8_t w = digital_colors[p++];
    uint8_t r = digital_colors[p++];
    uint8_t g = digital_colors[p++];
    uint8_t b = digital_colors[p++];
    set_color(w, r, g, b);
  }
}

/* ---- ANALOG MODE - Four potentiometers set levels of W, R, G, B. ---- */

static uint16_t analog_w;
static uint16_t analog_r;
static uint16_t analog_g;
static uint16_t analog_b;

void analog_setup() {
  Serial.println("Analog Mode");
  init_analog();
  analog_w = -1;
  analog_r = -1;
  analog_g = -1;
  analog_b = -1;
}

void analog_loop() {
  if (read_analog(&analog_w, &analog_r, &analog_g, &analog_b)) {
    uint8_t w = color_correct(analog_w >> 2);
    uint8_t r = color_correct(analog_r >> 2);
    uint8_t g = color_correct(analog_g >> 2);
    uint8_t b = color_correct(analog_b >> 2);
    set_color(w, r, g, b);
  }
}

/* ---- PROGRAM MODE - Four switches select one of 16 programs. ---- */

static uint8_t program_p;

void program_setup() {
  Serial.println("Program Mode");
  init_digital();
  program_p = -1;
}

void program_loop() {
  if (read_digital(&program_p)) {
    Serial.println(program_p);
    run_program_setup(program_p);
  }
  run_program_loop();
}

/* ---- SERIAL MODE - Serial port accepts commands. ---- */

static uint8_t serial_buf[256];
static uint8_t serial_ptr;
static uint8_t serial_pgm;
static uint8_t serial_adc;
static uint8_t serial_colors[32*4+1];

void serial_setup() {
  Serial.println("Serial Mode");
  serial_ptr = 0;
  serial_pgm = 0;
  serial_adc = 0;
}

static int free_ram() {
  extern int __heap_start, * __brkval; int v;
  return (int)&v - ((__brkval == 0) ? (int)&__heap_start : (int)__brkval);
}

static void serial_exec(uint8_t * buf) {
  uint8_t p = 0;
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
        serial_pgm = 0;
        break;
      case 'C': case 'c':
        w = color_correct(w);
        r = color_correct(r);
        g = color_correct(g);
        b = color_correct(b);
      case 'A': case 'a':
        set_color(w, r, g, b);
        serial_pgm = 0;
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
        run_program_setup(w);
        serial_pgm = 1;
        break;
      case 'F': case 'f':
        flicker_setup(w, r, g, b);
        serial_pgm = 2;
        break;
      case 'S': case 's':
        cycle_on_off_setup(serial_colors, 1000L*w, 1000L*r, 1000L*g, 1000L*b);
        serial_pgm = 3;
        break;
      case 'T': case 't':
        cycle_between_setup(serial_colors, 1000L*w, 1000L*r);
        serial_pgm = 4;
        break;
      case 'W': case 'w':
        w = color_correct(w);
        r = color_correct(r);
        g = color_correct(g);
        b = color_correct(b);
      case 'U': case 'u':
        serial_colors[0] = 1;
        serial_colors[1] = w;
        serial_colors[2] = r;
        serial_colors[3] = g;
        serial_colors[4] = b;
        break;
      case 'X': case 'x':
        w = color_correct(w);
        r = color_correct(r);
        g = color_correct(g);
        b = color_correct(b);
      case 'V': case 'v':
        p = (serial_colors[0]++) << 2;
        serial_colors[++p] = w;
        serial_colors[++p] = r;
        serial_colors[++p] = g;
        serial_colors[++p] = b;
        break;
      case 'Y': case 'y':
        load_colors(w, serial_colors);
        break;
      case 'Q': case 'q':
        if (serial_adc != 1) {
          serial_adc = 1;
          init_digital();
        }
        /* Without a new block here the rest of the switch block never executes. Compiler bug? */
        if (1) {
          uint8_t i = -1;
          read_digital(&i);
          Serial.println(i);
        }
        break;
      case 'R': case 'r':
        if (serial_adc != 2) {
          serial_adc = 2;
          init_analog();
        }
        /* Without a new block here the rest of the switch block never executes. Compiler bug? */
        if (1) {
          uint16_t i0 = -1;
          uint16_t i1 = -1;
          uint16_t i2 = -1;
          uint16_t i3 = -1;
          read_analog(&i0, &i1, &i2, &i3);
          Serial.print(i0); Serial.print(", ");
          Serial.print(i1); Serial.print(", ");
          Serial.print(i2); Serial.print(", ");
          Serial.print(i3); Serial.println();
        }
        break;
      case 'Z': case 'z':
        Serial.println(free_ram());
        break;
    }
  }
}

void serial_loop() {
  int ch;
  while ((ch = Serial.read()) > 0) {
    if (ch == '\r' || ch == '\n') {
      serial_buf[serial_ptr] = 0;
      serial_exec(serial_buf);
      serial_ptr = 0;
    } else if (serial_ptr < 255) {
      serial_buf[serial_ptr] = ch;
      serial_ptr++;
    }
  }
  switch (serial_pgm) {
    case 1: run_program_loop();   break;
    case 2: flicker_loop();       break;
    case 3: cycle_on_off_loop();  break;
    case 4: cycle_between_loop(); break;
  }
}

/* ---- MAIN ---- */

void setup() {
  Serial.begin(9600);
  Serial.println("AnalogPixelPusher v2.0");
  init_mode();
}

void loop() {
  switch (read_mode()) {
    case 0: digital_setup(); while (read_mode() == 0) digital_loop(); break;
    case 1: analog_setup();  while (read_mode() == 1) analog_loop();  break;
    case 2: program_setup(); while (read_mode() == 2) program_loop(); break;
    case 3: serial_setup();  while (read_mode() == 3) serial_loop();  break;
  }
}
