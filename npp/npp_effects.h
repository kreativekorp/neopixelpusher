#ifndef __NPP_EFFECTS_H__
#define __NPP_EFFECTS_H__

#include <Arduino.h>

#define NPP_EFFECTS 40
#define NPP_EFFECTS_VERSION "FX40A"

#define NPP_EFFECT_MAX_COLORS_RAM       32
#define NPP_EFFECT_MAX_FRAMES_RAM       96
#define NPP_EFFECT_MAX_COLORS_EEPROM    32
#define NPP_EFFECT_MAX_FRAMES_EEPROM    64
#define NPP_EFFECT_EEPROM_ADDR_COLORS   16
#define NPP_EFFECT_EEPROM_ADDR_FRAMES  112
#define NPP_EFFECT_EEPROM_ADDR_END     368

int npp_effect_setup(int eeaddr);
void npp_effect_loop(void);
char * npp_effect_command(char * buf);

#endif
