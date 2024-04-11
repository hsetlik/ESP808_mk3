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

//---------------------------------------------------------------------

#define BUTTON_CHECK_HZ 800


enum ButtonID
{
    Seq1,
    Seq2,
    Seq3,
    Seq4,
    Seq5,
    Seq6,
    Seq7,
    Seq8,
    Seq9,
    Seq10,
    Seq11,
    Seq12,
    Seq13,
    Seq14,
    Seq15,
    Seq16,
    Enc1,
    Enc2,
    Enc3,
    Enc4,
    PgLeft,
    PgRight,
    Load,
    Save,
    Alt,
    Playtoggle
};






