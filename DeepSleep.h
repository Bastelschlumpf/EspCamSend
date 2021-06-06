#pragma once


void DeepSleep(int sec)
{
   Serial.println("DeepSleep");
   delay(100);

// workaround when on usb
   esp_sleep_enable_timer_wakeup(sec * 1000000);
   esp_deep_sleep_start();
}
