#include "ui_robot_interface.h"
#include "robot_arm_comm.h"
#include "screens.h"
#include "esp_log.h"
#include <math.h>

static const char *UI_ROBOT_TAG = "UI_ROBOT";

// Joint ranges from the table you provided
#define BASE_MIN_RAD    -1.57f  // -90 degrees
#define BASE_MAX_RAD     1.57f  // +90 degrees
#define SHOULDER_MIN_RAD -0.2f  // ~-11 degrees
#define SHOULDER_MAX_RAD  1.4f  // ~+80 degrees
#define ARM_MIN_RAD     -1.0f   // ~-57 degrees
#define ARM_MAX_RAD      1.5f   // ~+86 degrees
#define GRIPPER_MIN_RAD  1.08f  // Open fully
#define GRIPPER_MAX_RAD  3.14f  // Close fully

// Control parameters
#define JOINT_SPEED     0       // 0 = use default speed
#define JOINT_ACCELERATION 10   // Acceleration value

// Utility function to map slider value (0-100) to joint range in radians
float map_slider_to_joint_range(int slider_value, float min_rad, float max_rad)
{
    // Clamp slider value to 0-100 range
    if (slider_value < 0) slider_value = 0;
    if (slider_value > 100) slider_value = 100;
    
    // Map 0-100 to min_rad to max_rad
    return min_rad + (float)slider_value * (max_rad - min_rad) / 100.0f;
}

// Utility function to map joint range to slider value (0-100)
int map_joint_range_to_slider(float radian_value, float min_rad, float max_rad)
{
    // Clamp radian value to safe range
    if (radian_value < min_rad) radian_value = min_rad;
    if (radian_value > max_rad) radian_value = max_rad;
    
    // Map min_rad to max_rad to 0-100
    return (int)((radian_value - min_rad) * 100.0f / (max_rad - min_rad));
}

// Event handler for base slider (Joint 1)
void on_base_slider_changed(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int slider_value = lv_slider_get_value(slider);
    
    float joint_angle = map_slider_to_joint_range(slider_value, BASE_MIN_RAD, BASE_MAX_RAD);
    
    ESP_LOGI(UI_ROBOT_TAG, "Base slider changed: %d%% -> %.3f rad (%.1f°)", 
             slider_value, joint_angle, joint_angle * 180.0f / M_PI);
    
    if (robot_arm_is_connected()) {
        robot_arm_comm_status_t result = robot_arm_move_base(joint_angle, JOINT_SPEED, JOINT_ACCELERATION);
        if (result != ROBOT_ARM_COMM_OK) {
            ESP_LOGE(UI_ROBOT_TAG, "Failed to move base joint");
        }
    }
}

// Event handler for shoulder slider (Joint 2)
void on_shoulder_slider_changed(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int slider_value = lv_slider_get_value(slider);
    
    float joint_angle = map_slider_to_joint_range(slider_value, SHOULDER_MIN_RAD, SHOULDER_MAX_RAD);
    
    ESP_LOGI(UI_ROBOT_TAG, "Shoulder slider changed: %d%% -> %.3f rad (%.1f°)", 
             slider_value, joint_angle, joint_angle * 180.0f / M_PI);
    
    if (robot_arm_is_connected()) {
        robot_arm_comm_status_t result = robot_arm_move_shoulder(joint_angle, JOINT_SPEED, JOINT_ACCELERATION);
        if (result != ROBOT_ARM_COMM_OK) {
            ESP_LOGE(UI_ROBOT_TAG, "Failed to move shoulder joint");
        }
    }
}

// Event handler for arm slider (Joint 3 - Elbow)
void on_arm_slider_changed(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int slider_value = lv_slider_get_value(slider);
    
    float joint_angle = map_slider_to_joint_range(slider_value, ARM_MIN_RAD, ARM_MAX_RAD);
    
    ESP_LOGI(UI_ROBOT_TAG, "Arm slider changed: %d%% -> %.3f rad (%.1f°)", 
             slider_value, joint_angle, joint_angle * 180.0f / M_PI);
    
    if (robot_arm_is_connected()) {
        robot_arm_comm_status_t result = robot_arm_move_elbow(joint_angle, JOINT_SPEED, JOINT_ACCELERATION);
        if (result != ROBOT_ARM_COMM_OK) {
            ESP_LOGE(UI_ROBOT_TAG, "Failed to move elbow joint");
        }
    }
}

// Event handler for gripper slider (Joint 4)
void on_gripper_slider_changed(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int slider_value = lv_slider_get_value(slider);
    
    float joint_angle = map_slider_to_joint_range(slider_value, GRIPPER_MIN_RAD, GRIPPER_MAX_RAD);
    
    // Calculate gripper openness percentage (0% = fully open, 100% = fully closed)
    float openness = (joint_angle - GRIPPER_MIN_RAD) / (GRIPPER_MAX_RAD - GRIPPER_MIN_RAD) * 100.0f;
    
    ESP_LOGI(UI_ROBOT_TAG, "Gripper slider changed: %d%% -> %.3f rad (%.1f° - %.0f%% closed)", 
             slider_value, joint_angle, joint_angle * 180.0f / M_PI, openness);
    
    if (robot_arm_is_connected()) {
        robot_arm_comm_status_t result = robot_arm_move_gripper(joint_angle, JOINT_SPEED, JOINT_ACCELERATION);
        if (result != ROBOT_ARM_COMM_OK) {
            ESP_LOGE(UI_ROBOT_TAG, "Failed to move gripper joint");
        }
    }
}

// Event handler for light switch
void on_light_switch_changed(lv_event_t *e)
{
    lv_obj_t *switch_obj = lv_event_get_target(e);
    bool is_on = lv_obj_has_state(switch_obj, LV_STATE_CHECKED);
    
    ESP_LOGI(UI_ROBOT_TAG, "Light switch changed: %s", is_on ? "ON" : "OFF");
    
    if (robot_arm_is_connected()) {
        robot_arm_comm_status_t result;
        if (is_on) {
            result = robot_arm_led_on();
        } else {
            result = robot_arm_led_off();
        }
        
        if (result != ROBOT_ARM_COMM_OK) {
            ESP_LOGE(UI_ROBOT_TAG, "Failed to control LED");
        }
    }
}

// Initialize the UI robot interface
void ui_robot_interface_init(void)
{
    ESP_LOGI(UI_ROBOT_TAG, "Initializing UI robot interface...");
    
    // Add event handlers to sliders
    lv_obj_add_event_cb(objects.base_slider, on_base_slider_changed, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.shoulder_slider, on_shoulder_slider_changed, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.arm_slider, on_arm_slider_changed, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.gripper_slider, on_gripper_slider_changed, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Add event handler to light switch
    lv_obj_add_event_cb(objects.light_switch_obj, on_light_switch_changed, LV_EVENT_VALUE_CHANGED, NULL);
    
    ESP_LOGI(UI_ROBOT_TAG, "UI robot interface initialized successfully");
}

// Update UI with robot arm status
void update_robot_status_display(void)
{
    // You can add status indicators here later
    // For example, change slider colors based on connection status
    if (robot_arm_is_connected()) {
        // Enable sliders
        lv_obj_clear_state(objects.base_slider, LV_STATE_DISABLED);
        lv_obj_clear_state(objects.shoulder_slider, LV_STATE_DISABLED);
        lv_obj_clear_state(objects.arm_slider, LV_STATE_DISABLED);
        lv_obj_clear_state(objects.gripper_slider, LV_STATE_DISABLED);
        lv_obj_clear_state(objects.light_switch_obj, LV_STATE_DISABLED);
    } else {
        // Disable sliders when not connected
        lv_obj_add_state(objects.base_slider, LV_STATE_DISABLED);
        lv_obj_add_state(objects.shoulder_slider, LV_STATE_DISABLED);
        lv_obj_add_state(objects.arm_slider, LV_STATE_DISABLED);
        lv_obj_add_state(objects.gripper_slider, LV_STATE_DISABLED);
        lv_obj_add_state(objects.light_switch_obj, LV_STATE_DISABLED);
    }
} 