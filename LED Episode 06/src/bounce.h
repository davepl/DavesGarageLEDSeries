//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        marque.h
//
// Description:
//
//   Draws a theatre-style marquee
//
// History:     Sep-15-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#include <U8g2lib.h>
#define FASTLED_INTERNAL
#include <FastLED.h>


#define BallCount 3
CRGB colors[BallCount] = { CRGB::Red, CRGB::Green, CRGB::Blue };

float Gravity = -9.81;
int   StartHeight = 1;
float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );         // Because math!
float Height[BallCount];
float ImpactVelocity[BallCount];
float TimeSinceLastBounce[BallCount];
int   Position[BallCount];
long  ClockTimeSinceLastBounce[BallCount];
float Dampening[BallCount];

void InitBounce()
{
  for (int i = 0 ; i < BallCount ; i++) 
  {  
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }
}

void DrawBounce()
{
  FastLED.clear();
  for (int i = 0 ; i < BallCount ; i++) 
  {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/5000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/5000;
 
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
 
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }

    for (int i = 0 ; i < BallCount ; i++) 
      g_LEDs[Position[i]] = CRGB(colors[i][0], colors[i][1], colors[i][2]);  
}
