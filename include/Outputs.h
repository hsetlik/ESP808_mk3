#pragma once
#include <Arduino.h>
#include "Pins.h"
#include <Adafruit_MCP23X17.h>
// functions to write to out MCP4331 pots
namespace MCP4331
{
    /*
        The pots are laid out like so (see schematic):
        POT1:
        0 - K1
        1 - K2
        2 - Snare
        3 - OH

        POT2:
        0 - Clap
        1 - CH
        2 - Clave
        3 - not used
    */
   /*
    set the level of one channel in the quad digital pot.
    note: only 7 bits of the 'level' value are used by the
    7-bit MCP4331 pots.
   */ 
    void setLevel(SPIClass* spi, uint8_t cs, uint8_t idx, uint8_t level);
    // set all 4 pots at once with the 'data' argument being packed from 4 8 bit values
    void setLevels(SPIClass* spi, uint8_t cs, uint32_t data);

}

namespace Pots
{
    // bit packing stuff
    uint32_t packPot1Levels(uint8_t kick1, uint8_t kick2, uint8_t snare, uint8_t oh);
    uint32_t packPot2Levels(uint8_t clap, uint8_t ch, uint8_t clave);
    //NOTE: the 32 bits for pot1 are on the left here
    uint64_t packAllPotLevels(uint8_t k1, uint8_t k2, uint8_t snare, uint8_t oh, uint8_t ch, uint8_t clap, uint8_t clave);
}

//----------------------------------------------------------------
enum GateBit : uint8_t
{
    K1,
    K2,
    SN,
    CL,
    CV,
    OH,
    CH,
    AC
};

namespace Gate
{
    uint8_t bitForTrack(HardwareTrack trk);
}


//----------------------------------------------------------------