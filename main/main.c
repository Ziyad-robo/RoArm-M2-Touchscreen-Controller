/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_log.h"
#include "waveshare_rgb_lcd_port.h"
#include "ui.h"  // Include your custom UI

static const char *MAIN_TAG = "MAIN";

// LVGL timer callback for UI updates
static void ui_tick_timer_cb(lv_timer_t *timer)
{
    ui_tick();  // Call the UI tick function
}

void app_main()
{
    waveshare_esp32_s3_rgb_lcd_init(); // Initialize the Waveshare ESP32-S3 RGB LCD 
    waveshare_rgb_lcd_bl_on();  // Fixed typo: was "wavesahre_rgb_lcd_bl_on"
    // waveshare_rgb_lcd_bl_off(); //Turn off the screen backlight 
    
    ESP_LOGI(MAIN_TAG, "Display custom SERB UI");
    
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(-1)) {
        // Initialize your custom UI once
        ui_init();
        
        // Create an LVGL timer for periodic UI updates (EEZ flow ticking)
        lv_timer_t *ui_timer = lv_timer_create(ui_tick_timer_cb, 50, NULL);
        lv_timer_set_repeat_count(ui_timer, -1); // Repeat indefinitely
        
        // Release the mutex
        lvgl_port_unlock();
    }
    
    ESP_LOGI(MAIN_TAG, "UI initialized successfully. LVGL timers will handle updates.");
    
    // Main application loop - do your non-UI tasks here
    while (1) {
        // Put your application logic here (sensors, communications, etc.)
        // The LVGL port task and timers handle all UI updates automatically
        
        vTaskDelay(pdMS_TO_TICKS(1000));  // Sleep for 1 second
    }
}
