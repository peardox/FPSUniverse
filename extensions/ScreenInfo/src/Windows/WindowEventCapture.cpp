#include "WindowEventCapture.h"
#include <vector>
#include <algorithm>
#include <cstdio>

// Global variables
static HHOOK g_hHook = NULL;
static HINSTANCE g_hInstance = NULL;
static std::vector<WindowEvent> g_eventBuffer;
static CRITICAL_SECTION g_criticalSection;
static bool g_initialized = false;

// Initialize critical section
void InitializeBuffer() {
    if (!g_initialized) {
        InitializeCriticalSection(&g_criticalSection);
        g_initialized = true;
    }
}

// Cleanup critical section
void CleanupBuffer() {
    if (g_initialized) {
        DeleteCriticalSection(&g_criticalSection);
        g_initialized = false;
    }
}

// Add event to buffer (thread-safe)
void AddEvent(HWND hwnd, WindowEventType eventType, const RECT& rect) {
    if (!g_initialized) return;
    
    EnterCriticalSection(&g_criticalSection);
    
    WindowEvent event;
    event.hwnd = hwnd;
    event.eventType = eventType;
    event.rect = rect;
    event.timestamp = GetTickCount();
    
    g_eventBuffer.push_back(event);
    
    // Limit buffer size to prevent memory issues
    if (g_eventBuffer.size() > 10000) {
        g_eventBuffer.erase(g_eventBuffer.begin(), 
                           g_eventBuffer.begin() + 1000);
    }
    
    LeaveCriticalSection(&g_criticalSection);
}

// Hook procedure for capturing window messages
LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        CWPSTRUCT* pCwp = (CWPSTRUCT*)lParam;
        
        if (pCwp->message == WM_MOVE || pCwp->message == WM_SIZE) {
            RECT rect;
            if (GetWindowRect(pCwp->hwnd, &rect)) {
                // Only capture events for main application windows
                if (IsWindow(pCwp->hwnd) && IsWindowVisible(pCwp->hwnd)) {
                    LONG style = GetWindowLong(pCwp->hwnd, GWL_STYLE);
                    if (style & WS_CAPTION) { // Has title bar (main window)
                        WindowEventType eventType = 
                            (pCwp->message == WM_MOVE) ? 
                            EVENT_WINDOW_MOVE : EVENT_WINDOW_SIZE;
                        
                        AddEvent(pCwp->hwnd, eventType, rect);
                    }
                }
            }
        }
    }
    
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// Install the hook
BOOL InstallHook() {
    if (g_hHook != NULL) {
        return FALSE; // Already installed
    }
    
    InitializeBuffer();
    
    g_hHook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        CallWndProc,
        g_hInstance,
        0 // All threads
    );
    
    return g_hHook != NULL;
}

// Uninstall the hook
BOOL UninstallHook() {
    if (g_hHook == NULL) {
        return FALSE;
    }
    
    BOOL result = UnhookWindowsHookEx(g_hHook);
    g_hHook = NULL;
    
    CleanupBuffer();
    return result;
}

// Get buffered events and clear buffer
int GetWindowEvents(WindowEvent* events, int maxEvents) {
    if (!g_initialized || events == NULL || maxEvents <= 0) {
        return 0;
    }
    
    EnterCriticalSection(&g_criticalSection);
    
    int eventCount = min((int)g_eventBuffer.size(), maxEvents);
    
    for (int i = 0; i < eventCount; i++) {
        events[i] = g_eventBuffer[i];
    }
    
    // Clear the buffer after copying events
    g_eventBuffer.clear();
    
    LeaveCriticalSection(&g_criticalSection);
    
    return eventCount;
}

// Clear event buffer without returning events
void ClearEventBuffer() {
    if (!g_initialized) return;
    
    EnterCriticalSection(&g_criticalSection);
    g_eventBuffer.clear();
    LeaveCriticalSection(&g_criticalSection);
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, 
                     LPVOID lpReserved) {
	fprintf(stderr, "Called DLLMain\n");
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            g_hInstance = hModule;
            DisableThreadLibraryCalls(hModule);
            break;
            
            case DLL_PROCESS_DETACH:
            if (g_hHook != NULL) {
                UnhookWindowsHookEx(g_hHook);
            }
            CleanupBuffer();
            break;
    }
    return TRUE;
}