#include "MusicPlayerView.h"
#include "framework/ui/managers/FontManager.h"

// 初始化静态成员
bool MusicPlayerView::styles_inited = false;
lv_style_t MusicPlayerView::style_transparent_panel;
lv_style_t MusicPlayerView::style_song_title;
lv_style_t MusicPlayerView::style_artist;
lv_style_t MusicPlayerView::style_time_label;
lv_style_t MusicPlayerView::style_func_btn;
lv_style_t MusicPlayerView::style_play_btn_pressed;

// 构造函数：执行创建
MusicPlayerView::MusicPlayerView(lv_obj_t* parent) {
    // 确保所有静态样式只被初始化一次
    if (!styles_inited) {
        init_styles();
        styles_inited = true;
    }
    
    lv_obj_remove_flag( parent, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x475741), 0);
    lv_obj_set_style_bg_opa(parent, 255, 0);

    // --- 专辑封面 ---
    ImgAlbumArt = lv_image_create(parent);
    lv_image_set_src(ImgAlbumArt, "L:/usr/share/myapp/assets/images/music/album400px_round14.png");
    lv_obj_set_align( ImgAlbumArt, LV_ALIGN_TOP_MID );
    lv_obj_add_flag( ImgAlbumArt, LV_OBJ_FLAG_CLICKABLE );
    lv_obj_remove_flag( ImgAlbumArt, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_set_pos(ImgAlbumArt, 0, 65);

    // --- 控制面板 (透明容器) ---
    PanelControl = lv_obj_create(parent);
    lv_obj_add_style(PanelControl, &style_transparent_panel, 0);
    lv_obj_set_height( PanelControl, 340);
    lv_obj_set_width( PanelControl, lv_pct(100));
    lv_obj_set_align( PanelControl, LV_ALIGN_BOTTOM_MID );

    // --- 歌曲信息区 (透明容器) ---
    AreaSongInfo = lv_obj_create(PanelControl);
    lv_obj_add_style(AreaSongInfo, &style_transparent_panel, 0); // *** 使用样式 ***
    lv_obj_set_height( AreaSongInfo, 68);
    lv_obj_set_width( AreaSongInfo, 398);
    lv_obj_set_align( AreaSongInfo, LV_ALIGN_TOP_MID );
    lv_obj_set_pos(AreaSongInfo, 0, 29);

    // --- 歌曲名 ---
    SongTitle = lv_label_create(AreaSongInfo);
    lv_label_set_text(SongTitle, "晴天");
    lv_obj_set_width(SongTitle, LV_SIZE_CONTENT);
    lv_obj_set_height(SongTitle, LV_SIZE_CONTENT);
    lv_obj_set_pos(SongTitle, -1, 0);
    lv_obj_set_style_text_color(SongTitle, lv_color_hex(0xFFFFFF), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(SongTitle, FontManager::getRegular(27), LV_STATE_DEFAULT);
    // lv_obj_add_style(SongTitle, &style_song_title, 0);

    // --- 艺术家 ---
    Artist = lv_label_create(AreaSongInfo);
    lv_label_set_text(Artist,"周杰伦");
    lv_obj_set_width(Artist, LV_SIZE_CONTENT);
    lv_obj_set_height(Artist, LV_SIZE_CONTENT);
    lv_obj_set_align(Artist, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_pos(Artist, -1, 0);
    lv_obj_set_style_text_color(Artist, lv_color_hex(0xFFFFFF), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(Artist, FontManager::getRegular(27), LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(Artist, 160, LV_STATE_DEFAULT);

    // lv_obj_add_style(Artist, &style_artist, 0);

    // --- 进度条区域 ---
    AreaSeekBar = lv_obj_create(PanelControl);
    lv_obj_add_style(AreaSeekBar, &style_transparent_panel, 0);
    lv_obj_set_height( AreaSeekBar, 50);
    lv_obj_set_width( AreaSeekBar, lv_pct(84));
    lv_obj_set_align( AreaSeekBar, LV_ALIGN_BOTTOM_MID );
    lv_obj_set_pos(AreaSeekBar, 0, -172);
    
    SeekBar = lv_slider_create(AreaSeekBar);
    lv_slider_set_value( SeekBar, 0, LV_ANIM_OFF);
    lv_obj_set_width( SeekBar, 398);
    lv_obj_set_height( SeekBar, 9);
    lv_obj_set_align( SeekBar, LV_ALIGN_TOP_MID );
    lv_obj_set_style_bg_color(SeekBar, lv_color_hex(0xE5E5E5), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(SeekBar, 56, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(SeekBar, lv_color_hex(0xE5E5E5), LV_PART_INDICATOR | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(SeekBar, 160, LV_PART_INDICATOR| LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(SeekBar, 0, LV_PART_KNOB| LV_STATE_DEFAULT);


    // --- 时间标签 ---
    CurrentTime = lv_label_create(AreaSeekBar);
    lv_label_set_text(CurrentTime,"-:--");
    lv_obj_set_align( CurrentTime, LV_ALIGN_BOTTOM_LEFT );
    lv_obj_set_pos(CurrentTime, 2, 0);
    lv_obj_add_style(CurrentTime, &style_time_label, 0); // *** 使用样式 ***
    
    RemainingTime = lv_label_create(AreaSeekBar);
    lv_label_set_text(RemainingTime,"--:--");
    lv_obj_set_align( RemainingTime, LV_ALIGN_BOTTOM_RIGHT );
    lv_obj_set_pos(RemainingTime, -2, 0);
    lv_obj_add_style(RemainingTime, &style_time_label, 0); // *** 使用样式 ***

    // --- 播放控制区 ---
    AreaPlayerControl = lv_obj_create(PanelControl);
    lv_obj_add_style(AreaPlayerControl, &style_transparent_panel, 0); // *** 使用样式 ***
    lv_obj_set_height( AreaPlayerControl, 120);
    lv_obj_set_width( AreaPlayerControl, lv_pct(76));
    lv_obj_set_align( AreaPlayerControl, LV_ALIGN_BOTTOM_MID );
    lv_obj_set_pos(AreaPlayerControl, 0, -60);

    ButtonPrev = lv_obj_create(AreaPlayerControl);
    lv_obj_remove_style_all(ButtonPrev);
    lv_obj_set_size( ButtonPrev, 110, 110);
    lv_obj_set_align( ButtonPrev, LV_ALIGN_LEFT_MID );
    lv_obj_set_style_bg_image_src( ButtonPrev, "L:/usr/share/myapp/assets/images/music/fast_rewind_76dp_FFFFFF.png", 0 );
    lv_obj_add_style(ButtonPrev, &style_play_btn_pressed, LV_STATE_PRESSED);

    ButtonPlay = lv_obj_create(AreaPlayerControl);
    lv_obj_remove_style_all(ButtonPlay);
    lv_obj_set_size( ButtonPlay, 110, 110);
    lv_obj_set_align( ButtonPlay, LV_ALIGN_CENTER );
    lv_obj_set_style_bg_image_src( ButtonPlay, "L:/usr/share/myapp/assets/images/music/pause_76dp_FFFFFF.png", 0 );
    lv_obj_add_style(ButtonPlay, &style_play_btn_pressed, LV_STATE_PRESSED);

    ButtonNext = lv_obj_create(AreaPlayerControl);
    lv_obj_remove_style_all(ButtonNext);
    lv_obj_set_size( ButtonNext, 110, 110);
    lv_obj_set_align( ButtonNext, LV_ALIGN_RIGHT_MID );
    lv_obj_set_style_bg_image_src( ButtonNext, "L:/usr/share/myapp/assets/images/music/fast_forward_76dp_FFFFFF.png", 0 );
    lv_obj_add_style(ButtonNext, &style_play_btn_pressed, LV_STATE_PRESSED);

    // --- 功能按钮区 ---
    AreaFunctionButton = lv_obj_create(PanelControl);
    lv_obj_add_style(AreaFunctionButton, &style_transparent_panel, 0);
    lv_obj_set_height( AreaFunctionButton, 50);
    lv_obj_set_width( AreaFunctionButton, lv_pct(63));
    lv_obj_set_align( AreaFunctionButton, LV_ALIGN_BOTTOM_MID );
    lv_obj_set_pos(AreaFunctionButton, 0, -16);

    ButtonLyrics = lv_obj_create(AreaFunctionButton);
    lv_obj_remove_style_all(ButtonLyrics);
    lv_obj_add_style(ButtonLyrics, &style_func_btn, 0);
    lv_obj_set_align( ButtonLyrics, LV_ALIGN_LEFT_MID );
    lv_obj_set_style_bg_image_src( ButtonLyrics, "L:/usr/share/myapp/assets/images/music/subtitles_36dp_FFFFFF.png", 0);

    ButtonPlayList = lv_obj_create(AreaFunctionButton);
    lv_obj_remove_style_all(ButtonPlayList);
    lv_obj_add_style(ButtonPlayList, &style_func_btn, 0);
    lv_obj_set_align( ButtonPlayList, LV_ALIGN_RIGHT_MID );
    lv_obj_set_style_bg_image_src( ButtonPlayList, "L:/usr/share/myapp/assets/images/music/play_list_36dp_FFFFFF.png", 0);
}

// 样式初始化
void MusicPlayerView::init_styles() {
    // --- 透明面板样式 ---
    lv_style_init(&style_transparent_panel);
    lv_style_set_radius(&style_transparent_panel, 0);
    lv_style_set_bg_opa(&style_transparent_panel, LV_OPA_TRANSP); // 透明背景
    lv_style_set_border_width(&style_transparent_panel, 0); // 无边框
    lv_style_set_pad_all(&style_transparent_panel, 0); // 无内边距

    // --- 歌曲名样式 ---
    lv_style_init(&style_song_title);
    lv_style_set_text_color(&style_song_title, lv_color_hex(0xFFFFFF));
    lv_style_set_text_opa(&style_song_title, 255);
    lv_style_set_text_font(&style_song_title, FontManager::getRegular(25));

    // --- 艺术家样式 ---
    lv_style_init(&style_artist);
    lv_style_set_text_color(&style_artist, lv_color_hex(0xFFFFFF));
    lv_style_set_text_opa(&style_artist, 160);
    lv_style_set_text_font(&style_artist, FontManager::getRegular(25));

    // --- 时间标签样式 ---
    lv_style_init(&style_time_label);
    lv_style_set_text_color(&style_time_label, lv_color_hex(0xE5E5E5));
    lv_style_set_text_opa(&style_time_label, 140);
    lv_style_set_text_font(&style_time_label, FontManager::getDemibold(16));
    lv_style_set_text_letter_space(&style_time_label, 1);

    // --- 功能按钮样式 ---
    lv_style_init(&style_func_btn);
    lv_style_set_size(&style_func_btn, 48, 48);
    lv_style_set_bg_opa(&style_func_btn, LV_OPA_TRANSP); // 透明背景
    lv_style_set_bg_image_opa(&style_func_btn, 160);

    // --- 播放按钮按下样式 ---
    lv_style_init(&style_play_btn_pressed);
    lv_style_set_radius(&style_play_btn_pressed, 255);
    lv_style_set_bg_color(&style_play_btn_pressed, lv_color_hex(0xE5E5E5));
    lv_style_set_bg_opa(&style_play_btn_pressed, 100);
}