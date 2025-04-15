/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v1.4.4  |
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

#include <SoftwareSerial.h>
#include "IEC104Slave.h"

SoftwareSerial modem(10, 8);           // TX, RX ke modem
IEC104Slave slave(&modem);             // Konstruktor pakai Stream*

void setup() {
  Serial.begin(9600);
  modem.begin(9600);
  Serial.println(F("GOES - IEC 60870-5-104 Slave v1.4.4"));
  slave.begin();
}

void loop() {
  slave.run();  // renamed from loop()
}