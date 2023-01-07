/*
 *  e69_iot v0.0.0.4
 *  Copyright (C) 2023 Iuri Guilherme <https://iuri.neocities.org>
 * 
 *  Boards:  esp32dev, esp8266
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

#include "ArduinoJson.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// TODO add multiple SSID / password strings
#ifndef ssid
#define ssid "aifaidovizinho"
#define password  "temquefazerumquete"
#define version "0.0.0.4" // software version
#define plant "0" // UUID of this IOT
#endif
#define host "192.168.0.254"
#define port "8000"
#define serialPort 9600
#define soilMoisturePin A0  // ESP8266
//#define soilMoisturePin 4  // ESP32
#define led LED_BUILTIN
#define interval 6000
#define minMoisture 0
#define maxMoisture 1024
float epsilon = 0.01;
float lastMoisture = 0.0;

float readAnalog(int pin) {
  return analogRead(pin);
}

int readDigital(int pin) {
  return digitalRead(pin);
}

bool compareFloat(float f1, float f2, float e) {
  return (((f1 - f2) > e) or ((f2 - f1) > e));
}

void wifiConnect() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting on ");
  Serial.print(ssid);
  Serial.print(" with password ");
  Serial.print(password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(serialPort); // setup serial monitor
  while(!Serial);
  pinMode(led, OUTPUT); // setup led
  digitalWrite(led, HIGH); // led off esp8266
  pinMode(soilMoisturePin, INPUT);
  wifiConnect();
}

void loop() {
  delay(interval);
  digitalWrite(led, LOW); // esp8266 led on
  delay(1000);
  float nextMoisture = readAnalog(soilMoisturePin);
  if (compareFloat(nextMoisture, lastMoisture, epsilon)) {
    lastMoisture = nextMoisture;
    int percentMoisture = map(lastMoisture, minMoisture, maxMoisture, 0, 100);
    Serial.printf("Moisture: %f, change: %d%%\n", lastMoisture, percentMoisture);
    if ((WiFi.status() == WL_CONNECTED)) {
      WiFiClient client;
      HTTPClient http;
      DynamicJsonDocument payload(2048);
      payload["version"] = (String) version;
      payload["moisture"] = (float) lastMoisture;
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
  digitalWrite(led, HIGH); // esp8266 led off
}
