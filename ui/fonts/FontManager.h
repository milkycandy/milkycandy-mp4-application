#pragma once
#include <map>
#include "lvgl.h"

class FontManager {
public:
    static void init();
    static lv_font_t* get_font(int size);

private:
    static std::map<int, lv_font_t*> font_map;
    static const int min_size = 20;
    static const int max_size = 60;
};
