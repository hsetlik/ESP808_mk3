#include "Processor.h"
#include "Outputs.h"
#define OUTPUT_UPDATE_HZ 300
/*
ISRs that need to go here:

1. Falling edge interrupt on the expander interrupt pin:
  - Check which pin triggered the interrupt, 
    pass that data to the business code to handle
    the rotary encoders

2. Timer interrupt to check the buttons
  - Check all the pushbutton states

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
Adafruit_MCP23X17 exp1;
Adafruit_MCP23X17 expander2;
Adafruit_MCP23X17 exp3;

// Display
ILI9341* display = nullptr;

// Control state stuff
Encoders encoders;
Buttons buttons;

// Processor
Processor proc;

// Timer stuff
hw_timer_t* buttonTimer = NULL;
hw_timer_t* outputTimer = NULL;
//portMUX_TYPE buttonTimerMux = portMUX_INITIALIZER_UNLOCKED;

// ISRs --------------------------------------------------------------------
// Button stuff
volatile bool buttonDataReady = false;
volatile uint16_t bits1 = 0;
volatile uint16_t bits2 = 0;
volatile uint16_t bits3 = 0;

void IRAM_ATTR buttonISR()
{
  bits1 = exp1.readGPIOAB();
  bits2 = expander2.readGPIOAB();
  bits3 = exp3.readGPIOAB();
  buttonDataReady = true;
}

// Encoder stuff
volatile bool encoderTriggered = false;
volatile uint8_t lastEncoderPin = 0;

void IRAM_ATTR encoderISR()
{
  encoderTriggered = true;
  lastEncoderPin = expander2.getLastInterruptPin();
}

// Output stuff
volatile bool newOutputNeeded = false;
volatile uint64_t lastPotLevels = 0; // note: this is 8 8bit values packed in
volatile uint64_t potLevels = 0; 
volatile uint8_t lastGateLevels = 0;
volatile uint8_t gateLevels = 0;

void IRAM_ATTR outputISR()
{
  if(potLevels != lastPotLevels)
  {
    // levels have changed and we need to update
    uint32_t data2 = (uint32_t)potLevels;
    uint32_t data1 = (uint32_t)(potLevels >> 32);
    MCP4331::setLevels(&SPI, POT1_CS, data1);
    MCP4331::setLevels(&SPI, POT2_CS, data2);
    lastPotLevels = potLevels;
  }
  if(gateLevels != lastGateLevels)
  {
    exp3.writeGPIOB(gateLevels);
    lastGateLevels = gateLevels;
  }
  newOutputNeeded = true;
}

//----------------------------------------------------------------------

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
  Expanders::setupExpander2(&expander2);
  Expanders::setupExpander3(&exp3);

  //initialize the display
  display = new ILI9341(&SPI, DISPLAY_DC, DISPLAY_RST);
  display->begin();

  // initialize the timers

  // button interrupt
  buttonTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(buttonTimer, &buttonISR, true);
  timerAlarmWrite(buttonTimer, 1000000 / BUTTON_CHECK_HZ, true);
  timerAlarmEnable(buttonTimer);

  // output interrupt
  outputTimer = timerBegin(1, 80, true);
  timerAttachInterrupt(outputTimer, &outputISR, true);
  timerAlarmWrite(outputTimer, 1000000 / OUTPUT_UPDATE_HZ, true);
  timerAlarmEnable(buttonTimer);

  // attach the encoder interrupt
  attachInterrupt(EXP_INTR, encoderISR, FALLING);


  // attach the encoder callbacks
  for(uint8_t e = 0; e < 4; e++)
  {
    encoders.setCallback(e,[e](bool up){proc.handleEncoder(e, up);});
  }

  // button callbacks
  for(uint8_t b = 0; b < NUM_BUTTONS; b++)
  {
    buttons[b].attachOnClick([b](){proc.handleClick(b);});
    buttons[b].attachOnPressStart([b](){proc.handlePressStart(b);});
    buttons[b].attachOnPressStop([b](){proc.handlePressEnd(b);});

  }

  /*
  SETUP NEEDS: 
  1. initialize pin modes
  2. initialize SPI
  3. Initialize variables for peripherals (expanders, digital pots, NeoPixels, etc)
  4. Attach interrupts (see above)
  5. Wire function pointers from controls to processor object
  */
}

void loop() 
{
  // 1. advance the processor
  proc.tick();
  
  // 2. Do any chores for the next round of interrupts as needed
  if(buttonDataReady)
  {
    buttons.tick(bits1, bits2, bits3);
    buttonDataReady = false;
  }

  if(encoderTriggered)
  {
    encoders.interruptSent(lastEncoderPin);
    encoderTriggered = false;
  }

  if(newOutputNeeded)
  {
    potLevels = proc.getPotLevels();
    gateLevels = proc.getTriggerState();
    newOutputNeeded = false;
  }
}
