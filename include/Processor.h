#pragma once
#include "Controls.h"
#include "Display.h"
#include "Outputs.h"
#include <Audio.h>

// definitions for param mins and maxes
#define TEMPO_MIN 30.0f
#define TEMPO_DEFAULT 120.0f
#define TEMPO_INCREMENT 1.0f
#define TEMPO_MAX 400.0f

#define LEVEL_MAX 127
#define LEVEL_MIN 30

#define SEQ_LENGTH 64
#define PG_LENGTH 16

#define NUM_HARDWARE_TRACKS 8

#define MAX_TRACKS 12

#define TRIGGER_DURATION_MS 15



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

    // timing state
    unsigned long lastTickMs;

    // control mode stuff
    bool digitalMode;

    // Pattern data
    Sequence seq;
    float fCurrentStep; 
    uint8_t lastStep;
    unsigned long lastTriggerMs[8];


    // state helpers

    bool altKey() { return altDown || (millis() - lastAltClickAt) < 50; }
    void nudgeTempo(bool up);
    void nudgeSelectedTrack(bool up);
    // timing helpers
    // how many ms should a step last before advancing?
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

};

