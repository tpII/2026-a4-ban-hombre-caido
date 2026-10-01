#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include "config.h"
#include "red_mqtt.h"

Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;
unsigned long lastMsg = 0;

void setup() {
  Serial.begin(115200);
  
  setup_wifi();
  setup_mqtt();

  Wire.begin(21, 22); 
  
  // Inicializar MPU6050
  if (!mpu.begin()) {
    Serial.println("¡Error MPU6050!");
  } else {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }

  // Inicializar BMP180
  if (!bmp.begin()) {
    Serial.println("¡Error BMP180! Revisa las conexiones.");
  } else {
    Serial.println("BMP180 encontrado.");
  }
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop(); 

  unsigned long now = millis();
  
  if (now - lastMsg > 5000) { 
    lastMsg = now;

    // Lectura MPU6050
    sensors_event_t a, g, temp_mpu;
    mpu.getEvent(&a, &g, &temp_mpu);

    // Lectura BMP180
    float temp_bmp = bmp.readTemperature(); // Temperatura en °C
    int32_t pressure = bmp.readPressure();  // Presión en Pascales (Pa)
    
    // Preparar el mensaje (Aumentamos a 200 bytes para acomodar todo)
    char payload[200];
    snprintf(payload, sizeof(payload), 
             "Acel Z:%.2f | Giro X:%.2f | Temp(MPU):%.2fC | Temp(BMP):%.2fC | Presion:%d Pa", 
             a.acceleration.z, g.gyro.x, temp_mpu.temperature, temp_bmp, pressure);

    Serial.print("Enviando: ");
    Serial.println(payload);

    client.publish("prueba/salida", payload);
  }
}