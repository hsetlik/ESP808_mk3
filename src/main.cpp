#include <Arduino.h>
#include "Controls.h"

/*
ISRs that need to go here:

1. Rising edge interrupt on the expander interrupt pin:
  - Check which pin triggered the interrupt, 
    pass that data to the business code to handle
    the rotary encoders

2. Timer interrupt to check and update the other expanders:
  - Update the state of the 8 analog trigger pins
  - Check all the pushbutton states
  - TODO: choose a frequency

3. Timer interrupt to update the audio DAC:
  - Send the next sample in the current buffer
    to the DAC (TI PCM5100A as of last board rev)
    over I2S
  - Run this at 44.1 kHZ and use it to measure time as well

4. Faux interrupt to update the display and NeoPixels
  - Update the ILI9341 display and the 28 NeoPixel LEDs
  - 24fps probably fine

*/





void setup() 
{
  Serial.begin(115200);
  // put your setup code here, to run once:
}

void loop() 
{
  // put your main code here, to run repeatedly:
}
