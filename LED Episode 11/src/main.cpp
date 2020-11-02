//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        LED Episode 11
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

#define FAN_SIZE      16        // Number of LEDs in each fan
#define NUM_FANS       3        // Number of Fans
#define LED_FAN_OFFSET 4        // How far from bottom first pixel is
#define NUM_LEDS      48        // FastLED definitions
#define LED_PIN        5

CRGB g_LEDs[NUM_LEDS] = {0};    // Frame buffer for FastLED

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight = 0;
int g_Brightness = 255;         // 0-255 LED brightness scale
int g_PowerLimit = 3000;        // 900mW Power Limit

#include "ledgfx.h"
#include "comet.h"
#include "marquee.h"
#include "twinkle.h"
#include "fire.h"

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

void DrawMarqueeComparison()
{
  static float scroll = 0.0f;
  scroll += 0.1f;
  if (scroll > 5.0)
    scroll -= 5.0;

  for (float i = scroll; i < NUM_LEDS/2 -1; i+= 5)
  {
    DrawPixels(i, 3, CRGB::Green);
    DrawPixels(NUM_LEDS-1-(int)i, 3, CRGB::Red);
  }
}

void loop() 
{
  bool bLED = 0;

  while (true)
  {
    FastLED.clear();

    /*
    // RGB Spinners
    float b = beat16(60) / 65535.0f * FAN_SIZE;
    DrawFanPixels(b, 1, CRGB::Red,   Sequential,  0);
    DrawFanPixels(b, 1, CRGB::Green, Sequential,  1);
    DrawFanPixels(b, 1, CRGB::Blue,  Sequential,  2);
    */

    /*
    // Left to Right Cyan Wipe
    float b = beatsin16(60) / 65535.0f * FAN_SIZE;
    for (int iFan = 0; iFan < NUM_FANS; iFan++)
      DrawFanPixels(0, b, CRGB::Cyan, LeftRight, iFan);
    */

    /* Right to Left Cyan Wipe
    float b = beatsin16(60) / 65535.0f * FAN_SIZE;
    for (int iFan = 0; iFan < NUM_FANS; iFan++)
      DrawFanPixels(0, b, CRGB::Cyan, RightLeft, iFan);
    */

    /*
    // Bottom Up Green Wipe
    float b = beatsin16(60) / 65535.0f * NUM_LEDS;
      DrawFanPixels(0, b, CRGB::Green, BottomUp);
    */

    /*
    // Top Down Green Wipe
    float b = beatsin16(60) / 65535.0f * NUM_LEDS;
        DrawFanPixels(0, b, CRGB::Green, TopDown);    
    */

    /*
    // Simple Color Cycle
    static byte hue = 0;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, CHSV(hue, 255, 255));
    hue += 4;
    */

    /*
    // Sequential Color Rainbows
    static byte basehue = 0;
    byte hue = basehue;
    basehue += 4;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, CHSV(hue+=16, 255, 255));
    basehue += 4;
    */

    /*
    // Vertical Rainbow Wipe
    static byte basehue = 0;
    byte hue = basehue;
    basehue += 8;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, CHSV(hue+=16, 255, 255), LeftRight);
    */

    /*
    static byte basehue = 0;
    byte hue = basehue;
    basehue += 8;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, CHSV(hue+=16, 255, 255), BottomUp);
    */

    /*
    // Rainbow Strip Palette Effect
    static CRGBPalette256 pal(RainbowStripeColors_p);
    static byte baseColor = 0;
    byte hue = baseColor;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, ColorFromPalette(pal, hue += 4), BottomUp);
    baseColor += 1;
    */

    /*
    // vu-style Meter bar
    int b = beatsin16(30) * NUM_LEDS / 65535L;
    static const CRGBPalette256 vuPaletteGreen = vu_gpGreen;
    for (int i = 0; i < b; i++)
      DrawFanPixels(i, 1, ColorFromPalette(vuPaletteGreen, (int)(255 * i / NUM_LEDS)), BottomUp);
    */

    /*
    // Sequential Fire Fans
    static FireEffect fire(NUM_LEDS, 20, 100, 3, NUM_LEDS, true, false);
    fire.DrawFire();
    */

    /*
    // Bottom Up Fire Effect with extra sparking on first fan only
    static FireEffect fire(NUM_LEDS, 20, 140, 3, FAN_SIZE, true, false);
    fire.DrawFire(BottomUp);
    */

    // LeftRight (Wide Style) Fire Effect with extra sparking on first fan only

    /*
    static FireEffect fire(NUM_LEDS, 20, 140, 3, FAN_SIZE, true, false);
    fire.DrawFire(LeftRight);
    for (int i = 0; i < FAN_SIZE; i++)
    {
      g_LEDs[i] = g_LEDs[i + 2 * FAN_SIZE];
      g_LEDs[i + FAN_SIZE] = g_LEDs[i + 2 * FAN_SIZE];
    }
    */

   int b = beatsin16(30) * NUM_LEDS / 65535L;
   static const CRGBPalette256 seahawksPalette = gpSeahawks;
   for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, ColorFromPalette(seahawksPalette, beat8(64) + (int)(255 * i / NUM_LEDS)), BottomUp);
  

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
