/*
   Copyright (C) 2021 SFini

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  * @file WiFi.h
  *
  * Helper function for the wifi connection.
  */
#pragma once
#include <WiFi.h>

/** Get rssi value as int value between 0 and 100 % */
int GetWifiRssi()
{
   int quality = 0;

   if (WiFi.status() == WL_CONNECTED) {
      int rssi = WiFi.RSSI();

      if (rssi <= -100) {
         quality = 0;
      } else if (rssi >= -50) {
         quality = 100;
      } else {
         quality = 2 * (rssi + 100);
      }
   }
   return quality;
}

/** Start the wifi connection */
bool StartWiFi()
{
   Serial.println("StartWiFi");
  
   WiFi.begin(WIFI_SSID, WIFI_PW);
   for (int retry = 0; WiFi.status() != WL_CONNECTED && retry < 30; retry++) {
      delay(500);
      Serial.print(".");
   }
   Serial.println("");

   if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected");
      return true;
   } else {
      return false;
   }
}

/** Stop the wifi connection */
bool StopWiFi()
{
   Serial.println("StopWiFi");
   
   WiFi.disconnect();
   WiFi.mode(WIFI_OFF);
   return true;
}
