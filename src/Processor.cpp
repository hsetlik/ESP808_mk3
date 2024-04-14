#include "Processor.h"

Processor::Processor() : isPlaying(false),
                         currentTempo(TEMPO_DEFAULT),
                         selectedTrack(0),
                         altDown(false),
                         lastAltClickAt(0),
                         lastTickMs(0),
                         seq(NUM_HARDWARE_TRACKS),
                         fCurrentStep(0.0f),
                         lastStep(0)
{
}

//--- Control Handling ---------------------------------

void Processor::handleClick(uint8_t button)
{
   ButtonID id = (ButtonID)button;
   switch (id)
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
   switch (id)
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
   switch (id)
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
   switch (enc)
   {
   case 0:
      break;
   case 1:
      break;
   case 2:
      break;
   case 3:
      break;
   default:
      break;
   }
}

void Processor::nudgeTempo(bool dir)
{
   if (dir)
      currentTempo = std::min<float>(currentTempo + TEMPO_INCREMENT, TEMPO_MAX);
   else
      currentTempo = std::max<float>(currentTempo - TEMPO_INCREMENT, TEMPO_MIN);
}

void Processor::nudgeSelectedTrack(bool up)
{
   if (up)
      selectedTrack = (selectedTrack + 1) % NUM_HARDWARE_TRACKS;
   else
      selectedTrack = (selectedTrack == 0) ? NUM_HARDWARE_TRACKS : selectedTrack - 1;
}

//-Timing -----------------------------------------------------

unsigned long Processor::stepDurationMs()
{
   unsigned long quarterNoteMs = 60000 / (unsigned long)currentTempo;
   return quarterNoteMs / 2;
}

//------------------------------------------------------
void Processor::tick()
{
   unsigned long now = millis();
   if (isPlaying)
   {
      float dStep = (float)(now - lastTickMs) / (float)stepDurationMs();
      fCurrentStep += dStep;
      uint8_t iStep = (uint8_t)fCurrentStep;
      if (iStep != lastStep)
      {
         // check if we need to trigger anything
         for(uint8_t trk = 0; trk < 8; trk++)
         {
            if(seq.tracks[trk].steps[iStep].on)
            {
               uint8_t idx = Gate::bitForTrack((HardwareTrack)trk);
               lastTriggerMs[idx] = now;
            }
         }
         lastStep = iStep;
      }
   }
   lastTickMs = now;
}

//------------------------------------------------------
uint8_t Processor::getTriggerState()
{
   uint8_t data = 0;
   for(uint8_t i = 0; i < 8; i++)
   {
      if(lastTickMs - lastTriggerMs[i] < TRIGGER_DURATION_MS)
      {
         data = data | (1 << i);
      }
   }
   return data;
}

uint64_t Processor::getPotLevels()
{
   uint32_t p1 = Pots::packPot1Levels(seq.tracks[HardwareTrack::Kick1].steps[lastStep].level,
                                      seq.tracks[HardwareTrack::Kick2].steps[lastStep].level,
                                      seq.tracks[HardwareTrack::Snare].steps[lastStep].level,
                                      seq.tracks[HardwareTrack::OpenHat].steps[lastStep].level);

   uint32_t p2 = Pots::packPot2Levels(seq.tracks[HardwareTrack::Clap].steps[lastStep].level,
                                      seq.tracks[HardwareTrack::ClosedHat].steps[lastStep].level,
                                      seq.tracks[HardwareTrack::Clave].steps[lastStep].level);
   return (uint64_t)(p2) | ((uint64_t)(p1 << 32));
}