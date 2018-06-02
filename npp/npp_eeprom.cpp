#include <Arduino.h>
#include <EEPROM.h>
#include "npp_eeprom.h"

void EEPROM_update(int a, char v) {
	if (EEPROM.read(a) != v) {
		EEPROM.write(a, v);
	}
}

uint16_t EEPROM_read16(int a) {
	union {
		uint16_t v;
		uint8_t vb[2];
	};
	vb[0] = EEPROM.read(a++);
	vb[1] = EEPROM.read(a++);
	return v;
}

void EEPROM_update16(int a, volatile uint16_t v) {
	volatile uint8_t * vb = (uint8_t *)(void *)(uint16_t *)(&v);
	EEPROM_update(a++, vb[0]);
	EEPROM_update(a++, vb[1]);
}

uint32_t EEPROM_read32(int a) {
	union {
		uint32_t v;
		uint8_t vb[4];
	};
	vb[0] = EEPROM.read(a++);
	vb[1] = EEPROM.read(a++);
	vb[2] = EEPROM.read(a++);
	vb[3] = EEPROM.read(a++);
	return v;
}

void EEPROM_update32(int a, volatile uint32_t v) {
	volatile uint8_t * vb = (uint8_t *)(void *)(uint32_t *)(&v);
	EEPROM_update(a++, vb[0]);
	EEPROM_update(a++, vb[1]);
	EEPROM_update(a++, vb[2]);
	EEPROM_update(a++, vb[3]);
}
