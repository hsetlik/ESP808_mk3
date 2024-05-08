#include "Processor.h"
#include "Outputs.h"
#include "Audio.h"
#define OUTPUT_UPDATE_HZ 300

#define DISPLAY_UPDATE_HZ 24

//check the battery level every 30 seconds
#define BATT_CHECK_INTERVAL 30000
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

4. Timer interrupt to request and render the next audio buffer for
the digital voices
  

4. Faux interrupt to update the display and NeoPixels
  - Update the ILI9341 display and the NUM_PIXELS NeoPixel LEDs
  - 24fps probably fine

*/



//----------------------------------------------------------------

// SPI port expanders
Adafruit_MCP23X17 exp1;
Adafruit_MCP23X17 expander2;
Adafruit_MCP23X17 exp3;

// Display
ILI9341* display = nullptr;

// Neopixels
CRGB pixels[NUM_PIXELS];

// DAC
PCM510xA audioDAC(0, I2S_BCK, I2S_SCK, I2S_LR, I2S_D);

// Control state stuff
Encoders encoders;
Buttons buttons;

// Processor
Processor proc;

// Timer stuff
hw_timer_t* buttonTimer = NULL;
hw_timer_t* outputTimer = NULL;
hw_timer_t* audioTimer = NULL;
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
volatile uint64_t lastPotLevels = 0; // note: this is 7 8bit values packed in
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

// Digital audio stuff
volatile bool needsNewAudio = false;
AudioBuffer audioBuf;

void IRAM_ATTR audioISR()
{
  audioDAC.transmitBuffer(audioBuf);
  needsNewAudio = true;
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

  //Init I2S
  audioDAC.init();

  // start SPI
  SPI.begin(SCK, MISO, MOSI, EXP_CS);

  // initialize expanders
  Expanders::setupExpander1(&exp1);
  Expanders::setupExpander2(&expander2);
  Expanders::setupExpander3(&exp3);

  //initialize the display
  display = new ILI9341(&SPI, DISPLAY_DC, DISPLAY_RST);
  display->begin();

  // set up the neopixels
  FastLED.addLeds<NEOPIXEL, PIXELS>(pixels, NUM_PIXELS);

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
  timerAlarmEnable(outputTimer);

  // audio interrupt
  audioTimer = timerBegin(2, 80, true);
  timerAttachInterrupt(audioTimer, &audioISR, true);
  timerAlarmWrite(audioTimer, audioDAC.getInputInterruptInterval(), true);
  timerAlarmEnable(audioTimer);

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

  // check our battery level at startup
  proc.checkBatteryLevel();
}

unsigned long lastUpdateMs = 0;
unsigned long lastBatteryCheckMs = 0;
unsigned long now = 0;
RingBuffer<unsigned long, 25> frameTimes;
uint16_t frameCounter = 0;

uint16_t getFrameRate()
{
  unsigned long sum = 0;
  for(uint8_t i = 1; i < 25; i++)
  {
    sum += (frameTimes[i] - frameTimes[i - 1]);
  }
  uint16_t averageMs = (uint16_t)(sum / 24);
  return 1000 / averageMs;
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

  while(encoderTriggered)
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
  // 3. check if we need to update our pixels & display
  now = millis();
  if(now - lastUpdateMs > (1000 / DISPLAY_UPDATE_HZ))
  {
    proc.updatePixels(pixels);
    FastLED.show();
    proc.updateDisplay(display);
    lastUpdateMs = now;
    frameTimes.push(now);
    frameCounter++;
    if(frameCounter >= 200)
    {
      frameCounter = 0;
      proc.updateFrameRate(getFrameRate());
    }
  }

  // 4. update the battery if needed
  if(now - lastBatteryCheckMs > BATT_CHECK_INTERVAL)
  {
    proc.checkBatteryLevel();
    lastBatteryCheckMs = now;
  }
}
