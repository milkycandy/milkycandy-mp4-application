#include "Dialog.h"
#include "../managers/FontManager.h"

void Dialog::showSingleButton(const std::string& title, const std::string& text, const std::function<void()>& positive_cb, const std::string& positive_text) {
    new Dialog(title, text, false, positive_cb, nullptr, positive_text, "");
}

void Dialog::showDualButton(const std::string& title, const std::string& text, const std::function<void()>& positive_cb, const std::function<void()>& negative_cb, const std::string& positive_text, const std::string& negative_text) {
    new Dialog(title, text, true, positive_cb, negative_cb, positive_text, negative_text);
}

Dialog::Dialog(const std::string& title, const std::string& text, bool is_dual_button, const std::function<void()>& positive_cb, const std::function<void()>& negative_cb, const std::string& positive_text, const std::string& negative_text)
    : positive_callback_(positive_cb), negative_callback_(negative_cb) {
    
    lv_obj_t* parent = ActivityManager::getInstance().getCurrentActivity()->root;
    
    create_ui(parent, title, text, is_dual_button, positive_text, negative_text);

    lv_coord_t dialog_height = lv_obj_get_height(alert_dialog_);
    printf("Dialog height is : %d\n", dialog_height);
    anim_end_y_ = -19;
    anim_start_y_ = 300;

    // 设置初始Y坐标为屏幕外
    lv_obj_set_y(alert_dialog_, anim_start_y_);

    show_animation();
}

void Dialog::create_ui(lv_obj_t* parent, const std::string& title, const std::string& text, bool is_dual_button, const std::string& positive_text, const std::string& negative_text) {
    container_ = lv_obj_create(parent);
    lv_obj_remove_style_all(container_);
    lv_obj_set_size(container_, lv_pct(100), lv_pct(100));
    lv_obj_set_align(container_, LV_ALIGN_CENTER);
    lv_obj_clear_flag(container_, (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));

    mask_ = lv_obj_create(container_);
    lv_obj_remove_style_all(mask_);
    lv_obj_set_size(mask_, lv_pct(100), lv_pct(100));
    lv_obj_set_align(mask_, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(mask_, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(mask_, 0, 0);
    lv_obj_add_flag(mask_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(mask_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(mask_, mask_event_cb, LV_EVENT_CLICKED, this);
    
    alert_dialog_ = lv_obj_create(container_);
    lv_obj_remove_style_all(alert_dialog_);
    lv_obj_set_width(alert_dialog_, 440);
    lv_obj_set_height(alert_dialog_, LV_SIZE_CONTENT);
    lv_obj_set_align(alert_dialog_, LV_ALIGN_BOTTOM_MID);

    lv_obj_set_flex_flow(alert_dialog_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(alert_dialog_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_radius(alert_dialog_, 27, 0);
    lv_obj_set_style_bg_color(alert_dialog_, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(alert_dialog_, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(alert_dialog_, 0, 0);
    lv_obj_set_style_pad_bottom(alert_dialog_, 24, 0);
    lv_obj_add_flag(alert_dialog_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(alert_dialog_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* ui_title = lv_label_create(alert_dialog_);
    lv_label_set_text(ui_title, title.c_str());
    lv_obj_set_style_text_font(ui_title, FontManager::getFont(22), 0);
    lv_obj_set_style_pad_top(ui_title, 26, 0);
    
    lv_obj_t* ui_text = lv_label_create(alert_dialog_);
    lv_label_set_text(ui_text, text.c_str());
    lv_label_set_long_mode(ui_text, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(ui_text, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(ui_text, FontManager::getFont(22), 0);
    lv_obj_set_style_pad_top(ui_text, title.empty() ? 34 : 20, 0);
    lv_obj_set_style_pad_bottom(ui_text, 34, 0);
    lv_obj_set_style_max_height(ui_text, 300, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_max_width(ui_text, 380, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 按钮
    if (is_dual_button) {
        lv_obj_t* btn_container = lv_obj_create(alert_dialog_);
        lv_obj_remove_style_all(btn_container);
        lv_obj_set_size(btn_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_column(btn_container, 20, 0);

        btn_negative_ = lv_button_create(btn_container);
        lv_obj_set_size(btn_negative_, 181, 58);
        lv_obj_set_style_radius(btn_negative_, 19, 0);
        lv_obj_set_style_bg_color(btn_negative_, lv_color_hex(0xF0F0F0), 0);
        lv_obj_set_style_shadow_width(btn_negative_, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_event_cb(btn_negative_, button_event_cb, LV_EVENT_CLICKED, this);
        lv_obj_t* label_neg = lv_label_create(btn_negative_);
        lv_label_set_text(label_neg, negative_text.c_str());
        lv_obj_set_style_text_color(label_neg, lv_color_hex(0x181818), 0);
        lv_obj_set_style_text_font(label_neg, FontManager::getFont(22), 0);
        lv_obj_center(label_neg);
        
        btn_positive_ = lv_button_create(btn_container);
        lv_obj_set_size(btn_positive_, 181, 58);
        lv_obj_set_style_radius(btn_positive_, 19, 0);
        lv_obj_set_style_bg_color(btn_positive_, lv_color_hex(0x3381FF), 0);
        lv_obj_set_style_shadow_width(btn_positive_, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_event_cb(btn_positive_, button_event_cb, LV_EVENT_CLICKED, this);
        lv_obj_t* label_pos = lv_label_create(btn_positive_);
        lv_label_set_text(label_pos, positive_text.c_str());
        lv_obj_set_style_text_font(label_pos, FontManager::getFont(22), 0);
        lv_obj_center(label_pos);
    } else {
        btn_positive_ = lv_button_create(alert_dialog_);
        lv_obj_set_size(btn_positive_, 392, 58);
        lv_obj_set_style_radius(btn_positive_, 19, 0);
        lv_obj_set_style_bg_color(btn_positive_, lv_color_hex(0x3381FF), 0);
        lv_obj_set_style_shadow_width(btn_positive_, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align(btn_positive_, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_event_cb(btn_positive_, button_event_cb, LV_EVENT_CLICKED, this);
        lv_obj_t* label_pos = lv_label_create(btn_positive_);
        lv_label_set_text(label_pos, positive_text.c_str());
        lv_obj_set_style_text_font(label_pos, FontManager::getFont(22), 0);
        lv_obj_center(label_pos);
    }
}

void Dialog::show_animation() {
    // 遮罩动画
    lv_anim_t a_mask;
    lv_anim_init(&a_mask);
    lv_anim_set_var(&a_mask, mask_);
    lv_anim_set_values(&a_mask, 0, 160);
    lv_anim_set_time(&a_mask, 200);
    lv_anim_set_exec_cb(&a_mask, [](void* var, int32_t v) {
        lv_obj_set_style_bg_opa((lv_obj_t*)var, v, 0);
    });
    lv_anim_set_path_cb(&a_mask, lv_anim_path_ease_out);
    lv_anim_start(&a_mask);

    // 对话框滑入动画
    lv_anim_t a_dialog;
    lv_anim_init(&a_dialog);
    lv_anim_set_var(&a_dialog, alert_dialog_);
    lv_anim_set_values(&a_dialog, anim_start_y_, anim_end_y_);
    lv_anim_set_time(&a_dialog, 200);
    lv_anim_set_path_cb(&a_dialog, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a_dialog, [](void* var, int32_t v) {
        lv_obj_set_y((lv_obj_t*)var, v);
    });
    lv_anim_start(&a_dialog);
}

void Dialog::close_animation() {
    
    lv_anim_t a_mask;
    lv_anim_init(&a_mask);
    lv_anim_set_var(&a_mask, mask_);
    lv_anim_set_values(&a_mask, lv_obj_get_style_bg_opa(mask_, LV_PART_MAIN), 0);
    lv_anim_set_time(&a_mask, 150);
    lv_anim_set_exec_cb(&a_mask, [](void* var, int32_t v) {
        lv_obj_set_style_bg_opa((lv_obj_t*)var, v, 0);
    });
    lv_anim_set_path_cb(&a_mask, lv_anim_path_ease_out);
    lv_anim_start(&a_mask);

    lv_anim_t a_dialog;
    lv_anim_init(&a_dialog);
    lv_anim_set_var(&a_dialog, alert_dialog_);
    // printf("AlertDialog y: %d\n", lv_obj_get_style_y(alert_dialog_, 0));
    lv_anim_set_values(&a_dialog, lv_obj_get_style_y(alert_dialog_, LV_PART_MAIN), anim_start_y_);
    lv_anim_set_time(&a_dialog, 150);
    lv_anim_set_path_cb(&a_dialog, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a_dialog, [](void* var, int32_t v) {
        lv_obj_set_y((lv_obj_t*)var, v);
    });
    
    lv_anim_set_ready_cb(&a_dialog, animation_finish_delete_cb);
    lv_anim_set_user_data(&a_dialog, this);
    lv_anim_start(&a_dialog);
}

void Dialog::button_event_cb(lv_event_t* e) {
    Dialog* dialog_instance = (Dialog*)lv_event_get_user_data(e);
    lv_obj_t* target_btn = static_cast<lv_obj_t*>(lv_event_get_target(e));

    // 判断按钮
    if (target_btn == dialog_instance->btn_positive_) {
        if (dialog_instance->positive_callback_) {
            dialog_instance->positive_callback_();
        }
    } else if (target_btn == dialog_instance->btn_negative_) {
        if (dialog_instance->negative_callback_) {
            dialog_instance->negative_callback_();
        }
    }

    dialog_instance->close_animation();
}

void Dialog::animation_finish_delete_cb(lv_anim_t* a) {
    Dialog* dialog_instance = (Dialog*)a->user_data;
    lv_obj_del(dialog_instance->container_);
    delete dialog_instance;
}

void Dialog::mask_event_cb(lv_event_t* e) {
    Dialog* dialog_instance = static_cast<Dialog*>(lv_event_get_user_data(e));
    if (dialog_instance->negative_callback_) {
        dialog_instance->negative_callback_();
    }
    dialog_instance->close_animation();
}