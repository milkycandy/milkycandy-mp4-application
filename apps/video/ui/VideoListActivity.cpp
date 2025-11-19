#include "VideoListActivity.h"
#include "VideoPlayerActivity.h"
#include "framework/activity/ActivityManager.h"
#include "framework/ui/managers/FontManager.h"
#include "framework/ui/managers/StyleManager.h"
#include "framework/ui/widgets/Toast.h"
#include "lvgl.h"

// 构造函数
VideoListActivity::VideoListActivity(ActivityManager* manager, std::string path)
    : Activity(manager), currentPath_(std::move(path)) {}

// 析构函数
VideoListActivity::~VideoListActivity() {
    // presenter_ 由 unique_ptr 自动管理
}

void VideoListActivity::onCreate() {
    presenter_ = std::make_unique<VideoPresenter>(this);

    listContainer_ = lv_obj_create(root);
    lv_obj_remove_style_all(listContainer_);
    lv_obj_set_width(listContainer_, lv_pct(100));
    lv_obj_set_height(listContainer_, lv_pct(94)); // 94% 高度
    lv_obj_set_align(listContainer_, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_flex_flow(listContainer_, LV_FLEX_FLOW_COLUMN); // 纵向flex布局
    lv_obj_set_flex_align(listContainer_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(listContainer_, LV_OBJ_FLAG_SCROLLABLE);

    // 让 Presenter 加载数据
    presenter_->loadEntries(currentPath_);
}

void VideoListActivity::onDestroy() {
    // 释放为列表项分配的所有内存
    for (auto* data : listEntryData_) {
        delete data;
    }
    listEntryData_.clear();
    LV_LOG_USER("VideoListActivity::onDestroy()");
}

void VideoListActivity::displayFileList(const std::vector<FileEntry>& entries, int folderCount, int fileCount) {
    lv_obj_clean(listContainer_); // 清空旧列表
    // 释放旧的内存
    for (auto* data : listEntryData_) {
        delete data;
    }
    listEntryData_.clear();

    int index = 0;
    for (const auto& entry : entries) {
        // 为每个条目动态创建数据副本，并存储指针以便稍后释放
        auto* entryData = new FileEntry(entry);
        listEntryData_.push_back(entryData);
        createListItem(listContainer_, *entryData, index++);
    }

    // 创建底部的统计信息标签
    footerLabel_ = lv_label_create(listContainer_); // 父控件是 listContainer_
    std::string summary = std::to_string(folderCount) + "个文件夹, " + std::to_string(fileCount) + "个文件";
    lv_label_set_text(footerLabel_, summary.c_str());

    lv_obj_set_style_text_color(footerLabel_, lv_color_hex(0x808080), 0);
    lv_obj_set_style_text_font(footerLabel_, FontManager::getDemibold(24), 0);
    lv_obj_set_style_pad_top(footerLabel_, 20, 0);
    lv_obj_set_style_pad_bottom(footerLabel_, 20, 0);

    // 根据最后一项设置列表背景色
    if (!entries.empty()) {
        bool last_is_odd = (entries.size() - 1) % 2 == 0;
        lv_color_t bg_color = lv_color_hex(last_is_odd ? 0xF4F5F5 : 0xFFFFFF);
        lv_obj_set_style_bg_color(listContainer_, bg_color, 0);
        lv_obj_set_style_bg_opa(listContainer_, 255, 0);
    }
}

// 创建单个列表项
void VideoListActivity::createListItem(lv_obj_t* parent, const FileEntry& entry, int index) {
    bool is_odd = index % 2 == 0;
    lv_obj_t* item = lv_obj_create(parent);
    lv_obj_remove_style_all(item);
    lv_obj_add_style(item, StyleManager::getFileListItemStyle(is_odd), 0);
    lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(item, LV_OBJ_FLAG_SCROLLABLE);

    // 存储数据
    lv_obj_set_user_data(item, (void*)&entry);
    lv_obj_add_event_cb(item, list_item_event_cb, LV_EVENT_CLICKED, this);

    // 图标
    lv_obj_t* icon = lv_image_create(item);
    if (entry.isDirectory) {
        lv_image_set_src(icon, "L:/usr/share/myapp/assets/images/common/folder_66px_blue.png");
    } else {
        lv_image_set_src(icon, "L:/usr/share/myapp/assets/images/common/movie_66px_grey.png");
    }
    lv_obj_set_align(icon, LV_ALIGN_LEFT_MID);
    // lv_obj_add_flag(icon, LV_OBJ_FLAG_HIDDEN);

    // 标签
    lv_obj_t* label = lv_label_create(item);
    lv_label_set_text(label, entry.name.c_str());
    lv_obj_add_style(label, StyleManager::getFileListLabelStyle(), 0);
    lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
    lv_obj_set_style_pad_left(label, 74, 0);
    // lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
}


// --- Presenter调用的导航方法 ---
void VideoListActivity::navigateToDirectory(const std::string& path) {
    manager->startActivity(new VideoListActivity(manager, path));
}

void VideoListActivity::navigateToVideoPlayer(const std::string& path) {
    Toast::show("即将播放: " + path);
    // 启动播放器
    manager->startActivity(new VideoPlayerActivity(manager, path));
}

// --- 静态事件回调 ---
void VideoListActivity::list_item_event_cb(lv_event_t* e) {
    auto* activity = static_cast<VideoListActivity*>(lv_event_get_user_data(e));
    lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
    auto* entry = static_cast<FileEntry*>(lv_obj_get_user_data(target));

    if (activity && activity->presenter_ && entry) {
        activity->presenter_->onEntryClicked(*entry);
    }
}