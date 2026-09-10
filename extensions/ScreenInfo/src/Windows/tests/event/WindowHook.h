#ifndef WINDOWHOOK_H
#define WINDOWHOOK_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>

#ifdef WINDOWHOOK_EXPORTS
#define WINDOWHOOK_API __declspec(dllexport)
#else
#define WINDOWHOOK_API __declspec(dllimport)
#endif

// Event categories for tracking most recent events
#define MAX_EVENT_CATEGORIES 5

// Event information structure
struct WindowEventInfo {
    DWORD event;
    HWND hwnd;
    LONG idObject;
    LONG idChild;
    DWORD dwEventThread;
    DWORD dwmsEventTime;
    RECT windowRect;
    char monitorName[128];
    int monitorIndex;
    char changeType[64];
    BOOL isValid;  // New field to indicate if this slot contains valid data
};

// Define the callback type
typedef void(__cdecl *EventCallback)(DWORD event, LONG idObject, HWND hwnd, 
                                    LONG idChild, LONG dwEventThread, 
                                    DWORD dwmsEventTime, DWORD hwndParam);

extern "C" {
    WINDOWHOOK_API BOOL __cdecl StartHook(DWORD processId = 0, DWORD threadId = 0);
    WINDOWHOOK_API BOOL __cdecl StopHook();
    WINDOWHOOK_API void __cdecl SetEventCallback(EventCallback callback);
    
    // Buffering functions (modified for category-based storage)
    WINDOWHOOK_API BOOL __cdecl EnableBuffering(BOOL enable);
    WINDOWHOOK_API int __cdecl GetBufferedEventCount();
    WINDOWHOOK_API BOOL __cdecl GetBufferedEvent(int index, WindowEventInfo* eventInfo);
    WINDOWHOOK_API void __cdecl ClearEventBuffer();
    WINDOWHOOK_API int __cdecl GetAndClearEvents(WindowEventInfo* events, int maxEvents);
    
    // New functions for category-based access
    WINDOWHOOK_API BOOL __cdecl GetMostRecentEvent(DWORD eventType, WindowEventInfo* eventInfo);
    WINDOWHOOK_API int __cdecl GetAllRecentEvents(WindowEventInfo* events, int maxEvents);
    
    // Process monitoring
    WINDOWHOOK_API DWORD __cdecl GetMonitoredProcessId();
}

#endif