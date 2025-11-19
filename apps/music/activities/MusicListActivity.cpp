#include "MusicListActivity.h"
#include "MusicActivity.h"
#include "framework/activity/ActivityManager.h"
#include "framework/ui/managers/FontManager.h"
#include "framework/ui/managers/StyleManager.h"
#include "framework/ui/widgets/Toast.h"
#include "lvgl.h"
#include "apps/music/presenters/MusicPresenter.h"

MusicListActivity::MusicListActivity(ActivityManager* manager, std::string path)
    : Activity(manager), currentPath_(std::move(path)) {}

MusicListActivity::~MusicListActivity() {}

void MusicListActivity::onCreate() {
    presenter_ = std::make_unique<MusicPresenter>(this);

    listContainer_ = lv_obj_create(root);
    lv_obj_remove_style_all(listContainer_);
    lv_obj_set_width(listContainer_, lv_pct(100));
    lv_obj_set_height(listContainer_, lv_pct(94));
    lv_obj_set_align(listContainer_, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_flex_flow(listContainer_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(listContainer_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(listContainer_, LV_OBJ_FLAG_SCROLLABLE);

    presenter_->loadEntries(currentPath_);
}

void MusicListActivity::onDestroy() {
    for (auto* p : listEntryData_) delete p;
    listEntryData_.clear();
}

void MusicListActivity::displayFileList(const std::vector<FileEntry>& entries, int folderCount, int fileCount) {
    lv_obj_clean(listContainer_);
    for (auto* p : listEntryData_) delete p;
    listEntryData_.clear();

    int index = 0;
    for (const auto& e : entries) {
        auto* heapCopy = new FileEntry(e);
        listEntryData_.push_back(heapCopy);
        createListItem(listContainer_, heapCopy, index++);
    }

    footerLabel_ = lv_label_create(listContainer_);
    std::string summary = std::to_string(folderCount) + "个文件夹, " + std::to_string(fileCount) + "首歌曲";
    lv_label_set_text(footerLabel_, summary.c_str());
    lv_obj_set_style_text_color(footerLabel_, lv_color_hex(0x808080), 0);
    lv_obj_set_style_text_font(footerLabel_, FontManager::getDemibold(24), 0);
    lv_obj_set_style_pad_top(footerLabel_, 20, 0);
    lv_obj_set_style_pad_bottom(footerLabel_, 20, 0);

    if (!entries.empty()) {
        bool last_is_odd = (entries.size() - 1) % 2 == 0;
        lv_color_t bg = lv_color_hex(last_is_odd ? 0xF4F5F5 : 0xFFFFFF);
        lv_obj_set_style_bg_color(listContainer_, bg, 0);
        lv_obj_set_style_bg_opa(listContainer_, 255, 0);
    }
}

void MusicListActivity::createListItem(lv_obj_t* parent, FileEntry* entry, int index) {
    bool is_odd = index % 2 == 0;
    lv_obj_t* item = lv_obj_create(parent);
    lv_obj_remove_style_all(item);
    lv_obj_add_style(item, StyleManager::getFileListItemStyle(is_odd), 0);
    lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(item, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_user_data(item, entry);
    lv_obj_add_event_cb(item, list_item_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* icon = lv_image_create(item);
    if (entry->isDirectory) lv_image_set_src(icon, "L:/usr/share/myapp/assets/images/common/folder_66px_blue.png");
    else                    lv_image_set_src(icon, "L:/usr/share/myapp/assets/images/common/movie_66px_grey.png");
    lv_obj_set_align(icon, LV_ALIGN_LEFT_MID);

    lv_obj_t* label = lv_label_create(item);
    lv_label_set_text(label, entry->name.c_str());
    lv_obj_add_style(label, StyleManager::getFileListLabelStyle(), 0);
    lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
    lv_obj_set_style_pad_left(label, 74, 0);
}

void MusicListActivity::navigateToDirectory(const std::string& path) {
    manager->startActivity(new MusicListActivity(manager, path));
}

void MusicListActivity::navigateToMusicPlayer(const std::string& path) {
    Toast::show("即将播放: " + path);
    manager->startActivity(new MusicActivity(manager, path)); // 先占位
}

void MusicListActivity::list_item_event_cb(lv_event_t* e) {
    auto* activity = static_cast<MusicListActivity*>(lv_event_get_user_data(e));
    lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
    auto* entry = static_cast<FileEntry*>(lv_obj_get_user_data(target));
    if (activity && activity->presenter_ && entry) {
        activity->presenter_->onEntryClicked(*entry);
    }
}
