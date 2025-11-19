#pragma once
#include <map>
#include "lvgl.h"

class FontManager {
public:
    static void init();

    /**
     * 根据指定大小返回对应的 Demibold 字体
     * 
     * @param size 字体大小
     * @return 对应大小的 Demibold 字体，如果不存在则返回默认28px
     */
    static lv_font_t* getDemibold(int size);

    /**
     * 根据指定大小返回对应的 Regular 字体
     * 
     * @param size 字体大小
     * @return 对应大小的 Regular 字体，如果不存在则返回默认28px
     */
    static lv_font_t* getRegular(int size);

private:
    static std::map<int, lv_font_t*> demibold_map;
    static std::map<int, lv_font_t*> regular_map;
    static const int min_size = 16;
    static const int max_size = 60;
    static const int default_size = 28;
};
