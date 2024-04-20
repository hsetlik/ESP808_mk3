#pragma once
#include <Arduino.h>
#include "soc/soc_caps.h"
#include <libb64/cencode.h>
#include <esp32-hal-log.h>
#include <atomic>

#if ESP_IDF_VERSION_MAJOR == 5
#include <driver/i2s_std.h>
#else
#include <driver/i2s.h>
#endif


/*
    This should handle all the digital audio output to I2S.
    This code is largely based on the existing "ESP32-audioI2S" library
    found here: https://github.com/schreibfaul1/ESP32-audioI2S/tree/master

    
*/
//----------------------------------------------------------------------------------------------------------------------

class AudioBuffer 
{
// AudioBuffer will be allocated in PSRAM, If PSRAM not available or has not enough space AudioBuffer will be
// allocated in FlashRAM with reduced size
//
//  m_buffer            m_readPtr                 m_writePtr                 m_endPtr
//   |                       |<------dataLength------->|<------ writeSpace ----->|
//   ▼                       ▼                         ▼                         ▼
//   ---------------------------------------------------------------------------------------------------------------
//   |                     <--m_buffSize-->                                      |      <--m_resBuffSize -->     |
//   ---------------------------------------------------------------------------------------------------------------
//   |<-----freeSpace------->|                         |<------freeSpace-------->|
//
//
//
//   if the space between m_readPtr and buffend < m_resBuffSize copy data from the beginning to resBuff
//   so that the mp3/aac/flac frame is always completed
//
//  m_buffer                      m_writePtr                 m_readPtr        m_endPtr
//   |                                 |<-------writeSpace------>|<--dataLength-->|
//   ▼                                 ▼                         ▼                ▼
//   ---------------------------------------------------------------------------------------------------------------
//   |                        <--m_buffSize-->                                    |      <--m_resBuffSize -->     |
//   ---------------------------------------------------------------------------------------------------------------
//   |<---  ------dataLength--  ------>|<-------freeSpace------->|
//
//

public:
    AudioBuffer(size_t maxBlockSize = 0);       // constructor
    ~AudioBuffer();                             // frees the buffer
    size_t   init();                            // set default values
    bool     isInitialized() { return m_f_init; };
    void     setBufsize(int ram, int psram);
    int32_t  getBufsize();
    void     changeMaxBlockSize(uint16_t mbs);  // is default 1600 for mp3 and aac, set 16384 for FLAC
    uint16_t getMaxBlockSize();                 // returns maxBlockSize
    size_t   freeSpace();                       // number of free bytes to overwrite
    size_t   writeSpace();                      // space fom writepointer to bufferend
    size_t   bufferFilled();                    // returns the number of filled bytes
    size_t   getMaxAvailableBytes();            // max readable bytes in one block
    void     bytesWritten(size_t bw);           // update writepointer
    void     bytesWasRead(size_t br);           // update readpointer
    uint8_t* getWritePtr();                     // returns the current writepointer
    uint8_t* getReadPtr();                      // returns the current readpointer
    uint32_t getWritePos();                     // write position relative to the beginning
    uint32_t getReadPos();                      // read position relative to the beginning
    void     resetBuffer();                     // restore defaults
    bool     havePSRAM() { return m_f_psram; };

protected:
    size_t   m_buffSizePSRAM    = UINT16_MAX * 10;   // most webstreams limit the advance to 100...300Kbytes
    size_t   m_buffSizeRAM      = 1600 * 10;
    size_t   m_buffSize         = 0;
    size_t   m_freeSpace        = 0;
    size_t   m_writeSpace       = 0;
    size_t   m_dataLength       = 0;
    size_t   m_resBuffSizeRAM   = 2048;     // reserved buffspace, >= one wav  frame
    size_t   m_resBuffSizePSRAM = 4096 * 4; // reserved buffspace, >= one flac frame
    size_t   m_maxBlockSize     = 1600;
    uint8_t* m_buffer           = NULL;
    uint8_t* m_writePtr         = NULL;
    uint8_t* m_readPtr          = NULL;
    uint8_t* m_endPtr           = NULL;
    bool     m_f_start          = true;
    bool     m_f_init           = false;
    bool     m_f_psram          = false;    // PSRAM is available (and used...)
};
