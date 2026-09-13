#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "mqtt.h"

// ==========================================
// CONFIGURACIÓN MQTT
// ==========================================

#define MQTT_SERVER "servidor-rasp.local"
#define MQTT_PORT 1883

// ==========================================
// OBJETOS
// ==========================================

WiFiClient espClient;

PubSubClient mqttClient(espClient);

// ==========================================
// VARIABLES
// ==========================================

String deviceID;

String topicTemperatura;
String topicHumedad;
String topicStatus;

// ==========================================
// INICIAR MQTT
// ==========================================

void iniciarMQTT(String id)
{
    // Guardar identificador
    deviceID = id;

    // Crear topics
    topicTemperatura = "esp8266/" + deviceID + "/temperatura";
    topicHumedad = "esp8266/" + deviceID + "/humedad";
    topicStatus = "esp8266/" + deviceID + "/status";


    // Comprobar IP del servidor MQTT
    IPAddress ip;

    if (WiFi.hostByName(MQTT_SERVER, ip))
    {
        Serial.print("MQTT server IP: ");
        Serial.println(ip);
    }
    else
    {
        Serial.println("No se pudo resolver MQTT_SERVER");
    }


    // Configurar servidor MQTT
    mqttClient.setServer(MQTT_SERVER,MQTT_PORT);


    Serial.println("MQTT configurado");

    Serial.print("Device ID: ");
    Serial.println(deviceID);

    Serial.print("Topic temperatura: ");
    Serial.println(topicTemperatura);

    Serial.print("Topic humedad: ");
    Serial.println(topicHumedad);

    Serial.print("Topic status: ");
    Serial.println(topicStatus);

}

// ==========================================
// MANTENER CONEXIÓN MQTT
// ==========================================

void mantenerMQTT()
{
if (!mqttClient.connected())
{
    Serial.println("Conectando a MQTT...");


    // Mantenemos el Client ID que ya sabemos
    // que funciona correctamente.
    if (mqttClient.connect("ESP8266"))
    {
        Serial.println("MQTT conectado");


        // Avisar de que el dispositivo está online
        bool resultado =mqttClient.publish(topicStatus.c_str(),"online");


        Serial.print("Publicación status: ");

        Serial.println(resultado);
    }
    else
    {
        Serial.print("Error MQTT: ");

        Serial.println(mqttClient.state());
    }
}


mqttClient.loop();


}

// ==========================================
// PUBLICAR TEMPERATURA
// ==========================================

void publicarTemperatura(float temperatura)
{
    if (!mqttClient.connected())
    {
        Serial.println("MQTT no conectado");
        return;
    }

    char mensaje[20];


    snprintf(mensaje,sizeof(mensaje),"%.1f",temperatura);


    bool resultado = mqttClient.publish(topicTemperatura.c_str(),mensaje);


    if (resultado)
    {
        Serial.print("Temperatura MQTT: ");

        Serial.println(mensaje);
    }
    else
    {
        Serial.println("Error publicando temperatura");
    }


}

// ==========================================
// PUBLICAR HUMEDAD
// ==========================================

void publicarHumedad(float humedad)
{
    if (!mqttClient.connected())
    {
    Serial.println("MQTT no conectado");
    return;
    }

char mensaje[20];

snprintf(mensaje,sizeof(mensaje),"%.1f",humedad);

bool resultado = mqttClient.publish(topicHumedad.c_str(),mensaje);


if (resultado)
    {
        Serial.print("Humedad MQTT: ");

        Serial.println(mensaje);
    }
    else
    {
        Serial.println("Error publicando humedad");
    }


}
