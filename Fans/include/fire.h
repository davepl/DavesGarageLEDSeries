//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// File:        fire.h                    
//
// Description: A realistic flame simulation for LED strips
//
// History:     Oct-23-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "ledgfx.h"

class FireEffect
{
  protected:
    int     Size;               // How many pixels the flame is total
    int     Cooling;            // Rate at which the pixels cool off
    int     Sparks;             // How many sparks will be attempted each frame
    int     SparkHeight;        // If created, max height for a spark
    int     Sparking;           // Probability of a spark each attempt
    bool    bReversed;          // If reversed we draw from 0 outwards
    bool    bMirrored;          // If mirrored we split and duplicate the drawing

    byte  * heat;

    // When diffusing the fire upwards, these control how much to blend in from the cells below (ie: downward neighbors)
    // You can tune these coefficients to control how quickly and smoothly the fire spreads

    static const byte BlendSelf = 2;
    static const byte BlendNeighbor1 = 3;
    static const byte BlendNeighbor2 = 2;
    static const byte BlendNeighbor3 = 1;
    static const byte BlendTotal = (BlendSelf + BlendNeighbor1 + BlendNeighbor2 + BlendNeighbor3);

  public:

    FireEffect(int size, int cooling = 20, int sparking = 100, int sparks = 3, int sparkHeight = 4, bool breversed = true, bool bmirrored = true)
        : Size(size),
          Cooling(cooling),
          Sparks(sparks),
          SparkHeight(sparkHeight),
          Sparking(sparking),
          bReversed(breversed),
          bMirrored(bmirrored)
    {
        if (bMirrored)
            Size = Size / 2;

        heat = new byte[size] { 0 };
    }

    virtual ~FireEffect()
    {
        delete [] heat;
    }

    virtual void DrawFire(PixelOrder order = Sequential)
    {
        // First cool each cell by a litle bit
        for (int i = 0; i < Size; i++)
            heat[i] = max(0L, heat[i] - random(0, ((Cooling * 10) / Size) + 2));

        // Next drift heat up and diffuse it a little bit
        for (int i = 0; i < Size; i++)
            heat[i] = (heat[i] * BlendSelf +
                       heat[(i + 1) % Size] * BlendNeighbor1 +
                       heat[(i + 2) % Size] * BlendNeighbor2 +
                       heat[(i + 3) % Size] * BlendNeighbor3)
                      / BlendTotal;

        // Randomly ignite new sparks down in the flame kernel

        for (int i = 0 ; i < Sparks; i++)
        {
            if (random(255) < Sparking)
            {
                int y = Size - 1 - random(SparkHeight);
                heat[y] = heat[y] + random(160, 255);       // Can roll over which actually looks good!
            }
        }

        // Finally, convert heat to a color

        for (int i = 0; i < Size; i++)
        {
            CRGB color = HeatColor(heat[i]);
            int j = bReversed ? (Size - 1 - i) : i;
            DrawFanPixels(j, 1, color, order);
            if (bMirrored)
                DrawFanPixels(!bReversed ? (2 * Size - 1 - i) : Size + i, 1, color, order);
        }
    }
};