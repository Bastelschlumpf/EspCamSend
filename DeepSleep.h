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
  * @file DeepSleep.h
  *
  * Deep sleep handling.
  */

#pragma once


/** 
 *  Starts the deep sleep mode. 
 *  We set both, the timer irq and the esp32 wakeup.
 *  safe is safe.
 */
void DeepSleep(int sec)
{
   Serial.println("DeepSleep");
   delay(100);

   // workaround when on usb
   esp_sleep_enable_timer_wakeup(sec * 1000000);
   // go to deep sleep
   esp_deep_sleep_start();
}
