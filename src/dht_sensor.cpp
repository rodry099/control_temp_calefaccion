#include <Arduino.h>
#include <DHT.h>

#include "dht_sensor.h"

// ==========================================
// CONFIGURACIÓN
// ==========================================

#define DHT_PIN D5
#define DHT_TYPE DHT22
#define DHT_POWER_PIN D6

const uint8_t DHT_POWER_ON = LOW;
const uint8_t DHT_POWER_OFF = HIGH;
const uint8_t MAX_NAN_CONSECUTIVOS = 5;
const unsigned long TIEMPO_DHT_APAGADO = 3000;
const unsigned long TIEMPO_DHT_ESTABILIZACION = 2000;

// ==========================================
// OBJETO DHT
// ==========================================

DHT dht(DHT_PIN, DHT_TYPE);

enum EstadoDHT { DHT_LISTO, DHT_APAGADO, DHT_ESTABILIZANDO };

EstadoDHT estadoDHT = DHT_LISTO;
uint8_t nanConsecutivos = 0;
unsigned long momentoCambioEstado = 0;

// ==========================================
// INICIAR DHT
// ==========================================

void iniciarDHT() {
  digitalWrite(DHT_POWER_PIN, DHT_POWER_OFF);
  pinMode(DHT_POWER_PIN, OUTPUT);
  digitalWrite(DHT_POWER_PIN, DHT_POWER_ON);
  pinMode(DHT_PIN, INPUT);

  dht.begin();

  Serial.println("DHT22 iniciado");
}

bool leerDHT(float &temperatura, float &humedad) {
  unsigned long ahora = millis();

  if (estadoDHT == DHT_APAGADO) {
    if (ahora - momentoCambioEstado < TIEMPO_DHT_APAGADO) {
      return false;
    }

    digitalWrite(DHT_POWER_PIN, DHT_POWER_ON);
    dht.begin();
    momentoCambioEstado = ahora;
    estadoDHT = DHT_ESTABILIZANDO;

    Serial.println("DHT22 encendido; esperando estabilizacion");
    return false;
  }

  if (estadoDHT == DHT_ESTABILIZANDO) {
    if (ahora - momentoCambioEstado < TIEMPO_DHT_ESTABILIZACION) {
      return false;
    }

    estadoDHT = DHT_LISTO;
  }

  temperatura = dht.readTemperature();
  humedad = dht.readHumidity();

  Serial.print("LECTURA TEMPERATURA = ");
  Serial.println(temperatura);
  Serial.print("LECTURA HUMEDAD = ");
  Serial.println(humedad);

  if (isnan(temperatura) || isnan(humedad)) {
    if (nanConsecutivos < MAX_NAN_CONSECUTIVOS) {
      nanConsecutivos++;
    }

    Serial.print("Lectura DHT22 invalida consecutiva: ");
    Serial.println(nanConsecutivos);

    if (nanConsecutivos >= MAX_NAN_CONSECUTIVOS) {
      pinMode(DHT_PIN, INPUT);
      digitalWrite(DHT_POWER_PIN, DHT_POWER_OFF);
      momentoCambioEstado = ahora;
      estadoDHT = DHT_APAGADO;

      Serial.println("DHT22 apagado para recuperar el sensor");
    }

    return false;
  }

  nanConsecutivos = 0;
  return true;
}