#ifndef __NPP_COLOR_H__
#define __NPP_COLOR_H__

#include <Arduino.h>
#include <inttypes.h>

// Gamma-corrected color levels, 0 - 15 out of 15
#define Cx0    0
#define Cx1    1
#define Cx2    2
#define Cx3    4
#define Cx4    8
#define Cx5   15
#define Cx6   24
#define Cx7   35
#define Cx8   50
#define Cx9   68
#define CxA   89
#define CxB  114
#define CxC  143
#define CxD  176
#define CxE  213
#define CxF  255

// Gamma-corrected color levels, 0 - 8 out of 8
#define Ce0    0
#define Ce1    1
#define Ce2    7
#define Ce3   20
#define Ce4   42
#define Ce5   76
#define Ce6  122
#define Ce7  182
#define Ce8  255

// Predefined color constants, RGB order
#define CnBLK  Ce0, Ce0, Ce0
#define CnDGY  Ce2, Ce2, Ce2
#define CnMGY  Ce4, Ce4, Ce4
#define CnLGY  Ce6, Ce6, Ce6
#define CnWHT  Ce8, Ce8, Ce8
#define CnWWT  CxF, CxD, Cx9
#define CnTAN  CxF, CxC, Cx9
#define CnBRN  Cx9, Cx6, Cx3
#define CnMRN  Ce4, Ce0, Ce0
#define CnUMB  Ce4, Ce2, Ce0
#define CnOLV  Ce4, Ce4, Ce0
#define CnFGR  Ce0, Ce4, Ce0
#define CnTEL  Ce0, Ce4, Ce4
#define CnNVY  Ce0, Ce0, Ce4
#define CnEGP  Ce2, Ce0, Ce4
#define CnPLM  Ce4, Ce0, Ce4
#define CnRED  Ce8, Ce0, Ce0
#define CnSCL  Ce8, Ce2, Ce0
#define CnORG  Ce8, Ce4, Ce0
#define CnGLD  Ce8, Ce6, Ce0
#define CnYEL  Ce8, Ce8, Ce0
#define CnCHR  Ce4, Ce8, Ce0
#define CnGRN  Ce0, Ce8, Ce0
#define CnAQU  Ce0, Ce8, Ce4
#define CnCYN  Ce0, Ce8, Ce8
#define CnAZR  Ce0, Ce4, Ce8
#define CnBLU  Ce0, Ce0, Ce8
#define CnIND  Ce2, Ce0, Ce8
#define CnVIO  Ce4, Ce0, Ce8
#define CnPUR  Ce6, Ce0, Ce8
#define CnMAG  Ce8, Ce0, Ce8
#define CnRSE  Ce8, Ce0, Ce4
#define CnCRL  Ce8, Ce4, Ce4
#define CnCRG  Ce8, Ce6, Ce4
#define CnLEM  Ce8, Ce8, Ce4
#define CnLIM  Ce4, Ce8, Ce4
#define CnSKY  Ce4, Ce8, Ce8
#define CnFRS  Ce4, Ce4, Ce8
#define CnLVN  Ce6, Ce4, Ce8
#define CnPNK  Ce8, Ce4, Ce8

uint8_t color_correct(uint8_t v);
uint8_t color_multiply(uint8_t v, uint8_t m, uint8_t gc);
uint8_t color_blend(uint8_t v1, uint8_t v2, uint32_t i, uint32_t n);

#endif
