#include "Processor.h"

Processor::Processor() : isPlaying(false),
                         currentTempo(TEMPO_DEFAULT),
                         selectedTrack(0),
                         altDown(false),
                         lastAltClickAt(0),
                         buttonHoldState(0),
                         lastTickMs(0),
                         digitalMode(false),
                         pageMode(false),
                         seq(NUM_HARDWARE_TRACKS),
                         fCurrentStep(0.0f),
                         lastStep(0)
{
   // initialize our track color references here
   const uint8_t hueOffset = 16;
   const uint8_t dHue = 256 / NUM_HARDWARE_TRACKS;

   // sat and val levels
   const uint8_t satDim = 100;
   const uint8_t satMid = 175;
   const uint8_t satBright = 250;

   const uint8_t valDim = 80;
   const uint8_t valMid = 180;
   const uint8_t valBright = 250;

   for (uint8_t trk = 0; trk < NUM_HARDWARE_TRACKS; trk++)
   {
      uint8_t hue = hueOffset + (dHue * trk);

      CHSV hsvDim(hue, satDim, valDim);
      CHSV hsvMid(hue, satMid, valMid);
      CHSV hsvBright(hue, satBright, valBright);

      // convert to RGB in our stored colors
      hsv2rgb_rainbow(hsvDim, trackColorsDim[trk]);
      hsv2rgb_rainbow(hsvMid, trackColorsMid[trk]);
      hsv2rgb_rainbow(hsvBright, trackColorsBright[trk]);
   }

   // pick an off color
   CHSV hsvOff(10, 10, 10);
   hsv2rgb_rainbow(hsvOff, offColor);
   // cursor color
   CHSV hsvCursor(35, 110, 110);
   hsv2rgb_rainbow(hsvCursor, cursorColor);
}

//--- Control Handling ---------------------------------

void Processor::handleClick(uint8_t button)
{
   ButtonID id = (ButtonID)button;
   switch (id)
   {
   case ButtonID::Seq1:
      handleSequenceKeyClick(0);
      break;
   case ButtonID::Seq2:
      handleSequenceKeyClick(1);
      break;
   case ButtonID::Seq3:
      handleSequenceKeyClick(2);
      break;
   case ButtonID::Seq4:
      handleSequenceKeyClick(3);
      break;
   case ButtonID::Seq5:
      handleSequenceKeyClick(4);
      break;
   case ButtonID::Seq6:
      handleSequenceKeyClick(5);
      break;
   case ButtonID::Seq7:
      handleSequenceKeyClick(6);
      break;
   case ButtonID::Seq8:
      handleSequenceKeyClick(7);
      break;
   case ButtonID::Seq9:
      handleSequenceKeyClick(8);
      break;
   case ButtonID::Seq10:
      handleSequenceKeyClick(9);
      break;
   case ButtonID::Seq11:
      handleSequenceKeyClick(10);
      break;
   case ButtonID::Seq12:
      handleSequenceKeyClick(11);
      break;
   case ButtonID::Seq13:
      handleSequenceKeyClick(12);
      break;
   case ButtonID::Seq14:
      handleSequenceKeyClick(13);
      break;
   case ButtonID::Seq15:
      handleSequenceKeyClick(14);
      break;
   case ButtonID::Seq16:
      handleSequenceKeyClick(15);
      break;
   //------------------------------
   case ButtonID::Enc1:
      digitalMode = !digitalMode;
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
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq2:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq3:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq4:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq5:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq6:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq7:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq8:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq9:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq10:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq11:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq12:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq13:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq14:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq15:
      buttonHoldState = buttonHoldState | (1 << button);
      break;
   case ButtonID::Seq16:
      buttonHoldState = buttonHoldState | (1 << button);
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
      pageMode = !pageMode;
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
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq2:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq3:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq4:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq5:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq6:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq7:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq8:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq9:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq10:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq11:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq12:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq13:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq14:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq15:
      buttonHoldState = buttonHoldState & ~(1 << button);
      break;
   case ButtonID::Seq16:
      buttonHoldState = buttonHoldState & ~(1 << button);
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
   case 0: // NORMAL MODE: adjust the selected track
      if (!digitalMode)
         nudgeSelectedTrack(up);
      break;
   case 1:
      break;
   case 2:
      // if one of the sequence keys is being held down, use enc3 to adjust the step's level
      if (sequenceKeyHeld())
      {
         nudgeStepLevel(getHeldSequenceKey(), up);
      }

      break;
   case 3: // NORMAL MODE: adjust the tempo
      if (!digitalMode)
         nudgeTempo(up);
      // TODO: else adjust the digital track level
      break;
   default:
      break;
   }
}

void Processor::nudgeSelectedTrack(bool up)
{
   if (up)
   {
      selectedTrack = (selectedTrack + 1) % NUM_HARDWARE_TRACKS;
      if (selectedTrack == HardwareTrack::Accent)
         selectedTrack++;
   }
   else
   {
      selectedTrack = (selectedTrack == 0) ? NUM_HARDWARE_TRACKS : selectedTrack - 1;
      if (selectedTrack == HardwareTrack::Accent)
         --selectedTrack;
   }
}

void Processor::nudgeStepLevel(uint8_t button, bool up)
{
   Step *s = stepForButton(button);
   if (up)
      s->level = std::min<uint8_t>(s->level + 1, LEVEL_MAX);
   else
      s->level = std::max<uint8_t>(s->level - 1, LEVEL_MIN);
}

void Processor::nudgeTempo(bool dir)
{
   const float incr = altKey() ? TEMPO_INCREMENT_SMALL : TEMPO_INCREMENT_BIG;
   if (dir)
      currentTempo = std::min<float>(TEMPO_MAX, currentTempo + incr);
   else
      currentTempo = std::max<float>(TEMPO_MIN, currentTempo - incr);
}

Step *Processor::stepForButton(uint8_t b)
{
   uint8_t offset = lastStep / 16;
   return &seq.tracks[selectedTrack].steps[(offset * 16) + b];
}

uint8_t Processor::stepIdxForButton(uint8_t button)
{
   uint8_t offset = lastStep / 16;
   return (offset * 16) + button;
}

uint8_t Processor::getHeldSequenceKey()
{
   for (uint8_t b = 0; b < 16; b++)
   {
      if (buttonHoldState & (1 << b))
         return b;
   }
   return 0;
}

void Processor::handleSequenceKeyClick(uint8_t button)
{
   uint8_t idx = stepIdxForButton(button);
   Step *step = altKey() ? &seq.tracks[HardwareTrack::Accent].steps[idx] : &seq.tracks[selectedTrack].steps[idx];
   step->on = !step->on;
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
      const float maxStep = pageMode ? 16.0f : 64.0f;
      if (fCurrentStep > maxStep)
         fCurrentStep -= maxStep;
      uint8_t iStep = (uint8_t)fCurrentStep;
      if (iStep != lastStep)
      {
         // check if we need to trigger anything
         for (uint8_t trk = 0; trk < 8; trk++)
         {
            if (seq.tracks[trk].steps[iStep].on)
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
   for (uint8_t i = 0; i < 8; i++)
   {
      if (lastTickMs - lastTriggerMs[i] <= TRIGGER_DURATION_MS)
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
   return (uint64_t)(p2) | ((uint64_t)p1 << 32);
}
//------------------------------------------------------

void Processor::updateDisplay(ILI9341 *display)
{
}

void Processor::updatePixels(CRGB *pixels)
{
   for(uint8_t p = 0; p < NUM_PIXELS; p++)
   {
      auto color = getPixelColor(p);
      uint8_t idx = Pixels::pixelIndex(p);
      pixels[idx] = color;
   }
}

CRGB Processor::getStepPixelColor(uint8_t button, uint8_t track)
{
   uint8_t step = stepIdxForButton(button);
   bool currentStep = step == lastStep;
   if (!seq.tracks[track].steps[step].on)
   {
      if(currentStep)
         return cursorColor;
      return offColor;
   }
   CRGB &maxColor = currentStep ? trackColorsBright[track] : trackColorsMid[track];
   CRGB &minColor = currentStep ? trackColorsMid[track] : trackColorsDim[track];
   uint8_t frac = seq.tracks[track].steps[step].level * 2;
   return blend(minColor, maxColor, (fract8)frac);
}


CRGB Processor::getTrackPixelColor(uint8_t track)
{
   if(track == selectedTrack)
      return trackColorsBright[track];
   
   if(seq.tracks[track].steps[lastStep].on && track != HardwareTrack::Accent)
      return trackColorsMid[track];
   return offColor;
}

CRGB Processor::getPagePixelColor(uint8_t pg)
{
   if(pageMode)
      return cursorColor;
   if(lastStep / 16 == pg)
      return cursorColor;
   return offColor;
}

CRGB Processor::getPixelColor(uint8_t pixel)
{
   PixelID id = (PixelID)pixel;
   switch(id)
   {
      case PixelID::S1:
         return getStepPixelColor(0, selectedTrack);
      case PixelID::S2:
         return getStepPixelColor(1, selectedTrack);
      case PixelID::S3:
         return getStepPixelColor(2, selectedTrack);
      case PixelID::S4:
         return getStepPixelColor(3, selectedTrack);
      case PixelID::S5:
         return getStepPixelColor(4, selectedTrack);
      case PixelID::S6:
         return getStepPixelColor(5, selectedTrack);
      case PixelID::S7:
         return getStepPixelColor(6, selectedTrack);
      case PixelID::S8:
         return getStepPixelColor(7, selectedTrack);
      case PixelID::S9:
         return getStepPixelColor(8, selectedTrack);
      case PixelID::S10:
         return getStepPixelColor(9, selectedTrack);
      case PixelID::S11:
         return getStepPixelColor(10, selectedTrack);
      case PixelID::S12:
         return getStepPixelColor(11, selectedTrack);
      case PixelID::S13:
         return getStepPixelColor(12, selectedTrack);
      case PixelID::S14:
         return getStepPixelColor(13, selectedTrack);
      case PixelID::S15:
         return getStepPixelColor(14, selectedTrack);
      case PixelID::S16:
         return getStepPixelColor(15, selectedTrack);
      case PixelID::TR1:
         return getTrackPixelColor(HardwareTrack::Kick1);
      case PixelID::TR2:
         return getTrackPixelColor(HardwareTrack::Kick2);
      case PixelID::TR3:
         return getTrackPixelColor(HardwareTrack::Snare);
      case PixelID::TR4:
         return getTrackPixelColor(HardwareTrack::ClosedHat);
      case PixelID::TR5:
         return getTrackPixelColor(HardwareTrack::OpenHat);
      case PixelID::TR6:
         return getTrackPixelColor(HardwareTrack::Clap);
      case PixelID::TR7:
         return getTrackPixelColor(HardwareTrack::Clave);
      case PixelID::TR8:
         return getTrackPixelColor(HardwareTrack::Digital);
      case PixelID::PG1:
         return getPagePixelColor(0);
      case PixelID::PG2:
         return getPagePixelColor(1);
      case PixelID::PG3:
         return getPagePixelColor(2);
      case PixelID::PG4:
         return getPagePixelColor(3);
      default:
         return offColor;
   }
}