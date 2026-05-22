cmake_minimum_required(VERSION 3.10.2)
project("imgui_overlay")

# 把 imgui.cpp 一起加進來編譯
add_library(imgui_overlay SHARED overlay.cpp imgui.cpp)

target_link_libraries(imgui_overlay
    android
    log
    EGL
    GLESv3
)
