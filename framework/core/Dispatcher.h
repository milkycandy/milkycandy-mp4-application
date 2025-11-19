#pragma once

/**
 * @brief 任务分发器
 *
 * 提供任务调度功能：
 * - runOnUiThread: 投递任务到 UI 线程执行
 * - postDelayed: 延迟执行任务（仅限 UI 线程调用）
 */

#include <functional>
#include <memory>
#include "lvgl.h"

namespace {

/**
 * @brief LVGL 异步回调：执行并清理已调度的任务
 * 
 */
void lvgl_async_executor(void* user_data) {
    if (!user_data) {
        return;
    }
    std::unique_ptr<std::function<void()>> func(static_cast<std::function<void()>*>(user_data));
    (*func)(); // 执行函数
}

/**
 * @brief LVGL 单次定时器回调：执行并清理已调度的 lambda
 *
 */
void lvgl_timer_lambda_executor(lv_timer_t* timer) {
    void* user_data = lv_timer_get_user_data(timer);
    if (!user_data) {
        return;
    }
    
    std::unique_ptr<std::function<void()>> func_ptr(
        static_cast<std::function<void()>*>(user_data)
    );

    (*func_ptr)();

    // func_ptr 离开作用域，自动 delete 堆上的 std::function
    // LVGL 也会因为 repeat_count 减到 0 而自动删除 lv_timer 本身
}

} // namespace

/**
 * @brief [线程安全] 将任务调度到 LVGL UI 线程执行
 *
 * 移交 `task` 的所有权给 LVGL
 * 任务在 `lvgl_async_executor` 中执行并在执行后销毁。
 */

inline void runOnUiThread(std::function<void()>&& task) {
    auto func_ptr = new std::function<void()>(std::move(task));
    lv_result_t res = lv_async_call(lvgl_async_executor, func_ptr);
    if (res != LV_RESULT_OK) {
        delete func_ptr;  // 防止内存泄漏
    }
}

/**
 * @brief [非线程安全] 将任务延迟调度到 LVGL UI 线程执行
 *
 * 类似 Android 的 postDelayed。
 * 任务在执行后会被自动销毁。
 * ⚠️必须确保在 UI 线程调用
 *
 * @param delay_ms 延迟的毫秒数
 * @param task 要执行的任务
 * @return lv_timer_t* 句柄，可用于取消
 */
inline lv_timer_t* postDelayed(uint32_t delay_ms, std::function<void()>&& task) {
    auto func_ptr = std::unique_ptr<std::function<void()>>(
        new std::function<void()>(std::move(task))
    );

    lv_timer_t* timer = lv_timer_create(
        lvgl_timer_lambda_executor,
        delay_ms,
        func_ptr.get()
    );

    lv_timer_set_repeat_count(timer, 1);

    func_ptr.release();

    return timer;
}