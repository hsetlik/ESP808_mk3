#pragma once
#include <Arduino.h>
#include <SPI.h>

// functions to write to out MCP4331 pots
namespace MCP4331
{
    void setLevel(SPIClass* spi, uint8_t cs, uint8_t idx, uint8_t level);
    // set all 4 pots at once with the 'data' argument being packed from 4 8 bit values
    void setLevels(SPIClass* spi, uint8_t cs, uint32_t data);
}