#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *title;
    lv_obj_t *base_motor;
    lv_obj_t *shoulder_motor;
    lv_obj_t *arm_motor;
    lv_obj_t *gripper_motor;
    lv_obj_t *obj0;
    lv_obj_t *light_switch;
    // Robot arm control sliders
    lv_obj_t *base_slider;
    lv_obj_t *shoulder_slider;
    lv_obj_t *arm_slider;
    lv_obj_t *gripper_slider;
    lv_obj_t *light_switch_obj;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/