cmake_minimum_required(VERSION 3.10.2)
project("imgui_overlay")

add_library(imgui_overlay SHARED overlay.cpp)

target_link_libraries(imgui_overlay
    android
    log
    EGL
    GLESv3
)
