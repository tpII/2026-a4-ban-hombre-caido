#include <Arduino.h>
#include "config.h"
#include "red_mqtt.h"

unsigned long lastMsg = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  setup_mqtt();
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop(); 

  unsigned long now = millis();
  if (now - lastMsg > 5000) { 
    lastMsg = now;
    client.publish("prueba/salida", "Hola desde el ESP32!");
  }
}