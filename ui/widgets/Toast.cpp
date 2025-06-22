#include "Toast.h"
#include "../managers/FontManager.h"
#include <string>
#include <cstring>

void Toast::show(const std::string& text, Duration duration) {
    // 创建在 LVGL 顶层
    lv_obj_t* container = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(container);
    lv_obj_set_size(container, LV_SIZE_CONTENT, 60);
    lv_obj_set_align(container, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(container, lv_pct(-12));
    lv_obj_set_style_radius(container, 255, 0);
    lv_obj_set_style_bg_color(container, lv_color_hex(0x323232), 0);
    lv_obj_set_style_bg_opa(container, 230, 0);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* label = lv_label_create(container);
    lv_label_set_text(label, text.c_str());
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_opa(label, 255, 0);
    lv_obj_set_style_text_font(label, FontManager::get_font(22), 0);
    lv_obj_set_style_pad_left(label, 22, 0);
    lv_obj_set_style_pad_right(label, 22, 0);
    lv_obj_set_style_pad_top(label, 0, 0);
    lv_obj_set_style_pad_bottom(label, 0, 0);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_SCROLLABLE);

    // 初始透明
    lv_obj_set_style_opa(container, 0, 0);
    fade_in(container);

    // 创建淡出计时器
    lv_timer_t* timer = lv_timer_create(fade_out_cb, duration, container);
    lv_timer_set_repeat_count(timer, 1);
}

void Toast::fade_in(lv_obj_t* obj) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_time(&a, 200);
    lv_anim_set_exec_cb(&a, [](void* target, int32_t v) {
        lv_obj_set_style_opa((lv_obj_t*)target, v, 0);
    });
    lv_anim_start(&a);
}

void Toast::fade_out_cb(lv_timer_t* timer) {
    lv_obj_t* obj = (lv_obj_t*)timer->user_data;
    lv_timer_del(timer);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 255, 0);
    lv_anim_set_time(&a, 150);
    lv_anim_set_exec_cb(&a, [](void* target, int32_t v) {
        lv_obj_set_style_opa((lv_obj_t*)target, v, 0);
    });
    lv_anim_set_ready_cb(&a, [](lv_anim_t* a) {
        lv_obj_del((lv_obj_t*)a->var);  // 动画结束时销毁 Toast
    });
    lv_anim_start(&a);
}
