#include "Sample.h"

Sample::Sample(const String &name) : fileName(name),
                                     state(SampleState::Idle),
                                     sampleIdx(0),
                                     data(nullptr),
                                     lengthSamples(0),
                                     pan(PAN_DEFAULT),
                                     currentLevel(1.0f),
                                     trackLevel(1.0f),
                                     samplesIntoRetrig(0)
{
    if (!SD.exists(fileName))
    {
        Serial.println("Error! No valid file at path: " + fileName);
    }
    if (!Audio::isValidWAV(fileName))
    {
        Serial.println("Error! Invalid WAV file at: " + fileName);
    }
    auto metadata = Audio::getMetadataFor(fileName);
    lengthSamples = (unsigned long)metadata.lengthSamples;
    // now we allocate psram based on duration of the file, not on the data size
    // because we'll mix everything down to 32 bit mono
    data = (float *)ps_malloc((size_t)(lengthSamples * 4));

    // now we do the rendering
    if (!Audio::mixDownAudio(metadata, data))
        Serial.println("Error! Failed to mix down audio");
}

Sample::~Sample()
{
    free(data);
}

void Sample::trigger(uint8_t level)
{
    currentLevel = trackLevel * ((float)level / 127.0f);
    if (state == SampleState::Idle)
    {
        state = SampleState::Playing;
        sampleIdx = 0;
        samplesIntoRetrig = 0;
    }
    else if (lengthSamples - sampleIdx > RETRIG_LENGTH)
    {
        state == SampleState::Retrig;
        samplesIntoRetrig = 0;
    }
}

void Sample::renderNext(float *l, float *r)
{
    switch (state)
    {
    case Idle:
        break;
    case Playing:
        float &fMono = data[sampleIdx];
        *l += currentLevel * (fMono * pan);
        *r += currentLevel * (fMono * (1.0f - pan));
        ++sampleIdx;
        if (sampleIdx == lengthSamples) // we've finished playback
        {
            state = SampleState::Idle;
            sampleIdx = 0;
        }
        break;
    case Retrig:
        float fMix = (float)samplesIntoRetrig / (float)RETRIG_LENGTH;
        float fMono = (data[samplesIntoRetrig] * fMix) + (data[sampleIdx] * (1.0f - fMix));
        *l += currentLevel * (fMono * pan);
        *r += currentLevel * (fMono * (1.0f - pan));
        ++samplesIntoRetrig;
        ++sampleIdx;
        // if we're at the end of the retrig
        if (samplesIntoRetrig == RETRIG_LENGTH)
        {
            sampleIdx = samplesIntoRetrig;
            samplesIntoRetrig = 0;
            state = SampleState::Playing;
        }
        break;
    default:
        break;
    }
}