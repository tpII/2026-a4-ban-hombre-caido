#include <Arduino.h>

// --- LIBRERÍAS, ESTRUCTURAS Y TAREAS ORIGINALES COMENTADAS ---
/*
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
//LIBRERIAS FREERTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include "config.h"
#include "red_mqtt.h"

Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;

//unsigned long lastMsg = 0;
bool mpuOK = false;
bool bmpOK = false;

// Offsets del giroscopio
float offsetGiroX = -0.110f;
float offsetGiroY = -0.019f;
float offsetGiroZ = 0.022f;

// Referencia del acelerómetro
float referenciaAcelX = -1.297f;
float referenciaAcelY = -0.158f;
float referenciaAcelZ = 0.595f;

// Referencia de presión atmosférica
float presionReferencia = 0;

// Estructura con todas las mediciones
struct Mediciones {
  float acelX;
  float acelY;
  float acelZ;

  float giroX;
  float giroY;
  float giroZ;

  float tempMPU;
  float tempBMP;
  int32_t presion;
};

// Estructura para solicitar una publicación MQTT
struct MensajeMQTT {
  char payload[200];
};

// Colas para comunicar las tareas
QueueHandle_t colaMediciones;
QueueHandle_t colaPublicaciones;

// Periodos de ejecución
const TickType_t PERIODO_SENSORES = pdMS_TO_TICKS(100);
const TickType_t PERIODO_PUBLICACION = pdMS_TO_TICKS(200);

// ==================================================
// 2. TAREA DE SENSORES
// ==================================================
void tareaSensores(void *pvParameters) {
  Mediciones datos = {};
  TickType_t ultimaEjecucion = xTaskGetTickCount();
  for (;;) {
    if (mpuOK) {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      datos.acelX = a.acceleration.x - referenciaAcelX;
      datos.acelY = a.acceleration.y - referenciaAcelY;
      datos.acelZ = a.acceleration.z - referenciaAcelZ;
      datos.giroX = g.gyro.x - offsetGiroX;
      datos.giroY = g.gyro.y - offsetGiroY;
      datos.giroZ = g.gyro.z - offsetGiroZ;
      datos.tempMPU = temp.temperature;
    }
    if (bmpOK) {
      datos.tempBMP = bmp.readTemperature();
      datos.presion = bmp.readPressure();
    }
    xQueueOverwrite(colaMediciones, &datos);
    vTaskDelayUntil(&ultimaEjecucion, PERIODO_SENSORES);
  }
}

// ==================================================
// 3. TAREA DE PUBLICACIÓN
// ==================================================
void tareaPublicacion(void *pvParameters) {
  Mediciones datos;
  TickType_t ultimaPublicacion = xTaskGetTickCount();
  for (;;) {
    vTaskDelayUntil(&ultimaPublicacion, PERIODO_PUBLICACION);
    if (xQueuePeek(colaMediciones, &datos, 0) == pdTRUE) {
      MensajeMQTT mensaje;
      snprintf(
        mensaje.payload, sizeof(mensaje.payload),
        "Acel X:%.2f Y:%.2f Z:%.2f m/s2 | Giro X:%.2f Y:%.2f Z:%.2f rad/s | Temp MPU:%.2f C | Temp BMP:%.2f C | Presion:%ld Pa",
        datos.acelX, datos.acelY, datos.acelZ, datos.giroX, datos.giroY, datos.giroZ, datos.tempMPU, datos.tempBMP, (long)datos.presion
      );
      if (xQueueSend(colaPublicaciones, &mensaje, 0) != pdTRUE) {
        Serial.println("Cola MQTT llena: mensaje descartado.");
      }
    }
  }
}

// ==================================================
// 4. TAREA DE COMUNICACIÓN
// ==================================================
void tareaComunicacion(void *pvParameters) {
  MensajeMQTT mensaje;
  setup_wifi();
  setup_mqtt();
  for (;;) {
    if (!client.connected()) {
      reconnect_mqtt();
    }
    client.loop();
    if (xQueueReceive(colaPublicaciones, &mensaje, pdMS_TO_TICKS(10)) == pdTRUE) {
      if (client.connected()) {
        client.publish("prueba/salida", mensaje.payload);
        Serial.print("Enviando: ");
        Serial.println(mensaje.payload);
      }
    }
  }
}
*/

// --- CONFIGURACIÓN DEL BUZZER ---
#define BUZZER_PIN 4 // Pin P4 de tu placa

void setup() {
  Serial.begin(115200);
  
  // Configuramos el pin del buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  /* --- SETUP ORIGINAL COMENTADO ---
  Wire.begin(21, 22);
  Wire1.begin(25, 26);
  
  mpuOK = mpu.begin();
  if (mpuOK) {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("MPU6050 encontrado.");
  } else {
    Serial.println("Error MPU6050.");
  }

  bmpOK = bmp.begin(BMP085_ULTRAHIGHRES, &Wire1);
  if (bmpOK) {
    Serial.println("BMP180 encontrado.");
  } else {
    Serial.println("Error BMP180.");
  }

  colaMediciones = xQueueCreate(1, sizeof(Mediciones));
  colaPublicaciones = xQueueCreate(3, sizeof(MensajeMQTT));

  if (colaMediciones == NULL || colaPublicaciones == NULL) {
    Serial.println("Error creando las colas.");
    while (true) {
      delay(1000);
    }
  }

  BaseType_t resultado1 = xTaskCreate(tareaSensores, "Sensores", 4096, NULL, 2, NULL);
  BaseType_t resultado2 = xTaskCreate(tareaPublicacion, "Publicacion", 4096, NULL, 1, NULL);
  BaseType_t resultado3 = xTaskCreate(tareaComunicacion, "Comunicacion", 6144, NULL, 2, NULL);

  if (resultado1 != pdPASS || resultado2 != pdPASS || resultado3 != pdPASS) {
    Serial.println("Error creando alguna tarea.");
  }
  */
}

void loop() {
  // --- TEST DEL BUZZER ---
  Serial.println("Buzzer ON");
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  
  Serial.println("Buzzer OFF");
  digitalWrite(BUZZER_PIN, LOW);
  delay(1000);

  /* --- LOOP ORIGINAL COMENTADO ---
  vTaskDelay(pdMS_TO_TICKS(1000));
  */
}