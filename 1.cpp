#include <thread>
#include <chrono>
#include <atomic>

// 1. 自動判斷平台並載入對應的圖形系統標頭檔
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <GL/gl.h>
    #pragma comment(lib, "opengl32.lib")
    #pragma comment(lib, "user32.lib")
    #pragma comment(lib, "gdi32.lib")
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
    #if defined(__ANDROID__)
        #include <android/native_window.h>
        #include <android/native_window_jni.h>
    #endif
#endif

// ---- 共享控制變數（Flet 前端與 C++ 後台雙向同步） ----
std::atomic<bool> is_running(false);
std::thread overlay_thread;

std::atomic<bool> draw_menu(true);      // 控制選單顯示/隱藏
std::atomic<bool> aimbot_switch(false);  // 自瞄功能開關
std::atomic<float> aim_fov(100.0f);     // 自瞄範圍範圍

// Windows 環境專用的視窗訊息事件回呼
#if defined(_WIN32) || defined(_WIN64)
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

// ---- C++ 後台獨立渲染與資料處理線程 ----
void OverlayThread() {
#if defined(_WIN32) || defined(_WIN64)
    // 【Windows 渲染初始化】
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "XS_Menu", NULL };
    RegisterClassEx(&wc);
    
    // 建立本地端除錯選單視窗
    HWND hwnd = CreateWindow(wc.lpszClassName, "XS PRO - Windows 測試菜單", WS_OVERLAPPEDWINDOW, 100, 100, 450, 350, NULL, NULL, wc.hInstance, NULL);
    
    // 初始化 Windows 本地 OpenGL 加速
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
    HDC hdc = GetDC(hwnd);
    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);
    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    
    // Windows 渲染主循環
    while (is_running && msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // 清空背景（深灰色底）
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (draw_menu) {
            // 【未來此處塞入 Dear ImGui 繪製邏輯】
            // 例如：ImGui 點擊打勾後，將 aimbot_switch 改為 true/false
        }

        SwapBuffers(hdc);
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 鎖定約 60 幀
    }

    // 釋放 Windows 圖形資源
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

#elif defined(__ANDROID__)
    // 【Android 渲染初始化】
    // 未來在此處執行：
    // 1. EGL Display 與 Context 綁定
    // 2. 透過 ANativeWindow 鎖定頂層透明 Surface
    while (is_running) {
        if (draw_menu) {
            // Android 端的 ImGui 選單刷新
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
#endif
}

// ---- 外露給 Flet (Python) 呼叫的 C 接口 ----
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

    EXPORT void ToggleMenu(bool show) { 
        draw_menu = show; 
    }
    
    EXPORT bool GetAimbotState() { 
        return aimbot_switch.load(); 
    }
    
    EXPORT float GetAimFov() { 
        return aim_fov.load(); 
    }
}
