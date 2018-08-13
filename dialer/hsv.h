#ifndef __HSV_H__
#define __HSV_H__

void hsv2rgb(int h, int s, int v, int * r, int * g, int * b);
void rgb2hsv(int r, int g, int b, int * h, int * s, int * v);

#endif
