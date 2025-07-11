#ifndef UI_ROBOT_INTERFACE_H
#define UI_ROBOT_INTERFACE_H

#include <lvgl.h>
#include <stdbool.h>

// Initialize the UI robot interface (setup event handlers)
void ui_robot_interface_init(void);

// Event handlers for UI controls
void on_base_slider_changed(lv_event_t *e);
void on_shoulder_slider_changed(lv_event_t *e);
void on_arm_slider_changed(lv_event_t *e);
void on_gripper_slider_changed(lv_event_t *e);
void on_light_switch_changed(lv_event_t *e);

// Utility functions
float map_slider_to_joint_range(int slider_value, float min_rad, float max_rad);
int map_joint_range_to_slider(float radian_value, float min_rad, float max_rad);

// Update UI with robot arm status
void update_robot_status_display(void);

#endif // UI_ROBOT_INTERFACE_H 