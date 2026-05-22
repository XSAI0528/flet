#include <thread>
#include <chrono>
#include <atomic>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <GL/gl.h>
    #pragma comment(lib, "opengl32.lib")
    #pragma comment(lib, "user32.lib")
    #pragma comment(lib, "gdi32.lib")
    #define EXPORT __declspec(dllexport)
    
    // 引入 ImGui 與對應的 Windows/OpenGL3 後端標頭檔
    #include "imgui/imgui.h"
    #include "imgui/imgui_impl_win32.h"
    #include "imgui/imgui_impl_opengl3.h"

    // 聲明外部 ImGui 的 Win32 訊息處理函式
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#else
    #define EXPORT __attribute__((visibility("default")))
    #if defined(__ANDROID__)
        #include <android/native_window.h>
        #include <android/native_window_jni.h>
    #endif
#endif

// ---- 共享控制變數 ----
std::atomic<bool> is_running(false);
std::thread overlay_thread;

std::atomic<bool> draw_menu(true);      
std::atomic<bool> aimbot_switch(false);  
std::atomic<float> aim_fov(100.0f);     

#if defined(_WIN32) || defined(_WIN64)
// Windows 訊息回呼：讓 ImGui 優先攔截滑鼠與鍵盤事件
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
        case WM_CLOSE:
            is_running = false;
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
#endif

void OverlayThread() {
#if defined(_WIN32) || defined(_WIN64)
    // 1. 註冊視窗並將標題由 UTF-8 轉為 Windows 寬字元 (解決亂碼)
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "XS_Menu", NULL };
    RegisterClassEx(&wc);
    
    HWND hwnd = CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, L"XS PRO - Windows 測試菜單", WS_OVERLAPPEDWINDOW, 100, 100, 450, 400, NULL, NULL, wc.hInstance, NULL);
    
    // 2. 初始化 OpenGL
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
    HDC hdc = GetDC(hwnd);
    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);
    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // 3. 初始化 ImGui 上下文與後端繫結
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); // 使用經典暗黑主題

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplOpenGL3_Init("#version 130");

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    
    // 4. 進入渲染循環
    while (is_running && msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // 開啟 ImGui 新的一幀
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 畫布背景清空
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (draw_menu) {
            // ---- 經典 Dear ImGui 選單繪製開始 ----
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size, ImGuiCond_Always);
            
            ImGui::Begin("XS PRO AI - 內嵌主菜單", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
            
            ImGui::TextColored(ImVec4(0.0f, 0.7f, 1.0f, 1.0f), "XS AI 核心底層整合架構");
            ImGui::Separator();

            // 實作自瞄打勾勾：直接綁定底層共用全域變數
            bool local_aim = aimbot_switch.load();
            if (ImGui::Checkbox("開啟 AI 自動對齊 (Aimbot)", &local_aim)) {
                aimbot_switch.store(local_aim);
            }

            float local_fov = aim_fov.load();
            if (ImGui::SliderFloat("鎖定範圍 (FOV)", &local_fov, 10.0f, 360.0f, "%.1f px")) {
                aim_fov.store(local_fov);
            }

            ImGui::End();
            // ---- ImGui 選單繪製結束 ----
        }

        // 執行圖形渲染與緩衝交換
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SwapBuffers(hdc);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); 
    }

    // 5. 安全清理資源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
#endif
}

extern "C" {
    EXPORT void StartOverlay() { if (!is_running) { is_running = true; overlay_thread = std::thread(OverlayThread); } }
    EXPORT void StopOverlay() { if (is_running) { is_running = false; if (overlay_thread.joinable()) overlay_thread.join(); } }
    EXPORT void ToggleMenu(bool show) { draw_menu = show; }
    EXPORT bool GetAimbotState() { return aimbot_switch.load(); }
    EXPORT float GetAimFov() { return aim_fov.load(); }
}
