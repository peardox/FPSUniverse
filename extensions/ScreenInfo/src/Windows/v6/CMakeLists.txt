cmake_minimum_required(VERSION 3.10)
project(WindowHookProject)

set(CMAKE_CXX_STANDARD 17)

# DLL
add_library(WindowHook SHARED
    WindowHook.cpp
    WindowHook.def
)

target_compile_definitions(WindowHook PRIVATE WINDOWHOOK_EXPORTS)

# Test application
add_executable(TestApp
    TestApp.cpp
)

target_link_libraries(TestApp WindowHook)
