#ifndef OLED_H
#define OLED_H

void iniciarOLED();

void mostrarTexto(const char* texto);

void mostrarWiFi(
    bool conectado,
    const char* ssid,
    const char* ip,
    int rssi
);

void mostrarNivelWiFi(int rssi);

void mostrarIconoWiFi(int nivel);

void mostrarDatos(
    float temperatura,
    float humedad
);

void actualizarPantalla(
    bool conectado,
    const char* ssid,
    const char* ip,
    int rssi,
    float temperatura,
    float humedad
);

#endif