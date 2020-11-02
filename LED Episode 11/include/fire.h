//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// File:        fire.h               
//
// Description:
//
//              LED Flame Effect
//
// History:     Sep-28-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "ledgfx.h"

class FireEffectSmooth
{
  protected:
    float  * Temperatures;
    float    LastDraw;                  // Last time we drew the flame

    const float IGNITION_KNOB = 50.0f;  // Preference-based constant factor for ignition rate
    const float SPREADRATE_KNOB = 12.0f; // Preference-based constant for flame spread rate
    const float SPARKRATE_KNOB  = 8.0f; // Preference-based constant for spark ignition rate

  public:
    bool    Mirrored;                   // Should the flame be mirrored, drawn from both sides?
    bool    Reversed;                   // Only applicable when not reversed, should it be reversed?
    float   Cooling;                    // Pixel cooldown rate 
    int     Size;
    int     SparkHeight;                // Ignition zone for where new pixels start up
    float   SparkProbability;           // Probability of a spark in each ignition zone pixel
    float   SpreadRate;                 // Rate at which fire spreads pixel to pixel
    
    FireEffectSmooth(int size, bool mirrored = true, bool reversed = false, int sparkHeight = 0, float sparkProbability = 1.0, float cooling = 1.0, float spreadRate = 1.0)
    {
        Mirrored         = mirrored;
        if (mirrored)
            size /= 2;

        Reversed         = reversed;
        Cooling          = cooling;
        Size             = size;                                            // 
        SparkHeight      = sparkHeight;                                     // 
        SparkProbability = sparkProbability * SPARKRATE_KNOB / SparkHeight; // Chance that each LED cell will ignite when tested
        Temperatures     = new float[Size];                                 // Array of temperatures, one per LED
        SpreadRate       = spreadRate * SPREADRATE_KNOB;                    // How fast the flame spreads per second
        LastDraw         = UnixTime();                                      // Start of time
    }

    virtual ~FireEffectSmooth()               // Because we have a virtual function, destructor is virtual as well
    {
        delete [] Temperatures;
    }

    void DrawFire()
    {
        FastLED.clear();

        float elapsedSeconds = UnixTime() - LastDraw;
        float cooldown = 1.0f * RandomFloat() * Cooling * elapsedSeconds;
        LastDraw = UnixTime();

        for (int i = 0; i < Size; i++)
        {
            Temperatures[i] = max(0.0f, Temperatures[i] - cooldown); // Cool cell by cooldown amount, but don't go below zero
            
            int neighborIndex = (i == 0) ? Size - 1 : i - 1;        // Index of cell to our left, wrapping around to front
            float spreadAmount = min(0.25f, Temperatures[neighborIndex]) * SpreadRate * elapsedSeconds;
            spreadAmount = min(Temperatures[neighborIndex], spreadAmount);
            Temperatures[i]             += spreadAmount;            // Exchange 'spreadAmount' of heat between cells
            Temperatures[neighborIndex] -= spreadAmount;

            // Check to see if this cell ignites a new spark
            if (i <= SparkHeight && RandomFloat() < SparkProbability * elapsedSeconds)
            {
                //Temperatures[i] = Temperatures[i] + RandomFloat() * 30 * elapsedSeconds;
                Temperatures[i] = 2.0; // min(1.0f, (Temperatures[i] + RandomFloat() * 30 * elapsedSeconds));
                //printf("Spark at %d: %f", i, Temperatures[i]);
            }
        }
        for (int i = 0; i < Size; i++)
        {
            FastLED.leds()[i] = HeatColor(240 * min(1.0f, Temperatures[i]));
        }
    }
};

class ClassicFireEffect
{
protected:
    int     Size;
    int     Cooling;
    int     Sparks;
    int     SparkHeight;
    int     Sparking;
    bool    bReversed;
    bool    bMirrored;

    byte  * heat;

    // When diffusing the fire upwards, these control how much to blend in from the cells below (ie: downward neighbors)
    // You can tune these coefficients to control how quickly and smoothly the fire spreads.  

    static const byte BlendSelf = 2;
    static const byte BlendNeighbor1 = 3;
    static const byte BlendNeighbor2 = 2;
    static const byte BlendNeighbor3 = 1;
    static const byte BlendTotal = (BlendSelf + BlendNeighbor1 + BlendNeighbor2 + BlendNeighbor3);

public:
    
    // Lower sparking -> more flicker.  Higher sparking -> more consistent flame

    ClassicFireEffect(int size, int cooling = 80, int sparking = 50, int sparks = 3, int sparkHeight = 4, bool breversed = true, bool bmirrored = true) 
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

    virtual ~ClassicFireEffect()
    {
        delete [] heat;
    }

    virtual void DrawFire()
    {
        // First cool each cell by a little bit
        for (int i = 0; i < Size; i++)
            heat[i] = max(0L, heat[i] - random(0, ((Cooling * 10) / Size) + 2));

        // Next drift heat up and diffuse it a little but
        for (int i = 0; i < Size; i++)
            heat[i] = (heat[i] * BlendSelf + 
                       heat[(i + 1) % Size] * BlendNeighbor1 + 
                       heat[(i + 2) % Size] * BlendNeighbor2 + 
                       heat[(i + 3) % Size] * BlendNeighbor3) 
                      / BlendTotal;

        // Randomly ignite new sparks down in the flame kernel
        for (int i = 0; i < Sparks; i++)
        {
            if (random(255) < Sparking)
            {
                int y = Size - 1 - random(SparkHeight);
                heat[y] = heat[y] + random(160, 255); // This randomly rolls over sometimes of course, and that's essential to the effect
            }
        }

        // Finally convert heat to a color
        for (int i = 0; i < Size; i++)
        {
            CRGB color = HeatColor(heat[i]);
            int j = bReversed ? (Size - 1 - i) : i;
            DrawPixels(j, 1, color);
            if (bMirrored)
            {
                int j2 = !bReversed ? (2 * Size - 1 - i) : Size + i;
                DrawPixels(j2, 1, color);
            }
        }
    }
};
