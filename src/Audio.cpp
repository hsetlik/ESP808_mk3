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

bool Audio::isValidWAV(const String& name)
{
    if(SD.exists(name))
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

        for(uint8_t i = 0; i < 4; i++)
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

WAVMetadata Audio::getMetadataFor(const String& name)
{
    WAVMetadata data;
    data.fileName = name;
    if(SD.exists(name))
    {
        auto file = SD.open(name);
        // the total header should be 44 bytes long
        char header[44];
        if(file.readBytes(header, 44) != 44)
            Serial.println("Failed to read WAV header at: " + name);
        file.close();
        // the channel mode is a 16 bit word at bytes 22 & 23
        uint16_t channelMode = ((uint16_t)header[22] << 8) | (uint16_t)header[23];
        data.isStereo = (channelMode == 0x01) ? false : true;

        // right of that is a 32 bit dword containing the sample rate
        uint32_t sampleRate = 0;
        for(uint8_t i = 0; i < 4; i++)
        {
            sampleRate = sampleRate | ((uint32_t)header[24 + i] << (i * 8));
        }
        data.sampleRate = sampleRate;

        // bit depth is again a 16 bit word on bytes 34 & 35
        uint16_t bitsPerSample = ((uint16_t)header[34] << 8) | (uint16_t)header[35];
        data.bitsPerSample = bitsPerSample;

        // and finally the data chunk size is a dword at byte 40
        uint32_t size = 0;
        for(uint8_t i = 0; i < 4; i++)
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

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PCM510xA::PCM510xA(uint8_t i2sPort, uint8_t bck, uint8_t ws, uint8_t dout) : port(i2sPort), bckPin(bck), wsPin(ws), doutPin(dout)
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
    if(installErr != ESP_OK)
    {
        String errName = esp_err_to_name(installErr);
        Serial.println("I2S install failed with error: " + errName);
    }
    else
        Serial.println("I2S install succeded");
    
    // 2. Set up pin config
    pinConfig.bck_io_num = bckPin;
    pinConfig.ws_io_num = wsPin;
    pinConfig.data_out_num = doutPin;
    pinConfig.data_in_num = I2S_PIN_NO_CHANGE;

    esp_err_t pinErr = i2s_set_pin((i2s_port_t)port, &pinConfig);
    if(pinErr != ESP_OK)
    {
        String errName = esp_err_to_name(pinErr);
        Serial.println("I2S pin setup failed with error: " + errName);
    }
    else
        Serial.println("I2S pin setup succeeded");

    i2s_zero_dma_buffer((i2s_port_t)port);

}

uint32_t PCM510xA::getInputInterruptInterval()
{
    float sampleLengthMicros = 1000000.0f / (float)SAMPLE_RATE;
    return (uint32_t)(sampleLengthMicros * (float)BUFFER_LENGTH);
}