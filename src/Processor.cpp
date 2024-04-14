#include "Processor.h"

Processor::Processor() :
isPlaying(false),
currentTempo(TEMPO_DEFAULT),
altDown(false),
lastAltClickAt(0),
seq(NUM_HARDWARE_TRACKS),
currentStep(0.0f)
{
}

//--- Control Handling ---------------------------------

void Processor::handleClick(uint8_t button)
{
    ButtonID id = (ButtonID)button;
    switch(id)
    {
     case ButtonID::Seq1:
        break;   
     case ButtonID::Seq2:
        break;
     case ButtonID::Seq3:
        break;
     case ButtonID::Seq4:
        break;
     case ButtonID::Seq5:
        break;   
     case ButtonID::Seq6:
        break;
     case ButtonID::Seq7:
        break;
     case ButtonID::Seq8:
        break;
     case ButtonID::Seq9:
        break;   
     case ButtonID::Seq10:
        break;
     case ButtonID::Seq11:
        break;
     case ButtonID::Seq12:
        break;
     case ButtonID::Seq13:
        break;   
     case ButtonID::Seq14:
        break;
     case ButtonID::Seq15:
        break;
     case ButtonID::Seq16:
        break;
    //------------------------------
    case ButtonID::Enc1:
        break;
    case ButtonID::Enc2:
        break;    
    case ButtonID::Enc3:
        break;   
    case ButtonID::Enc4:
        break;        
    //------------------------------
    case ButtonID::PgLeft:
        break;
    case ButtonID::PgRight:
        break;   
    case ButtonID::Load:
        break;   
    case ButtonID::Save:
        break;   
    case ButtonID::Alt:
        lastAltClickAt = millis();
        break;   
    case ButtonID::Playtoggle:
        isPlaying = !isPlaying;
        break;   
    default:
        break;
    }

}

void Processor::handlePressStart(uint8_t button)
{
    ButtonID id = (ButtonID)button;
    switch(id)
    {
     case ButtonID::Seq1:
        break;   
     case ButtonID::Seq2:
        break;
     case ButtonID::Seq3:
        break;
     case ButtonID::Seq4:
        break;
     case ButtonID::Seq5:
        break;   
     case ButtonID::Seq6:
        break;
     case ButtonID::Seq7:
        break;
     case ButtonID::Seq8:
        break;
     case ButtonID::Seq9:
        break;   
     case ButtonID::Seq10:
        break;
     case ButtonID::Seq11:
        break;
     case ButtonID::Seq12:
        break;
     case ButtonID::Seq13:
        break;   
     case ButtonID::Seq14:
        break;
     case ButtonID::Seq15:
        break;
     case ButtonID::Seq16:
        break;
    //------------------------------
    case ButtonID::Enc1:
        break;
    case ButtonID::Enc2:
        break;    
    case ButtonID::Enc3:
        break;   
    case ButtonID::Enc4:
        break;        
    //------------------------------
    case ButtonID::PgLeft:
        break;
    case ButtonID::PgRight:
        break;   
    case ButtonID::Load:
        break;   
    case ButtonID::Save:
        break;   
    case ButtonID::Alt:
        altDown = true;
        break;   
    case ButtonID::Playtoggle:
        break;   
    default:
        break;
    }
}


void Processor::handlePressEnd(uint8_t button)
{
    ButtonID id = (ButtonID)button;
    switch(id)
    {
     case ButtonID::Seq1:
        break;   
     case ButtonID::Seq2:
        break;
     case ButtonID::Seq3:
        break;
     case ButtonID::Seq4:
        break;
     case ButtonID::Seq5:
        break;   
     case ButtonID::Seq6:
        break;
     case ButtonID::Seq7:
        break;
     case ButtonID::Seq8:
        break;
     case ButtonID::Seq9:
        break;   
     case ButtonID::Seq10:
        break;
     case ButtonID::Seq11:
        break;
     case ButtonID::Seq12:
        break;
     case ButtonID::Seq13:
        break;   
     case ButtonID::Seq14:
        break;
     case ButtonID::Seq15:
        break;
     case ButtonID::Seq16:
        break;
    //------------------------------
    case ButtonID::Enc1:
        break;
    case ButtonID::Enc2:
        break;    
    case ButtonID::Enc3:
        break;   
    case ButtonID::Enc4:
        break;        
    //------------------------------
    case ButtonID::PgLeft:
        break;
    case ButtonID::PgRight:
        break;   
    case ButtonID::Load:
        break;   
    case ButtonID::Save:
        break;   
    case ButtonID::Alt:
        altDown = false;
        break;   
    case ButtonID::Playtoggle:
        break;   
    default:
        break;
    }
}


void Processor::handleEncoder(uint8_t enc, bool up)
{
}

void Processor::nudgeTempo(bool dir)
{
    if(dir)
        currentTempo = std::min<float>(currentTempo + TEMPO_INCREMENT, TEMPO_MAX);
    else
        currentTempo = std::max<float>(currentTempo - TEMPO_INCREMENT, TEMPO_MIN);
}

//------------------------------------------------------