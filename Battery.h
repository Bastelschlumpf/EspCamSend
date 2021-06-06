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
  * @file Battery.h
  *
  * Helper function for battery handling.
  */
#pragma once
#include <battery.h>


/** Initialize the battery on module */
void InitBattery()
{
   bat_init();
}

/** Read the battery value */
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

/** Disable the battery for saving energy */
void StopBatteryOutput()
{
   bat_disable_output();   
}
