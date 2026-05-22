#include "imgui.h"
namespace ImGui {
    ImGuiContext* CreateContext(void* shared_font_atlas) { return nullptr; }
    void DestroyContext(ImGuiContext* ctx) {}
    ImGuiIO& GetIO() { static ImGuiIO io; return io; }
    void NewFrame() {}
    void Render() {}
    ImDrawData* GetDrawData() { return nullptr; }
    bool Begin(const char* name, bool* p_open, int flags) { return true; }
    void End() {}
    void Text(const char* fmt, ...) {}
    bool Button(const char* label, const float& size) { return true; }
}
