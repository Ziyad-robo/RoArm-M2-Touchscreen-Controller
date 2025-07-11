idf_component_register(
    SRCS "waveshare_rgb_lcd_port.c" 
         "main.c" 
         "lvgl_port.c"
         "screens.c"
         "ui.c"
         "images.c"
         "styles.c"
         "eez-flow.cpp"
         "eez-flow-lz4.c"
         "wifi_manager.c"
         "robot_arm_comm.c"
         "ui_robot_interface.c"
    INCLUDE_DIRS ".")

idf_component_get_property(lvgl_lib lvgl__lvgl COMPONENT_LIB)
target_compile_options(${lvgl_lib} PRIVATE -Wno-format)
target_compile_definitions(${lvgl_lib} PUBLIC -DLV_LVGL_H_INCLUDE_SIMPLE)
