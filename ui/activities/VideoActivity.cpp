#include "VideoActivity.h"

#include "../ActivityManager.h"
#include "../managers/FontManager.h"
#include "../widgets/Dialog.h"
#include "../widgets/Toast.h"
#include "lvgl.h"

void VideoActivity::onCreate() {
    lv_obj_t* list = lv_obj_create(root);
    lv_obj_remove_style_all(list);
    lv_obj_set_width(list, lv_pct(100));
    lv_obj_set_height(list, lv_pct(94));
    lv_obj_set_align(list, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(list, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
};

void VideoActivity::onDestroy() {

};