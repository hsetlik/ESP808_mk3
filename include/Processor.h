#pragma once
#include "Controls.h"
#include "Display.h"
// all the actual sequencing and playback logic goes here


class Processor
{
public:
    Processor();
    // these get wired up to the hardware code from Controls.h in main.cpp
    void handleClick(uint8_t button);
    void handlePress(uint8_t button);
    void handleEncoder(uint8_t enc, bool up);
};

