/*
 *  e69_iot v0.0.0.3
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

#define soilMoisturePin A0  // ESP8266
//#define pin 4  // ESP32
#define led LED_BUILTIN
int interval = 6000;
float epsilon = 0.01;
float lastMoisture = 0.0;
int minMoisture = 0;
int maxMoisture = 1024;

float readAnalog(int pin) {
  return analogRead(pin);
}

int readDigital(int pin) {
  return digitalRead(pin);
}

bool compareFloat(float f1, float f2, float e) {
  return (((f1 - f2) > e) or ((f2 - f1) > e));
}

void setup() {
  pinMode(led, OUTPUT); // setup led
  pinMode(soilMoisturePin, INPUT);
  digitalWrite(led, HIGH); // led off esp8266
  Serial.begin(9600); // setup serial monitor
}

void loop() {
  delay(interval);
  digitalWrite(led, LOW); // esp8266 led on
  delay(1000);
  float nextMoisture = readAnalog(soilMoisturePin);
  if (compareFloat(nextMoisture, lastMoisture, epsilon)) {
    lastMoisture = nextMoisture;
    Serial.printf("Moisture: %f, change: %d%%\n", lastMoisture,
    map(lastMoisture, minMoisture, maxMoisture, 0, 100));
  }
  delay(1);
  digitalWrite(led, HIGH); // esp8266 led off
}
