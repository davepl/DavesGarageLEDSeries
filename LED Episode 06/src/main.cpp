//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        LED Episode 06
//
// Description:
//
//   Draws on a 144px/m strip
//
// History:     Sep-15-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#include <U8g2lib.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#define OLED_CLOCK  15          // Pins for the OLED display
#define OLED_DATA   4
#define OLED_RESET  16

#define NUM_LEDS    45          // FastLED definitions
#define LED_PIN     5

CRGB g_LEDs[NUM_LEDS] = {0};    // Frame buffer for FastLED

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight = 0;
int g_Brightness = 64;           // 0-255 brightness scale

#include "twinkle.h"
#include "marquee.h"
#include "bounce.h"

// FramesPerSecond
//
// Tracks a weighted average to smooth out the values that it calcs as the simple reciprocal
// of the amount of time taken specified by the caller.  So 1/3 of a second is 3 fps, and it
// will take up to 10 frames or so to stabilize on that value.

double FramesPerSecond(double seconds)
{
  static double framesPerSecond; 
  framesPerSecond = (framesPerSecond * .9) + (1.0 / seconds * .1);
  return framesPerSecond;
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
  FastLED.setBrightness(255);
}

void loop() 
{
  bool bLED = 0;
  double fps = 0;

  InitBounce();

  for (;;)
  {
    bLED = !bLED;                                                         // Blink the LED off and on  
    digitalWrite(LED_BUILTIN, bLED);

    double dStart = millis() / 1000.0;                                    // Display a frame and calc how long it takes

    // Handle OLED drawing

    static unsigned long msLastUpdate = millis();
    if (millis() - msLastUpdate > 250)
    {
      g_OLED.clearBuffer();
      g_OLED.setCursor(0, g_lineHeight);
      g_OLED.printf("FPS: %.1lf", fps);
      g_OLED.sendBuffer();
      msLastUpdate = millis();
    }

    // Handle LEDs

    DrawBounce();

    FastLED.show(g_Brightness);

    double dEnd = millis() / 1000.0;
    fps = FramesPerSecond(dEnd - dStart);
  }
}
