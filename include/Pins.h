#pragma once
#include <Arduino.h>

// sneaky forward declaration for the debug situation
void debugMsg(const String& msg);

 

enum HardwareTrack : uint8_t
{
Kick1,
Kick2,
Snare,
Clap,
Clave,
OpenHat,
ClosedHat,
Accent,
Digital
};

const String TrackNames[9] = {
    "Kick 1",
    "Kick 2",
    "Snare",
    "Clap",
    "Clave",
    "Open Hat",
    "Closed Hat",
    "Accent",
    "Digital"
};



// ------- Pins on the ESP32-S3-WROOM-1 module---------------------

// SPI  common lines and chip select lines
#define MISO 10
#define MOSI 11
#define SCK 12

#define EXP_CS 13 // Adafruit_MCP23X17 port expander (x3)
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

// ------- Pins on the three Adafruit_MCP23X17 expanders--------------------

//first expander handles the 16 step buttons
#define EXP_1_ADDR 0b100 

#define ST0 8
#define ST1 9
#define ST2 10
#define ST3 11
#define ST4 12
#define ST5 13
#define ST6 14
#define ST7 15

#define ST8 7
#define ST9 6
#define ST10 5
#define ST11 4
#define ST12 3
#define ST13 2
#define ST14 1
#define ST15 0

// second handles the encoders and pg left/right buttons
#define EXP_2_ADDR 0b110 

#define ENC3_L 8
#define ENC3_R 9
#define ENC3_B 10

#define ENC2_L 11
#define ENC2_R 12
#define ENC2_B 13

#define ENC1_L 14
#define ENC1_R 15
#define ENC1_B 0

#define ENC4_L 1
#define ENC4_R 2
#define ENC4_B 3

#define PG_LEFT 4
#define PG_RIGHT 5

// third handles the trigger outputs to the analog voices
// and the remaining buttons
#define EXP_3_ADDR 0b001

#define K1_TRIG 8
#define K2_TRIG 9
#define SNARE_TRIG 10
#define CLAP_TRIG 11
#define CLAVE_TRIG 12
#define OH_TRIG 13
#define CH_TRIG 14
#define HAT_ACCENT 15

#define LOAD_PIN 0
#define SAVE_PIN 1
#define PLAYTOGLE 2
#define ALT_PIN 3