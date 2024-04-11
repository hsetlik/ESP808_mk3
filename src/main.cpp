#include "Controls.h"
#include "Display.h"

/*
ISRs that need to go here:

1. Rising edge interrupt on the expander interrupt pin:
  - Check which pin triggered the interrupt, 
    pass that data to the business code to handle
    the rotary encoders

2. Timer interrupt to check the buttons
  - Check all the pushbutton states
  - TODO: choose a frequency

3. Timer interrupt to update the gates and digital pots
  - Set the correct level for each of the 8 gate lines on Expander 3
  - Set both quad digital potentiometers to the correct value
  - 

4. Timer interrupt to update the audio DAC:
  - Send the next sample in the current buffer
    to the DAC (TI PCM5100A as of last board rev)
    over I2S
  - Run this at 44.1 kHZ and use it to measure time as well

5. Faux interrupt to update the display and NeoPixels
  - Update the ILI9341 display and the 28 NeoPixel LEDs
  - 24fps probably fine

*/

//----------------------------------------------------------------

// SPI port expanders
MCP23S17 exp1;
MCP23S17 exp2;
MCP23S17 exp3;

// Display
ILI9341* display = nullptr;

void setup() 
{
  Serial.begin(115200);

  // set up our chip select lines
  pinMode(EXP_CS, OUTPUT);
  digitalWrite(EXP_CS, HIGH);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  pinMode(DISPLAY_CS, OUTPUT);
  digitalWrite(DISPLAY_CS, HIGH);
  pinMode(POT1_CS, OUTPUT);
  digitalWrite(POT1_CS, HIGH);
  pinMode(POT2_CS, OUTPUT);
  digitalWrite(POT2_CS, HIGH);

  // extra expander lines
  pinMode(EXP_RST, OUTPUT);
  digitalWrite(EXP_RST, HIGH);
  pinMode(EXP_INTR, INPUT);

  // I2S lines
  pinMode(I2S_D, OUTPUT);
  pinMode(I2S_LR, OUTPUT);
  pinMode(I2S_SCK, OUTPUT);
  pinMode(I2S_BCK, OUTPUT);

  // start SPI
  SPI.begin(SCK, MISO, MOSI, EXP_CS);

  // initialize expanders
  Expanders::setupExpander1(&exp1);
  Expanders::setupExpander2(&exp2);
  Expanders::setupExpander3(&exp3);

  //initialize the display
  display = new ILI9341(&SPI, DISPLAY_DC, DISPLAY_RST);
  display->begin();


  /*
  SETUP NEEDS: 
  1. initialize pin modes
  2. initialize SPI
  3. Initialize variables for peripherals (expanders, digital pots, NeoPixels, etc)
  4. Attach interrupts (see above)
  */
}

void loop() 
{
  // put your main code here, to run repeatedly:
}
