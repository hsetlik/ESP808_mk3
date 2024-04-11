#include "Controls.h"

//----------------------------------------------------------------------------

void Expanders::setupExpander1(MCP23S17* exp)
{
    if(!exp->begin_SPI(EXP_CS, &SPI, EXP_1_ADDR))
    {
        Serial.println("Failed to initialize Expander 1!");
    }

    exp->pinMode(0, INPUT_PULLUP);
    exp->pinMode(1, INPUT_PULLUP);
    exp->pinMode(2, INPUT_PULLUP);
    exp->pinMode(3, INPUT_PULLUP);
    exp->pinMode(4, INPUT_PULLUP);
    exp->pinMode(5, INPUT_PULLUP);
    exp->pinMode(6, INPUT_PULLUP);
    exp->pinMode(7, INPUT_PULLUP);
    exp->pinMode(8, INPUT_PULLUP);
    exp->pinMode(9, INPUT_PULLUP);
    exp->pinMode(10, INPUT_PULLUP);
    exp->pinMode(11, INPUT_PULLUP);
    exp->pinMode(12, INPUT_PULLUP);
    exp->pinMode(13, INPUT_PULLUP);
    exp->pinMode(14, INPUT_PULLUP);
    exp->pinMode(15, INPUT_PULLUP);
}

void Expanders::setupExpander2(MCP23S17* exp)
{
    if(!exp->begin_SPI(EXP_CS, &SPI, EXP_2_ADDR))
    {
        Serial.println("Failed to initialize Expander 2!");
    }

    exp->pinMode(PG_LEFT, INPUT_PULLUP);
    exp->pinMode(PG_RIGHT, INPUT_PULLUP);

    exp->pinMode(ENC1_B, INPUT_PULLUP);
    exp->pinMode(ENC2_B, INPUT_PULLUP);
    exp->pinMode(ENC3_B, INPUT_PULLUP);
    exp->pinMode(ENC4_B, INPUT_PULLUP);

    // setup interrupts and encoder pins
    exp->setupInterrupts(true, false, LOW);

    exp->pinMode(ENC1_L, INPUT);
    exp->setupInterruptPin(ENC1_L, LOW);
    exp->pinMode(ENC1_R, INPUT);
    exp->setupInterruptPin(ENC1_R, LOW);   

    exp->pinMode(ENC2_L, INPUT);
    exp->setupInterruptPin(ENC2_L, LOW);
    exp->pinMode(ENC2_R, INPUT);
    exp->setupInterruptPin(ENC2_R, LOW);   

    exp->pinMode(ENC3_L, INPUT);
    exp->setupInterruptPin(ENC3_L, LOW);
    exp->pinMode(ENC3_R, INPUT);
    exp->setupInterruptPin(ENC3_R, LOW);   

    exp->pinMode(ENC4_L, INPUT);
    exp->setupInterruptPin(ENC4_L, LOW);
    exp->pinMode(ENC4_R, INPUT);
    exp->setupInterruptPin(ENC4_R, LOW);   

}

void Expanders::setupExpander3(MCP23S17* exp)
{
    if(!exp->begin_SPI(EXP_CS, &SPI, EXP_3_ADDR))
    {
        Serial.println("Failed to initialize Expander 3!");
    }

    // set up triger pins
    exp->pinMode(K1_TRIG, OUTPUT);
    exp->pinMode(K2_TRIG, OUTPUT);
    exp->pinMode(SNARE_TRIG, OUTPUT);
    exp->pinMode(CLAP_TRIG, OUTPUT);
    exp->pinMode(CLAVE_TRIG, OUTPUT);
    exp->pinMode(OH_TRIG, OUTPUT);
    exp->pinMode(CH_TRIG, OUTPUT);
    exp->pinMode(HAT_ACCENT, OUTPUT);

    // and the last four buttons
    exp->pinMode(LOAD_PIN, INPUT_PULLUP);
    exp->pinMode(SAVE_PIN, INPUT_PULLUP);
    exp->pinMode(PLAYTOGLE, INPUT_PULLUP);
    exp->pinMode(ALT_PIN, INPUT_PULLUP);
}

//----------------------------------------------------------------------------

uint8_t Enc::siblingPin(uint8_t pin)
{
    switch(pin)
    {
        case ENC1_L:
            return ENC1_R;
        case ENC1_R:
            return ENC1_L;
        case ENC2_L:
            return ENC2_R;
        case ENC2_R:
            return ENC2_L;
        case ENC3_L:
            return ENC3_R;
        case ENC3_R:
            return ENC3_L;
        case ENC4_L:
            return ENC4_R;
        case ENC4_R:
            return ENC4_L;
        default:
            return 0;
    }
}


uint8_t Enc::getEncoder(uint8_t pin)
{
    switch(pin)
    {
        case ENC1_L:
            return 0;
        case ENC1_R:
            return 0;
        case ENC2_L:
            return 1;
        case ENC2_R:
            return 1;
        case ENC3_L:
            return 2;
        case ENC3_R:
            return 2;
        case ENC4_L:
            return 3;
        case ENC4_R:
            return 3;
        default:
            return 0;
    }
}


bool Enc::isLeftPin(uint8_t pin)
{
    switch(pin)
    {
        case ENC1_L:
            return true;
        case ENC1_R:
            return false;
        case ENC2_L:
            return true;
        case ENC2_R:
            return false;
        case ENC3_L:
            return true;
        case ENC3_R:
            return false;
        case ENC4_L:
            return true;
        case ENC4_R:
            return false;
        default:
            return false;
    }
}

//----------------------------------------------------------------------------

Encoders::Encoders()
{
    for(uint8_t e = 0; e < 4; e++)
    {
        callbackSet[e] = false;
        moveStarted[e] = false;
    }
}


void Encoders::setCallback(uint8_t encoder, EncCallback cb)
{
    callbacks[encoder] = cb;
    callbackSet[encoder] = true;
}

void Encoders::interruptSent(uint8_t pin)
{
    auto e = Enc::getEncoder(pin);
    if(moveStarted[e])
    {
        callbacks[e](Enc::isLeftPin(pin));
        moveStarted[e] = false;
    }
    else
    {
        moveStarted[e] = true;
    }
}