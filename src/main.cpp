/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v1.6.2  |
|==============================================================================|
|  Copyright (C) 2024-2025 Mr. Pegagan (agungjulianperkasa@gmail.com)         |
|  All rights reserved.                                                        |
|==============================================================================|
|  IEC 60870-5-104 Arduino Slave is free software: you can redistribute it     |
|  and/or modify it under the terms of the Lesser GNU General Public License   |
|  as published by the Free Software Foundation, either version 3 of the       |
|  License, or (at your option) any later version.                             |
|                                                                              |
|  This program is distributed in the hope that it will be useful,             |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of              |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                        |
|  See the Lesser GNU General Public License for more details.                 |
|==============================================================================*/

#include "IEC104Slave.h"
#include <esp_task_wdt.h> // untuk hardware reset penuh

IEC104Slave slave(&Serial1);             // Konstruktor pakai Stream*

void setup() {
  Serial.begin(115200); //debug
  Serial1.begin(9600);
  Wire.begin();
  #ifdef DEBUG
  Serial.println(F("GOES - IEC 60870-5-104 Slave v1.6.2"));
  #endif
  slave.begin();
  esp_task_wdt_init(8, true); // 8 detik timeout, true untuk mengaktifkan reset
  esp_task_wdt_add(NULL);     // Tambahkan task saat ini ke watchdog
}

void loop() {
  // digitalWrite(LED_BUILTIN, millis() % 1000 < 100);
  slave.run();
  esp_task_wdt_reset();
}
