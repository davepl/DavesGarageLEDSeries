#include <Arduino.h>

#define RED_PIN   16
#define GREEN_PIN 17
#define BLUE_PIN  18

// Courtesy http://www.instructables.com/id/How-to-Use-an-RGB-LED/?ALLSTEPS
// function to convert a color to its Red, Green, and Blue components.

uint32_t R, G, B;           // the Red Green and Blue color components

void hueToRGB(uint8_t hue, uint8_t brightness)
{
    uint16_t scaledHue = (hue * 6);
    uint8_t segment = scaledHue / 256; // segment 0 to 5 around the
                                            // color wheel
    uint16_t segmentOffset =
      scaledHue - (segment * 256); // position within the segment

    uint8_t complement = 0;
    uint16_t prev = (brightness * ( 255 -  segmentOffset)) / 256;
    uint16_t next = (brightness *  segmentOffset) / 256;

    switch(segment ) {
    case 0:      // red
        R = brightness;
        G = next;
        B = complement;
    break;
    case 1:     // yellow
        R = prev;
        G = brightness;
        B = complement;
    break;
    case 2:     // green
        R = complement;
        G = brightness;
        B = next;
    break;
    case 3:    // cyan
        R = complement;
        G = prev;
        B = brightness;
    break;
    case 4:    // blue
        R = next;
        G = complement;
        B = brightness;
    break;
   case 5:      // magenta
    default:
        R = brightness;
        G = complement;
        B = prev;
    break;
    }
}

void setup() 
{
  pinMode(RED_PIN,   OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN,  OUTPUT);

  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN,  HIGH);  

  ledcAttachPin(RED_PIN, 1);        // Assign PWM generator 1 to RED
  ledcAttachPin(GREEN_PIN, 2);      // Assign PWM generator 2 to GREEN
  ledcAttachPin(BLUE_PIN, 3);       // Assign PWM generator 3 to BLUE

  ledcSetup(1, 12000, 8);           // Set it to 12kHZ and 8-bit resolution
  ledcSetup(2, 12000, 8);           // Set it to 12kHZ and 8-bit resolution
  ledcSetup(3, 12000, 8);           // Set it to 12kHZ and 8-bit resolution
}

void loop() 
{
  for (int c = 0; c < 256; c++)
  {
    hueToRGB(c, 255);             // Convert color to max brightness

    ledcWrite(1, R);
    ledcWrite(2, G);
    ledcWrite(3, B);

    delay(100);
  }
}