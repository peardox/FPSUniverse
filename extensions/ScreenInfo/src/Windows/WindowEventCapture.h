#ifndef WINDOW_EVENT_CAPTURE_H
#define WINDOW_EVENT_CAPTURE_H

#include <windows.h>

#ifdef BUILDING_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

typedef enum {
    EVENT_WINDOW_MOVE = 1,
    EVENT_WINDOW_SIZE = 2
} WindowEventType;

typedef struct {
    HWND hwnd;
    WindowEventType eventType;
    RECT rect;
    DWORD timestamp;
} WindowEvent;

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPORT BOOL InstallHook();
DLL_EXPORT BOOL UninstallHook();
DLL_EXPORT int GetWindowEvents(WindowEvent* events, int maxEvents);
DLL_EXPORT void ClearEventBuffer();

#ifdef __cplusplus
}
#endif

#endif // WINDOW_EVENT_CAPTURE_H