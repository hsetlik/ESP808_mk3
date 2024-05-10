#pragma once
#include "Audio.h"
#include <memory>

// defaults and ranges for the digital voices' parameters

#define PAN_MIN 0.0f
#define PAN_MAX 1.0f
#define PAN_DEFAULT 0.5f
#define PAN_INCR 0.02f

#define LEVEL_MIN 0.0f
#define LEVEL_MAX 0.85f
#define LEVEL_DEFAULT 0.65f
#define LEVEL_INCR 0.01f

// when retriggering, fade between the beginning and end of the sample for about 30ms
#define RETRIG_LENGTH 600 

/*
    With 32 bit mono and current sample rate, 1s of audio takes up 176,400 bytes out of the total 2MB of PSRAM
    in the WROOM module. In other words we can fit about 11.33 seconds of mono audio data in PSRAM altogether. 
    If we round down to 11s (for a small margin of error) we can do:
    - 4 channels with a 2.75 second length limit
    - 3 channels with a 3.66 second length limit

    TODO: perhaps worth buying a WROOM with more PSRAM or compressing down to 24 or 16 bit for more channels?
*/

#ifdef THREE_CHANNEL
    #define NUM_SAMPLER_VOICES 3
    #define MAX_LENGTH_SAMPLES 161700
#else
    #define NUM_SAMPLER_VOICES 4
    #define MAX_LENGTH_SAMPLES 121275
#endif

// Ugly typedef/pointer situation but in the interest of not using vector I'll live with it
#define FILES_TO_SCAN 300
typedef String SamplePathArr[FILES_TO_SCAN];


//------------------------------------------------------------------------------------

enum SampleState : uint8_t
{
Idle,
Playing,
Retrig
};

class Sample
{
    private:
        const String fileName;
;
        // the buffer to be allocated in PSRAM
        float* data;
        uint32_t lengthSamples;

        //helper thing
        static void getSamplesInDir(SamplePathArr* paths, uint16_t* numSamples, File dir);
    public:
        Sample(const String& path);
        ~Sample();

        float getSample(uint32_t idx) {return data[idx];}

        uint32_t getLength() { return lengthSamples; }

        // helper to check if a given WAV on the SD card can be safely used as a sample
        static bool isValidSampleFile(const String& path);

        // get a list of all the valid sample files on the SD card
        static void getAvailableSamples(SamplePathArr* paths, uint16_t* numSamples);

};

//-----------------------------------------------------

class SamplerVoice
{
private:
    std::unique_ptr<Sample> sample;
    bool active; // whether the Sample Ptr is valid

    // playback state stuff
    uint32_t sampleIdx;
    uint32_t retrigIdx;
    SampleState state;
    uint8_t stepLevel;
    uint8_t retrigStepLevel;

    // control state stuff
    float pan;
    float level;
    // helpers to not recalculate the step's float level for every sample
    float netLevel;
    float netRetrigLevel;
public:
    SamplerVoice();
    SamplerVoice(const String& path);
    bool isActive() { return active; }
    
    // adjust this sample's parameters
    void nudgeLevel(bool dir);
    void nudgePan(bool dir);

    // trigger playback with a given level
    void trigger(uint8_t stepLevel);

    // the heavy lifting of rendering the audio
    void renderSample(float* left, float* right);
};