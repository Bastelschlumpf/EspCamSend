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
  * @file SendTo.h
  *
  * Helper function to send the data to the raspberry server.
  */

#pragma once
#include <WiFi.h>
#include <WiFiClient.h> 


WiFiClient client;

/** Increment the amount of wakups */
void IncrementWakeupCount()
{
   int wakeupCount = NVS.getInt("wakeupCount");

   wakeupCount++;
   NVS.setInt("wakeupCount", wakeupCount);
}

/** Increment the amount of equal images */
void IncrementEqualCount()
{
   int equalCount = NVS.getInt("equalCount");

   equalCount++;
   NVS.setInt("equalCount", equalCount);
}

/** Returns the time in which the system was active over all deep sleep phases */
int64_t GetAllActiveTime()
{
   int64_t allActiveTime = NVS.getInt("activeTime");
   allActiveTime += millis();
   return allActiveTime;
}

/** Added the current active time to the overall time. */
void IncrementAllActiveTime()
{
   int64_t activeTime = GetAllActiveTime();
   
   NVS.setInt("activeTime", activeTime);
}

/** Send the image to the server. */
bool SendImage()
{
   bool ret = false;
   
   Serial.println("SendImage");

   if (!jpg_fb) {
      Serial.println("   -> Missing Image");
   } else {
      String getAll;
      String getBody;
      
      Serial.println("Connecting to server: " + (String) serverName);
      
      if (!client.connect(serverName, serverPort)) {
         getBody = "Connection to " + (String) serverName +  " failed.";
         Serial.println(getBody);
      } else {
         Serial.println("Connection successful!");    
         String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"EspCamSend.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
         String tail = "\r\n--RandomNerdTutorials--\r\n";
         
         uint32_t imageLen = jpg_fb->len;
         uint32_t extraLen = head.length() + tail.length();
         uint32_t totalLen = imageLen + extraLen;
         
         client.println("POST "  + (String) serverPath + " HTTP/1.1");
         client.println("Host: " + (String) serverName);
         client.println("Content-Length: " + String(totalLen));
         client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
         client.println();
         client.print(head);
         
         uint8_t *fbBuf = jpg_fb->buf;
         size_t   fbLen = jpg_fb->len;
         
         for (size_t n=0; n<fbLen; n=n+1024) {
            if (n+1024 < fbLen) {
               client.write(fbBuf, 1024);
               fbBuf += 1024;
            } else if (fbLen%1024>0) {
               size_t remainder = fbLen%1024;
               client.write(fbBuf, remainder);
            }
         }   
         client.print(tail);
         
         int     timoutTimer = 10000;
         long    startTimer  = millis();
         boolean state       = false;
      
         while ((startTimer + timoutTimer) > millis()) {
            Serial.print(".");
            delay(100);      
            while (client.available()) {
               char c = client.read();
               
               if (c == '\n') {
                  if (getAll.length()==0) { 
                     state=true; 
                  }
                  getAll = "";
               } else if (c != '\r') { 
                  getAll += String(c); 
               }
               if (state==true) { 
                  getBody += String(c); 
               }
               startTimer = millis();
            }
            if (getBody.length()>0) { 
               break; 
            }
         }
      }
      Serial.println();
      client.stop();
      Serial.println(getBody);
      ret = true;
   }
   return ret;
}

/** Send the calculated and environment data to the server. */
bool SendInfo(float voltage, int capacity, int frameDiff, int frameAvg, int frameSum, int rssi)
{
   Serial.println("SendInfo");

   String info;
   char filename[50];
   int  allActiveTimeSec = GetAllActiveTime() / 1000;
   int  idx              = NVS.getInt("infoIdx");
   int  wakeupCount      = NVS.getInt("wakeupCount");
   int  equalCount       = NVS.getInt("equalCount");
   int  sendCount        = NVS.getInt("sendCount");
   int  errorCount       = NVS.getInt("errorCount");

   sprintf(filename, "Info%02d.txt", idx);
   info += "WakeupCount: " + String(wakeupCount)      + "\r\n";
   info += "SendCount:   " + String(sendCount)        + "\r\n";
   info += "EqualCount:  " + String(equalCount)       + "\r\n";
   info += "ErrorCount:  " + String(errorCount)       + "\r\n";
   info += "OverallTime: " + String(allActiveTimeSec) + " sec\r\n";
   info += "CurrentTime: " + String(millis())         + " ms\r\n";
   info += "Battery:     " + String(voltage)          + "V " + "(" + String(capacity) + " %)\r\n";
   info += "Rssi:        " + String(rssi)             + " %\r\n";
   info += "FrameDiff:   " + String(frameDiff)        + "\r\n";
   info += "FrameAvg:    " + String(frameAvg)         + "\r\n";
   info += "FrameSum:    " + String(frameSum)         + "\r\n";
   
   if (idx++ > 20) {
      idx = 0;
   }
   NVS.setInt("infoIdx", idx);

   String getAll;
   String getBody;
   
   Serial.println("Connecting to server: " + (String) serverName);
   
   if (!client.connect(serverName, serverPort)) {
      errorCount++;
      getBody = "Connection to " + (String) serverName +  " failed.";
      Serial.println(getBody);
   } else {
      sendCount++;
      Serial.println("Connection successful!");    
      String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"EspCamSend.txt\"\r\nContent-Type: text/plain\r\n\r\n";
      String tail = "\r\n--RandomNerdTutorials--\r\n";
      
      uint32_t extraLen = head.length() + tail.length();
      uint32_t totalLen = info.length() + extraLen;
      
      client.println("POST "  + (String) serverPath + " HTTP/1.1");
      client.println("Host: " + (String) serverName);
      client.println("Content-Length: " + String(totalLen));
      client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
      client.println();
      client.print(head);
      
      client.write(info.c_str(), info.length());
      client.print(tail);

      int     timoutTimer = 10000;
      long    startTimer  = millis();
      boolean state       = false;
   
      while ((startTimer + timoutTimer) > millis()) {
         Serial.print(".");
         delay(100);      
         while (client.available()) {
            char c = client.read();
            
            if (c == '\n') {
               if (getAll.length()==0) { 
                  state=true; 
               }
               getAll = "";
            } else if (c != '\r') { 
               getAll += String(c); 
            }
            if (state==true) { 
               getBody += String(c); 
            }
            startTimer = millis();
         }
         if (getBody.length()>0) { 
            break; 
         }
      }
   }
   NVS.setInt("sendCount",  sendCount);
   NVS.setInt("errorCount", errorCount);
   Serial.println();
   client.stop();
   Serial.println(getBody);
   return true;
}
