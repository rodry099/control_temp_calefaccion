#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>

#include "icon_wifi.h"
#include "oled.h"

#define ANCHO_OLED 128
#define ALTO_OLED 64

#define OLED_RESET -1

// ==================================================
// OBJETO OLED
// ==================================================

Adafruit_SH1106G display(ANCHO_OLED, ALTO_OLED, &Wire, OLED_RESET);

/*
    MOSTRAR PANTALLA DE DATOS EN OLED
 */
void iniciarOLED() {
  if (!display.begin(0x3D, true)) {
    Serial.println("Error iniciando OLED");

    return;
  }

  display.clearDisplay();

  display.drawBitmap(48,     // X
                     16,     // Y
                     wifi_0, // Bitmap
                     16,     // Ancho
                     16,     // Alto
                     SH110X_WHITE);

  display.display();
}

/*=================================================*/

// ==================================================
// CONFIGURACIÓN OLED
// ==================================================

int obtenerNivelWiFi(int rssi) {
  if (rssi >= -50)
    return 4;

  if (rssi >= -60)
    return 3;

  if (rssi >= -70)
    return 2;

  if (rssi >= -80)
    return 1;

  return 0;
}

void mostrarIconoWiFi(int nivel) {
  const unsigned char *icono = nullptr;

  switch (nivel) {
  case 1:
    icono = wifi_1;
    break;

  case 2:
    icono = wifi_2;
    break;

  case 3:
    icono = wifi_3;
    break;

  case 4:
    icono = wifi_4;
    break;

  default:
    return;
  }

  display.drawBitmap(96, 0, icono, 16, 16, SH110X_WHITE);
}

// ==================================================
// MOSTRAR TEXTO
// ==================================================

void mostrarTexto(const char *texto) {
  display.clearDisplay();

  display.setTextSize(1);

  display.setTextColor(SH110X_WHITE);

  display.setCursor(0, 0);

  display.println(texto);

  display.display();
}

// ==================================================
// MOSTRAR INFORMACIÓN WIFI
// ==================================================

void mostrarWiFi(bool conectado, const char *ssid, const char *ip, int rssi) {
  display.clearDisplay();

  display.setTextSize(1);

  display.setTextColor(SH110X_WHITE);

  display.setCursor(0, 0);

  if (conectado) {
    display.setCursor(0, 0);

    display.print("WiFi: ");

    if (conectado) {
      display.print("OK");
    } else {
      display.print("NO");
    }

    display.setCursor(0, 16);

    display.print("Red: ");
    display.println(ssid);

    display.print("IP: ");
    display.println(ip);

    display.print("RSSI: ");
    display.print(rssi);
    display.println(" dBm");

    // Obtener nivel 0-4

    int nivel = obtenerNivelWiFi(rssi);

    // Dibujar icono

    mostrarIconoWiFi(nivel);
  } else {
    display.println("WiFi: NO CONECTADO");

    display.println();

    display.println("Punto de acceso");

    display.print("IP: ");
    display.println(ip);
  }

  display.display();
}

void mostrarDatos(float temperatura, float humedad) {
  display.clearDisplay();

  display.setTextSize(1);

  display.setTextColor(SH110X_WHITE);

  display.setCursor(0, 0);

  display.println("DATOS DHT11");

  display.setCursor(0, 20);

  display.print("Temp: ");

  display.print(temperatura, 1);

  display.println(" C");

  display.setCursor(0, 35);

  display.print("Hum:  ");

  display.print(humedad, 1);

  display.println(" %");

  display.display();
}

void actualizarPantalla(bool conectado, const char *ssid, const char *ip,
                        int rssi, float temperatura, float humedad) {
  display.clearDisplay();

  display.setTextSize(1);

  display.setTextColor(SH110X_WHITE);

  // ==========================================
  // ESTADO
  // ==========================================

  display.setCursor(0, 0);

  if (conectado) {
    display.print("WiFi: OK");

    int nivel = obtenerNivelWiFi(rssi);

    mostrarIconoWiFi(nivel);
  } else {
    display.print("MODO: AP");
  }

  // ==========================================
  // RED
  // ==========================================

  display.setCursor(0, 16);

  display.print("Red: ");

  display.println(ssid);

  // ==========================================
  // IP
  // ==========================================

  display.setCursor(0, 27);

  display.print("IP: ");

  display.println(ip);

  // ==========================================
  // TEMPERATURA
  // ==========================================

  display.setCursor(0, 41);

  display.print("Temp: ");

  display.print(temperatura, 1);

  display.println(" C");

  // ==========================================
  // HUMEDAD
  // ==========================================

  display.setCursor(0, 53);

  display.print("Hum: ");

  display.print(humedad, 1);

  display.println(" %");

  display.display();
}