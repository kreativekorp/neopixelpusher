#ifndef __ANALOGPP_LIB_H__
#define __ANALOGPP_LIB_H__

#include <Arduino.h>
#include <inttypes.h>

#define W_PIN 5
#define R_PIN 9
#define G_PIN 10
#define B_PIN 11

#define MODE_PIN0 A0
#define MODE_PIN1 A1
#define MODE_PIN2 A2
#define MODE_PIN3 A3
#define MODE_PIN4 A4
#define MODE_PIN5 A5

void    init_mode();
uint8_t read_mode();
void    init_digital();
uint8_t read_digital(uint8_t * i);
void    init_analog();
uint8_t read_analog(uint16_t * w, uint16_t * r, uint16_t * g, uint16_t * b);
void    set_color(uint8_t w, uint8_t r, uint8_t g, uint8_t b);

#endif
