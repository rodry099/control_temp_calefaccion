#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "wifi.h"
#include "webserver.h"
#include "config.h"

ESP8266WebServer servidor(80);


// ------------------------------------------------
// Determina el tipo de archivo
// ------------------------------------------------

String obtenerTipoContenido(String nombre)
{
    if (nombre.endsWith(".html"))
        return "text/html";

    if (nombre.endsWith(".css"))
        return "text/css";

    if (nombre.endsWith(".js"))
        return "application/javascript";

    if (nombre.endsWith(".png"))
        return "image/png";

    if (nombre.endsWith(".jpg"))
        return "image/jpeg";

    if (nombre.endsWith(".ico"))
        return "image/x-icon";

    return "text/plain";
}

//--------------------------------------------
// envia redes encontradas a pagina web
//---------------------------------------------

void enviarRedes()
{
    Serial.println();
    Serial.println("Escaneando redes WiFi...");

    int numeroRedes = WiFi.scanNetworks();

    Serial.print("Redes encontradas: ");
    Serial.println(numeroRedes);

    String json = "[";

    for (int i = 0; i < numeroRedes; i++)
    {
        if (i > 0)
        {
            json += ",";
        }

        json += "{";
        json += "\"ssid\":\"";
        json += WiFi.SSID(i);
        json += "\",";
        json += "\"rssi\":";
        json += WiFi.RSSI(i);
        json += "}";
    }

    json += "]";

    Serial.print("JSON: ");
    Serial.println(json);

    servidor.send(200,"application/json",json);

    WiFi.scanDelete();
}
/*---------------------------------------------
    envia estado de la coneccion a pagina web
-----------------------------------------------*/

void enviarEstado()
{
    String json = "{";

    if (WiFi.status() == WL_CONNECTED)
    {
        json += "\"conectado\":true,";

        json += "\"ssid\":\"";
        json += WiFi.SSID();
        json += "\",";

        json += "\"ip\":\"";
        json += WiFi.localIP().toString();
        json += "\",";

        json += "\"rssi\":";
        json += WiFi.RSSI();
    }
    else
    {
        json += "\"conectado\":false,";

        json += "\"ssid\":\"\",";

        json += "\"ip\":\"";
        json += WiFi.softAPIP().toString();
        json += "\",";

        json += "\"rssi\":0";
    }

    json += "}";

    servidor.send(200,"application/json",json);
}

// ------------------------------------------------
// Servir un archivo de LittleFS
// ------------------------------------------------

bool servirArchivo(String ruta)
{
    Serial.print("Solicitado: ");
    Serial.println(ruta);

    if (!LittleFS.exists(ruta))
    {
        Serial.println("Archivo no encontrado");
        return false;
    }

    File archivo = LittleFS.open(ruta, "r");

    if (!archivo)
    {
        Serial.println("Error al abrir archivo");
        return false;
    }

    String tipo = obtenerTipoContenido(ruta);

    servidor.streamFile(archivo, tipo);

    archivo.close();

    return true;
}


// ------------------------------------------------
// Página no encontrada
// ------------------------------------------------

void manejarNoEncontrado()
{
    Serial.print("404: ");
    Serial.println(servidor.uri());

    if (!servirArchivo(servidor.uri()))
    {
        servidor.send(
            404,
            "text/plain",
            "Archivo no encontrado"
        );
    }
}

void configurarWiFi()
{
    String ssid = servidor.arg("ssid");
    String password = servidor.arg("password");

    Serial.println();
    Serial.println("Datos recibidos desde el navegador");

    Serial.print("SSID: ");
    Serial.println(ssid);

    bool conectado = conectarWifi(ssid, password);

    if (conectado)
    {
        guardarCredenciales(ssid, password);

        String json = "{";

        json += "\"exito\":true,";
        json += "\"ip\":\"";
        json += WiFi.localIP().toString();
        json += "\"";

        json += "}";

        servidor.send(
            200,
            "application/json",
            json
        );
    }
    else
    {
        String json = "{";

        json += "\"exito\":false";

        json += "}";

        servidor.send(
            200,
            "application/json",
            json
        );
    }
}

// ------------------------------------------------
// Iniciar servidor
// ------------------------------------------------

void iniciarServidor()
{
   
    servidor.on("/", []()
    {
        servirArchivo("/index.html");
    });

    servidor.on("/redes", HTTP_GET, enviarRedes);
    servidor.on("/estado", HTTP_GET, enviarEstado);
    servidor.on("/configurar", HTTP_POST, configurarWiFi);
    
    servidor.onNotFound(manejarNoEncontrado);

    servidor.begin();

    Serial.println("Servidor web iniciado");
}


// ------------------------------------------------
// Atender servidor
// ------------------------------------------------

void atenderServidor()
{
    servidor.handleClient();
}