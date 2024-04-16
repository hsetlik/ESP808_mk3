#pragma once
#include "Controls.h"
#include "Display.h"
#include "Outputs.h"
#include <Audio.h>
#include <FastLED.h>

// definitions for param mins and maxes
#define TEMPO_MIN 30.0f
#define TEMPO_DEFAULT 120.0f
#define TEMPO_INCREMENT_BIG 2.0f
#define TEMPO_INCREMENT_SMALL 0.1f
#define TEMPO_MAX 400.0f

#define LEVEL_MAX 127
#define LEVEL_MIN 30

#define SEQ_LENGTH 64
#define PG_LENGTH 16

#define NUM_HARDWARE_TRACKS 8

#define MAX_TRACKS 12

#define TRIGGER_DURATION_MS 15

#define NUM_LEDS 28



//----------------------------------------------------------------
struct Step
{
    bool on = false;
    uint8_t level = LEVEL_MAX;

    bool operator==(Step& other)
    {
        return (on == other.on) && (level == other.level);
    }
    void operator=(Step& other)
    {
        on = other.on;
        level = other.level;
    }
};

struct Track
{
Step steps[SEQ_LENGTH];
};

class Sequence
{
public:
    const uint8_t numTracks;
    Track tracks[MAX_TRACKS];
    Sequence(uint8_t nTracks) :
    numTracks(std::min<uint8_t>(nTracks, MAX_TRACKS))
    {

    }
};



//----------------------------------------------------------------


//----------------------------------------------------------------
class Processor
{
private:
    // state variables
    bool isPlaying;
    float currentTempo;
    uint8_t selectedTrack;
    bool altDown;
    unsigned long lastAltClickAt;
    uint16_t buttonHoldState;

    // timing state
    unsigned long lastTickMs;

    // control mode stuff
    bool digitalMode;
    bool pageMode;

    // Pattern data
    Sequence seq;
    float fCurrentStep; 
    uint8_t lastStep;
    unsigned long lastTriggerMs[8];

    // Color references for pixels
    CRGB trackColorsDim[NUM_HARDWARE_TRACKS];
    CRGB trackColorsMid[NUM_HARDWARE_TRACKS];
    CRGB trackColorsBright[NUM_HARDWARE_TRACKS];

    CRGB offColor;
    CRGB cursorColor;

    // pixel helpers
    CRGB getStepPixelColor(uint8_t button, uint8_t track);
    CRGB getTrackPixelColor(uint8_t track);
    CRGB getPagePixelColor(uint8_t pg);
    CRGB getPixelColor(uint8_t pixel);




    // state helpers
    bool altKey() { return altDown || (millis() - lastAltClickAt) < 80; }
    void nudgeTempo(bool up);
    void nudgeSelectedTrack(bool up);
    void nudgeStepLevel(uint8_t button, bool up);
    void handleSequenceKeyClick(uint8_t button);
    // get a pointer to the step state that a given button currently refers to
    uint8_t stepIdxForButton(uint8_t button);
    Step* stepForButton(uint8_t b);
    
    // sequence button helpers
    bool sequenceKeyHeld() {return buttonHoldState != 0; }
    uint8_t getHeldSequenceKey();
    // timing helpers
    // how many ms should a step last before advancing
    unsigned long stepDurationMs();

public:
    Processor();
    // these get wired up to the hardware code from Controls.h in main.cpp
    void handleClick(uint8_t button);
    void handlePressStart(uint8_t button);
    void handlePressEnd(uint8_t button);
    void handleEncoder(uint8_t enc, bool up);
    // call this in loop() in main.cpp, it updates all the logic and timing
    void tick();
    // these get called as requested to update values for the ISRs
    uint8_t getTriggerState();
    uint64_t getPotLevels();
    // these get called however often to update the screen and neopixels
    void updateDisplay(ILI9341* display);
    void updatePixels(CRGB* pixels);

};

