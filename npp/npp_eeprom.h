#ifndef __NPP_EEPROM_H__
#define __NPP_EEPROM_H__

#include <Arduino.h>

void EEPROM_update(int a, char v);

uint16_t EEPROM_read16(int a);
void EEPROM_update16(int a, uint16_t v);

uint32_t EEPROM_read32(int a);
void EEPROM_update32(int a, uint32_t v);

#endif
