/* minimf_gas_neo1
 receives values from a MQ-3 gas sensor and 
 translates that into an LED strand (Neopixel) display
 
 Basically this is a variation of the Neopixel LED strand code
 see this tutorial on the adafruit site:
 https://learn.adafruit.com/adafruit-neopixel-uberguide
 
 The gas sensor requires 5V heater wire.
 You can read about how to hook up the gas sensor here:
 https://www.sparkfun.com/products/8880
 http://wiring.org.co/learning/basics/airqualitymq135.html 
 
 These settings for the Arduino Micro:
 https://www.adafruit.com/product/1086
 
 ChrisJ, 10.15.2014, initial version
*/

// include some standard libs
#include <Adafruit_NeoPixel.h>

// I use the strands with 60 LEDs / meter but cut 
// them into short strands of 25 for this project
#define LEDPIN 6  // the neopixel data pin on the arduino
#define LEDCOUNT 25  // the number of LEDs in the strand

/* From the Neopixel strip, 
     Connect Din    to digital pin 6 of Arduino Micro
     Connect 5V     to 5V of Arduino Micro
     Connect GND    to GND of Arduino Micro

25 LEDs at nominal 20mA each = 500 mA
The 5V is expected to provide ~400 mA from the USB, so...
This might not be a very robust power setup, but it works for me.
It also holds up for the heater in the MQ-3 but this is pushing it.
*/


// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDCOUNT,LEDPIN, NEO_GRB + NEO_KHZ800);

int AnalogPin0 = 0; // Alcohol on bread board is connected to analog pin A0
int sensorReading;      // the analog reading from the resistor divider

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
}

void loop() {
  sensorReading = analogRead(AnalogPin0);
  Serial.print("Analog reading = ");
  Serial.println(sensorReading);

  // colorLevel(strip.Color(255, 0, 0), sensorReading); // Red
  colorLevel(strip.Color(0, 255, 0), sensorReading); // Green
  // colorLevel(strip.Color(0, 0, 255), sensorReading); // Blue
}

// Fill the dots one after the other with a color
void colorLevel(uint32_t c, int level) {
  
  uint16_t scaledLevel = map(level,90,460, 0,LEDCOUNT);  // for alcohol sensor

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    
    if(i >= scaledLevel) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    else
    {
      strip.setPixelColor(i, c);
    }
  }
  strip.show();
  delay(2);
}

