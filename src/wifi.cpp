#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "wifi.h"

namespace {
String ssidGuardado;
String passwordGuardada;
unsigned long ultimoIntentoReconexion = 0;
const unsigned long intervaloReconexion = 10000;
}

void iniciarPuntoAcceso() {
  const char *ssid = "ESP8266_AP";
  const char *password = "12345678";

  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(ssid, password);

  Serial.println("Punto de acceso iniciado");

  Serial.print("IP del AP: ");
  Serial.println(WiFi.softAPIP());
}

void escanearRedes() {
  Serial.println("Escaneando redes WiFi...");

  int numeroRedes = WiFi.scanNetworks();

  Serial.print("Redes encontradas: ");
  Serial.println(numeroRedes);

  for (int i = 0; i < numeroRedes; i++) {
    Serial.print(i + 1);
    Serial.print(": ");

    Serial.print(WiFi.SSID(i));

    Serial.print(" RSSI: ");

    Serial.println(WiFi.RSSI(i));
  }
}

bool conectarWifi(String ssid, String password) {
  Serial.println();
  Serial.println("Intentando conectar...");

  Serial.print("SSID: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid.c_str(), password.c_str());

  int intentos = 0;

  while (!conexionWifiValida() && intentos < 20) {
    delay(500);

    Serial.print(".");

    intentos++;
  }

  Serial.println();

  if (conexionWifiValida()) {
    ssidGuardado = ssid;
    passwordGuardada = password;
    ultimoIntentoReconexion = millis();

    Serial.println("Conectado correctamente");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    return true;
  }

  ultimoIntentoReconexion = millis();
  Serial.println("No se pudo conectar");

  return false;
}

void recordarCredencialesWifi(const String &ssid, const String &password) {
  if (ssid.isEmpty()) {
    return;
  }

  ssidGuardado = ssid;
  passwordGuardada = password;
}

bool conexionWifiValida() {
  return WiFi.status() == WL_CONNECTED &&
         WiFi.localIP() != IPAddress(0, 0, 0, 0);
}

void mantenerConexionWifi() {
  if (conexionWifiValida() || ssidGuardado.isEmpty()) {
    return;
  }

  unsigned long ahora = millis();
  if (ahora - ultimoIntentoReconexion < intervaloReconexion) {
    return;
  }

  ultimoIntentoReconexion = ahora;

  Serial.println("WiFi desconectado o sin IP; reintentando conexion...");
  WiFi.begin(ssidGuardado.c_str(), passwordGuardada.c_str());
}