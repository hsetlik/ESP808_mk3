#pragma once
#include "Pins.h"
#include <Adafruit_ILI9341.h>
#include <FastLED.h>

using ILI9341 = Adafruit_ILI9341;

#define DISPLAY_W 360
#define DISPLAY_H 240

#define NUM_PIXELS 28

namespace Display
{
    uint16_t to565(uint8_t r, uint8_t g, uint8_t b);
}

//-------------------------------------------------


enum PixelID : uint8_t
{
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    S8,
    S9,
    S10,
    S11,
    S12,
    S13,
    S14,
    S15,
    S16,
    TR1,
    TR2,
    TR3,
    TR4,
    TR5,
    TR6,
    TR7,
    TR8,
    PG1,
    PG2,
    PG3,
    PG4
};

//-----------------------------------------------------------------------------------

namespace Pixels
{

    /**
     * @brief Convert the more intuitive pixel index 
     * used by the 'PixelID' enum to the actual index of the pixel
     * in hardware
     * 
     * @param pixel - the pixel's ID
     * @return uint8_t - the pixel's index in the circuit 
     */
    uint8_t pixelIndex(uint8_t pixel);
    uint8_t pixelIndex(PixelID id);

    /**
     * @brief Get the PixelID for a HardwareTrack
     * 
     * @param trk 
     * @return PixelID- the ID of the track's pixel
     */
    PixelID pixelForTrack(HardwareTrack trk);
    PixelID pixelForTrack(uint8_t trk);

}
