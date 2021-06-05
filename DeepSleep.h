#pragma once


void DeepSleep()
{
   Serial.println("DeepSleep");
   delay(100);

// workaround when on usb
// esp_sleep_enable_timer_wakeup(10 * 1000000);
   esp_deep_sleep_start();
}
