#ifndef TLC_CONFIG_H
#define TLC_CONFIG_H

// PINS for XIAO_ESP32C3 see https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/
#define HSPI_MISO  D9  // TLC: Do not connect
#define HSPI_MOSI  D10 // TLC: SIN
#define HSPI_SCLK  D8  // TLC: SCLK

#define XLAT_PIN  D4   // TLC: XLAT

#define GSCLK_PIN D5   // TLC: GSCLK
#define GSCLK_FREQ 10e6
#define GSCLK_TIMER 0
#define GSCLK_TIMER_RESULUTION 1

#define BLANK_PIN D2   // TLC: BLANK
#define BLANK_FREQ 2441
#define BLANK_HIGH 1
#define BLANK_RESULUTION 15

#ifndef NUM_TLCS
#define NUM_TLCS  1
#endif

#endif
