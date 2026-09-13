#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "wifi.h"


void iniciarPuntoAcceso()
{
    const char* ssid = "ESP8266_AP";
    const char* password = "12345678";

    WiFi.mode(WIFI_AP_STA);

    WiFi.softAP(ssid, password);

    Serial.println("Punto de acceso iniciado");

    Serial.print("IP del AP: ");
    Serial.println(WiFi.softAPIP());
}


void escanearRedes()
{
    Serial.println("Escaneando redes WiFi...");

    int numeroRedes = WiFi.scanNetworks();

    Serial.print("Redes encontradas: ");
    Serial.println(numeroRedes);

    for (int i = 0; i < numeroRedes; i++)
    {
        Serial.print(i + 1);
        Serial.print(": ");

        Serial.print(WiFi.SSID(i));

        Serial.print(" RSSI: ");

        Serial.println(WiFi.RSSI(i));
    }
}


bool conectarWifi(String ssid, String password)
{
    Serial.println();
    Serial.println("Intentando conectar...");

    Serial.print("SSID: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);

    WiFi.begin(
        ssid.c_str(),
        password.c_str()
    );

    int intentos = 0;

    while (
        WiFi.status() != WL_CONNECTED &&
        intentos < 20
    )
    {
        delay(500);

        Serial.print(".");

        intentos++;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Conectado correctamente");

        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        return true;
    }

    Serial.println("No se pudo conectar");

    return false;
}