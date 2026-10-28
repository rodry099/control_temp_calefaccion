#include <Arduino.h>
#include <LittleFS.h>

#include "config.h"

bool guardarCredenciales(String ssid, String password)
{
    File archivo = LittleFS.open("/config.txt", "w");

    if (!archivo)
    {
        Serial.println("Error al abrir config.txt para escribir");
        return false;
    }

    archivo.println(ssid);
    archivo.println(password);

    archivo.close();

    Serial.println("Credenciales guardadas");

    return true;
}


bool leerCredenciales(String &ssid, String &password)
{
    File archivo = LittleFS.open("/config.txt", "r");

    if (!archivo)
    {
        Serial.println("No existe config.txt");
        return false;
    }

    ssid = archivo.readStringUntil('\n');
    password = archivo.readStringUntil('\n');

    ssid.trim();
    password.trim();

    archivo.close();

    return true;
}


bool hayCredenciales()
{
    return LittleFS.exists("/config.txt");
}