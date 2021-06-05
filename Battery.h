#pragma once
#include <battery.h>


void InitBattery()
{
   bat_init();
}

bool ReadBatterie(float &volt, int &capacity)
{
   volt = bat_get_voltage() / 1000.0;

   if (volt < 3.3) {
      volt = 3.3;
   } else if (volt > 4.3) {
      volt = 4.3;
   }
  
   float battery = (float) (volt - 3.3) / (float) (4.3 - 3.3);
   
   if (battery <= 0.01) {
      battery = 0.01;
   }
   if (battery > 1) {
      battery = 1;
   }
   capacity = (int) (battery * 100);
   
   Serial.println("Battery: " + String(volt, 2) + "V (" + String(capacity) + "%)");
   return true;
}

void StopBatteryOutput()
{
   bat_disable_output();   
}
