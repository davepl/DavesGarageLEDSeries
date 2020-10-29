//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        ledgfx.h
//
// Description:
//
//   LED Drawing Routines for Dave's Garage Tutorial series
//
// History:     OCt-18-2020     davepl      Created from main.cpp code
//---------------------------------------------------------------------------

#pragma once

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include <sys/time.h>                   // For time-of-day

// Utility Macros

#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000/x)

//inline double RandomDouble()
//{
//    return random(UINT32_MAX) / (double) UINT32_MAX;
//}

inline float RandomFloat()
{
    float r = random(1000000L) / 1000000.0f;
    return r;
}

inline double UnixTime()
{
    timeval tv = { 0 };
    gettimeofday(&tv, nullptr);
    return (double)(tv.tv_usec / 1000000.0 + (double) tv.tv_sec);
}

// FractionalColor
//
// Returns a fraction of a color; abstracts the fadeToBlack out to this function in case we
// want to improve the color math or do color correction all in one location at a later date.

CRGB ColorFraction(CRGB colorIn, float fraction)
{
  fraction = min(1.0f, fraction);
  return CRGB(colorIn).fadeToBlackBy(255 * (1.0f - fraction));
}

void DrawPixels(float fPos, float count, CRGB color)
{
  // Calculate how much the first pixel will hold
  float availFirstPixel = 1.0f - (fPos - (long)(fPos));
  float amtFirstPixel = min(availFirstPixel, count);
  float remaining = min(count, FastLED.size()-fPos);
  int iPos = fPos;

  // Blend (add) in the color of the first partial pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[iPos++] += ColorFraction(color, amtFirstPixel);
    remaining -= amtFirstPixel;
  }

  // Now draw any full pixels in the middle

  while (remaining > 1.0f)
  {
    FastLED.leds()[iPos++] += color;
    remaining--;
  }

  // Draw tail pixel, up to a single full pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[iPos] += ColorFraction(color, remaining);
  }
}
