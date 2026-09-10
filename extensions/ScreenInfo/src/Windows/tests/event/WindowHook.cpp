#include "WindowHook.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <winuser.h>

// Define the specific events we care about
#define EVENT_SYSTEM_MOVESIZEEND    0x000B
#define EVENT_OBJECT_LOCATIONCHANGE 0x800B
#define EVENT_SYSTEM_DESKTOPSWITCH  0x0020

// Global variables - modified to use array of hooks
static HWINEVENTHOOK g_hHooks[2] = {NULL, NULL};
static int g_hookCount = 0;
static HMODULE g_hModule = NULL;
static EventCallback g_EventCallback = nullptr;
static DWORD g_currentProcessId = 0;

// Category-based buffering variables
static BOOL g_bufferingEnabled = FALSE;
static std::map<DWORD, WindowEventInfo> g_mostRecentEvents;
static CRITICAL_SECTION g_bufferCS;
static BOOL g_csInitialized = FALSE;

// Helper function to get monitor info
std::string GetMonitorInfoString(HWND hwnd, int& monitorIndex) {
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEXA mi;
    mi.cbSize = sizeof(mi);
    
    if (GetMonitorInfoA(hMonitor, &mi)) {
        monitorIndex = (int)(((uintptr_t)hMonitor) % 10);
        
        std::ostringstream oss;
        oss << mi.szDevice << " (" 
            << (mi.rcMonitor.right - mi.rcMonitor.left) << "x"
            << (mi.rcMonitor.bottom - mi.rcMonitor.top) << ")";
        return oss.str();
    }
    
    monitorIndex = 0;
    return "Unknown Monitor";
}

// Helper function to determine change type
std::string DetermineChangeType(DWORD event, HWND hwnd) {
    switch (event) {
        case EVENT_SYSTEM_MOVESIZEEND:
            return "MOVE/RESIZE END";
        case EVENT_OBJECT_LOCATIONCHANGE:
            return "LOCATION CHANGE";
        default:
            return "OTHER";
    }
}

// Add/Update most recent event for category (thread-safe)
void UpdateMostRecentEvent(const WindowEventInfo& eventInfo) {
    if (!g_bufferingEnabled || !g_csInitialized) return;
    
    EnterCriticalSection(&g_bufferCS);
    
    // Store/update the most recent event for this event type
    g_mostRecentEvents[eventInfo.event] = eventInfo;
    
    LeaveCriticalSection(&g_bufferCS);
}

// Check if window belongs to our process
BOOL IsOurProcessWindow(HWND hwnd) {
    DWORD windowProcessId;
    GetWindowThreadProcessId(hwnd, &windowProcessId);
    return (windowProcessId == g_currentProcessId);
}

// Event callback function - only process events from our process
void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD dwEventThread,
    DWORD dwmsEventTime
) {
    // Only process window events
    if (hwnd == NULL || idObject != OBJID_WINDOW) {
        return;
    }

    // IMPORTANT: Only process windows from our own process
    if (!IsOurProcessWindow(hwnd)) {
        return;
    }

    // Filter to only the events we care about
    if (event != EVENT_SYSTEM_MOVESIZEEND && 
        event != EVENT_OBJECT_LOCATIONCHANGE) {
        return;
    }

    // Get window rectangle
    RECT windowRect = {0};
    GetWindowRect(hwnd, &windowRect);
    
    // Get monitor information
    int monitorIndex;
    std::string monitorInfo = GetMonitorInfoString(hwnd, monitorIndex);
    
    // Create event info structure
    WindowEventInfo eventInfo = {0};
    eventInfo.event = event;
    eventInfo.hwnd = hwnd;
    eventInfo.idObject = idObject;
    eventInfo.idChild = idChild;
    eventInfo.dwEventThread = dwEventThread;
    eventInfo.dwmsEventTime = dwmsEventTime;
    eventInfo.windowRect = windowRect;
    eventInfo.monitorIndex = monitorIndex;
    eventInfo.isValid = TRUE;
    
    strncpy_s(eventInfo.monitorName, monitorInfo.c_str(), sizeof(eventInfo.monitorName) - 1);
    
    std::string changeType = DetermineChangeType(event, hwnd);
    strncpy_s(eventInfo.changeType, changeType.c_str(), sizeof(eventInfo.changeType) - 1);

    // Add debug output to verify we're only getting our process events
    std::string debugMsg = "Event for our process: (" + changeType + ")\n";
    OutputDebugStringA(debugMsg.c_str());

    // Update most recent event if enabled
    if (g_bufferingEnabled) {
        UpdateMostRecentEvent(eventInfo);
    }

    // Call user-defined callback if set
    if (g_EventCallback) {
        g_EventCallback(event, idObject, hwnd, idChild, 
                       (LONG)dwEventThread, dwmsEventTime, (DWORD)(uintptr_t)hwnd);
    }
}

extern "C" {
    WINDOWHOOK_API BOOL __cdecl StartHook(DWORD processId, DWORD threadId) {
        if (g_hookCount > 0) {
            return FALSE;  // Already started
        }

        // Store our current process ID for filtering
        g_currentProcessId = GetCurrentProcessId();

        // Initialize critical section if not done
        if (!g_csInitialized) {
            InitializeCriticalSection(&g_bufferCS);
            g_csInitialized = TRUE;
        }

        // Hook for system events (0x000A to 0x001E) - covers MOVESIZESTART, MOVESIZEEND, DISPLAYCHANGE
        g_hHooks[0] = SetWinEventHook(
            EVENT_SYSTEM_MOVESIZEEND,   // 0x000A
            EVENT_SYSTEM_MOVESIZEEND,   // 0x000A
            NULL,
            WinEventProc,
            g_currentProcessId,
            threadId,
            WINEVENT_OUTOFCONTEXT
        );

        // Hook for object events (0x8004 to 0x800B) - covers REORDER, LOCATIONCHANGE
        g_hHooks[1] = SetWinEventHook(
            EVENT_OBJECT_LOCATIONCHANGE,
            EVENT_OBJECT_LOCATIONCHANGE,
            NULL,
            WinEventProc,
            g_currentProcessId,
            threadId,
            WINEVENT_OUTOFCONTEXT
        );

        // Check if both hooks were successful
        if (g_hHooks[0] && g_hHooks[1]) {
            g_hookCount = 2;
            return TRUE;
        }

        // Cleanup on failure
        if (g_hHooks[0]) {
            UnhookWinEvent(g_hHooks[0]);
            g_hHooks[0] = NULL;
        }
        if (g_hHooks[1]) {
            UnhookWinEvent(g_hHooks[1]);
            g_hHooks[1] = NULL;
        }
        g_hookCount = 0;
        
        return FALSE;
    }

    WINDOWHOOK_API BOOL __cdecl StopHook() {
        if (g_hookCount == 0) {
            return FALSE;
        }

        BOOL result = TRUE;
        
        // Unhook all active hooks
        for (int i = 0; i < 2; i++) {
            if (g_hHooks[i] != NULL) {
                result &= UnhookWinEvent(g_hHooks[i]);
                g_hHooks[i] = NULL;
            }
        }
        
        g_hookCount = 0;
        return result;
    }

    WINDOWHOOK_API void __cdecl SetEventCallback(EventCallback callback) {
        g_EventCallback = callback;
    }

    WINDOWHOOK_API BOOL __cdecl EnableBuffering(BOOL enable) {
        g_bufferingEnabled = enable;
        return TRUE;
    }

    // Modified to return count of different event categories with data
    WINDOWHOOK_API int __cdecl GetBufferedEventCount() {
        if (!g_csInitialized) return 0;
        
        EnterCriticalSection(&g_bufferCS);
        int count = (int)g_mostRecentEvents.size();
        LeaveCriticalSection(&g_bufferCS);
        
        return count;
    }

    // Modified to access events by index (order may vary)
    WINDOWHOOK_API BOOL __cdecl GetBufferedEvent(int index, WindowEventInfo* eventInfo) {
        if (!eventInfo || !g_csInitialized) return FALSE;
        
        EnterCriticalSection(&g_bufferCS);
        
        if (index < 0 || index >= (int)g_mostRecentEvents.size()) {
            LeaveCriticalSection(&g_bufferCS);
            return FALSE;
        }
        
        auto it = g_mostRecentEvents.begin();
        std::advance(it, index);
        *eventInfo = it->second;
        
        LeaveCriticalSection(&g_bufferCS);
        
        return TRUE;
    }

    WINDOWHOOK_API void __cdecl ClearEventBuffer() {
        if (!g_csInitialized) return;
        
        EnterCriticalSection(&g_bufferCS);
        g_mostRecentEvents.clear();
        LeaveCriticalSection(&g_bufferCS);
    }

    // Modified to return all recent events (one per category)
    WINDOWHOOK_API int __cdecl GetAndClearEvents(WindowEventInfo* events, int maxEvents) {
        if (!events || maxEvents <= 0 || !g_csInitialized) return 0;
        
        EnterCriticalSection(&g_bufferCS);
        
        int count = min((int)g_mostRecentEvents.size(), maxEvents);
        int i = 0;
        for (auto& pair : g_mostRecentEvents) {
            if (i >= count) break;
            events[i++] = pair.second;
        }
        
        g_mostRecentEvents.clear();
        
        LeaveCriticalSection(&g_bufferCS);
        
        return count;
    }

    // New function to get most recent event of specific type
    WINDOWHOOK_API BOOL __cdecl GetMostRecentEvent(DWORD eventType, WindowEventInfo* eventInfo) {
        if (!eventInfo || !g_csInitialized) return FALSE;
        
        EnterCriticalSection(&g_bufferCS);
        
        auto it = g_mostRecentEvents.find(eventType);
        if (it == g_mostRecentEvents.end()) {
            LeaveCriticalSection(&g_bufferCS);
            return FALSE;
        }
        
        *eventInfo = it->second;
        
        LeaveCriticalSection(&g_bufferCS);
        
        return TRUE;
    }

    // New function to get all recent events in a single call
    WINDOWHOOK_API int __cdecl GetAllRecentEvents(WindowEventInfo* events, int maxEvents) {
        if (!events || maxEvents <= 0 || !g_csInitialized) return 0;
        
        EnterCriticalSection(&g_bufferCS);
        
        int count = min((int)g_mostRecentEvents.size(), maxEvents);
        int i = 0;
        for (auto& pair : g_mostRecentEvents) {
            if (i >= count) break;
            events[i++] = pair.second;
        }
        
        LeaveCriticalSection(&g_bufferCS);
        
        return count;
    }

    // New function to get current process ID being monitored
    WINDOWHOOK_API DWORD __cdecl GetMonitoredProcessId() {
        return g_currentProcessId;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
        g_currentProcessId = GetCurrentProcessId(); // Store process ID on attach
        break;
    case DLL_PROCESS_DETACH:
        if (g_hookCount > 0) {
            StopHook();
        }
        if (g_csInitialized) {
            DeleteCriticalSection(&g_bufferCS);
            g_csInitialized = FALSE;
        }
        break;
    }
    return TRUE;
}