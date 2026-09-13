#ifndef CONFIG_H
#define CONFIG_H

bool guardarCredenciales(String ssid, String password);
bool leerCredenciales(String &ssid, String &password);
bool hayCredenciales();

#endif