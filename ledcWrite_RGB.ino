#include <WiFi.h>
#include "mqtt.h"

/*
  ledcWrite_RGB.ino
  Runs through the full 255 color spectrum for an rgb led 
  Demonstrate ledcWrite functionality for driving leds with PWM on ESP32
 
  This example code is in the public domain.
  
  Some basic modifications were made by vseven, mostly commenting.
 */
 
// Set up the rgb led names
uint8_t ledR = A12;
uint8_t ledG = A13;
uint8_t ledB = A14; 

uint8_t ledArray[3] = {1, 2, 3}; // three led channels

const boolean invert = true; // set true if common anode, false if common cathode

uint8_t color = 0;          // a value from 0 to 255 representing the hue
uint8_t brightness = 255;  // 255 is maximum brightness, but can be changed.  Might need 256 for common anode to fully turn off.

const char* ssid = "Livebox-7F7F";
const char* password = "Mikusia2012";

void setup() 
{            
  Serial.begin(115200);
  delay(10); 
  
  ledcAttachPin(ledR, 1); // assign RGB led pins to channels
  ledcAttachPin(ledG, 2);
  ledcAttachPin(ledB, 3);
  
  // Initialize channels 
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);

  Serial.println("Send all LEDs a 255 and wait 2 seconds.");
  // If your RGB LED turns off instead of on here you should check if the LED is common anode or cathode.
  // If it doesn't fully turn off and is common anode try using 256.
  ledcWrite(1, 255);
  ledcWrite(2, 255);
  ledcWrite(3, 255);
  delay(2000);
  Serial.println("Send all LEDs a 0 and wait 2 seconds.");
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  delay(2000);
 
  Serial.println("Tests done. Connecting to Wifi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("Connection estabilished! IP: ");
  Serial.print(WiFi.localIP());
  Serial.println();

  Serial.println("Watching for changes...");

  watch();
}

// void loop runs over and over again
void loop() 
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(color_change){
    if(power_off){
      ledcWrite(1, 0);
      ledcWrite(2, 0);
      ledcWrite(3, 0);
    } else {
      ledcWrite(1, red);
      ledcWrite(2, green);
      ledcWrite(3, blue);
    }

    color_change = false;
  }

  delay(50);
}

// Courtesy http://www.instructables.com/id/How-to-Use-an-RGB-LED/?ALLSTEPS
// function to convert a color to its Red, Green, and Blue components.

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

    if(invert)
    {
      brightness = 255 - brightness;
      complement = 255;
      prev = 255 - prev;
      next = 255 - next;
    }

    switch(segment ) {
    case 0:      // red
        red = brightness;
        green = next;
        blue = complement;
    break;
    case 1:     // yellow
        red = prev;
        green = brightness;
        blue = complement;
    break;
    case 2:     // green
        red = complement;
        green = brightness;
        blue = next;
    break;
    case 3:    // cyan
        red = complement;
        green = prev;
        blue = brightness;
    break;
    case 4:    // blue
        red = next;
        green = complement;
        blue = brightness;
    break;
   case 5:      // magenta
    default:
        red = brightness;
        green = complement;
        blue = prev;
    break;
    }
}
