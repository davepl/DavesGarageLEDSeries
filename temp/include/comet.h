//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// File:                  
//
// Description:
//
//   
//
// History:     Sep-28-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

void DrawComet()
{
    const byte fadeAmt = 128;
    const int cometSize = 5;
    const int deltaHue  = 4;

    static byte hue = HUE_RED;
    static int iDirection = 1;
    static int iPos = 0;

    hue += deltaHue;

    iPos += iDirection;
    if (iPos == (FastLED.count() - cometSize) || iPos == 0)
        iDirection *= -1;
    
    for (int i = 0; i < cometSize; i++)
        FastLED.leds()[iPos + i].setHue(hue);
    
    // Randomly fade the LEDs
    for (int j = 0; j < FastLED.count(); j++)
        if (random(10) > 5)
            FastLED.leds()[j] = FastLED.leds()[j].fadeToBlackBy(fadeAmt);  

    delay(30);
}