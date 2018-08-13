#include "hsv.h"

static int max3(int a, int b, int c) {
	int d = (a > b) ? a : b;
	return (c > d) ? c : d;
}

static int min3(int a, int b, int c) {
	int d = (a < b) ? a : b;
	return (c < d) ? c : d;
}

void hsv2rgb(int h, int s, int v, int * r, int * g, int * b) {
	int i = h / 60;
	int j = h % 120;
	int k = (j <= 60) ? j : (120 - j);
	int l = 100 - s;
	int m = (int)((((long)l * (long)v * 51L) + 1000L) / 2000L);
	int w = l * 60 + s * k;
	int x = (int)((((long)w * (long)v * 17L) + 20000L) / 40000L);
	int y = ((v * 51) + 10) / 20;
	switch (i % 6) {
		case 0: *r = y; *g = x; *b = m; break;
		case 1: *r = x; *g = y; *b = m; break;
		case 2: *r = m; *g = y; *b = x; break;
		case 3: *r = m; *g = x; *b = y; break;
		case 4: *r = x; *g = m; *b = y; break;
		case 5: *r = y; *g = m; *b = x; break;
	}
}

void rgb2hsv(int r, int g, int b, int * h, int * s, int * v) {
	int m = max3(r, g, b);
	int n = min3(r, g, b);
	int c = m - n;
	if (c == 0) {
		*h = 0;
	} else if (m == g) {
		if (b < r) *h = (120 - ((60 * (r - b)) + (c / 2)) / c);
		else       *h = (120 + ((60 * (b - r)) + (c / 2)) / c);
	} else if (m == b) {
		if (r < g) *h = (240 - ((60 * (g - r)) + (c / 2)) / c);
		else       *h = (240 + ((60 * (r - g)) + (c / 2)) / c);
	} else {
		if (g < b) *h = (360 - ((60 * (b - g)) + (c / 2)) / c);
		else       *h = (      ((60 * (g - b)) + (c / 2)) / c);
	}
	*s = m ? (((c * 100) + (m / 2)) / m) : 0;
	*v = ((m * 40) + 51) / 102;
}
