#include "Display.h"



uint16_t Display::to565(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t red = (uint16_t)(((float)r / 255.0f) * 31.0f);
    uint16_t green = (uint16_t)(((float)g / 255.0f) * 63.0f);
    uint16_t blue = (uint16_t)(((float)b / 255.0f) * 31.0f);
    return (red << 11) | (green << 5) | blue; 
}

//-----------------------------------------------------

uint8_t Pixels::pixelIndex(PixelID id)
{
    switch (id)
    {
    case S1:
        return 8;
    case S2:
        return 9;
    case S3:
        return 10;
    case S4:
        return 11;
    case S5:
        return 12;
    case S6:
        return 13;
    case S7:
        return 14;
    case S8:
        return 15;
    case S9:
        return 7;
    case S10:
        return 6;
    case S11:
        return 5;
    case S12:
        return 4;
    case S13:
        return 3;
    case S14:
        return 2;
    case S15:
        return 1;
    case S16:
        return 0;
    case TR1:
        return 23;
    case TR2:
        return 22;
    case TR3:
        return 21;
    case TR4:
        return 20;
    case TR5:
        return 19;
    case TR6:
        return 18;
    case TR7:
        return 17;
    case TR8:
        return 16;
    case PG1:
        return 24;
    case PG2:
        return 25;
    case PG3:
        return 26;
    case PG4:
        return 27;
    default:
        return 28;
    }
}

uint8_t Pixels::pixelIndex(uint8_t idx)
{
    PixelID id = (PixelID)idx;
    return pixelIndex(id);
}


PixelID Pixels::pixelForTrack(HardwareTrack trk)
{
   switch(trk)
   {
      case Kick1:
         return TR1;
      case Kick2:
         return TR2;
      case Snare:
         return TR3;
      case ClosedHat:
         return TR4;
      case OpenHat:
         return TR5;
      case Clap:
         return TR6;
      case Clave:
         return TR7;
      case Digital:
         return TR8;
      default:
         return TR5;
   }
}


PixelID Pixels::pixelForTrack(uint8_t trk)
{
    HardwareTrack id = (HardwareTrack)trk;
    return pixelForTrack(id);
}
