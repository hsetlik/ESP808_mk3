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
    public:
        Sample(const String& path);
        ~Sample();

        float getSample(uint32_t idx) {return data[idx];}

        uint32_t getLength() { return lengthSamples; }

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