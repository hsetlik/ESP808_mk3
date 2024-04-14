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

}


void Encoders::setCallback(uint8_t encoder, EncCallback cb)
{
    encoders[encoder].callback = cb;
    encoders[encoder].hasCallback = true;
}

void Encoders::interruptSent(uint8_t pin)
{
    auto& enc = encoders[Enc::getEncoder(pin)];
    if(enc.moveStarted)
    {
        enc.callback(Enc::isLeftPin(pin));
        enc.lastMovedAt = millis();
        enc.moveStarted = false;
    }
    else
    {
        enc.moveStarted = true;        
    }
}

//----------------------------------------------------------------------------

void Buttons::tick(uint16_t exp1bits, uint16_t exp2bits, uint16_t exp3bits)
{
    // exp 1
    buttons[ButtonID::Seq1].tick(exp1bits & (1 << ST0));
    buttons[ButtonID::Seq2].tick(exp1bits & (1 << ST1));
    buttons[ButtonID::Seq3].tick(exp1bits & (1 << ST2));
    buttons[ButtonID::Seq4].tick(exp1bits & (1 << ST3));
    buttons[ButtonID::Seq5].tick(exp1bits & (1 << ST4));
    buttons[ButtonID::Seq6].tick(exp1bits & (1 << ST5));
    buttons[ButtonID::Seq7].tick(exp1bits & (1 << ST6));
    buttons[ButtonID::Seq8].tick(exp1bits & (1 << ST7));
    buttons[ButtonID::Seq9].tick(exp1bits & (1 << ST8));
    buttons[ButtonID::Seq10].tick(exp1bits & (1 << ST9));
    buttons[ButtonID::Seq11].tick(exp1bits & (1 << ST10));
    buttons[ButtonID::Seq12].tick(exp1bits & (1 << ST11));
    buttons[ButtonID::Seq13].tick(exp1bits & (1 << ST12));
    buttons[ButtonID::Seq14].tick(exp1bits & (1 << ST13));
    buttons[ButtonID::Seq15].tick(exp1bits & (1 << ST14));
    buttons[ButtonID::Seq16].tick(exp1bits & (1 << ST15));

    // exp 2
    buttons[ButtonID::Enc1].tick(exp2bits & (1 << ENC1_B));
    buttons[ButtonID::Enc2].tick(exp2bits & (1 << ENC2_B));
    buttons[ButtonID::Enc3].tick(exp2bits & (1 << ENC3_B));
    buttons[ButtonID::Enc4].tick(exp2bits & (1 << ENC4_B));

    buttons[ButtonID::PgLeft].tick(exp2bits & (1 << PG_LEFT));
    buttons[ButtonID::PgRight].tick(exp2bits & (1 << PG_RIGHT));

    // exp 3
    buttons[ButtonID::Load].tick(exp3bits & (1 << LOAD_PIN));
    buttons[ButtonID::Save].tick(exp3bits & (1 << SAVE_PIN));
    buttons[ButtonID::Playtoggle].tick(exp3bits & (1 << PLAYTOGLE));
    buttons[ButtonID::Alt].tick(exp3bits & (1 << ALT_PIN));
}