#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    {
        lv_obj_t *parent_obj = obj;
        {
            // title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.title = obj;
            lv_obj_set_pos(obj, 117, 31);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Control the robot");
        }
        {
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.base_slider = obj;  // Store base slider
            lv_obj_set_pos(obj, 227, 100);
            lv_obj_set_size(obj, 522, 47);
            lv_slider_set_value(obj, 50, LV_ANIM_OFF);  // Center position
            lv_slider_set_range(obj, 0, 100);  // 0-100 range for easier mapping
        }
        {
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.shoulder_slider = obj;  // Store shoulder slider
            lv_obj_set_pos(obj, 227, 193);
            lv_obj_set_size(obj, 522, 47);
            lv_slider_set_value(obj, 20, LV_ANIM_OFF);  // Start position (safe range)
            lv_slider_set_range(obj, 0, 100);  // 0-100 range for easier mapping
        }
        {
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.arm_slider = obj;  // Store arm slider
            lv_obj_set_pos(obj, 227, 267);
            lv_obj_set_size(obj, 522, 47);
            lv_slider_set_value(obj, 40, LV_ANIM_OFF);  // Safe starting position
            lv_slider_set_range(obj, 0, 100);  // 0-100 range for easier mapping
        }
        {
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.gripper_slider = obj;  // Store gripper slider
            lv_obj_set_pos(obj, 227, 345);
            lv_obj_set_size(obj, 522, 47);
            lv_slider_set_value(obj, 0, LV_ANIM_OFF);  // Start at 0% (fully open)
            lv_slider_set_range(obj, 0, 100);  // 0-100 range for easier mapping
        }
        {
            // base_motor
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.base_motor = obj;
            lv_obj_set_pos(obj, 33, 108);
            lv_obj_set_size(obj, 108, 31);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Base Joint");
        }
        {
            // shoulder_motor
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.shoulder_motor = obj;
            lv_obj_set_pos(obj, 34, 201);
            lv_obj_set_size(obj, 134, 29);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Shoulder Joint");
        }
        {
            // arm_motor
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.arm_motor = obj;
            lv_obj_set_pos(obj, 21, 276);
            lv_obj_set_size(obj, 134, 29);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Arm Joint");
        }
        {
            // gripper_motor
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.gripper_motor = obj;
            lv_obj_set_pos(obj, 21, 354);
            lv_obj_set_size(obj, 134, 29);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Gripper");
        }
        {
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.obj0 = obj;
            objects.light_switch_obj = obj;  // Store light switch reference
            lv_obj_set_pos(obj, 644, 8);
            lv_obj_set_size(obj, 126, 67);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff85e30a), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // light_switch
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.light_switch = obj;
            lv_obj_set_pos(obj, 568, 31);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Light");
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
}


static const char *screen_names[] = { "Main" };
static const char *object_names[] = { "main", "title", "base_motor", "shoulder_motor", "arm_motor", "gripper_motor", "obj0", "light_switch", "base_slider", "shoulder_slider", "arm_slider", "gripper_slider", "light_switch_obj" };


typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
