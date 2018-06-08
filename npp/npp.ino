#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BluefruitLE_UART.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include "npp_blink_patterns.h"
#include "npp_color_patterns.h"
#include "npp_color.h"
#include "npp_clock.h"
#include "npp_eeprom.h"
#include "npp_effects.h"
#include "npp_xlm.h"


// BEGIN CONFIGURATION


// NEOPIXEL SETTINGS

#define NEOPIXEL_NUM  60
#define NEOPIXEL_PIN  6
#define NEOPIXEL_SET  (NEO_GRB + NEO_KHZ800)


// SERIAL SETTINGS

// Comment to disable serial.
#define NPP_SERIAL_NAME   Serial
#define NPP_SERIAL_SPEED  9600


// BLUEFRUIT UART SETTINGS

// Uncomment to enable Bluefruit UART.
// Comment to disable Bluefruit UART.
// #define BLUEFRUIT_UART_MODE_PIN  12

// Uncomment for software UART.
// Comment for hardware UART.
// #define BLUEFRUIT_UART_RXD_PIN  9
// #define BLUEFRUIT_UART_TXD_PIN  10
// #define BLUEFRUIT_UART_CTS_PIN  11
// #define BLUEFRUIT_UART_RTS_PIN  -1

// Uncomment for hardware UART.
// Comment for software UART.
// #define BLUEFRUIT_UART_SERIAL  Serial1


// BLUEFRUIT SPI SETTINGS

// Uncomment to enable Bluefruit SPI.
// Comment to disable Bluefruit SPI.
// #define BLUEFRUIT_SPI_CS   8
// #define BLUEFRUIT_SPI_IRQ  7
// #define BLUEFRUIT_SPI_RST  4

// Uncomment for software SPI.
// Comment for hardware SPI.
// #define BLUEFRUIT_SPI_SCK   13
// #define BLUEFRUIT_SPI_MISO  12
// #define BLUEFRUIT_SPI_MOSI  11


// NPP-SPECIFIC SETTINGS

// Comment to disable "factory reset" pin.
#define FACTORY_RESET_PIN  5

// Comment any of these to remove that mode.
#define PGM_DEFAULT  0
#define PGM_EFFECTS  0
#define PGM_XLM      1
#define PGM_CLOCK    2
#define PGM_MAX      3


// END CONFIGURATION


char buf[32];
uint8_t ptr = 0;
uint8_t pgm = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEOPIXEL_SET);

#ifdef BLUEFRUIT_UART_MODE_PIN
#ifdef BLUEFRUIT_UART_SERIAL

#define BLUEFRUIT_ENABLED
Adafruit_BluefruitLE_UART ble(BLUEFRUIT_UART_SERIAL, BLUEFRUIT_UART_MODE_PIN);

#else
#if defined(BLUEFRUIT_UART_TXD_PIN) && defined(BLUEFRUIT_UART_RXD_PIN) && defined(BLUEFRUIT_UART_CTS_PIN) && defined(BLUEFRUIT_UART_RTS_PIN)

#define BLUEFRUIT_ENABLED
#include <SoftwareSerial.h>
SoftwareSerial bleSS = SoftwareSerial(BLUEFRUIT_UART_TXD_PIN, BLUEFRUIT_UART_RXD_PIN);
Adafruit_BluefruitLE_UART ble(bleSS, BLUEFRUIT_UART_MODE_PIN, BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);

#endif
#endif
#endif

#if defined(BLUEFRUIT_SPI_CS) && defined(BLUEFRUIT_SPI_IRQ) && defined(BLUEFRUIT_SPI_RST)
#if defined(BLUEFRUIT_SPI_SCK) && defined(BLUEFRUIT_SPI_MISO) && defined(BLUEFRUIT_SPI_MOSI)

#define BLUEFRUIT_ENABLED
Adafruit_BluefruitLE_SPI ble(
	BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO, BLUEFRUIT_SPI_MOSI,
	BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST
);

#else

#define BLUEFRUIT_ENABLED
Adafruit_BluefruitLE_SPI ble(
	BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST
);

#endif
#endif

void setup() {
	randomSeed(analogRead(2));

#ifdef FACTORY_RESET_PIN
	pinMode(FACTORY_RESET_PIN, INPUT_PULLUP);
	if (!digitalRead(FACTORY_RESET_PIN)) {
		for (int a = 0; a < 512; a++) {
			EEPROM_update(a, -1);
		}
	}
#endif

	int eeaddr = 16;

#ifdef PGM_CLOCK
	eeaddr = npp_clock_setup(eeaddr);
#else
	eeaddr += 16;
#endif

#ifdef PGM_XLM
	eeaddr = npp_xlm_setup(eeaddr);
#else
	eeaddr += 32;
#endif

#ifdef PGM_EFFECTS
	eeaddr = npp_effect_setup(eeaddr);
#else
	eeaddr += 368;
#endif

#ifdef NPP_SERIAL_NAME
	NPP_SERIAL_NAME.begin(NPP_SERIAL_SPEED);
#endif

#ifdef BLUEFRUIT_ENABLED
	ble.begin(false);
	ble.echo(false);
#endif

#ifdef PGM_CLOCK
	Wire.begin();
#endif

	strip.begin();
	strip.show();
	strip.setBrightness(EEPROM.read(4));

	pgm = EEPROM.read(5);
	if (pgm >= PGM_MAX) pgm = PGM_DEFAULT;
}

void loop() {

#ifdef NPP_SERIAL_NAME
	while (NPP_SERIAL_NAME.available() > 0) {
		char b = NPP_SERIAL_NAME.read();
		if (b == '\n' || b == '\r') {
			buf[ptr] = 0;
			char * res = npp_execute_command(buf);
			if (res) NPP_SERIAL_NAME.println(res);
			ptr = 0;
		} else if (b && ptr < 31) {
			buf[ptr] = b;
			ptr++;
		}
	}
#endif

#ifdef BLUEFRUIT_ENABLED
	ble.println("AT+BLEUARTRX");
	if (ble.readline(buf, 32) && strcmp(buf, "OK") && strcmp(buf, "ERROR")) {
		char * res = npp_execute_command(buf);
		if (res) {
			ble.print("AT+BLEUARTTX=");
			ble.println(res);
			ble.waitForOK();
		}
	}
#endif

	switch (pgm) {

#ifdef PGM_EFFECTS
		case PGM_EFFECTS:
			npp_effect_loop();
			break;
#endif

#ifdef PGM_XLM
		case PGM_XLM:
			npp_xlm_loop();
			break;
#endif

#ifdef PGM_CLOCK
		case PGM_CLOCK:
			npp_clock_loop();
			break;
#endif

	}
}

static char * npp_execute_command(char * buf) {
	switch (buf[0]) {

#ifdef PGM_CLOCK
		case 'C':
			if (pgm != PGM_CLOCK) EEPROM_update(5, pgm = PGM_CLOCK);
			if (buf[1]) return npp_clock_command(&buf[1]);
			break;
#endif

#ifdef PGM_EFFECTS
		case 'E':
			if (pgm != PGM_EFFECTS) EEPROM_update(5, pgm = PGM_EFFECTS);
			if (buf[1]) return npp_effect_command(&buf[1]);
			break;
#endif

		case 'P':
			if (buf[1]) return npp_system_command(&buf[1]);
			break;

#ifdef PGM_XLM
		case 'X':
			if (pgm != PGM_XLM) EEPROM_update(5, pgm = PGM_XLM);
			if (buf[1]) return npp_xlm_command(&buf[1]);
			break;
#endif

	}
	return 0;
}

static char * npp_system_command(char * buf) {
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

static int free_ram() {
  extern int __heap_start, * __brkval; int v;
  return (int)&v - ((__brkval == 0) ? (int)&__heap_start : (int)__brkval);
}
