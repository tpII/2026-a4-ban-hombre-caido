#include <Arduino.h>
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
// Lee el MPU6050 y el BMP180 cada 100 ms
// ==================================================

void tareaSensores(void *pvParameters) {

  Mediciones datos = {};

  TickType_t ultimaEjecucion = xTaskGetTickCount();

  for (;;) {

    // Leer MPU6050
    if (mpuOK) {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);

      // Aceleración en los tres ejes
      datos.acelX = a.acceleration.x - referenciaAcelX;
      datos.acelY = a.acceleration.y - referenciaAcelY;
      datos.acelZ = a.acceleration.z - referenciaAcelZ;

      // Velocidad angular en los tres ejes
      datos.giroX = g.gyro.x - offsetGiroX;
      datos.giroY = g.gyro.y - offsetGiroY;
      datos.giroZ = g.gyro.z - offsetGiroZ;

      datos.tempMPU = temp.temperature;
    }

    // Leer BMP180
    if (bmpOK) {
      datos.tempBMP = bmp.readTemperature();
      datos.presion = bmp.readPressure();
    }

    // Guardar la última medición disponible
    xQueueOverwrite(colaMediciones, &datos);

    // Esperar hasta la próxima ejecución periódica
    vTaskDelayUntil(
      &ultimaEjecucion,
      PERIODO_SENSORES
    );
  }
}


// ==================================================
// 3. TAREA DE PUBLICACIÓN
// Prepara el mensaje cada 5 segundos
// No accede directamente al cliente MQTT
// ==================================================

void tareaPublicacion(void *pvParameters) {

  Mediciones datos;

  TickType_t ultimaPublicacion = xTaskGetTickCount();

  for (;;) {

    // Esperar 5 segundos entre publicaciones
    vTaskDelayUntil(
      &ultimaPublicacion,
      PERIODO_PUBLICACION
    );

    // Obtener la medición más reciente
    if (xQueuePeek(
          colaMediciones,
          &datos,
          0
        ) == pdTRUE) {

      MensajeMQTT mensaje;

      snprintf(
        mensaje.payload,
        sizeof(mensaje.payload),
        "Acel X:%.2f Y:%.2f Z:%.2f m/s2 | "
        "Giro X:%.2f Y:%.2f Z:%.2f rad/s | "
        "Temp MPU:%.2f C | Temp BMP:%.2f C | "
        "Presion:%ld Pa",
        datos.acelX,
        datos.acelY,
        datos.acelZ,
        datos.giroX,
        datos.giroY,
        datos.giroZ,
        datos.tempMPU,
        datos.tempBMP,
        (long)datos.presion
      );

      // Enviar el mensaje a la tarea de comunicación
      if (xQueueSend(
            colaPublicaciones,
            &mensaje,
            0
          ) != pdTRUE) {

        Serial.println(
          "Cola MQTT llena: mensaje descartado."
        );
      }
    }
  }
}


// ==================================================
// 4. TAREA DE COMUNICACIÓN
// Gestiona Wi-Fi, MQTT y las publicaciones
// ==================================================

void tareaComunicacion(void *pvParameters) {

  MensajeMQTT mensaje;

  // Conectar a Wi-Fi e inicializar MQTT
  setup_wifi();
  setup_mqtt();

  for (;;) {

    // Reconectar si se pierde la conexión
    if (!client.connected()) {
      reconnect_mqtt();
    }

    // Atender la conexión MQTT frecuentemente
    client.loop();

    // Comprobar si hay un mensaje para publicar
    if (xQueueReceive(
          colaPublicaciones,
          &mensaje,
          pdMS_TO_TICKS(10)
        ) == pdTRUE) {

      if (client.connected()) {
        client.publish(
          "prueba/salida",
          mensaje.payload
        );

        Serial.print("Enviando: ");
        Serial.println(mensaje.payload);
      }
    }
  }
}

//SETUP Inicializa hardware, colas y tareas
void setup() {
  Serial.begin(115200);
  
  //setup_wifi();
  //setup_mqtt();

  Wire.begin(21, 22); //inicializa I2C con pines SDA=21 y SCL=22
  Wire1.begin(25, 26);
  
  /*
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
  */

// Inicializar MPU6050
  mpuOK = mpu.begin();

  if (mpuOK) {
    mpu.setAccelerometerRange(
      MPU6050_RANGE_8_G
    );

    mpu.setGyroRange(
      MPU6050_RANGE_500_DEG
    );

    mpu.setFilterBandwidth(
      MPU6050_BAND_21_HZ
    );

    Serial.println("MPU6050 encontrado.");
  } else {
    Serial.println("Error MPU6050.");
  }

  // Inicializar BMP180
  bmpOK = bmp.begin(BMP085_ULTRAHIGHRES, &Wire1);

  if (bmpOK) {
    Serial.println("BMP180 encontrado.");
  } else {
    Serial.println("Error BMP180.");
  }

  // Crear cola para la última medición
  colaMediciones = xQueueCreate(
    1,
    sizeof(Mediciones)
  );

  // Crear cola para mensajes pendientes de MQTT
  colaPublicaciones = xQueueCreate(
    3,
    sizeof(MensajeMQTT)
  );

  if (colaMediciones == NULL ||
      colaPublicaciones == NULL) {

    Serial.println("Error creando las colas.");

    while (true) {
      delay(1000);
    }
  }

  // Crear tarea de sensores
  BaseType_t resultado1 = xTaskCreate(
    tareaSensores,
    "Sensores",
    4096,
    NULL,
    2,
    NULL
  );

  // Crear tarea de publicación
  BaseType_t resultado2 = xTaskCreate(
    tareaPublicacion,
    "Publicacion",
    4096,
    NULL,
    1,
    NULL
  );

  // Crear tarea de comunicación
  BaseType_t resultado3 = xTaskCreate(
    tareaComunicacion,
    "Comunicacion",
    6144,
    NULL,
    2,
    NULL
  );

  if (resultado1 != pdPASS ||
      resultado2 != pdPASS ||
      resultado3 != pdPASS) {

    Serial.println("Error creando alguna tarea.");
  }
}


// LOOP DE ARDUINO
// El trabajo se realiza en las tareas FreeRTO
void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
  /*
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
  */
}