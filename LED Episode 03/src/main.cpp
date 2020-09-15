#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C g_TFT(U8G2_R2, 15, 4, 16);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);           // Set the LED pin to output
    g_TFT.begin();                          // One-time startup
    g_TFT.clear();                          // Clear the screen
    g_TFT.setFont(u8g2_font_profont15_tf);  // Choose a suitable font
    g_TFT.setCursor(2, 12);                 // Cursor is at bottom of letter, so we need to move down
    //g_TFT.println("Hello World!");          // Print something

    // Draw a  border around the screen

    g_TFT.drawFrame(0, 0, 128, 64);

    // Draw a moire pattern like its 1984
    
    for (int x=0; x<128; x+= 4)
      g_TFT.drawLine(x, 0, 128-x, 64);

    // Draw some text on the left hand side

    g_TFT.setCursor(5, 32);
    g_TFT.print("Hello");
    g_TFT.setCursor(5, 42);
    g_TFT.print("World");

    // Draw a reticle on the right hand side

    const int reticleY = g_TFT.getHeight() / 2;           // Vertical center
    const int reticleR = g_TFT.getHeight() / 4 - 2;       // Slightly less than 1/4 screen height
    const int reticleX = g_TFT.getWidth() - reticleR - 8; // Right-justified with a small margin
    
    for (int r = reticleR; r > 0; r -= 3)
      g_TFT.drawCircle(reticleX, reticleY, r);
    g_TFT.drawHLine(reticleX - reticleR - 5, reticleY, 2 * reticleR + 10);
    g_TFT.drawVLine(reticleX, reticleY - reticleR - 5, 2 * reticleR + 10);

    g_TFT.sendBuffer();                     // Ship-It!

}

void loop() 
{
    digitalWrite(LED_BUILTIN, 0);
    delay(100);
    digitalWrite(LED_BUILTIN, 1);
    delay(100);
}