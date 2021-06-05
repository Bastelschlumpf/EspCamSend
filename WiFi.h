#pragma once
#include <WiFi.h>

/* Get rssi value as int value between 0 and 100 % */
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

bool StopWiFi()
{
   Serial.println("StopWiFi");
   
   WiFi.disconnect();
   WiFi.mode(WIFI_OFF);
   return true;
}
