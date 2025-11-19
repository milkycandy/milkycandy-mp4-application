// ------------------------------------
// views/main_player_view.h
// ------------------------------------
#pragma once

// 必须包含 lvgl.h 和 SLS 生成的资源 (字体、图片)
#include "lvgl.h"

class MusicPlayerView {
public:

    lv_obj_t *ImgAlbumArt;
    lv_obj_t *PanelControl;
    lv_obj_t *AreaSongInfo;
    lv_obj_t *SongTitle;
    lv_obj_t *Artist;
    lv_obj_t *AreaSeekBar;
    lv_obj_t *SeekBar;
    lv_obj_t *CurrentTime;
    lv_obj_t *RemainingTime;
    lv_obj_t *AreaPlayerControl;
    lv_obj_t *ButtonPrev;
    lv_obj_t *ButtonPlay;
    lv_obj_t *ButtonNext;
    lv_obj_t *AreaFunctionButton;
    lv_obj_t *ButtonLyrics;
    lv_obj_t *ButtonPlayList;

    /**
     * @brief 构造函数：创建所有UI组件
     * @param parent "Activity" 提供的根对象 (例如 lv_scr_act())
     */
    explicit MusicPlayerView(lv_obj_t* parent);

    /**
     * @brief 析构函数：现在是默认的，因为样式是静态的
     */
    ~MusicPlayerView() = default;

private:
    // ----------------------------------------------------
    // 2. 静态样式：取代所有重复的 set_style...
    // ----------------------------------------------------
    static bool styles_inited; // 确保样式只初始化一次
    static void init_styles(); // 样式初始化函数

    // SLS 中大量使用了 "移除所有样式" 的透明容器，我们定义一个
    static lv_style_t style_transparent_panel;
    
    // 字体和颜色样式
    static lv_style_t style_song_title;
    static lv_style_t style_artist;
    static lv_style_t style_time_label;

    // 按钮样式
    static lv_style_t style_func_btn;
    static lv_style_t style_play_btn_pressed; // 播放按钮的按下状态
};