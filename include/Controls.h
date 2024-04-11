#pragma once
#include "Pins.h"
#include "RingBuffer.h"
#include <Adafruit_MCP23X17.h>
#include <functional>

using MCP23S17 = Adafruit_MCP23X17;

typedef std::function<void(bool)> EncCallback;

namespace Expanders
{
    // this just helps our setup() in main.cpp be more readable
    void setupExpander1(MCP23S17* exp);
    void setupExpander2(MCP23S17* exp);
    void setupExpander3(MCP23S17* exp);
}

//---------------------------------------------------------------------
namespace Enc
{
    uint8_t siblingPin(uint8_t pin);
    uint8_t getEncoder(uint8_t pin);
    bool isLeftPin(uint8_t pin);
}

// stores the business logic for our four encoders

class Encoders
{
private:
    bool callbackSet[4];
    EncCallback callbacks[4];
// keep track of encoder state
    bool moveStarted[4];

public:
    Encoders();
    void interruptSent(uint8_t pin);
    void setCallback(uint8_t encoder, EncCallback cb);


};
