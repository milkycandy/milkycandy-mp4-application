#include "FontManager.h"
#include <stdio.h>

#define FONT_PATH "/usr/share/myapp/assets/fonts/MiSans-Demibold.ttf"

std::map<int, lv_font_t*> FontManager::font_map;

void FontManager::init() {
    for (int size = min_size; size <= max_size; ++size) {
        lv_font_t* font = lv_freetype_font_create(
            FONT_PATH,
            LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
            size,
            LV_FREETYPE_FONT_STYLE_NORMAL
        );

        if (!font) {
            printf("[ERROR] FontManager: Failed to load font size %d\n", size);
        } else {
            font_map[size] = font;
            // printf("[INFO] FontManager: Loaded font size %d successfully\n", size);
        }
    }
}

lv_font_t* FontManager::getFont(int size) {
    auto it = font_map.find(size);
    if (it != font_map.end()) {
        return it->second;
    } else {
        printf("[ERROR] FontManager: requested font size %d not found. Falling back to default.\n", size);
        return font_map[28]; // 默认字号
    }
}
