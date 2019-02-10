// Copyright (c) 2019 Jidesh Veeramachaneni
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).

#include "PMS7003-Particle-Sensor-Serial.h"

PMS7003Serial<USARTSerial> pms7003(Serial1, D0);

void setup() {
  Serial.begin();
}

unsigned long last = 0;
unsigned long last_pm_reading = 0;

void loop() {
  unsigned long now = millis();

  // check every time to see if there is data
  if (pms7003.Read()) {
    last_pm_reading = now;
  }

  if ((now - last) > 10000) {
    // Let us be generous. Active state the device
    // reports at least every 2.3 seconds.
    if ((now - last_pm_reading) > 10000) {
      Serial.println("No reading for at least 10 seconds!");
    } else {
      Serial.println("pm1:" + String(pms7003.GetData(pms7003.pm1_0)));
      Serial.println("pm2_5:" + String(pms7003.GetData(pms7003.pm2_5)));
      Serial.println("pm10:" + String(pms7003.GetData(pms7003.pm10)));
      Serial.println("um_3:" + String(pms7003.GetData(pms7003.count0_3um)));
      Serial.println("um_5:" + String(pms7003.GetData(pms7003.count0_5um)));
      Serial.println("um1:" + String(pms7003.GetData(pms7003.count1um)));
      Serial.println("um2_5:" + String(pms7003.GetData(pms7003.count2_5um)));
      Serial.println("um5:" + String(pms7003.GetData(pms7003.count5um)));
      Serial.println("um10:" + String(pms7003.GetData(pms7003.count10um)));
    }
    last = now;
  }
}
