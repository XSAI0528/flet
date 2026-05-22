#include <thread>
#include <chrono>
#include <atomic>

// 引入 Windows 原生視窗與圖形庫
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <GL/gl.h>
    #pragma comment(lib, "opengl32.lib")
    #pragma comment(lib, "user32.lib")
    #pragma comment(lib, "gdi32.lib")
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

// 這裡假設你已經把 imgui 放入子目錄，未來正式編譯需要解開以下註解
// #include "imgui/imgui.h"

std::atomic<bool> is_running(false);
std::thread overlay_thread;

std::atomic<bool> draw_menu(true);
std::atomic<bool> aimbot_switch(false);
std::atomic<float> aim_fov(100.0f);

// Windows 視窗訊息處理回呼函式
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

// C++ 后台獨立線程
void OverlayThread() {
#if defined(_WIN32) || defined(_WIN64)
    // 1. 註冊 Windows 視窗類別
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "XS_Menu", NULL };
    RegisterClassEx(&wc);
    
    // 2. 建立隱形或獨立的測試視窗
    HWND hwnd = CreateWindow(wc.lpszClassName, "XS PRO - Windows 測試菜單", WS_OVERLAPPEDWINDOW, 100, 100, 450, 350, NULL, NULL, wc.hInstance, NULL);
    
    // 3. 初始化基底 OpenGL 上下文
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
    HDC hdc = GetDC(hwnd);
    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);
    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    // 顯示視窗
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // 這裡未來初始化 ImGui Windows 後端
    // ImGui::CreateContext();
    // ImGui_ImplWin32_Init(hwnd);
    // ImGui_ImplOpenGL3_Init();

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    
    while (is_running && msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // ---- 這裡就是繪製迴圈 ----
        // 畫布清空
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (draw_menu) {
            // 未來這裡放正式的 ImGui 渲染碼
            // ImGui_ImplOpenGL3_NewFrame();
            // ImGui_ImplWin32_NewFrame();
            // ImGui::NewFrame();
            
            // ImGui::Begin("XS PRO AI");
            // ImGui::Checkbox("Aimbot Switch", (bool*)&aimbot_switch);
            // ImGui::End();
            
            // ImGui::Render();
            // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SwapBuffers(hdc);
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 鎖定 ~60 幀
    }

    // 釋放資源
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
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

    EXPORT void ToggleMenu(bool show) { draw_menu = show; }
    EXPORT bool GetAimbotState() { return aimbot_switch; }
    EXPORT float GetAimFov() { return aim_fov; }
}
