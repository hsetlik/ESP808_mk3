#pragma once
#include "Pins.h"
#include "RingBuffer.h"
#include "Button.h"
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



struct Encoder
{
    bool hasCallback = false;
    EncCallback callback;
    bool moveStarted = false;
    unsigned long lastMovedAt = 0;
};


class Encoders
{
private:
    Encoder encoders[4];
public:
    Encoders();
    void interruptSent(uint8_t pin);
    void setCallback(uint8_t encoder, EncCallback cb);
};

//---------------------------------------------------------------------

#define BUTTON_CHECK_HZ 800

#define NUM_BUTTONS 26

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

// the collective buttons
class Buttons
{
private:
    Button buttons[NUM_BUTTONS];
public:
    Buttons() {}
    Button& get(ButtonID id)
    {
        return buttons[id];
    }

    Button& operator[](uint8_t idx)
    {
        return buttons[idx];
    }

    void tick(uint16_t exp1bits, uint16_t exp2bits, uint16_t exp3bits);
};





