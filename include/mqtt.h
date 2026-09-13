#ifndef MQTT_H
#define MQTT_H

void iniciarMQTT(String deviceID);

void mantenerMQTT();

void publicarTemperatura(
float temperatura
);

void publicarHumedad(
float humedad
);

#endif
