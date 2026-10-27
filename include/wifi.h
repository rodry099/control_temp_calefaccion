#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>

void iniciarPuntoAcceso();
void escanearRedes();
bool conectarWifi(String ssid, String password);
void recordarCredencialesWifi(const String &ssid, const String &password);
bool conexionWifiValida();
void mantenerConexionWifi();

#endif