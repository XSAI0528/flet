#include <jni.h>
#include <pthread.h>
#include <unistd.h>

bool is_running = false;
pthread_t thread_id;

// 純 C++ 後台線程
void* OverlayThread(void* arg) {
    while (is_running) {
        // 這裡未來可以放入你真正的 Android 懸浮窗渲染邏輯
        usleep(100000); // 休息 0.1 秒，避免 CPU 飆高
    }
    return nullptr;
}

extern "C" {
    // 給 Flet 呼叫的啟動函數
    __attribute__((visibility("default"))) void StartOverlay() {
        if (!is_running) {
            is_running = true;
            pthread_create(&thread_id, nullptr, OverlayThread, nullptr);
        }
    }

    // 給 Flet 呼叫的關閉函數
    __attribute__((visibility("default"))) void StopOverlay() {
        if (is_running) {
            is_running = false;
            pthread_join(thread_id, nullptr);
        }
    }
}
