#include "DigitalPot.h"

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
        if(spi->transfer16(cmd) != 65535)
        {
            Serial.println("Failed to transmit potentiometer SPI bits!");
        }
    }
    digitalWrite(cs, HIGH);
}