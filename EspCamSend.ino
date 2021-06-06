
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

void CaptureCompareAndSend()
{
   StartCameraGreyScale();
   if (CaptureGreyScaleImage()) {
      ReadOldIFrameFromNVS();
      int frameDiff = GetFrameDiff();
      int frameAvg  = GetFrameAvg();
      int frameSum  = GetFrameSum();

      // Only when changed and not at night
      /*
      if (frameDiff <= 100) { // nothing changed
         IncrementEqualCount();
      } else if (frameAvg >= 5) { // only at daylight
      */
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
      // }
   }
   StopCamera();
}

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

void loop()
{
   // Should never reached
   // only for checking
   if (StartWiFi()) {
      SendInfo(0.0, 0, 0, 0, 0, 0);
      StopWiFi();
   }
   delay(30000);
}
