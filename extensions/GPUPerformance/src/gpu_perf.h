#ifndef GPU_PERF_H
#define GPU_PERF_H

#ifdef GPU_PERF_EXPORTS
#define GPU_PERF_API __declspec(dllexport)
#else
#define GPU_PERF_API __declspec(dllimport)
#endif

extern "C" {

// Exported wrapper: query the current GPU preference for the given exe.
// Returns 0, 1 or 2 on success.
//   -1 if the UserGpuPreferences key does not exist.
//   -2 on any other failure.
GPU_PERF_API double gpu_get_performance(char* exefile);

// Exported wrapper: set the GPU preference for the given exe.
//   perf: 0 = let Windows decide, 1 = power saving, 2 = high performance
// Returns 1.0 on success, 0.0 on any failure.
GPU_PERF_API double gpu_set_performance(char* exefile, double perf);

}  // extern "C"

#endif  // GPU_PERF_H
