#pragma once

namespace ImGui {
    struct ImGuiContext {};
    struct ImDrawData {};

    ImGuiContext* CreateContext(void* shared_font_atlas = nullptr);
    void DestroyContext(ImGuiContext* ctx = nullptr);
    void NewFrame();
    void Render();
    ImDrawData* GetDrawData();
    bool Begin(const char* name, bool* p_open = nullptr, int flags = 0);
    void End();
    void Text(const char* fmt, ...);
    bool Button(const char* label, const float& size = 0.0f);
}
