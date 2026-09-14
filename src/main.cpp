#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>

#include "wifi.h"
#include "webserver.h"
#include "config.h"
#include "oled.h"
#include "dht_sensor.h"
#include "mqtt.h"
#include "mdns.h"

const int PIN_ALIMENTACION = 14; // D5

// ==================================================
// TEMPORIZADORES
// ==================================================

unsigned long tiempoOLED = 0;
const unsigned long intervaloOLED = 2000;

unsigned long tiempoDHT = 0;
const unsigned long intervaloDHT = 2500;

unsigned long tiempoMQTT = 0;
const unsigned long intervaloMQTT = 5000;

// ==================================================
// DATOS ACTUALES DEL DHT22
// ==================================================

float temperaturaActual = NAN;
float humedadActual = NAN;

int contadorErroresDHT22 = 0;
bool errorDHT22 = false;

// ==================================================
// ACTUALIZAR OLED
// ==================================================

void actualizarOLED()
{
    bool conectado = WiFi.status() == WL_CONNECTED;

    String ssid;
    String ip;
    int rssi = 0;

    // ------------------------------------------
    // DATOS WIFI
    // ------------------------------------------

    if (conectado)
    {
        ssid = WiFi.SSID();
        ip = WiFi.localIP().toString();
        rssi = WiFi.RSSI();
    }
    else
    {
        ssid = WiFi.softAPSSID();
        ip = WiFi.softAPIP().toString();
        rssi = 0;
    }

    // ------------------------------------------
    // UTILIZAR DATOS DHT22 YA LEÍDOS
    // ------------------------------------------

    float temperatura = temperaturaActual;

    float humedad = humedadActual;

    // ------------------------------------------
    // SI TODAVÍA NO HAY UNA LECTURA VÁLIDA
    // MOSTRAR 0
    // ------------------------------------------

    if (isnan(temperatura))
    {
        temperatura = 0;
    }

    if (isnan(humedad))
    {
        humedad = 0;
    }

    // ------------------------------------------
    // ACTUALIZAR OLED
    // ------------------------------------------

    actualizarPantalla(
        conectado,
        ssid.c_str(),
        ip.c_str(),
        rssi,
        temperatura,
        humedad);
}

// ==================================================
// ACTUALIZAR DHT22
// ==================================================

bool actualizarDHT()
{
    float temperatura = leerTemperatura();

    float humedad = leerHumedad();

    // ------------------------------------------
    // COMPROBAR LECTURA
    // ------------------------------------------

    if (isnan(temperatura) || isnan(humedad))
    {
        Serial.println("Error leyendo DHT22");

        return false;;
    }

    // ------------------------------------------
    // GUARDAR VALORES
    // ------------------------------------------

    temperaturaActual = temperatura;

    humedadActual = humedad;

    // ------------------------------------------
    // MONITOR SERIE
    // ------------------------------------------

    Serial.print("Temperatura: ");

    Serial.print(temperaturaActual);

    Serial.println(" C");

    Serial.print("Humedad: ");

    Serial.print(humedadActual);

    Serial.println(" %");

    return true;
}

void resetearDHT()
{
    Serial.println("Reiniciando DHT22");
    digitalWrite(PIN_ALIMENTACION, LOW);
    
    delay(2000);

    digitalWrite(PIN_ALIMENTACION, HIGH);
    contadorErroresDHT22 = 0;
}

// ==================================================
// SETUP
// ==================================================

void setup()
{
    Serial.begin(115200);

    // ==================================================
    // IDENTIFICAR ESP8266
    // ==================================================

    String id = "ESP8266_" + String(ESP.getChipId(), HEX);

    id.toUpperCase();

    Serial.print("Device ID: ");

    Serial.println(id);
    // ==================================================
    //  iniciar pin de alimentación del DHT22
    //===================================================

    pinMode(PIN_ALIMENTACION, OUTPUT);

    digitalWrite(PIN_ALIMENTACION, HIGH);

    delay(2000); // Esperar 2 segundos para que el DHT22 se estabilice

    // ==================================================
    // INICIAR DHT22
    // ==================================================

    iniciarDHT();

    // ==================================================
    // INICIAR OLED
    // ==================================================

    iniciarOLED();

    // ==================================================
    // INICIAR LITTLEFS
    // ==================================================

    if (!LittleFS.begin())
    {
        Serial.println(
            "Error al iniciar LittleFS");

        return;
    }

    Serial.println(
        "LittleFS iniciado correctamente");

    // ==================================================
    // COMPROBAR CREDENCIALES
    // ==================================================

    if (hayCredenciales())
    {
        Serial.println(
            "Credenciales encontradas");

        String ssid;
        String password;

        if (leerCredenciales(
                ssid,
                password))
        {
            Serial.print(
                "Intentando conectar a: ");

            Serial.println(
                ssid);

            if (conectarWifi(
                    ssid,
                    password))
            {
                Serial.println(
                    "Conexión automática correcta");

                // ------------------------------------------
                // MDNS
                // ------------------------------------------

                iniciarMDNS();

                // ------------------------------------------
                // MQTT
                // ------------------------------------------

                String id =
                    "ESP8266_" +
                    String(
                        ESP.getChipId(),
                        HEX);

                id.toUpperCase();

                Serial.print(
                    "Device ID: ");

                Serial.println(
                    id);

                iniciarMQTT(id);
            }
            else
            {
                Serial.println(
                    "No se pudo conectar");

                iniciarPuntoAcceso();
            }
        }
        else
        {
            Serial.println(
                "Error leyendo credenciales");

            iniciarPuntoAcceso();
        }
    }
    else
    {
        Serial.println(
            "No existen credenciales");

        iniciarPuntoAcceso();
    }

    // ==================================================
    // MOSTRAR ESTADO WIFI EN OLED
    // ==================================================

    if (WiFi.status() == WL_CONNECTED)
    {
        String ssid =
            WiFi.SSID();

        String ip =
            WiFi.localIP().toString();

        int rssi =
            WiFi.RSSI();

        mostrarWiFi(
            true,
            ssid.c_str(),
            ip.c_str(),
            rssi);
    }
    else
    {
        String ip =
            WiFi.softAPIP().toString();

        mostrarWiFi(
            false,
            "",
            ip.c_str(),
            0);
    }

    // ==================================================
    // INICIAR SERVIDOR
    // ==================================================

    iniciarServidor();
}

// ==================================================
// LOOP
// ==================================================

void loop()
{
    // Serial.println("LOOP OK");

    // ==================================================
    // SERVIDOR WEB
    // ==================================================

    atenderServidor();

    // ==================================================
    // MANTENER MQTT
    // ==================================================

    mantenerMQTT();

    // ==================================================
    // TIEMPO ACTUAL
    // ==================================================

    unsigned long ahora =
        millis();

    // ==================================================
    // LEER DHT22 CADA 2,5 SEGUNDOS
    // ==================================================

    if (
        ahora - tiempoDHT >=
        intervaloDHT)
    {
        tiempoDHT =
            ahora;

        errorDHT22 = actualizarDHT();
    }

    if(errorDHT22 == false)
    {
        contadorErroresDHT22++;

        if(contadorErroresDHT22 >= 5)
        {
            Serial.println("Error DHT22: reiniciando DHT22");

            resetearDHT();
        }
    }

    // ==================================================
    // PUBLICAR MQTT CADA 5 SEGUNDOS
    // ==================================================

    if (
        ahora - tiempoMQTT >=
        intervaloMQTT)
    {
        tiempoMQTT =
            ahora;

        // ------------------------------------------
        // TEMPERATURA
        // ------------------------------------------

        if (!isnan(temperaturaActual))
        {
            publicarTemperatura(temperaturaActual);
        }

        // ------------------------------------------
        // HUMEDAD
        // ------------------------------------------

        if (!isnan(humedadActual))
        {
            publicarHumedad(humedadActual);
        }
    }

    // ==================================================
    // ACTUALIZAR OLED CADA 2 SEGUNDOS
    // ==================================================

    if (ahora - tiempoOLED >= intervaloOLED)
    {
        tiempoOLED = ahora;
        actualizarOLED();
    }
}