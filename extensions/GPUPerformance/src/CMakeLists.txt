
# Build a shared library (DLL) named gpu_perf.
add_library(GPUPerformance_x64 SHARED
    gpu_perf.cpp
    gpu_perf.h
)

# C++17 for std::size_t usage etc.
target_compile_features(GPUPerformance_x64 PUBLIC cxx_std_17)

# Ensure __declspec(dllexport) is used when building the DLL.
target_compile_definitions(GPUPerformance_x64 PRIVATE GPU_PERF_EXPORTS)

# Link against the Windows registry APIs (advapi32).
target_link_libraries(GPUPerformance_x64 PRIVATE advapi32)

# Place output in a predictable location.
set_target_properties(GPUPerformance_x64 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)