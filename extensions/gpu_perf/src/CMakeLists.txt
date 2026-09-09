cmake_minimum_required(VERSION 4.0)

project(gpu_perf LANGUAGES CXX)

# Build a shared library (DLL) named gpu_perf.
add_library(gpu_perf SHARED
    gpu_perf.cpp
    gpu_perf.h
)

# C++17 for std::size_t usage etc.
target_compile_features(gpu_perf PUBLIC cxx_std_17)

# Ensure __declspec(dllexport) is used when building the DLL.
target_compile_definitions(gpu_perf PRIVATE GPU_PERF_EXPORTS)

# Link against the Windows registry APIs (advapi32).
target_link_libraries(gpu_perf PRIVATE advapi32)

# Place output in a predictable location.
set_target_properties(gpu_perf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)