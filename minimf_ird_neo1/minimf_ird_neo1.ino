/* minimf_ird_neo1
 receives values from a infrared/IR distance sensor and 
 translates that into an LED strand (Neopixel) display
 The effect is to raise and lower your hand over the senor which is
 pointing up and have the LEDs track with your hand
 
 Basically this is a variation of the Neopixel LED strand code
 see this tutorial on the adafruit site:
 https://learn.adafruit.com/adafruit-neopixel-uberguide
 
 You can read about how to hook up IRD sensor here:
 https://www.adafruit.com/products/164  - range 10cm - 80cm 
 http://sharp-world.com/products/device/lineup/data/pdf/datasheet/gp2y0a21yk_e.pdf 
 
 This code for the Arduino Micro:
 https://www.adafruit.com/product/1086
 
 ChrisJ, 10.15.2014, initial version
*/

// include some standard libs
#include <Adafruit_NeoPixel.h>


// I use the strands with 60 LEDs / meter but cut 
// them into short strands of 25 for this project
#define LEDCOUNT 25 // the number of LEDs in the strand
#define NEOPIN 6 // the neopixel data pin on the arduino

/* From the Neopixel strip, 
     Connect Din    to digital pin 6 of Arduino Micro
     Connect 5V     to 5V of Arduino Micro
     Connect GND    to GND of Arduino Micro

25 LEDs at nominal 20mA each = 500 mA
The 5V is expected to provide ~400 mA from the USB, so...
This might not be a very robust power setup, but it works for me.
*/

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDCOUNT,NEOPIN, NEO_GRB + NEO_KHZ800);

int AnalogPin = 0; // sensor is connected to analog 0
int sensorReading; // the analog reading from the resistor divider

// the IRD sensor responds with a larger value as the target moves closer, 
// max is around 600, min is about 60
// use these values to calibrate and refine the hand to LED response 
int minLevel = 600;  // the off or low end value
int maxLevel = 132;  // the max or high end value


void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
}


void loop() {
  
  sensorReading = analogRead(AnalogPin);
  Serial.print("   Analog reading = ");
  Serial.println (sensorReading);
  
  // set the colors based on the levels
  // vuLevel(sensorReading); // use this to get a banded vu meter look
  vuLevelRainbow(sensorReading);  // use this to get a vu meter look with rainbow of colors

}

// #####################################################


// Fill the dots one after the other to look like a vu meter
void vuLevel(int level) {

  // set the color thresholds
  int vuLowLow = 4;  // yellow
  int vuMidLow = 9;  // green
  int vuMid = 14;  // blue
  int vuMidHigh = 19;  // purple
  int vuHigh = 25;  // red

  // map the range from the the values from the sensor to the 
  // number of LED in the strip
  uint16_t constrLevel = constrain(level,maxLevel,minLevel);
  uint16_t scaledLevel = map(constrLevel,minLevel,maxLevel, 0,strip.numPixels());

  // iterate through each LED in the strip and set them on/off
  // and set the color based on the level
  for(uint16_t i=0; i<strip.numPixels(); i++) {
 
//    Serial.print("Scaled Level: ");
//    Serial.print(scaledLevel);
//    Serial.print(" : ");
//    Serial.println(i);

    if (i >= scaledLevel) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));  // blank/off
    }
    else if (i <= vuLowLow) {
      strip.setPixelColor(i, strip.Color(255, 255, 0)); // yellow
    }
    else if (i <= vuMidLow) {
      strip.setPixelColor(i, strip.Color(0, 255, 0)); // green
    }
    else if (i <= vuMid) {
      strip.setPixelColor(i, strip.Color(0, 0, 255)); // blue
    }
    else if (i <= vuMidHigh) {
      strip.setPixelColor(i, strip.Color(255, 0, 255)); // purple
    }
    else {
      strip.setPixelColor(i, strip.Color(255, 0, 0)); // red
    }

    strip.show(); // show what we have set
    delay(1); // make it very responsive
  }
}


// Fill the dots one after the other to look like a rainbow vu meter
void vuLevelRainbow(int level) { 
  uint16_t i, j;

  uint16_t constrLevel = constrain(level, maxLevel, minLevel);
  uint16_t scaledLevel = map(constrLevel, minLevel, maxLevel, 0, strip.numPixels());

  Serial.print("Contrained Level: ");
  Serial.print(constrLevel);
  Serial.print("  Scaled Level: ");
  Serial.print(scaledLevel);

  for(j=1; j<strip.numPixels(); j++) {
    for(i=0; i< strip.numPixels(); i++) {
      if (i >= scaledLevel) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));  // blank/off
      }
      else {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) ) & 255));
        // strip.setPixelColor(i, Wheel(1));
      }
    }
  }
    strip.show();
    delay(100);
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


