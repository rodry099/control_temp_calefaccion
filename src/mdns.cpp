#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>

#include "mdns.h"

void iniciarMDNS()
{
    if (MDNS.begin("esp8266"))
    {
        Serial.println("mDNS iniciado");
        Serial.println("ESP8266: esp8266.local");
    }
    else
    {
        Serial.println("Error iniciando mDNS");
    }


    IPAddress ip;

    int resultado = WiFi.hostByName("servidor-rasp.local",ip);


    if (resultado == 1)
    {
        Serial.print("Servidor encontrado: ");

        Serial.println(ip);
    }
    else
    {
        Serial.println("Servidor NO encontrado");
    }
}