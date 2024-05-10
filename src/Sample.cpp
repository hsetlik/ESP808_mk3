#include "Sample.h"

Sample::Sample(const String &name) : fileName(name),
                                     data(nullptr),
                                     lengthSamples(0)
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
    lengthSamples = metadata.lengthSamples;
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


bool Sample::isValidSampleFile(const String& path)
{
    if(Audio::isValidWAV(path))
    {
        auto metadata = Audio::getMetadataFor(path);
        return metadata.lengthSamples <= MAX_LENGTH_SAMPLES;
    }
    return false;
}

void Sample::getAvailableSamples(SamplePathArr* paths, uint16_t* numSamples)
{
    File root = SD.open("/");
    getSamplesInDir(paths, numSamples, root);
}

void Sample::getSamplesInDir(SamplePathArr* paths, uint16_t* numSamples, File dir)
{
    while(true)
    {
        File f = dir.openNextFile();
        // if the file is invalid, we've finished with this directory and break the recursion
        if(!f)
        {
            return;
        }
        if(f.isDirectory())
        {
            getSamplesInDir(paths, numSamples, f);
        }
        else if (isValidSampleFile(f.path()))
        {
            *paths[*numSamples] = f.path();
            *numSamples += 1;
        }
        f.close();

    }

}


//-----------------------------------------------------------------------------------

SamplerVoice::SamplerVoice() : sample(nullptr),
                               active(false),
                               sampleIdx(0),
                               retrigIdx(0),
                               state(Idle),
                               stepLevel(127),
                               retrigStepLevel(127),
                               pan(PAN_DEFAULT),
                               level(LEVEL_DEFAULT),
                               netLevel(LEVEL_DEFAULT),
                               netRetrigLevel(LEVEL_DEFAULT)
{
}

SamplerVoice::SamplerVoice(const String &path) : sample(nullptr),
                                                 active(false),
                                                 sampleIdx(0),
                                                 retrigIdx(0),
                                                 state(Idle),
                                                 stepLevel(127),
                                                 retrigStepLevel(127),
                                                 pan(PAN_DEFAULT),
                                                 level(LEVEL_DEFAULT),
                                                 netLevel(LEVEL_DEFAULT),
                                                 netRetrigLevel(LEVEL_DEFAULT)
{
    if (Audio::isValidWAV(path))
    {
        sample.reset(new Sample(path));
        active = true;
    }
    else
    {
        Serial.println("Error! No Valid WAV file at " + path);
    }
}

void SamplerVoice::nudgeLevel(bool dir)
{
    if(dir)
        level = std::min<float>(level + LEVEL_INCR, LEVEL_MAX);
    else
        level = std::max<float>(level - LEVEL_INCR, LEVEL_MIN);
}

void SamplerVoice::nudgePan(bool dir)
{
    if(dir)
        pan = std::min<float>(pan + PAN_INCR, PAN_MAX);
    else
        pan = std::max<float>(pan - PAN_INCR, PAN_MIN);
}

void SamplerVoice::trigger(uint8_t sLevel)
{
    switch(state)
    {
        case Idle:
            sampleIdx = 0;
            retrigIdx = 0;
            state = Playing;
            stepLevel = sLevel;
            // calculate the level for this step now
            netLevel = ((float)stepLevel / 127.0f) * level;
            break;
        case Playing:
            retrigIdx = 0;
            state = Retrig;
            retrigStepLevel = sLevel;
            netRetrigLevel = ((float)retrigStepLevel / 127.0f) * level;
            break;
        case Retrig:
            retrigIdx = 0;
            state = Retrig;
            retrigStepLevel = sLevel;
            netRetrigLevel = ((float)retrigStepLevel / 127.0f) * level;
            break;
    }
}


void SamplerVoice::renderSample(float* left, float* right)
{
    // this is basically just a finite state machine
    if(!active)
        return;
    switch(state)
    {
        case Idle:
            break;
        case Playing:
            float mono = sample->getSample(sampleIdx);
            float dLeft = (mono * netLevel) * pan;
            float dRight = (mono * netLevel) * (1.0f - pan);
            *left += dLeft;
            *right += dRight;
            sampleIdx++;
            if(sampleIdx >= sample->getLength()) // sample is finished playing
            {
                sampleIdx = 0;
                retrigIdx = 0;
                state = Idle;
            }
            break;
        case Retrig:
            float t = (float)retrigIdx / (float)RETRIG_LENGTH;
            float mono = Audio::fLerp(
                sample->getSample(sampleIdx) * netLevel, 
                sample->getSample(retrigIdx) * netRetrigLevel, 
                t);
            *left += mono * pan;
            *right += mono * (1.0f - pan);
            sampleIdx++;
            retrigIdx++;
            if(retrigIdx >= RETRIG_LENGTH || sampleIdx >= sample->getLength()) // retrig phase is done
            {
                // pass the retrig stuff to the regular stuff
                sampleIdx = retrigIdx;
                stepLevel = retrigStepLevel;
                netLevel = netRetrigLevel;
                state = Playing;
            }
            break;
        default:
            break; 
    }
}
