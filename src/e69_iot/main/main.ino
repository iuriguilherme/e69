/*
 *  e69_iot v0.0.0.7
 *  Copyright (C) 2023 Iuri Guilherme <https://iuri.neocities.org>
 * 
 *  Boards:  esp32, esp8266
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.  
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.  
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.  
*/

#include <ESP8266WiFi.h> // ESP8266
#include <ESP8266HTTPClient.h> // ESP8266
//~ #include <WiFi.h> // ESP32
//~ #include <HTTPClient.h> // ESP32
//~ #include <Wire.h> // ESP32
#include <Arduino.h> // ESP32
//~ #include <driver/adc.h> // ESP32
//~ #include <ESP32AnalogRead.h> // ESP32

#include "ArduinoJson.h"

#include "credentials.h" // WiFi and server information

// TODO add multiple SSID / password strings
#ifndef ssid
#define ssid "None"
#endif
#ifndef password
#define password  "None"
#endif
#define version "0.0.0.7" // software version
#define plant "0esp8266" // UUID of this IOT
#ifndef host
#define host "localhost"
#endif
#ifndef port
#define port "0"
#endif
#define serialPort 9600
#define soilMoisturePin A0 // ESP8266
//~ #define soilMoisturePin 34 // ESP32
#define led LED_BUILTIN // ESP8266
//~ #define led 2 // ESP32
#define liga LOW // ESP8266
#define desliga HIGH // ESP8266
//~ #define liga HIGH // ESP32
//~ #define desliga LOW // ESP32
#define interval 3000 // três segundos
#define minMoisture 0 // ESP8266
#define maxMoisture 1024 // ESP8266
//~ #define maxMoisture 4096 // ESP32
//~ float epsilon = 0.01;
int lastMoisture = maxMoisture;
int nextMoisture = minMoisture;

//~ bool compareFloat(float f1, float f2, float e) {
  //~ return (((f1 - f2) > e) or ((f2 - f1) > e));
//~ }

void wifiConnect() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting on ");
  //Serial.print(ssid);
  Serial.print(" with password ");
  //Serial.print(password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void blink(int limit) {
  for (int i = 0; i < limit; i++) {
    digitalWrite(led, liga);
    delay(300);
    digitalWrite(led, desliga);
    delay(150);
  }
}

void setup() {
  pinMode(led, OUTPUT); // setup led
  blink(3);
  digitalWrite(led, desliga);
  Serial.begin(serialPort); // setup serial monitor
  while(!Serial); // ESP8266
  pinMode(soilMoisturePin, INPUT); // ESP8266
  //~ adc.attach(soilMoisturePin); // ESP32
  //~ adc_power_on(); // ESP32
  //~ adc_gpio_init(ADC_UNIT_1, ADC_CHANNEL_6); // ESP32
  //~ adc1_config_width(ADC_WIDTH_12Bit); // ESP32
  //~ adc1_config_width(ADC_WIDHT_BIT_12);
  //~ adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_0db); // ESP32
  wifiConnect();
}

void loop() {
  delay(interval);
  digitalWrite(led, liga);
  delay(1);
  nextMoisture = analogRead(soilMoisturePin); // ESP8266
  //~ nextMoisture = adc1_get_raw(ADC1_CHANNEL_6); // ESP32
  //~ nextMoisture = adc.readVoltage(); // ESP32;
  Serial.printf("nextMoisture: %d\n", nextMoisture);
  if (nextMoisture < lastMoisture) {
    lastMoisture = nextMoisture;
    int percentMoisture = map(lastMoisture, maxMoisture, minMoisture, 0, 100);
    Serial.printf("lastMoisture: %d\n", lastMoisture);
    Serial.printf("minMoisture: %d\n", minMoisture);
    Serial.printf("maxMoisture: %d\n", maxMoisture);
    Serial.printf("change: %d%%\n", percentMoisture);
    if ((WiFi.status() == WL_CONNECTED)) {
      WiFiClient client;
      HTTPClient http;
      DynamicJsonDocument payload(2048);
      payload["version"] = (String) version;
      payload["percentMoisture"] = (int) percentMoisture;
      payload["nextMoisture"] = (int) nextMoisture;
      payload["lastMoisture"] = (int) lastMoisture;
      payload["minMoisture"] = (int) minMoisture;
      payload["maxMoisture"] = (int) maxMoisture;
      payload["plant"] = (String) plant;
      String json;
      json.reserve(2048);
      serializeJson(payload, json);
      http.begin(client, "http://" host ":" port "/debug/");
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(json);
      if (httpCode > 0) {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
          const String& payload = http.getString();
          Serial.println("received payload:\n<<");
          Serial.println(payload);
          Serial.println(">>");
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      wifiConnect();
    }
  }
  delay(1);
  digitalWrite(led, desliga);
}
