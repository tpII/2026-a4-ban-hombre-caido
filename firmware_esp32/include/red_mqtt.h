#ifndef RED_MQTT_H
#define RED_MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>

// Declaración global del cliente MQTT para que main.cpp pueda usar .publish()
extern PubSubClient client;

void setup_wifi();
void setup_mqtt();
void reconnect_mqtt();
void mqtt_callback(char* topic, byte* payload, unsigned int length);

#endif