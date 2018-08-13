#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include "gamma.h"
#include "hsv.h"

/* -------- Begin Configurable Values -------- */

#define NEOPIXEL_NUM  8
#define NEOPIXEL_PIN  13
#define NEOPIXEL_SET  (NEO_GRB + NEO_KHZ800)

#define LCD_RS    5
#define LCD_EN    6
#define LCD_D4    9
#define LCD_D5    10
#define LCD_D6    11
#define LCD_D7    12
#define LCD_COLS  16
#define LCD_ROWS  2

#define POT1  5
#define POT2  4
#define POT3  3

#define SW1  A2
#define SW2  A1

#define RES_PIXEL_1  7
#define RES_PIXEL_2  5
#define RES_PIXEL_3  3
#define RES_PIXEL_4  0

/* -------- End Configurable Values -------- */

#define NUM_MODES  7
#define NUM_TOL    9

static unsigned char csram[8][8] = {
  { 0x0C, 0x12, 0x12, 0x0C, 0x00, 0x00, 0x00, 0x00 },
  { 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00, 0x1F, 0x00 },
  { 0x19, 0x05, 0x02, 0x02, 0x04, 0x04, 0x04, 0x00 },
  { 0x0E, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x1B, 0x00 },
  { 0x02, 0x05, 0x05, 0x05, 0x05, 0x15, 0x12, 0x00 },
  { 0x02, 0x06, 0x02, 0x02, 0x02, 0x12, 0x17, 0x00 },
  { 0x02, 0x05, 0x01, 0x02, 0x02, 0x14, 0x17, 0x00 },
  { 0x07, 0x04, 0x06, 0x01, 0x01, 0x15, 0x12, 0x00 },
};

static char * hex[256] = {
  "00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F",
  "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F",
  "20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F",
  "30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F",
  "40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F",
  "50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F",
  "60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F",
  "70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E","7F",
  "80","81","82","83","84","85","86","87","88","89","8A","8B","8C","8D","8E","8F",
  "90","91","92","93","94","95","96","97","98","99","9A","9B","9C","9D","9E","9F",
  "A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF",
  "B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF",
  "C0","C1","C2","C3","C4","C5","C6","C7","C8","C9","CA","CB","CC","CD","CE","CF",
  "D0","D1","D2","D3","D4","D5","D6","D7","D8","D9","DA","DB","DC","DD","DE","DF",
  "E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","EA","EB","EC","ED","EE","EF",
  "F0","F1","F2","F3","F4","F5","F6","F7","F8","F9","FA","FB","FC","FD","FE","FF",
};

static char * res_ccs[12] = {
  "SLV", "GLD", "BLK", "BRN", "RED", "ORG",
  "YEL", "GRN", "BLU", "VLT", "GRY", "WHT",
};

static unsigned char res_cc[12][3] = {
  {192,192,192}, {255,192,0}, {0,0,0}, {0x99,0x66,0x33}, {255,0,0}, {255,128,0},
  {255,255,0}, {0,255,0}, {0,0,255}, {128,0,255}, {128,128,128}, {255,255,255},
};

static char * res_tol_ccs[NUM_TOL] = {
  "NON", "SLV", "GLD", "RED", "BRN",
  "GRN", "BLU", "VLT", "GRY",
};

static unsigned char res_tol_cc[NUM_TOL][3] = {
  {0,0,0}, {192,192,192}, {255,192,0}, {255,0,0}, {0x99,0x66,0x33},
  {0,255,0}, {0,0,255}, {128,0,255}, {128,128,128},
};

static char * res_tol_str[NUM_TOL] = {
  "\x01\x32\x30%", "\x01\x31\x30%", "\x01 5%", "\x01 2%", "\x01 1%",
  "\x01\x07\x30%", "\x01\x06\x35%", "\x01\x05\x30%", "\x01\x04\x35%",
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEOPIXEL_SET);
LiquidCrystal lcd = LiquidCrystal(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
int mode, r, g, b, h, s, v, tol, gamma, brightness;

void setup() {
  strip.begin();
  lcd.begin(LCD_COLS, LCD_ROWS);
  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, csram[i]);
  }
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  
  mode       = EEPROM.read(0) & 0xFF;
  tol        = EEPROM.read(1) & 0xFF;
  gamma      = EEPROM.read(2) & 0xFF;
  brightness = EEPROM.read(3) & 0xFF;
  if (mode >= NUM_MODES) mode = 0;
  if (tol  >= NUM_TOL  ) tol  = 0;
  if (gamma < 20 || gamma > 30) gamma = 0;
  strip.setBrightness((mode == 6) ? 255 : brightness);
}

static void writeEEPROM() {
  if ((EEPROM.read(0) & 0xFF) != mode      ) EEPROM.write(0, mode      );
  if ((EEPROM.read(1) & 0xFF) != tol       ) EEPROM.write(1, tol       );
  if ((EEPROM.read(2) & 0xFF) != gamma     ) EEPROM.write(2, gamma     );
  if ((EEPROM.read(3) & 0xFF) != brightness) EEPROM.write(3, brightness);
}

static void printDec(int x) {
  if (x < 100) lcd.print(" ");
  if (x <  10) lcd.print(" ");
  lcd.print(x);
}

static void showColor(int y) {
  long c = strip.Color(
    gamma_correct(y, r),
    gamma_correct(y, g),
    gamma_correct(y, b)
  );
  for (int i = 0; i < NEOPIXEL_NUM; i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

static void showResCC(int y) {
  for (int i = 0; i < NEOPIXEL_NUM; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.setPixelColor(RES_PIXEL_1, strip.Color(
    gamma_correct(y, res_cc[h + 2][0]),
    gamma_correct(y, res_cc[h + 2][1]),
    gamma_correct(y, res_cc[h + 2][2])
  ));
  strip.setPixelColor(RES_PIXEL_2, strip.Color(
    gamma_correct(y, res_cc[s + 2][0]),
    gamma_correct(y, res_cc[s + 2][1]),
    gamma_correct(y, res_cc[s + 2][2])
  ));
  strip.setPixelColor(RES_PIXEL_3, strip.Color(
    gamma_correct(y, res_cc[v][0]),
    gamma_correct(y, res_cc[v][1]),
    gamma_correct(y, res_cc[v][2])
  ));
  strip.setPixelColor(RES_PIXEL_4, strip.Color(
    gamma_correct(y, res_tol_cc[tol][0]),
    gamma_correct(y, res_tol_cc[tol][1]),
    gamma_correct(y, res_tol_cc[tol][2])
  ));
  strip.show();
}

static void showRamp(int y) {
  for (int i = 0; i < NEOPIXEL_NUM; i++) {
    int x = gamma_correct(y, 255 * i / (NEOPIXEL_NUM - 1));
    strip.setPixelColor(i, strip.Color(x, x, x));
  }
  strip.show();
}

void loop() {
  int a1 = analogRead(POT1);
  int a2 = analogRead(POT2);
  int a3 = analogRead(POT3);
  int d1 = !digitalRead(SW1);
  int d2 = !digitalRead(SW2);
  
  if (d1) {
    lcd.clear();
    delay(200);
    while (!digitalRead(SW1));
    delay(200);
    mode++;
    if (mode >= NUM_MODES) mode = 0;
    strip.setBrightness((mode == 6) ? 255 : brightness);
    writeEEPROM();
  }
  
  switch (mode) {
    case 0: case 1:
      r = a1 >> 2;
      g = a2 >> 2;
      b = a3 >> 2;
      rgb2hsv(r, g, b, &h, &s, &v);
      break;
    case 2: case 3:
      h = (int)((long)a1 * 360L / 1023L);
      s = (int)((long)a2 * 100L / 1023L);
      v = (int)((long)a3 * 100L / 1023L);
      hsv2rgb(h, s, v, &r, &g, &b);
      break;
    case 4:
      h = (a1 * 10) >> 10;
      s = (a2 * 10) >> 10;
      v = (a3 * 12) >> 10;
      if (d2) {
        tol++;
        if (tol >= NUM_TOL) tol = 0;
        writeEEPROM();
      }
      break;
    case 5:
      v = (a2 * 12) >> 10;
      if (v) v += 19;
      if (d2) gamma = v;
      break;
    case 6:
      r = g = b = v = a2 >> 2;
      if (d2) brightness = v;
      break;
  }
  
  switch (mode) {
    case 0: case 1:
      lcd.setCursor( 0, 0); lcd.print("HSV");
      lcd.setCursor( 4, 0); printDec(h);
      lcd.setCursor( 8, 0); printDec(s);
      lcd.setCursor(12, 0); printDec(v);
      break;
    case 2:
      lcd.setCursor( 0, 0); lcd.print("RGB");
      lcd.setCursor( 4, 0); printDec(r);
      lcd.setCursor( 8, 0); printDec(g);
      lcd.setCursor(12, 0); printDec(b);
      break;
    case 3:
      lcd.setCursor( 0, 0); lcd.print("RGB");
      lcd.setCursor( 5, 0); lcd.print(hex[r]);
      lcd.setCursor( 9, 0); lcd.print(hex[g]);
      lcd.setCursor(13, 0); lcd.print(hex[b]);
      break;
    case 4:
      lcd.setCursor( 0, 0); lcd.print(res_ccs[h + 2]);
      lcd.setCursor( 4, 0); lcd.print(res_ccs[s + 2]);
      lcd.setCursor( 8, 0); lcd.print(res_ccs[v]);
      lcd.setCursor(12, 0); lcd.print(res_tol_ccs[tol]);
      break;
    case 5:
      lcd.setCursor(0, 0);
      lcd.print("Gamma");
      lcd.setCursor(LCD_COLS - 3, 0);
      if (gamma) {
        lcd.print(gamma / 10);
        lcd.print(".");
        lcd.print(gamma % 10);
      } else {
        lcd.print("OFF");
      }
      break;
    case 6:
      lcd.setCursor(0, 0);
      lcd.print("Brightness");
      lcd.setCursor(LCD_COLS - 3, 0);
      printDec(brightness);
      break;
  }
  
  switch (mode) {
    case 0:
      lcd.setCursor( 0, 1); lcd.print("RGB");
      lcd.setCursor( 4, 1); printDec(r);
      lcd.setCursor( 8, 1); printDec(g);
      lcd.setCursor(12, 1); printDec(b);
      break;
    case 1:
      lcd.setCursor( 0, 1); lcd.print("RGB");
      lcd.setCursor( 5, 1); lcd.print(hex[r]);
      lcd.setCursor( 9, 1); lcd.print(hex[g]);
      lcd.setCursor(13, 1); lcd.print(hex[b]);
      break;
    case 2: case 3:
      lcd.setCursor( 0, 1); lcd.print("HSV");
      lcd.setCursor( 4, 1); printDec(h);
      lcd.setCursor( 8, 1); printDec(s);
      lcd.setCursor(12, 1); printDec(v);
      break;
    case 4:
      lcd.setCursor( 0, 1); lcd.print(v ? " " : ".");
      lcd.setCursor( 2, 1); lcd.print(h);
      lcd.setCursor( 4, 1); lcd.print(((v % 3) == 1) ? "." : " ");
      lcd.setCursor( 6, 1); lcd.print(s);
      lcd.setCursor( 8, 1); lcd.print(((v % 3) || !v) ? " " : "0");
      lcd.setCursor(10, 1); lcd.print((v < 4) ? "\x03" : (v < 7) ? "K" : (v < 10) ? "M" : "G");
      lcd.setCursor(12, 1); lcd.print(res_tol_str[tol]);
      break;
    case 5:
      lcd.setCursor(0, 1);
      lcd.print("Set to");
      lcd.setCursor(LCD_COLS - 3, 1);
      if (v) {
        lcd.print(v / 10);
        lcd.print(".");
        lcd.print(v % 10);
      } else {
        lcd.print("OFF");
      }
      break;
    case 6:
      lcd.setCursor(0, 1);
      lcd.print("Set to");
      lcd.setCursor(LCD_COLS - 3, 1);
      printDec(v);
      break;
  }
  
  switch (mode) {
    case 0: case 1: case 2: case 3:
      showColor(gamma);
      break;
    case 4:
      showResCC(26);
      if (d2) {
        delay(200);
        while (!digitalRead(SW2));
        delay(200);
      }
      break;
    case 5:
      showRamp(v);
      break;
    case 6:
      showColor(0);
      break;
  }
}
