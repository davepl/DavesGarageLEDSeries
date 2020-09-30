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
// History:     Sep-15-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#define NUM_COLORS 5
static const CRGB TwinkleColors [NUM_COLORS] = 
{
    CRGB::Red,
    CRGB::Blue,
    CRGB::Purple,
    CRGB::Green,
    CRGB::Yellow
};

void DrawTwinkle()
{
    FastLED.clear(false);

    for (int i=0; i<NUM_LEDS/4; i++) 
    {
        g_LEDs[random(NUM_LEDS)] = TwinkleColors[random(0, NUM_COLORS)];
        FastLED.show(g_Brightness);
        delay(200);
    }
}

void DrawTwinkle2()
{
    static int passCount = 0;
    if (passCount++ == NUM_LEDS/4)
    {
        passCount = 0;
        FastLED.clear(false);
    }
    g_LEDs[random(NUM_LEDS)] = TwinkleColors[random(0, NUM_COLORS)];
    delay(200);       
}