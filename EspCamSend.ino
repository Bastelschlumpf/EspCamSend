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
  * @file EspCamSend.ino
  *
  * Main module with setup() and loop()
  */

#include "Battery.h"
#include "Camera.h"
#include "Config.h"
#include "ConfigOverride.h" // uncomment this
#include "DeepSleep.h"
#include "SendTo.h"
#include "Led.h"
#include "bmm8563.h"
#include "WiFi.h"


#define DEEP_SLEEP_TIME (60 * 60) // Deep sleep: 1 hour
// #define DEEP_SLEEP_TIME (10 * 60) // 10 minutes

/** Helper function to set the RTC clock. */
void SetClock()
{
   rtc_date_t date;
   date.year   = 2020;
   date.month  = 6;
   date.day    = 3;
   date.hour   = 10;
   date.minute = 22;
   date.second = 00;
   bmm8563_setTime(&date);
}

/** 
 *  Main function to get the greyscale imgage, analyse and compare it with the previous
 *  and if it is different and not at night take a high resolution image and send it
 *  with additional information to the raspberry server.
 */
void CaptureCompareAndSend()
{
   StartCameraGreyScale();
   if (CaptureGreyScaleImage()) {
      ReadOldIFrameFromNVS();
      int frameDiff = GetFrameDiff();
      int frameAvg  = GetFrameAvg();
      int frameSum  = GetFrameSum();

      // Only when changed and not at night
      if (frameDiff <= 100) { // nothing changed
         IncrementEqualCount();
      } else if (frameAvg >= 5) { // only at daylight
         float voltage  = 0.0;
         int   capacity = 0;

         WriteNewFrameToNVS();
         StopCamera();
         StartCameraJpg();
         if (StartWiFi()) {
            int rssi = GetWifiRssi();
            
            if (ReadBatterie(voltage, capacity)) {
               SendInfo(voltage, capacity, frameDiff, frameAvg, frameSum, rssi);
            }
            if (CaptureJpgImage()) {
               SendImage();
               ReleaseJpgImage();
            }
            StopWiFi();
         }
      }
   }
   StopCamera();
}

/** setup() function. Starts everything and go to deep sleep. */
void setup()
{
   Serial.begin(115200);
   
   NVS.begin();  
   // NVS.eraseAll();
   InitBattery();
   bmm8563_init();
   bmm8563_setTimerIRQ(DEEP_SLEEP_TIME);
   pinMode(CAMERA_LED_GPIO, OUTPUT);
   digitalWrite(CAMERA_LED_GPIO, HIGH);

   //SetClock();
   IncrementWakeupCount();
   CaptureCompareAndSend();
   IncrementAllActiveTime();
   NVS.close();
   StopBatteryOutput();
   DeepSleep(DEEP_SLEEP_TIME);
}

/** 
 *  loop() function. 
 *  Should never reached
 *  This is only for checking
 */
void loop()
{
   if (StartWiFi()) {
      SendInfo(0.0, 0, 0, 0, 0, 0);
      StopWiFi();
   }
   delay(30000);
}
