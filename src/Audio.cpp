#include "Audio.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int32_t Audio::toDACSample(float sample)
{
    // sample is in the range -1.0f to 1.0f
    return (int32_t)(sample * (float)I32_MAX);
}

float Audio::toFloatSample(uint32_t sample)
{
    return (float)sample / F32_MAX;
}

bool Audio::isValidWAV(const String &name)
{
    if (SD.exists(name))
    {
        auto file = SD.open(name);
        // if we have a file with this name,
        // load its first 16 bytes into memory
        // to check that we have a valid WAV header
        char header[16];
        file.readBytes(header, 16);
        // vars for the three strings we need to check
        String riffStr = "";
        String wavStr = "";
        String fmtStr = "";

        for (uint8_t i = 0; i < 4; i++)
        {
            riffStr += header[0 + i];
            wavStr += header[8 + i];
            fmtStr += header[12 + i];
        }
        file.close();
        return (riffStr == "RIFF") && (wavStr == "WAVE") && (fmtStr == "fmt ");
    }
    else
        return false;
}

WAVMetadata Audio::getMetadataFor(const String &name)
{
    WAVMetadata data;
    data.fileName = name;
    if (SD.exists(name))
    {
        auto file = SD.open(name);
        // the total header should be 44 bytes long
        char header[44];
        if (file.readBytes(header, 44) != 44)
            debugMsg("Failed to read WAV header at: " + name);
        file.close();
        // the channel mode is a 16 bit word at bytes 22 & 23
        uint16_t channelMode = ((uint16_t)header[22] << 8) | (uint16_t)header[23];
        data.isStereo = (channelMode == 0x01) ? false : true;

        // right of that is a 32 bit dword containing the sample rate
        uint32_t sampleRate = 0;
        for (uint8_t i = 0; i < 4; i++)
        {
            sampleRate = sampleRate | ((uint32_t)header[24 + i] << (i * 8));
        }
        data.sampleRate = sampleRate;

        // bit depth is again a 16 bit word on bytes 34 & 35
        uint16_t bitsPerSample = ((uint16_t)header[34] << 8) | (uint16_t)header[35];
        data.bitsPerSample = bitsPerSample;

        // and finally the data chunk size is a dword at byte 40
        uint32_t size = 0;
        for (uint8_t i = 0; i < 4; i++)
        {
            size = size | ((uint32_t)header[40 + i] << (i * 8));
        }
        data.dataChunkSize = size;

        // now we can calculate how much PSRAM we need to allocate and how long our sample is
        uint32_t sampleBytes = data.isStereo ? (uint32_t)(bitsPerSample * 2) / 8 : (uint32_t)bitsPerSample / 8;
        data.lengthSamples = data.dataChunkSize / sampleBytes;
    }
    return data;
}

bool Audio::mixDownAudio(WAVMetadata &wav, float *buffer)
{
    // 1. Grip the file
    auto file = SD.open(wav.fileName);
    if (!file.seek(44))
    {
        debugMsg("Could not seek past header for file: " + wav.fileName);
        return false;
    }

    const size_t bytesPerSample = (size_t)wav.bitsPerSample / 8;
    size_t bufferIdx = 0;
    if (bytesPerSample == 4) // our samples are 32-bit floats
    {
        if (wav.isStereo)
        {

            for (size_t s = 0; s < wav.lengthSamples; s++)
            {
                float lSample = 0.0f;
                uint8_t *lBuf = reinterpret_cast<uint8_t *>(&lSample);
                file.read(lBuf, bytesPerSample);
                float rSample = 0.0f;
                uint8_t *rBuf = reinterpret_cast<uint8_t *>(&rSample);
                file.read(rBuf, bytesPerSample);
                buffer[s] = (lSample + rSample) * 0.5f;
            }
        }
        else
        {
            for (size_t s = 0; s < wav.lengthSamples; s++)
            {
                float fSample = 0.0f;
                uint8_t *buf = reinterpret_cast<uint8_t *>(&fSample);
                file.read(buf, bytesPerSample);
                buffer[s] = fSample;
            }
        }
        file.close();
        return true;
    }
    else // our samples are either 16-bit or 24-bit unsigned ints
    {
        uint32_t maxSampleVal = 0;
        for (uint8_t i = 0; i < bytesPerSample; i++)
        {
            uint8_t b = ~0;
            uint32_t mask = (uint32_t)b << (i * 8);
            maxSampleVal = mask | maxSampleVal;
        }

        if (wav.isStereo)
        {
            for (size_t s = 0; s < wav.lengthSamples; s++)
            {
                uint32_t iLeft = 0;
                uint8_t *lBuf = reinterpret_cast<uint8_t *>(&iLeft);
                file.read(lBuf, bytesPerSample);
                float nLeft = (float)iLeft / (float)maxSampleVal;
                nLeft = (nLeft - 0.5f) * 2.0f;

                uint32_t iRight = 0;
                uint8_t *rBuf = reinterpret_cast<uint8_t *>(&iRight);
                file.read(rBuf, bytesPerSample);
                float nRight = (float)iRight / (float)maxSampleVal;
                nRight = (nRight - 0.5f) * 2.0f;

                buffer[s] = (nLeft + nRight) * 0.5f;
            }
        }
        else
        {
            for (size_t s = 0; s < wav.lengthSamples; s++)
            {
                uint32_t iSample = 0;
                uint8_t *buf = reinterpret_cast<uint8_t *>(&iSample);
                file.read(buf, bytesPerSample);

                float fNorm = (float)iSample / (float)maxSampleVal;
                buffer[s] = (fNorm - 0.5f) * 2.0f;
            }
        }
        file.close();
        return true;
    }
}


float Audio::fLerp(float a, float b, float t)
{
    return a + ((b - a) * t);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PCM510xA::PCM510xA(uint8_t i2sPort, uint8_t bck, uint8_t sck, uint8_t ws, uint8_t dout) : 
port(i2sPort), 
bckPin(bck), 
mckPin(sck), 
wsPin(ws), 
doutPin(dout), 
bytesWritten(0)
{
    // wait to set up the config objects until init()
}

void PCM510xA::init()
{
    // 1. Set up config and call i2s_driver_install

    config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    config.sample_rate = SAMPLE_RATE;
    config.bits_per_sample = (i2s_bits_per_sample_t)I2S_BITS_PER_SAMPLE_32BIT;
    config.channel_format = (i2s_channel_fmt_t)I2S_CHANNEL_FMT_RIGHT_LEFT;
    config.communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_I2S;
    config.tx_desc_auto_clear = false;
    config.dma_buf_count = 8;
    config.dma_buf_len = BUFFER_LENGTH;
    config.use_apll = false;
    config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;

    esp_err_t installErr = i2s_driver_install((i2s_port_t)port, &config, 0, NULL);
    if (installErr != ESP_OK)
    {
        String errName = esp_err_to_name(installErr);
        debugMsg("I2S install failed with error: " + errName);
    }
    else
        debugMsg("I2S install succeded");

    // 2. Set up pin config
    pinConfig.bck_io_num = bckPin;
    pinConfig.mck_io_num = mckPin;
    pinConfig.ws_io_num = wsPin;
    pinConfig.data_out_num = doutPin;
    pinConfig.data_in_num = I2S_PIN_NO_CHANGE;

    esp_err_t pinErr = i2s_set_pin((i2s_port_t)port, &pinConfig);
    if (pinErr != ESP_OK)
    {
        String errName = esp_err_to_name(pinErr);
        debugMsg("I2S pin setup failed with error: " + errName);
    }
    else
        debugMsg("I2S pin setup succeeded");

    i2s_zero_dma_buffer((i2s_port_t)port);
}

uint32_t PCM510xA::getInputInterruptInterval()
{
    float sampleLengthMicros = 1000000.0f / (float)SAMPLE_RATE;
    return (uint32_t)(sampleLengthMicros * (float)BUFFER_LENGTH);
}

void PCM510xA::transmitBuffer(AudioBuffer &buf)
{
    // NOTE: if we were using a 16 or 24 bit DAC we would need to convert the input data to PCM int formats,
    //  but I2S uses the same 32 bit float format that c++ natively uses when using 32 bit devices
    esp_err_t i2sErr = i2s_write((i2s_port_t)port, &buf, AUDIO_BUFFER_BYTES, &bytesWritten, portMAX_DELAY);
    if(i2sErr != ESP_OK)
    {
        String errName = esp_err_to_name(i2sErr);
        debugMsg("I2S PCM transmission failed with error: " + errName);      
    }
    else if(bytesWritten != AUDIO_BUFFER_BYTES)
    {
        debugMsg("Warning! Only wrote " + String(bytesWritten) + " bytes of intended " + String(AUDIO_BUFFER_BYTES));
    }
}