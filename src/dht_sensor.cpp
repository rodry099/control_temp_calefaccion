#include <Arduino.h>
#include <DHT.h>

#include "dht_sensor.h"


// ==========================================
// CONFIGURACIÓN
// ==========================================

#define DHT_PIN 2
#define DHT_TYPE DHT22

// ==========================================
// OBJETO DHT
// ==========================================

DHT dht(DHT_PIN,DHT_TYPE);

// ==========================================
// INICIAR DHT
// ==========================================

void iniciarDHT()
{
    dht.begin();

    Serial.println("DHT11 iniciado");
}


float leerTemperatura()
{
    float temperatura = dht.readTemperature();

    Serial.print("LECTURA TEMPERATURA = ");
    Serial.println(temperatura);

    return temperatura;
}

float leerHumedad()
{
    float humedad = dht.readHumidity();

    Serial.print("LECTURA HUMEDAD = ");
    Serial.println(humedad);

    return humedad;
}