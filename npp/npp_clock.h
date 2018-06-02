#ifndef __NPP_CLOCK_H__
#define __NPP_CLOCK_H__

#include <Arduino.h>

#define NPP_CLOCK_RTC_ADDR  0x68

#define NPP_CLOCK_MODE_POINT   0
#define NPP_CLOCK_MODE_LINE    1
#define NPP_CLOCK_MODE_BINARY  2
#define NPP_CLOCK_MODE_MAX     3

int npp_clock_setup(int eeaddr);
void npp_clock_loop(void);
char * npp_clock_command(char * buf);
void npp_clock_get_time(char * buf);
void npp_clock_set_time(char * buf);

#endif
