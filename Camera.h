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
  * @file Camera.h
  *
  * Helper function for camera handling.
  */
#pragma once
#include "ArduinoNvs.h"
#include "esp_camera.h"
#include "camera_pins.h"

// FRAMESIZE_QVGA
#define WIDTH      320
#define HEIGHT     240
#define BLOCK_SIZE  40
#define W          (WIDTH  / BLOCK_SIZE)
#define H          (HEIGHT / BLOCK_SIZE) 

float old_frame[H][W]     = { 0.0 }; 
float current_frame[H][W] = { 0.0 }; 

int frameDiff = 0;

camera_fb_t *jpg_fb = NULL;

camera_config_t camera_config;


/** Set the m5stack camery settings */
void SetCameraConfig(camera_config_t &config, pixformat_t pixel_format, framesize_t frame_size)
{
   Serial.println("SetCameraConfig");
   
   config.ledc_channel = LEDC_CHANNEL_0;
   config.ledc_timer   = LEDC_TIMER_0;
   config.pin_d0       = Y2_GPIO_NUM;
   config.pin_d1       = Y3_GPIO_NUM;
   config.pin_d2       = Y4_GPIO_NUM;
   config.pin_d3       = Y5_GPIO_NUM;
   config.pin_d4       = Y6_GPIO_NUM;
   config.pin_d5       = Y7_GPIO_NUM;
   config.pin_d6       = Y8_GPIO_NUM;
   config.pin_d7       = Y9_GPIO_NUM;
   config.pin_xclk     = XCLK_GPIO_NUM;
   config.pin_pclk     = PCLK_GPIO_NUM;
   config.pin_vsync    = VSYNC_GPIO_NUM;
   config.pin_href     = HREF_GPIO_NUM;
   config.pin_sscb_sda = SIOD_GPIO_NUM;
   config.pin_sscb_scl = SIOC_GPIO_NUM;
   config.pin_pwdn     = PWDN_GPIO_NUM;
   config.pin_reset    = RESET_GPIO_NUM;
   config.xclk_freq_hz = 20000000;
   config.pixel_format = pixel_format;
   config.frame_size   = frame_size;
   config.jpeg_quality = 10;
   config.fb_count     = 1;
}

/** Flip the camera image */
void FlipCamera(bool brighter)
{
   sensor_t *s = esp_camera_sensor_get();
   //initial sensors are flipped vertically and colors are a bit saturated
   if (s) {
      s->set_vflip(s, 1);          // flip it back
      if (brighter) {
         s->set_brightness(s, 1);  // up the brightness just a bit
         s->set_saturation(s, -2); // lower the saturation}
      }
   }
}

/** Setup greyscale camera image */
bool StartCameraGreyScale()
{
   Serial.println("StartCameraGreyScale");
   
   esp_camera_deinit();
   SetCameraConfig(camera_config, PIXFORMAT_GRAYSCALE, FRAMESIZE_QVGA);
   esp_err_t err = esp_camera_init(&camera_config);
   if (err == ESP_OK) {
      FlipCamera(false);
      return true;
   }
   Serial.println("   -> failed: " + String(err));
   return false;
}

/** Setup hight resolution color image */
bool StartCameraJpg()
{
   Serial.println("StartCameraJpg");
   
   esp_camera_deinit();
   SetCameraConfig(camera_config, PIXFORMAT_JPEG, FRAMESIZE_SXGA);
   esp_err_t err = esp_camera_init(&camera_config);
   if (err == ESP_OK) {
      FlipCamera(true);
      return true;
   }
   Serial.println("   -> failed: " + String(err));
   return false;
}

/** Trace the greyscale image analyse data */
void PrintFrame(float frame[H][W]) 
{
   Serial.println("PrintFrame");
   
   for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
         Serial.print(frame[y][x]);
         Serial.print('\t');
      }
      Serial.println();
   }
}

/** Capture the greyscale image */
bool CaptureGreyScaleImage()
{
   Serial.println("CaptureGreyScaleImage");
   
   camera_fb_t *frame_buffer = esp_camera_fb_get();
   
   if (!frame_buffer) {
      Serial.println("   -> error on esp_camera_fb_get!");
      return false;
   }

   memset(current_frame, 0, sizeof(current_frame));

   // down-sample image in blocks
   for (uint32_t i = 0; i < WIDTH * HEIGHT; i++) {
      const uint16_t x       = i % WIDTH;
      const uint16_t y       = floor(i / WIDTH);
      const uint8_t  block_x = floor(x / BLOCK_SIZE);
      const uint8_t  block_y = floor(y / BLOCK_SIZE);
      const uint8_t  pixel   = frame_buffer->buf[i];
      const uint16_t current = current_frame[block_y][block_x];

      current_frame[block_y][block_x] += pixel;
   }

   // average pixels in block (rescale)
   for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
         current_frame[y][x] /= BLOCK_SIZE * BLOCK_SIZE;
      }
   }

   PrintFrame(current_frame);

   return true;
}

/** Capture the color image */
bool CaptureJpgImage()
{
   Serial.println("CaptureJpgImage");

   if (jpg_fb) {
      esp_camera_fb_return(jpg_fb);
      jpg_fb = NULL;
   }
   jpg_fb = esp_camera_fb_get();
   
   if (!jpg_fb) {
      Serial.println("   -> error on esp_camera_fb_get!");
      return false;
   }
   return true;
}

/** Release the image buffer */
bool ReleaseJpgImage()
{
   Serial.println("ReleaseJpgImage");

   if (jpg_fb) {
      esp_camera_fb_return(jpg_fb);
      jpg_fb = NULL;
   }
   return true;
}

/** Get the previous analysed information from NVS */
bool ReadOldFrameFromNVS()
{
   Serial.println("ReadOldIFrameFromNVS");

   if (!NVS.getBlob("frame", (uint8_t *) old_frame, sizeof(old_frame))) {
      Serial.println("   -> Error on reading old froma from NVS!");
      return false;
   }
   return true;
}

/** Write the new analysed information to NVS */
bool WriteNewFrameToNVS()
{
   Serial.println("WriteNewFrameToNVS");
   
   if (!NVS.setBlob("frame", (uint8_t *) current_frame, sizeof(current_frame))) {
      Serial.println("   -> Error on saving new fram to NVS!");
      return false;
   }
   return true;
}

/** Calculate the frame difference */
float GetFrameDiff()
{
   Serial.println("GetFrameDiff");
   
   float ret = 0.0;
   
   for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
         ret += abs(current_frame[y][x] - old_frame[y][x]) ;
      }
   }
   Serial.println("   -> Diff: " + String(ret, 2));
   return ret;
}

/** Calculate the average pixel density */
float GetFrameAvg()
{
   Serial.println("GetCurrentFrameSum");
   
   float sum   = 0.0;
   int   count = 0;
   float ret   = 0.0;
   
   for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
         sum += current_frame[y][x];
         count++;
      }
   }
   ret = sum / (float) count;
   Serial.println("   -> Avg: " + String(ret, 2));
   return ret;
}

/** Calculate the pixel summery */
float GetFrameSum()
{
   Serial.println("GetCurrentFrameSum");
   
   float ret = 0.0;
   
   for (int y = 0; y < H; y++) {
      for (int x = 0; x < W; x++) {
         ret += current_frame[y][x];
      }
   }
   Serial.println("   -> Sum: " + String(ret, 2));
   return ret;
}

/** Stop the camera to save energy */
bool StopCamera()
{
   Serial.println("StopCamera");
   
   esp_err_t err = esp_camera_deinit();
   if (err == ESP_OK) {
      return true;
   }
   Serial.println("-> failed: " + String(err));
   return false;
}
