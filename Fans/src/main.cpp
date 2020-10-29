//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        LED Episode 10
//
// Description:
//
//   Draws sample effects on a an addressable strip using FastLED
//
// History:     Sep-15-2020     davepl      Created
//              Oct-05-2020     davepl      Revised for Episode 07
//              Oct-11-2020     davepl      Revised for Episode 08
//              Oct-16-2020     davepl      Revised for Episode 09
//              Oct-23-2020     davepl      Revised for Episode 10
//---------------------------------------------------------------------------

#include <Arduino.h>            // Arduino Framework
#include <U8g2lib.h>            // For text on the little on-chip OLED
#define FASTLED_INTERNAL        // Suppress build banner
#include <FastLED.h>

#define OLED_CLOCK  15          // Pins for the OLED display
#define OLED_DATA   4
#define OLED_RESET  16

#define FAN_SIZE       16       // How many pixels per fan
#define NUM_FANS       3        // Number of fans in the strans
#define LED_FAN_OFFSET 3        // How far from 12 o'clock first pixel is
#define NUM_LEDS       (FAN_SIZE*NUM_FANS)
#define LED_PIN        5

CRGB g_LEDs[NUM_LEDS] = {0};    // Frame buffer for FastLED

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight = 0;
int g_Brightness = 255;         // 0-255 LED brightness scale
int g_PowerLimit = 3000;         // 900mW Power Limit

#include "ledgfx.h"
#include "comet.h"
#include "marquee.h"
#include "twinkle.h"
#include "fire.h"

int FanPixelsVertical[FAN_SIZE] =
{
  0, 1, 15, 2, 14, 3, 13, 4, 12, 5, 11, 6, 10, 7, 9, 8
};

int FanPixelsHorizontal[FAN_SIZE] =
{
  3, 4, 2, 5, 1, 6, 0, 7, 15, 8, 14, 9, 13, 10, 12, 11
};

enum PixelOrder
{
  Sequential = 0,
  Reverse    = 1,
  BottomUp   = 2,
  TopDown    = 4,
  LeftRight  = 8,
  RigthLeft  = 16
};

int GetFanPixelOrder(int iPos, PixelOrder order = Sequential)
{
  while (iPos < 0)
    iPos += FAN_SIZE;

  int fanBase = (iPos % FAN_SIZE); 
  int offset = iPos - fanBase;

  switch (order)
  {
    case Reverse:
      return fanBase + FAN_SIZE - offset;

    case BottomUp:
      return fanBase + FanPixelsVertical[offset];
    
    case TopDown:
      return fanBase + FAN_SIZE - FanPixelsVertical[offset];

    case LeftRight:
      return fanBase + FanPixelsHorizontal[offset];

    case RigthLeft:
      return fanBase + FAN_SIZE - FanPixelsHorizontal[offset];

    case Sequential:
    default:
      return fanBase + offset;

  }
}

void DrawFanPixels(float fPos, float count, CRGB color, PixelOrder order = Sequential)
{
  // Calculate how much the first pixel will hold
  float availFirstPixel = 1.0f - (fPos - (long)(fPos));
  float amtFirstPixel = min(availFirstPixel, count);
  float remaining = min(count, FastLED.size()-fPos);
  int iPos = fPos;

  // Blend (add) in the color of the first partial pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[GetFanPixelOrder(iPos++, order)] += ColorFraction(color, amtFirstPixel);
    remaining -= amtFirstPixel;
  }

  // Now draw any full pixels in the middle

  while (remaining > 1.0f)
  {
    FastLED.leds()[GetFanPixelOrder(iPos++, order)] += color;
    remaining--;
  }

  // Draw tail pixel, up to a single full pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[GetFanPixelOrder(iPos, order)] += ColorFraction(color, remaining);
  }
}


void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) { }
  Serial.println("ESP32 Startup");

  g_OLED.begin();
  g_OLED.clear();
  g_OLED.setFont(u8g2_font_profont15_tf);
  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent();        // Descent is a negative number so we add it to the total

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS);               // Add our LED strip to the FastLED library
  FastLED.setBrightness(g_Brightness);
  set_max_power_indicator_LED(LED_BUILTIN);                               // Light the builtin LED if we power throttle
  FastLED.setMaxPowerInMilliWatts(g_PowerLimit);                          // Set the power limit, above which brightness will be throttled
}

void loop() 
{
  bool bLED = 0;

  FireEffect fire(NUM_LEDS, 20, 100, 3, NUM_LEDS, true, false);

  while (true)
  {
    FastLED.clear();
   
    /*
    fire.DrawFire();
    */

    /*
    float b = beat88(30)/255.0 * FAN_SIZE;
    DrawFanPixel(0, b, 1, CRGB::Red);
    DrawFanPixel(1, b, 1, CRGB::Green);
    DrawFanPixel(2, b, 1, CRGB::Blue);
    */

    float b = beat16(10) / 65535.0 * NUM_LEDS;
    for (int i = 0; i < b; i++)
      DrawFanPixels(i, 1, CRGB::Red);


    FastLED.show(g_Brightness);                          //  Show and delay

    EVERY_N_MILLISECONDS(250)
    {
      g_OLED.clearBuffer();
      g_OLED.setCursor(0, g_lineHeight);
      g_OLED.printf("FPS  : %u", FastLED.getFPS());
      g_OLED.setCursor(0, g_lineHeight * 2);
      g_OLED.printf("Power: %u mW", calculate_unscaled_power_mW(g_LEDs, 4));
      g_OLED.setCursor(0, g_lineHeight * 3);
      g_OLED.printf("Brite: %d", calculate_max_brightness_for_power_mW(g_Brightness, g_PowerLimit));
      g_OLED.sendBuffer();
    }
    delay(33);
  }
}
