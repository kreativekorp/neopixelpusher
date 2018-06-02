#ifndef __NPP_XLM_H__
#define __NPP_XLM_H__

#include <Arduino.h>

int npp_xlm_setup(int eeaddr);
void npp_xlm_loop(void);
char * npp_xlm_command(char * buf);

#endif
