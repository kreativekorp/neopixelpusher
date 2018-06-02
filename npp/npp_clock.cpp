#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include "npp_clock.h"
#include "npp_eeprom.h"

extern Adafruit_NeoPixel strip;

static int eestart;
static uint8_t century;
static uint8_t clock_mode;
static uint8_t hr, hg, hb;
static uint8_t mr, mg, mb;
static uint8_t sr, sg, sb;

int npp_clock_setup(int eeaddr) {
	eestart = eeaddr;

	century = EEPROM.read(eestart);
	if (century >= 100) century = 20;

	clock_mode = EEPROM.read(eestart + 1);
	if (clock_mode >= NPP_CLOCK_MODE_MAX) clock_mode = NPP_CLOCK_MODE_POINT;

	if (EEPROM.read(eestart + 2) == 'h') {
		hr = EEPROM.read(eestart + 3);
		hg = EEPROM.read(eestart + 4);
		hb = EEPROM.read(eestart + 5);
	} else {
		hr = 255;
		hg =   0;
		hb =   0;
	}

	if (EEPROM.read(eestart + 6) == 'm') {
		mr = EEPROM.read(eestart + 7);
		mg = EEPROM.read(eestart + 8);
		mb = EEPROM.read(eestart + 9);
	} else {
		mr =   0;
		mg = 255;
		mb =   0;
	}

	if (EEPROM.read(eestart + 10) == 's') {
		sr = EEPROM.read(eestart + 11);
		sg = EEPROM.read(eestart + 12);
		sb = EEPROM.read(eestart + 13);
	} else {
		sr =   0;
		sg =   0;
		sb = 255;
	}

	return eestart + 16;
}

static uint8_t apply_mode(uint8_t v, uint8_t i) {
	switch (clock_mode) {
		case NPP_CLOCK_MODE_POINT:
			return i == v;
		case NPP_CLOCK_MODE_LINE:
			return i < v;
		case NPP_CLOCK_MODE_BINARY:
			return v & (1 << i);
		default:
			return 0;
	}
}

void npp_clock_loop(void) {
	uint8_t h, m, s, i, r, g, b;
	Wire.beginTransmission(NPP_CLOCK_RTC_ADDR);
	Wire.write(0x00);
	Wire.endTransmission();
	Wire.requestFrom(NPP_CLOCK_RTC_ADDR, 3);
	s = Wire.read();
	m = Wire.read();
	h = Wire.read();

	s = ((s >> 4) * 10) + (s & 0xF);
	m = ((m >> 4) * 10) + (m & 0xF);

	if (h & 0x40) {
		i = h & 0xF;
		if (h & 0x10) i += 10;
		if (clock_mode != NPP_CLOCK_MODE_BINARY) {
			if (i >= 12) i -= 12;
			if (h & 0x20) i += 12;
		}
		h = i;
	} else {
		h = ((h >> 4) * 10) + (h & 0xF);
	}

	if (clock_mode != NPP_CLOCK_MODE_BINARY) {
		h *= 5;
		h += m / 12;
		if (h >= 60) h -= 60;
	}

	for (i = 0; i < 60; i++) {
		r = g = b = 0;
		if (apply_mode(h, i)) {
			if (r < hr) r = hr;
			if (g < hg) g = hg;
			if (b < hb) b = hb;
		}
		if (apply_mode(m, i)) {
			if (r < mr) r = mr;
			if (g < mg) g = mg;
			if (b < mb) b = mb;
		}
		if (apply_mode(s, i)) {
			if (r < sr) r = sr;
			if (g < sg) g = sg;
			if (b < sb) b = sb;
		}
		strip.setPixelColor(i, strip.Color(r, g, b));
	}

	strip.show();
}

char * npp_clock_command(char * buf) {
	uint8_t m, r, g, b, p, i, d;
	switch (buf[0]) {
		case 'C':
			if ((m = buf[1])) {
				r = g = b = p = 0;
				for (i = 2; buf[i]; i++) {
					if (buf[i] >= '0' && buf[i] <= '9') d = buf[i] - '0';
					else if (buf[i] >= 'A' && buf[i] <= 'F') d = buf[i] - 'A' + 10;
					else if (buf[i] >= 'a' && buf[i] <= 'f') d = buf[i] - 'a' + 10;
					else continue;
					switch (p) {
						case 0: r |= d << 4; break;
						case 1: r |= d     ; break;
						case 2: g |= d << 4; break;
						case 3: g |= d     ; break;
						case 4: b |= d << 4; break;
						case 5: b |= d     ; break;
					}
					p++;
				}
				switch (m) {
					case 'H':
						EEPROM_update(eestart + 2, 'h');
						EEPROM_update(eestart + 3, hr = r);
						EEPROM_update(eestart + 4, hg = g);
						EEPROM_update(eestart + 5, hb = b);
						break;
					case 'M':
						EEPROM_update(eestart + 6, 'm');
						EEPROM_update(eestart + 7, mr = r);
						EEPROM_update(eestart + 8, mg = g);
						EEPROM_update(eestart + 9, mb = b);
						break;
					case 'S':
						EEPROM_update(eestart + 10, 's');
						EEPROM_update(eestart + 11, sr = r);
						EEPROM_update(eestart + 12, sg = g);
						EEPROM_update(eestart + 13, sb = b);
						break;
				}
			}
			break;
		case 'M':
			m = 0;
			for (i = 1; buf[i]; i++) {
				if (buf[i] >= '0' && buf[i] <= '9') {
					m = m * 10 + (buf[i] & 0xF);
				}
			}
			clock_mode = (m < NPP_CLOCK_MODE_MAX) ? m : NPP_CLOCK_MODE_POINT;
			EEPROM_update(eestart + 1, clock_mode);
			break;
		case 'S':
			npp_clock_set_time(buf);
		case 'T':
			npp_clock_get_time(buf);
			return buf;
	}
	return 0;
}

void npp_clock_get_time(char * buf) {
	uint8_t c, y, mo, d, h, mi, s;
	Wire.beginTransmission(NPP_CLOCK_RTC_ADDR);
	Wire.write(0x00);
	Wire.endTransmission();
	Wire.requestFrom(NPP_CLOCK_RTC_ADDR, 7);
	s  = Wire.read();
	mi = Wire.read();
	h  = Wire.read();
	/**/ Wire.read();
	d  = Wire.read();
	mo = Wire.read();
	y  = Wire.read();
	c  = century;

	if (mo & 0x80) {
		c++;
		mo &= 0x7F;
	}

	*buf++ = '0' | (c / 10);
	*buf++ = '0' | (c % 10);
	*buf++ = '0' | (y >> 4);
	*buf++ = '0' | (y & 15);
	*buf++ = '-';
	*buf++ = '0' | (mo >> 4);
	*buf++ = '0' | (mo & 15);
	*buf++ = '-';
	*buf++ = '0' | (d >> 4);
	*buf++ = '0' | (d & 15);
	*buf++ = ' ';

	if (h & 0x40) {
		*buf++ = (h & 0x10) ? '1' : '0';
	} else {
		*buf++ = '0' | (h >> 4);
	}

	*buf++ = '0' | (h & 15);
	*buf++ = ':';
	*buf++ = '0' | (mi >> 4);
	*buf++ = '0' | (mi & 15);
	*buf++ = ':';
	*buf++ = '0' | (s >> 4);
	*buf++ = '0' | (s & 15);

	if (h & 0x40) {
		*buf++ = ' ';
		*buf++ = (h & 0x20) ? 'P' : 'A';
		*buf++ = 'M';
	}

	*buf = 0;
}

void npp_clock_set_time(char * buf) {
	uint8_t c, y, mo, d, h, mi, s, p, v;
	c = y = mo = d = h = mi = s = p = 0;
	while ((v = *buf++)) {
		if (v >= '0' && v <= '9') {
			v &= 0xF;
			switch (p) {
				case  0: c  += v * 10; break;
				case  1: c  += v     ; break;
				case  2: y  |= v << 4; break;
				case  3: y  |= v     ; break;
				case  4: mo |= v << 4; break;
				case  5: mo |= v     ; break;
				case  6: d  |= v << 4; break;
				case  7: d  |= v     ; break;
				case  8: h  |= v << 4; break;
				case  9: h  |= v     ; break;
				case 10: mi |= v << 4; break;
				case 11: mi |= v     ; break;
				case 12: s  |= v << 4; break;
				case 13: s  |= v     ; break;
			}
			p++;
		} else if (v == 'A' || v == 'a') {
			h |= 0x40;
		} else if (v == 'P' || v == 'p') {
			h |= 0x60;
		}
	}

	EEPROM_update(eestart, century = c);

	Wire.beginTransmission(NPP_CLOCK_RTC_ADDR);
	Wire.write(0x00);
	Wire.write(s);
	Wire.write(mi);
	Wire.write(h);
	Wire.write(1);
	Wire.write(d);
	Wire.write(mo);
	Wire.write(y);
	Wire.endTransmission();

	Wire.beginTransmission(NPP_CLOCK_RTC_ADDR);
	Wire.write(0x0E);
	Wire.write(0x1C);
	Wire.endTransmission();
}
