#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <EEPROM.h>
#include "npp_eeprom.h"
#include "npp_xlm.h"

extern Adafruit_NeoPixel strip;

static int eestart;
static struct {
	uint8_t stopped;
	uint8_t reversed;
	uint8_t on_count;
	uint8_t off_count;
	uint32_t color;
	uint16_t ms_per_frame;
	uint32_t frame_time;
	uint16_t frame;
} channels[2];

static void channel_setup(int ch, int addr, uint8_t r, uint8_t g, uint8_t b) {
	if (EEPROM.read(addr) == 'x') {
		uint8_t flags = EEPROM.read(addr + 1);
		channels[ch].stopped = flags & 0x80;
		channels[ch].reversed = flags & 0x40;
		channels[ch].on_count = EEPROM.read(addr + 2);
		channels[ch].off_count = EEPROM.read(addr + 3);
		channels[ch].color = EEPROM_read32(addr + 4);
		channels[ch].ms_per_frame = EEPROM_read16(addr + 12);
	} else {
		channels[ch].stopped = 0;
		channels[ch].reversed = 0;
		channels[ch].on_count = 1;
		channels[ch].off_count = 1;
		channels[ch].color = strip.Color(r, g, b);
		channels[ch].ms_per_frame = 100;
	}
	channels[ch].frame_time = millis();
	channels[ch].frame = 0;
}

static void channel_update(int ch, uint32_t tick) {
	if (!channels[ch].stopped) {
		uint8_t total = channels[ch].on_count + channels[ch].off_count;
		uint8_t t = (tick - channels[ch].frame_time) / channels[ch].ms_per_frame; if (!t) return;
		channels[ch].frame = (channels[ch].frame + (channels[ch].reversed ? t : (total - t))) % total;
	}
	channels[ch].frame_time = tick;
}

static void channel_save(int ch, int addr) {
	uint8_t flags = 0;
	if (channels[ch].stopped) flags |= 0x80;
	if (channels[ch].reversed) flags |= 0x40;
	EEPROM_update(addr, 'x');
	EEPROM_update(addr + 1, flags);
	EEPROM_update(addr + 2, channels[ch].on_count);
	EEPROM_update(addr + 3, channels[ch].off_count);
	EEPROM_update32(addr + 4, channels[ch].color);
	EEPROM_update16(addr + 12, channels[ch].ms_per_frame);
}

int npp_xlm_setup(int eeaddr) {
	eestart = eeaddr;
	channel_setup(0, eestart, 255, 0, 0);
	channel_setup(1, eestart + 16, 0, 0, 255);
	return eestart + 32;
}

void npp_xlm_loop(void) {
	uint32_t tick = millis();
	uint16_t i, n = strip.numPixels();
	channel_update(0, tick);
	channel_update(1, tick);
	for (i = 0; i < n; i++) {
		uint16_t ch = i & 1;
		uint8_t total = channels[ch].on_count + channels[ch].off_count;
		uint8_t frame = (channels[ch].frame + (i >> 1)) % total;
		strip.setPixelColor(i, (frame < channels[ch].on_count) ? channels[ch].color : 0);
	}
	strip.show();
}

char * npp_xlm_command(char * buf) {
	uint8_t cmd = buf[0];
	uint16_t ch = buf[1] - '0';
	uint8_t dir = buf[2];
	if (ch < 2) {
		switch (cmd) {
			case 'N':
				if (dir == '+' && channels[ch].on_count < 120) channels[ch].on_count++;
				if (dir == '-' && channels[ch].on_count >   1) channels[ch].on_count--;
				break;
			case 'O':
				if (dir == '+' && channels[ch].off_count < 120) channels[ch].off_count++;
				if (dir == '-' && channels[ch].off_count >   1) channels[ch].off_count--;
				break;
			case 'S':
				if (dir == '+' && channels[ch].ms_per_frame >    20) channels[ch].ms_per_frame -= 20;
				if (dir == '-' && channels[ch].ms_per_frame < 60000) channels[ch].ms_per_frame += 20;
				break;
			case 'V':
				channels[ch].reversed = !channels[ch].reversed;
				break;
			case 'R':
				channels[ch].stopped = 0;
				break;
			case 'F':
				channels[ch].stopped = 1;
				break;
			case 'Z':
				channels[ch].stopped = 0;
				channels[ch].reversed = 0;
				channels[ch].on_count = 1;
				channels[ch].off_count = 1;
				channels[ch].ms_per_frame = 100;
				break;
		}
		channel_save(ch, eestart + (ch << 4));
	}
	return 0;
}
