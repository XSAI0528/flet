// #include <jni.h>
#include <thread>
#include <chrono>
#include <atomic>

// 為了相容 Windows MSVC 編譯器的導出語法
#if defined(_WIN32) || defined(_WIN64)
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

std::atomic<bool> is_running(false);
std::thread overlay_thread;

void OverlayThread() {
    while (is_running) {
        // 未來你的後台渲染邏輯
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

extern "C" {
    EXPORT void StartOverlay() {
        if (!is_running) {
            is_running = true;
            overlay_thread = std::thread(OverlayThread);
        }
    }

    EXPORT void StopOverlay() {
        if (is_running) {
            is_running = false;
            if (overlay_thread.joinable()) {
                overlay_thread.join();
            }
        }
    }
}
