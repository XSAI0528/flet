#include "imgui.h"

namespace ImGui {
    ImGuiContext* CreateContext(void* shared_font_atlas) { 
        static ImGuiContext dummy_ctx;
        return &dummy_ctx; 
    }
    void DestroyContext(ImGuiContext* ctx) {}
    void NewFrame() {}
    void Render() {}
    ImDrawData* GetDrawData() { return nullptr; }
    bool Begin(const char* name, bool* p_open, int flags) { return true; }
    void End() {}
    void Text(const char* fmt, ...) {}
    bool Button(const char* label, const float& size) { return true; }
}
