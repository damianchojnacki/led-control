#include <WiFi.h>
#include "PubSubClient.h"
#include "arduino_json.h"

const char* mqtt_server = "192.168.0.100";
unsigned int red = 0, green = 0, blue = 0;           // the Red Green and Blue color components
bool color_change = false;
bool power_off = false;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.println();

  // Changes the LED color according to the message
  if (String(topic) == "lights/1") {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, message);

    if(doc["state"] == "on"){
      power_off = false;

      Serial.println("Power on");
    } else if(doc["state"] == "off"){
      power_off = true;

      Serial.println("Power off");
    } else{
      unsigned int r = doc["color"]["r"];
      unsigned int g = doc["color"]["g"];
      unsigned int b = doc["color"]["b"];
      
      Serial.print("Color change: ");
      Serial.print("rgb(");
      Serial.print(r);
      Serial.print(", ");
      Serial.print(g);
      Serial.print(", ");
      Serial.print(b);
      Serial.print(")");
      Serial.println();
      
      red = r;
      green = g;
      blue = b;
    }

    color_change = true;
  }
}

void watch(){
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.subscribe("lights/1");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("lights/1")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("lights/1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
