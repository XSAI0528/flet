#pragma once
namespace ImGui {
    struct ImGuiContext;
    ImGuiContext* CreateContext(void* shared_font_atlas = nullptr);
    void DestroyContext(ImGuiContext* ctx = nullptr);
    struct ImGuiIO& GetIO();
    void NewFrame();
    void Render();
    struct ImDrawData* GetDrawData();
    bool Begin(const char* name, bool* p_open = nullptr, int flags = 0);
    void End();
    void Text(const char* fmt, ...);
    bool Button(const char* label, const float& size = 0.0f);
}
struct ImGuiIO {
    bool WantCaptureMouse;
};
