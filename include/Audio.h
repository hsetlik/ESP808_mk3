#pragma once
#include "Pins.h"
#include "soc/soc_caps.h"
#include <libb64/cencode.h>
#include <esp32-hal-log.h>
#include <atomic>
#include <SD.h>

#if ESP_IDF_VERSION_MAJOR == 5
#include <driver/i2s_std.h>
#else
#include <driver/i2s.h>
#endif

#define BUFFER_LENGTH 120
#define NUM_DMA_BUFFERS 4
/*
Size of the DMA buffer in bytes = BUFFER_LENGTH * NUM_DMA_BUFFERS * bytes per sample (4 in our case)
Note: it's important that the total size of the DMA buffers not exceed 4092 bytes
Explained here: https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32/api-reference/peripherals/i2s.html#_CPPv4N19i2s_driver_config_t11dma_buf_lenE

WITH CURRENT VALUES: should have be using 3840 bytes which *should* be ok
*/
#define SAMPLE_RATE 44100

// using 32 bit samples (4 bytes) x 2 channels for stereo
static const size_t AUDIO_BUFFER_BYTES = BUFFER_LENGTH * 4 * 2;

static const int32_t I32_MAX = 2147483647;

static const float F32_MAX = 2147483647.0f;

typedef int32_t DACBuffer[BUFFER_LENGTH * 2];

typedef float AudioBuffer[BUFFER_LENGTH * 2];

// struct to store the metadata of a given WAV file
// See here for more info: https://www.ringthis.com/dev/wave_format.htm

struct WAVMetadata
{
    String fileName = "invalid";
    bool isStereo = true;
    uint32_t sampleRate = SAMPLE_RATE;
    uint16_t bitsPerSample = 32;
    uint32_t dataChunkSize = 0;
    uint32_t lengthSamples = 0;

};



// namespace for all our PCM/ audio file handling 
namespace Audio
{
    int32_t toDACSample(float sample);
    float toFloatSample(uint32_t dacSample);
    //WAV decoding stuff--------------------------------------------
    // check if the file at this path on the SD card has a valid WAV header
    bool isValidWAV(const String& name);
    /**
     * @brief Get the WAV Metadata For a file on the SD card
     * 
     * @param name the file path
     * @return WAVMetadata 
     */
    WAVMetadata getMetadataFor(const String& name);

    /**
     * @brief This does the heavy lifting of 
     * loading the wav PCM data into our PSRAM buffer in a 
     * processing-friendly way
     * 
     * @param wav The file's metadata
     * @param buffer The buffer in PRSAM (note: this must be allocated with ps_malloc() BEFORE it's passed to this function)
     * @return success or failure 
     */
    bool mixDownAudio(WAVMetadata& wav, float* buffer);
}

//--------------------------------------------------------------


/*
    This should handle all the digital audio output to I2S.
    This code is loosely based on the existing "ESP32-audioI2S" library
    found here: https://github.com/schreibfaul1/ESP32-audioI2S/tree/master

    Documentation for esp_idf's I2C API: https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32/api-reference/peripherals/i2s.html

    If we store each sample as mono and limit length to 2000ms, one sample will need:
    - 352.8 kB at 32 bit / 44.1kHz
    - 384 kB at 32 bit / 48kHz
    
*/



//----------------------------------------------------------------------------------------------------------------------

/*
    Interface for Texas Instruments' PCM501xA
    series of I2S stereo DACs
    - Datasheet: https://www.ti.com/lit/ds/symlink/pcm5100a.pdf?ts=1713486434955
    
*/
class PCM510xA
{
public:
    /**
     * @brief Default constructor
     * 
     * @param i2sPort The I2S port to use- the ESP32-S3 has two I2S peripherals to choose from
     * @param bck Clock pin
     * @param ws  Word select pin 
     * @param dout Data output pin
     */
    PCM510xA(uint8_t i2sPort, uint8_t bck, uint8_t ws, uint8_t dout);

    /**
     * @brief Set up the pins and DMA stuff for I2S.
     * This needs to set up our config objects and run the
     * appropriate calls to esp's I2S API 
     * 
     */
    void init();

    /**
     * @brief This calculates how often the ISR to
     *  request a new data buffer should run
     *  
     * @return uint32_t interval between input requests in 
     * terms of cycles of a 1MHz timer
     */
    uint32_t getInputInterruptInterval();
    
private:
    const uint8_t port;
    const uint8_t bckPin;
    const uint8_t wsPin;
    const uint8_t doutPin;
    i2s_config_t config;
    i2s_pin_config_t pinConfig;

};