#include <jni.h>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include "imgui.h"

bool is_running = false;
pthread_t thread_id;

void* OverlayThread(void* arg) {
    // 初始化 ImGui 上下文
    ImGuiContext* ctx = ImGui::CreateContext();
    
    while (is_running) {
        ImGui::NewFrame();
        
        // 你的懸浮窗選單邏輯
        ImGui::Begin("Flet ImGui Window");
        ImGui::Text("Hello from C++ Base Overlay");
        ImGui::End();
        
        ImGui::Render();
        
        // 限制影格率 60 FPS
        usleep(16666); 
    }
    
    ImGui::DestroyContext(ctx);
    return nullptr;
}

extern "C" {
    __attribute__((visibility("default"))) void StartOverlay() {
        if (!is_running) {
            is_running = true;
            pthread_create(&thread_id, nullptr, OverlayThread, nullptr);
        }
    }

    __attribute__((visibility("default"))) void StopOverlay() {
        if (is_running) {
            is_running = false;
            pthread_join(thread_id, nullptr);
        }
    }
}
