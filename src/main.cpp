#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>

#include "config.h"
#include "dht_sensor.h"
#include "mdns.h"
#include "mqtt.h"
#include "oled.h"
#include "webserver.h"
#include "wifi.h"

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

// ==================================================
// ACTUALIZAR OLED
// ==================================================

void actualizarOLED() {
  bool conectado = WiFi.status() == WL_CONNECTED;

  String ssid;
  String ip;
  int rssi = 0;

  // ------------------------------------------
  // DATOS WIFI
  // ------------------------------------------

  if (conectado) {
    ssid = WiFi.SSID();
    ip = WiFi.localIP().toString();
    rssi = WiFi.RSSI();
  } else {
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

  if (isnan(temperatura)) {
    temperatura = 0;
  }

  if (isnan(humedad)) {
    humedad = 0;
  }

  // ------------------------------------------
  // ACTUALIZAR OLED
  // ------------------------------------------

  actualizarPantalla(conectado, ssid.c_str(), ip.c_str(), rssi, temperatura,
                     humedad);
}

// ==================================================
// ACTUALIZAR DHT22
// ==================================================

void actualizarDHT() {
  float temperatura;
  float humedad;

  // ------------------------------------------
  // COMPROBAR LECTURA
  // ------------------------------------------

  if (!leerDHT(temperatura, humedad)) {
    Serial.println("Error leyendo DHT22");

    return;
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
}

// ==================================================
// SETUP
// ==================================================

void setup() {
  Serial.begin(9600);

  // ==================================================
  // IDENTIFICAR ESP8266
  // ==================================================

  String id = "ESP8266_" + String(ESP.getChipId(), HEX);

  id.toUpperCase();

  Serial.print("Device ID: ");

  Serial.println(id);

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

  if (!LittleFS.begin()) {
    Serial.println("Error al iniciar LittleFS");

    return;
  }

  Serial.println("LittleFS iniciado correctamente");

  // ==================================================
  // COMPROBAR CREDENCIALES
  // ==================================================

  if (hayCredenciales()) {
    Serial.println("Credenciales encontradas");

    String ssid;
    String password;

    if (leerCredenciales(ssid, password)) {
      Serial.print("Intentando conectar a: ");

      Serial.println(ssid);

      if (conectarWifi(ssid, password)) {
        Serial.println("Conexión automática correcta");

        // ------------------------------------------
        // MDNS
        // ------------------------------------------

        iniciarMDNS();

        // ------------------------------------------
        // MQTT
        // ------------------------------------------

        String id = "ESP8266_" + String(ESP.getChipId(), HEX);

        id.toUpperCase();

        Serial.print("Device ID: ");

        Serial.println(id);

        iniciarMQTT(id);
      } else {
        Serial.println("No se pudo conectar");

        iniciarPuntoAcceso();
      }
    } else {
      Serial.println("Error leyendo credenciales");

      iniciarPuntoAcceso();
    }
  } else {
    Serial.println("No existen credenciales");

    iniciarPuntoAcceso();
  }

  // ==================================================
  // MOSTRAR ESTADO WIFI EN OLED
  // ==================================================

  if (WiFi.status() == WL_CONNECTED) {
    String ssid = WiFi.SSID();

    String ip = WiFi.localIP().toString();

    int rssi = WiFi.RSSI();

    mostrarWiFi(true, ssid.c_str(), ip.c_str(), rssi);
  } else {
    String ip = WiFi.softAPIP().toString();

    mostrarWiFi(false, "", ip.c_str(), 0);
  }

  // ==================================================
  // INICIAR SERVIDOR
  // ==================================================

  iniciarServidor();
}

// ==================================================
// LOOP
// ==================================================

void loop() {
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

  unsigned long ahora = millis();

  // ==================================================
  // LEER DHT22 CADA 2,5 SEGUNDOS
  // ==================================================

  if (ahora - tiempoDHT >= intervaloDHT) {
    tiempoDHT = ahora;

    actualizarDHT();
  }

  // ==================================================
  // PUBLICAR MQTT CADA 5 SEGUNDOS
  // ==================================================

  if (ahora - tiempoMQTT >= intervaloMQTT) {
    tiempoMQTT = ahora;

    // ------------------------------------------
    // TEMPERATURA
    // ------------------------------------------

    if (!isnan(temperaturaActual)) {
      publicarTemperatura(temperaturaActual);
    }

    // ------------------------------------------
    // HUMEDAD
    // ------------------------------------------

    if (!isnan(humedadActual)) {
      publicarHumedad(humedadActual);
    }
  }

  // ==================================================
  // ACTUALIZAR OLED CADA 2 SEGUNDOS
  // ==================================================

  if (ahora - tiempoOLED >= intervaloOLED) {
    tiempoOLED = ahora;
    actualizarOLED();
  }
}