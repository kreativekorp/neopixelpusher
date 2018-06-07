#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include "npp_blink_patterns.h"
#include "npp_color_patterns.h"
#include "npp_color.h"
#include "npp_clock.h"
#include "npp_eeprom.h"
#include "npp_effects.h"
#include "npp_xlm.h"

#define NEOPIXEL_NUM  60
#define NEOPIXEL_PIN  6
#define NEOPIXEL_SET  (NEO_GRB + NEO_KHZ800)

#define FACTORY_RESET_PIN  5

#define PGM_EFFECTS  0
#define PGM_XLM      1
#define PGM_CLOCK    2
#define PGM_MAX      3

char buf[32];
uint8_t ptr = 0;
uint8_t pgm = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEOPIXEL_SET);

void setup() {
	randomSeed(analogRead(2));

	pinMode(FACTORY_RESET_PIN, INPUT_PULLUP);
	if (!digitalRead(FACTORY_RESET_PIN)) {
		for (int a = 0; a < 512; a++) {
			EEPROM_update(a, -1);
		}
	}

	int eeaddr = 16;
	eeaddr = npp_clock_setup(eeaddr);
	eeaddr = npp_xlm_setup(eeaddr);
	eeaddr = npp_effect_setup(eeaddr);

	Serial.begin(9600);
	Wire.begin();

	strip.begin();
	strip.show();
	strip.setBrightness(EEPROM.read(4));

	pgm = EEPROM.read(5);
	if (pgm >= PGM_MAX) pgm = PGM_EFFECTS;
}

void loop() {
	while (Serial.available() > 0) {
		char b = Serial.read();
		if (b == '\n' || b == '\r') {
			buf[ptr] = 0;
			char * res = npp_execute_command(buf);
			if (res) Serial.println(res);
			ptr = 0;
		} else if (b && ptr < 31) {
			buf[ptr] = b;
			ptr++;
		}
	}
	switch (pgm) {
		case PGM_EFFECTS: npp_effect_loop(); break;
		case PGM_XLM:     npp_xlm_loop();    break;
		case PGM_CLOCK:   npp_clock_loop();  break;
	}
}

char * npp_execute_command(char * buf) {
	switch (buf[0]) {
		case 'C':
			if (pgm != PGM_CLOCK) EEPROM_update(5, pgm = PGM_CLOCK);
			if (buf[1]) return npp_clock_command(&buf[1]);
			break;
		case 'E':
			if (pgm != PGM_EFFECTS) EEPROM_update(5, pgm = PGM_EFFECTS);
			if (buf[1]) return npp_effect_command(&buf[1]);
			break;
		case 'P':
			if (buf[1]) return npp_system_command(&buf[1]);
			break;
		case 'X':
			if (pgm != PGM_XLM) EEPROM_update(5, pgm = PGM_XLM);
			if (buf[1]) return npp_xlm_command(&buf[1]);
			break;
	}
	return 0;
}

char * npp_system_command(char * buf) {
	uint8_t v, i;
	uint16_t v16;
	switch (buf[0]) {
		case 'B':
			v = 0;
			for (i = 1; buf[i]; i++) {
				if (buf[i] >= '0' && buf[i] <= '9') {
					v = v * 10 + (buf[i] & 0xF);
				}
			}
			strip.setBrightness(v);
			EEPROM_update(4, v);
			break;
		case 'Z':
			v16 = free_ram();
			v = 8;
			do {
				buf[v] = '0' + (v16 % 10);
				v16 /= 10;
				v++;
			}
			while (v16);
			for (i = 0; v > 8; buf[i++] = buf[--v]);
			buf[i] = 0;
			return buf;
	}
	return 0;
}

int free_ram() {
  extern int __heap_start, * __brkval; int v;
  return (int)&v - ((__brkval == 0) ? (int)&__heap_start : (int)__brkval);
}
