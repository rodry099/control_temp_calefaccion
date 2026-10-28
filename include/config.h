#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

bool guardarCredenciales(String ssid, String password);
bool leerCredenciales(String &ssid, String &password);
bool hayCredenciales();

#endif