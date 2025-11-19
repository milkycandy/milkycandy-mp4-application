#include "FontManager.h"
#include <stdio.h>

#define Demibold_PATH "/usr/share/myapp/assets/fonts/MiSans-Demibold.ttf"
#define Regular_PATH "/usr/share/myapp/assets/fonts/MiSans-Regular.ttf"

std::map<int, lv_font_t*> FontManager::demibold_map;
std::map<int, lv_font_t*> FontManager::regular_map;
const int FontManager::default_size;

void FontManager::init() {
    for (int size = min_size; size <= max_size; ++size) {
        lv_font_t* demibold_font = lv_freetype_font_create(
            Demibold_PATH,
            LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
            size,
            LV_FREETYPE_FONT_STYLE_NORMAL
        );

        lv_font_t* regular_font = lv_freetype_font_create(
            Regular_PATH,
            LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
            size,
            LV_FREETYPE_FONT_STYLE_NORMAL
        );

        if (!demibold_font || !regular_font) {
            printf("[ERROR] FontManager: Failed to load font size %d\n", size);
        } else {
            demibold_map[size] = demibold_font;
            regular_map[size] = regular_font;
            // printf("[INFO] FontManager: Loaded font size %d successfully\n", size);
        }
    }
}

lv_font_t* FontManager::getDemibold(int size) {
    auto it = demibold_map.find(size);
    if (it != demibold_map.end()) {
        return it->second;
    } else {
        printf("[ERROR] FontManager: requested Demibold size %d not found. Falling back to default.\n", size);
        return demibold_map[default_size]; // 默认字号
    }
}

lv_font_t* FontManager::getRegular(int size) {
    auto it = regular_map.find(size);
    if (it != regular_map.end()) {
        return it->second;
    } else {
        printf("[ERROR] FontManager: requested Regular size %d not found. Falling back to default.\n", size);
        return regular_map[default_size]; // 默认字号
    }
}