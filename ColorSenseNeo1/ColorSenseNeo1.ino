/* ColorSensorNeo1
 receives values from a color sensor, the Flora Color Sensor with 
 White Illumination LED - TCS34725 from Adafruit and 
 translates that into an LED strand (Neopixel)
 
 Basically this is a combination of code meant for the TCS34725 and the Neopixel LEDs
 see this tutorial on the adafruit site:
 https://learn.adafruit.com/adafruit-color-sensors
 https://learn.adafruit.com/adafruit-neopixel-uberguide
 
 ChrisJ, 10.9.2014, 1st mashup version
 ChrisJ, 10.15.2014, tweaked the integration time and gain
*/

// include some standard libs
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

// I use the strands with 60 LEDs / meter but cut 
// them into short strands of 25 for this project
int ledCount = 25;
int neoDataPin = 6;

/* From the Flora Color Sensor, 
     Connect SCL    to digital pin 3 (aka, PWM,SCL) of Arduino Micro
     Connect SDA    to digital pin 2 (aka, SDA) of Arduino Micro
     Connect 3V     to 3v3 of Arduino Micro
     Connect GND    to GND of Arduino Micro
*/

/* From the Neopixel strip, 
     Connect Din    to digital pin 6 of Arduino Micro
     Connect 5V     to 5V of Arduino Micro
     Connect GND    to GND of Arduino Micro

25 LEDs at nominal 20mA each = 500 mA
The 5V is expected to provide ~400 mA from the USB, so...
This might not be a very robust power setup, but it works for me.
*/

// Get the neopixel strip configured./..
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(ledCount, neoDataPin, NEO_GRB + NEO_KHZ800);

// our RGB -> eye-recognized gamma color
byte gammatable[256];

// Initialise with default values.
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();
// Initialise with specific int time and gain values:
// Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
// Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_1X);

//==========================================
void setup() {
  Serial.begin(9600);
  Serial.println("Put a color card over the sensor...");
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    gammatable[i] = x;      
    //Serial.println(gammatable[i]);
  }

  // Now we're ready to get readings!
  //white, but dimmer-- 255 for all three values makes it blinding!
  strip.setPixelColor (0, strip.Color(188, 188, 188)); 

}


//######################################
void loop() {
  uint16_t clear, red, green, blue;

  // turn on the illumination LED, get the color data, turn off the illumination LED.
  tcs.setInterrupt(false);      // turn on illumination LED
  delay(50);  // takes 50ms to read?
  tcs.getRawData(&red, &green, &blue, &clear);
  delay(10); 
  tcs.setInterrupt(true);  // turn off illumination LED

  // print some info to the serial monitor for testing
  Serial.print("raw: C: "); Serial.print(clear);
  Serial.print("\tR: "); Serial.print(red);
  Serial.print("\tG: "); Serial.print(green);
  Serial.print("\tB: "); Serial.print(blue);

  // Figure out some basic hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;
  sum = clear;
  float r, g, b;
  r = red; 
  r /= sum;
  g = green; 
  g /= sum;
  b = blue; 
  b /= sum;
  r *= 256; 
  g *= 256; 
  b *= 256;
  
  // more printing for tests
  Serial.print("\t #");
  Serial.print((int)r, HEX); 
  Serial.print((int)g, HEX); 
  Serial.print((int)b, HEX);
  Serial.print(" ");

  Serial.print("\t calc: "); 
  Serial.print((int)r ); 
  Serial.print(" "); 
  Serial.print((int)g);
  Serial.print(" ");  
  Serial.print((int)b );
  Serial.println("");

  // usually the clear value is below 100 when there's nothing in front of it
  // so we'll turn on the strip only if it has a meaningful value
  if (clear > 100 ) {
    colorWipe(strip.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]), 0);
  }

  // display for 2 secs then clear the leds
  delay(2000);
  colorWipe(strip.Color(0, 0, 0), 0);
  delay(1000);

}
//######################################

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

