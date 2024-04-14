#include "Outputs.h"

void MCP4331::setLevel(SPIClass *spi, uint8_t cs, uint8_t idx, uint8_t level)
{
    digitalWrite(cs, LOW);
    uint16_t cmd = 0;
    // these command bytes are explained on pg 52
    // of the datasheet here: https://www.mouser.com/datasheet/2/268/22242A-54588.pdf
    switch (idx)
    {
    case 0:
        break;
    case 1:
        cmd = 1 << 12;
        break;
    case 2:
        cmd = (1 << 13) | (1 << 14);
        break;
    case 3:
        cmd = (1 << 13) | (1 << 14) | (1 << 12);
        break;
    default:
        Serial.println("Error! Invalid potentiometer index: " + String(idx));
        break;
    }
    cmd = cmd | (uint16_t)level;
    spi->transfer16(cmd);
    digitalWrite(cs, HIGH);
}

void MCP4331::setLevels(SPIClass *spi, uint8_t cs, uint32_t data)
{
    digitalWrite(cs, LOW);
    for (uint8_t p = 0; p < 4; p++)
    {
        uint8_t level = (uint8_t)(data >> (p * 8));
        uint16_t cmd = 0;
        switch (p)
        {
        case 0:
            break;
        case 1:
            cmd = 1 << 12;
            break;
        case 2:
            cmd = (1 << 13) | (1 << 14);
            break;
        case 3:
            cmd = (1 << 13) | (1 << 14) | (1 << 12);
            break;
        default:
            Serial.println("Error! Invalid potentiometer index: " + String(p));
            break;
        }
        cmd = cmd | (uint16_t)level;
        if (spi->transfer16(cmd) != 65535)
        {
            Serial.println("Failed to transmit potentiometer SPI bits!");
        }
    }
    digitalWrite(cs, HIGH);
}

uint32_t Pots::packPot1Levels(uint8_t kick1, uint8_t kick2, uint8_t snare, uint8_t oh)
{

    uint32_t bits = (uint32_t)oh | (snare << 8) | (kick2 << 16) | (kick1 << 24);
    return bits;
}

uint32_t Pots::packPot2Levels(uint8_t clap, uint8_t ch, uint8_t clave)
{
    // rightmost 8 bits go unused here
    uint32_t bits = (uint32_t)(clave << 24) | (ch << 16) | (clap << 8);
    return bits;
}

uint64_t Pots::packAllPotLevels(uint8_t k1, uint8_t k2, uint8_t snare, uint8_t oh, uint8_t ch, uint8_t clap, uint8_t clave)
{
    uint64_t pot1 = (uint64_t)packPot1Levels(k1, k2, snare, oh);
    uint64_t pot2 = (uint64_t)packPot2Levels(clap, ch, clave);
    return (pot1 << 32) | pot2;
}

//------------------------------------------------------------------
uint8_t Gate::bitForTrack(HardwareTrack trk)
{
    switch(trk)
    {
        case Kick1:
            return (uint8_t)GateBit::K1;
        case Kick2:
            return (uint8_t)GateBit::K2;
        case Snare:
            return (uint8_t)GateBit::Snare;
        case OpenHat:
            return (uint8_t)GateBit::OH;
        case ClosedHat:
            return (uint8_t)GateBit::CH;
        case Clap:
            return (uint8_t)GateBit::Clap;
        case Clave:
            return (uint8_t)GateBit::Clave;
        case Accent:
            return (uint8_t)GateBit::Accent;
        default:
            return 9;
    }
}