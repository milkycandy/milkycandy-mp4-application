#pragma once
#include <map>
#include "lvgl.h"

class FontManager {
public:
    static void init();

    /**
     * 根据指定大小返回对应的字体
     * 
     * @param size 字体大小
     * @return 对应大小的字体，如果不存在则返回NULL
     */
    static lv_font_t* getFont(int size);

private:
    static std::map<int, lv_font_t*> font_map;
    static const int min_size = 20;
    static const int max_size = 60;
};
