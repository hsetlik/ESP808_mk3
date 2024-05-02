#include "Sample.h"


Sample::Sample(const String& name) : 
fileName(name),
isPlaying(false),
sampleIdx(0),
data(nullptr),
lengthSamples(0)
{
    if(!SD.exists(fileName))
    {
        Serial.println("Error! No valid file at path: " + fileName);
    }
    if(!Audio::isValidWAV(fileName))
    {
        Serial.println("Error! Invalid WAV file at: " + fileName);
    }
    auto metadata = Audio::getMetadataFor(fileName);
    lengthSamples = (unsigned long)metadata.lengthSamples;
    // now we allocate psram based on duration of the file, not on the data size
    // because we'll mix everything down to 32 bit mono
    data = (float*)ps_malloc((size_t)(lengthSamples * 4));

    // now we do the rendering
    if(!Audio::mixDownAudio(metadata, data))
        Serial.println("Error! Failed to mix down audio");

}


Sample::~Sample()
{
    free(data);
}