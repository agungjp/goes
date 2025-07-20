/*=============================================================================|
|  PROJECT GOES - IEC 60870-5-104 Arduino Slave                        v1.6.1  |
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
#include <avr/wdt.h>    // untuk hardware reset penuh

SoftwareSerial modem(10, 8);           // TX=10 → RX_modem, RX=8 ← TX_modem
IEC104Slave slave(&modem);             // Konstruktor pakai Stream*

void setup() {
  Serial.begin(115200); //debug
  modem.begin(9600);
  #ifdef DEBUG
  Serial.println(F("GOES - IEC 60870-5-104 Slave v1.6.1"));
  #endif
  slave.begin();
  wdt_enable(WDTO_8S);  
}

void loop() {
  // digitalWrite(LED_BUILTIN, millis() % 1000 < 100);
  slave.run();
  wdt_reset();
}