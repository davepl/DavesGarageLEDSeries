#include <Arduino.h>
#include <U8g2lib.h>

#define DISPLAY_CLOCK_PIN   15
#define DISPLAY_DATA_PIN    4
#define DISPLAY_RESET_PIN   16

U8G2_SSD1306_128X64_NONAME_F_SW_I2C g_OLED(U8G2_R2, DISPLAY_CLOCK_PIN, DISPLAY_DATA_PIN, DISPLAY_RESET_PIN);

void setup() 
{
  g_OLED.begin();
  g_OLED.clear();
  g_OLED.setFont(u8g2_font_profont15_tf);  // Choose a suitable font
  g_OLED.setCursor(2, 12);                 // Cursor is at bottom of letter, so we need to move down
  g_OLED.print("Hello World");
  g_OLED.sendBuffer();
}

void loop() 
{
    digitalWrite(LED_BUILTIN, 0);
    delay(100);
    digitalWrite(LED_BUILTIN, 1);
    delay(100);
}


