#pragma once
#include "Controls.h"
#include "Display.h"

// definitions for param mins and maxes
#define TEMPO_MIN 30.0f
#define TEMPO_DEFAULT 120.0f
#define TEMPO_INCREMENT 3.0f
#define TEMPO_MAX 400.0f

#define LEVEL_MAX 127
#define LEVEL_MIN 30

#define SEQ_LENGTH 64
#define PG_LENGTH 16

#define NUM_HARDWARE_TRACKS 8

#define MAX_TRACKS 12


enum HardwareTrack : uint8_t
{
Kick1,
Kick2,
Snare,
Clap,
Clave,
OpenHat,
ClosedHat,
Digital
};

//----------------------------------------------------------------
struct Step
{
    bool on = false;
    uint8_t level = LEVEL_MAX;

    bool operator==(Step& other)
    {
        return (on == other.on) && (level == other.level);
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


class Processor
{
private:
    // state variables
    bool isPlaying;
    float currentTempo;
    HardwareTrack selectedTrack;
    bool altDown;
    unsigned long lastAltClickAt;
    Sequence seq;
    // our playhead in the sequence. 
    // Represented as a float for reasons that will
    // make sense in the .cpp file
    float currentStep; 


    // state helpers

    bool altKey() { return altDown || (millis() - lastAltClickAt) < 50; }
    void nudgeTempo(bool up);

    // timing helpers

    


public:
    Processor();
    // these get wired up to the hardware code from Controls.h in main.cpp
    void handleClick(uint8_t button);
    void handlePressStart(uint8_t button);
    void handlePressEnd(uint8_t button);
    void handleEncoder(uint8_t enc, bool up);
};

