#pragma once
#include "Audio.h"




class Sample
{
    private:
        const String fileName;
        // playback state stuff
        bool isPlaying;
        unsigned long sampleIdx;
        // the buffer to be allocated in PSRAM
        float* data;
        unsigned long lengthSamples;

    public:
        Sample(const String& path);
        ~Sample();

};