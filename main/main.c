/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_log.h"
#include "waveshare_rgb_lcd_port.h"
#include "ui.h"  // Include your custom UI
#include "wifi_manager.h"  // Include WiFi manager
#include "robot_arm_comm.h"  // Include robot arm communication
#include "ui_robot_interface.h"  // Include UI robot interface

static const char *MAIN_TAG = "MAIN";

// Robot arm state tracking
static bool robot_arm_initialized = false;
static const char* ROBOT_ARM_IP = "192.168.4.1";  // Default robot arm IP (adjust as needed)

// LVGL timer callback for UI updates
static void ui_tick_timer_cb(lv_timer_t *timer)
{
    ui_tick();  // Call the UI tick function
}

// WiFi status monitoring timer callback
static void wifi_status_timer_cb(lv_timer_t *timer)
{
    static wifi_status_t last_status = WIFI_STATUS_DISCONNECTED;
    wifi_status_t current_status = wifi_get_status();
    
    if (current_status != last_status) {
        switch (current_status) {
            case WIFI_STATUS_DISCONNECTED:
                ESP_LOGI(MAIN_TAG, "WiFi Status: Disconnected");
                robot_arm_initialized = false;
                break;
            case WIFI_STATUS_CONNECTING:
                ESP_LOGI(MAIN_TAG, "WiFi Status: Connecting...");
                break;
            case WIFI_STATUS_CONNECTED:
                ESP_LOGI(MAIN_TAG, "WiFi Status: Connected - IP: %s", wifi_get_ip_address());
                
                // Initialize robot arm communication once WiFi is connected
                if (!robot_arm_initialized) {
                    robot_arm_comm_status_t result = robot_arm_init(ROBOT_ARM_IP);
                    if (result == ROBOT_ARM_COMM_OK) {
                        robot_arm_initialized = true;
                        ESP_LOGI(MAIN_TAG, "Robot arm communication initialized successfully");
                        
                        // Enable robot arm torque when connected
                        robot_arm_enable_torque();
                    } else {
                        ESP_LOGE(MAIN_TAG, "Failed to initialize robot arm communication");
                    }
                }
                break;
            case WIFI_STATUS_FAILED:
                ESP_LOGI(MAIN_TAG, "WiFi Status: Connection Failed");
                robot_arm_initialized = false;
                break;
        }
        last_status = current_status;
        
        // Update UI status display
        update_robot_status_display();
    }
}

// UI status update timer callback
static void ui_status_timer_cb(lv_timer_t *timer)
{
    // Update UI display with current robot arm status
    update_robot_status_display();
}

void app_main()
{
    ESP_LOGI(MAIN_TAG, "Starting Robot Arm Touch Screen Controller");
    
    // Initialize the display first
    waveshare_esp32_s3_rgb_lcd_init(); // Initialize the Waveshare ESP32-S3 RGB LCD 
    waveshare_rgb_lcd_bl_on();  // Turn on the screen backlight
    
    ESP_LOGI(MAIN_TAG, "Display initialized, initializing UI...");
    
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(-1)) {
        // Initialize your custom UI once
        ui_init();
        
        // Initialize the UI robot interface (setup event handlers)
        ui_robot_interface_init();
        
        // Create an LVGL timer for periodic UI updates (EEZ flow ticking)
        lv_timer_t *ui_timer = lv_timer_create(ui_tick_timer_cb, 50, NULL);
        lv_timer_set_repeat_count(ui_timer, -1); // Repeat indefinitely
        
        // Create a timer for WiFi status monitoring
        lv_timer_t *wifi_timer = lv_timer_create(wifi_status_timer_cb, 1000, NULL);
        lv_timer_set_repeat_count(wifi_timer, -1); // Repeat indefinitely
        
        // Create a timer for UI status updates
        lv_timer_t *status_timer = lv_timer_create(ui_status_timer_cb, 2000, NULL);
        lv_timer_set_repeat_count(status_timer, -1); // Repeat indefinitely
        
        // Release the mutex
        lvgl_port_unlock();
    }
    
    ESP_LOGI(MAIN_TAG, "UI initialized successfully. Initializing WiFi...");
    
    // Initialize WiFi and connect to robot arm
    wifi_init_sta();
    
    ESP_LOGI(MAIN_TAG, "System initialization complete. Touch screen controls are now active!");
    ESP_LOGI(MAIN_TAG, "Move the sliders to control robot arm joints:");
    ESP_LOGI(MAIN_TAG, "  - Base Joint: ±90° rotation");
    ESP_LOGI(MAIN_TAG, "  - Shoulder Joint: -11° to +80° lift");
    ESP_LOGI(MAIN_TAG, "  - Arm Joint: -57° to +86° extend/retract");
    ESP_LOGI(MAIN_TAG, "  - Gripper Joint: ±29° tilt");
    ESP_LOGI(MAIN_TAG, "  - Light Switch: ON/OFF LED control");
    
    // Main application loop - monitor connections
    while (1) {
        // Check connection status periodically
        if (!wifi_is_connected() || !robot_arm_is_connected()) {
            ESP_LOGW(MAIN_TAG, "Connection Status - WiFi: %s, Robot: %s",
                     wifi_is_connected() ? "Connected" : "Disconnected",
                     robot_arm_is_connected() ? "Connected" : "Disconnected");
        }
        
        vTaskDelay(pdMS_TO_TICKS(10000));  // Check every 10 seconds
    }
}
