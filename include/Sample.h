#pragma once
#include "Audio.h"

// defaults and ranges for the digital voices' parameters

#define PAN_MIN 0.0f
#define PAN_MAX 1.0f
#define PAN_DEFAULT 0.5f

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
        // playback state stuff
        SampleState state;
        unsigned long sampleIdx;
        // the buffer to be allocated in PSRAM
        float* data;
        unsigned long lengthSamples;

        // pan and level state
        float pan;
        float currentLevel;
        
        float trackLevel;

        // retrigger handling stuff
        uint16_t samplesIntoRetrig;

    public:
        Sample(const String& path);
        ~Sample();
        // trigger this sample
        void trigger(uint8_t level=127);

        void renderNext(float* l, float* r);
};