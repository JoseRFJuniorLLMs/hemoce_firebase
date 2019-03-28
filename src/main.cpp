#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>
#include "Creds.h"

// comment this line if you want to use DHT11
#define TEST

#define DHTTYPE DHT11
#define DHTPIN D2

DHT dht(DHTPIN, DHTTYPE);

void connectToWiFi() {
    delay(10);
    Serial.println();
    Serial.println();
    Serial.print("Conectando...");
    Serial.print(SSID);
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
    would try to act as both a client and an access-point and could cause
    network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    //start connecting to WiFi
    WiFi.begin(SSID, PASSWORD);
    //while client is not connected to WiFi keep loading
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi conectado a ");
    Serial.println(SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
}

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    dht.begin();
}

void loop() {
    #ifdef TEST
        // === Push o valor da temperatura para o Firebase ===
        String tempValueID = Firebase.pushInt("dht11/temperatura", random(0, 80));
        Serial.print("[INFO] temperatura: ");
        Serial.println(random(0, 80));
        if (Firebase.failed()) {
            Serial.print("[ERROR] pushing /dht11/temperatura failed:");
            Serial.println(Firebase.error());
            return;
        }
        Serial.print("[INFO] pushed: /dht11/temperatura \tkey: ");
        Serial.println(tempValueID);
        
        // === Push valor de umidade para o Firebase ===
        String humValueID = Firebase.pushInt("dht11/humidade", random(0, 80));
        Serial.print("[INFO] humidade: ");
        Serial.println(random(0, 80));
        if (Firebase.failed()) {
            Serial.print("[ERROR] pushing /dht11/humidade failed:");
            Serial.println(Firebase.error());
            return;
        }
        Serial.print("[INFO] pushed: /dht11/humidade    \tkey: ");
        Serial.println(humValueID);
        Serial.println();
    #else
        // === Ler e registrar temperatura e umidade no Monitor Serial ===
        float h = dht.readhumidade();
        float t = dht.readtemperatura();
        Serial.print("[INFO] humidade atual = ");
        Serial.print(h);
        Serial.println(" %");
        Serial.print("[INFO] temperatura atual = ");
        Serial.print(t);
        Serial.println(" °C");
        
        // Elimine valores anormais
        if ((t >= -15 && t <= 80) && (h >= 0 && h <= 100)) {
            // === Push temperatura valor para Firebase ===
            String tempValueID = Firebase.pushFloat("dht11/temperatura", t);
            if (Firebase.failed()) {
                Serial.print("[ERROR] pushing /dht11/temperatura faliu:");
                Serial.println(Firebase.error());
                return;
            }
            Serial.print("[INFO] pushed: /dht11/temperatura \tkey: ");
            Serial.println(tempValueID);

            // === Push humidade valor para Firebase ===
            String humValueID = Firebase.pushFloat("dht11/humidade", h);
            if (Firebase.failed()) {
                Serial.print("[ERROR] pushing /dht11/humidade failed:");
                Serial.println(Firebase.error());
                return;
            }
            Serial.print("[INFO] pushed: /dht11/humidade    \tkey: ");
            Serial.println(humValueID);
            Serial.println();
        } else {
            Serial.println("[ERROR] nao enviado...!");
        }
    #endif

    delay(1000);
}
