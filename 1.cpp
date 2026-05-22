#include <thread>
#include <chrono>
#include <atomic>

// 判斷平台引入 Android 專屬標頭檔
#if defined(__ANDROID__)
    #include <android/native_window.h>
    #include <android/native_window_jni.h>
    // 這裡未來會引入 imgui.h, imgui_impl_android.h, imgui_impl_opengl3.h
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

std::atomic<bool> is_running(false);
std::thread overlay_thread;

// 透過選單控制的共享變數（Flet 與 ImGui 都可以讀寫）
std::atomic<bool> draw_menu(true);
std::atomic<bool> aimbot_switch(false);
std::atomic<float> aim_fov(100.0f);

// C++ 後台 ImGui 渲染與數據處理主線程
void OverlayThread() {
    #if defined(__ANDROID__)
    // 1. 初始化 EGL (EGLDisplay, EGLConfig, EGLContext)
    // 2. 初始化 ImGui 上下文 (ImGui::CreateContext())
    // 3. 綁定 Android 輸入事件
    #endif

    while (is_running) {
        if (draw_menu) {
            // ----這裡放你經典的 Dear ImGui 菜單代碼 ----
            // ImGui::Begin("XS PRO - AI Menu", &draw_menu);
            // ImGui::Checkbox("Aimbot Enable", &aimbot_switch);
            // ImGui::SliderFloat("FOV Size", &aim_fov, 10.0f, 360.0f);
            // ImGui::End();
        }

        // 模擬渲染同步與降低 CPU 負載（一秒 60 幀約 16 毫秒）
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    #if defined(__ANDROID__)
    // 釋放 ImGui 與 EGL 資源
    #endif
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

    // 提供給 Flet 遠端強制切換選單顯示狀態的接口
    EXPORT void ToggleMenu(bool show) {
        draw_menu = show;
    }

    // 獲取目前 C++ 內部的選單參數（供 Flet 讀取同步狀態）
    EXPORT bool GetAimbotState() { return aimbot_switch; }
    EXPORT float GetAimFov() { return aim_fov; }
}
