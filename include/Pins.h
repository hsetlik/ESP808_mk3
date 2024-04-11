#pragma once
#include <Arduino.h>
 
typedef unsigned char uint8;

// ------- Pins on the ESP32-S3-WROOM-1 module---------------------

// SPI  common lines and chip select lines
#define MISO 10
#define MOSI 11
#define SCK 12

#define EXP_CS 13 // MCP23S17 port expander (x3)
#define SD_CS 21 // SD card reader
#define DISPLAY_CS 35 // ILI9341-driven display
#define POT1_CS 42 // MCP4331 quad digital potentiometer (x2)
#define POT2_CS 41

// Interrupt and reset lines for the expander
#define EXP_RST 7
#define EXP_INTR 14

// Extra I/O for the display
#define DISPLAY_DC 6
#define DISPLAY_RST 36

// I2S lines for the audio DAC
#define I2S_D 8
#define I2S_LR 9
#define I2S_SCK 17
#define I2S_BCK 18

// MIDI in/out jacks
#define MIDI_OUT 38
#define MIDI_IN 37

// NeoPixel data pin
#define PIXELS 37

// ADC pin for monitoring the battery level
#define BATT_PIN 1

// ------- Pins on the three MCP23S17 expanders--------------------

//first expander handles the 16 step buttons
#define EXP_1_ADDR 0b100 

#define ST0 0
#define ST1 1
#define ST2 2
#define ST3 3
#define ST4 4
#define ST5 5
#define ST6 6
#define ST7 7
#define ST8 15
#define ST9 14
#define ST10 13
#define ST11 12
#define ST12 11
#define ST13 10
#define ST14 9
#define ST15 8

// second handles the encoders and pg left/right buttons
#define EXP_2_ADDR 0b110 

#define ENC3_L 0
#define ENC3_R 1
#define ENC3_B 2

#define ENC2_L 3
#define ENC2_R 4
#define ENC2_B 5

#define ENC1_L 6
#define ENC1_R 7
#define ENC1_B 8

#define ENC4_L 9
#define ENC4_R 10
#define ENC4_B 11

#define PG_LEFT 12
#define PG_RIGHT 13

// third handles the trigger outputs to the analog voices
// and the remaining buttons
#define EXP_3_ADDR 0b001

#define K1_TRIG 0
#define K2_TRIG 1
#define SNARE_TRIG 2
#define CLAP_TRIG 3
#define CLAVE_TRIG 4
#define OH_TRIG 5
#define CH_TRIG 6
#define HAT_ACCENT 7

#define LOAD_PIN 8
#define SAVE_PIN 9
#define PLAYTOGLE 10
#define ALT_PIN