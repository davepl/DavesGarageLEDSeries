//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// File:        marquee.h   
//
// Description:
//
//   Draws an old Movie Theatre style marquee, but in color
//
// History:     Sep-25-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

void DrawMarquee()
{
    static byte j = HUE_BLUE;
    j += 4;
    byte k = j;

    // The following is roughly equivalent to fill_rainbow(g_LEDs, NUM_LEDS, j, 8)

    CRGB c;
    for (int i = 0; i < NUM_LEDS; i++)
        g_LEDs[i] = c.setHue(k+=8);

    static int scroll = 0;
    scroll++;

    for (int i=scroll % 5; i < NUM_LEDS - 1; i += 5)
        g_LEDs[i] = CRGB::Black;
    
    delay(50);
}
