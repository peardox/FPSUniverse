#include <windows.h>

// Custom message code
static const UINT WM_FULLSCREEN_ENTERED = WM_USER + 100;

// Poll interval in milliseconds
static const DWORD POLL_INTERVAL_MS = 500;

// Thread control
static HANDLE g_hThread = NULL;
static HANDLE g_hStopEvent = NULL;

// Keeps track of the last window we reported as fullscreen
static HWND g_lastFullscreenWnd = NULL;

// Helper: returns true if hwnd is a popup window that exactly covers its monitor
bool IsWindowExclusiveFullscreen(HWND hwnd) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd))
        return false;

    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    if ((style & WS_POPUP) == 0)
        return false;

    // Get window rect
    RECT wr = {};
    if (!GetWindowRect(hwnd, &wr))
        return false;

    // Find which monitor the window is mostly on
    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    if (!hMon)
        return false;

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMon, &mi);

    // Compare window rect to monitor rect
    return (wr.left   <= mi.rcMonitor.left   &&
            wr.top    <= mi.rcMonitor.top    &&
            wr.right  >= mi.rcMonitor.right  &&
            wr.bottom >= mi.rcMonitor.bottom);
}

// Worker thread: polls foreground window
DWORD WINAPI WatcherThread(LPVOID) {
    while (WaitForSingleObject(g_hStopEvent, POLL_INTERVAL_MS) == WAIT_TIMEOUT) {
        HWND fg = GetForegroundWindow();
        if (fg != NULL && IsWindowExclusiveFullscreen(fg)) {
            if (fg != g_lastFullscreenWnd) {
                // Fire the message exactly once per window transition
                PostMessage(HWND_BROADCAST,
                            WM_FULLSCREEN_ENTERED,
                            (WPARAM)fg,
                            0);
                g_lastFullscreenWnd = fg;
            }
        } else {
            // reset when no longer fullscreen
            g_lastFullscreenWnd = NULL;
        }
    }
    return 0;
}

// DLL entry
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        // Create stop event
        g_hStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (!g_hStopEvent)
            return FALSE;
        // Launch watcher thread
        g_hThread = CreateThread(
            nullptr, 0, WatcherThread, nullptr, 0, nullptr);
        if (!g_hThread) {
            CloseHandle(g_hStopEvent);
            g_hStopEvent = NULL;
            return FALSE;
        }
        break;

    case DLL_PROCESS_DETACH:
        if (g_hStopEvent) {
            // Signal thread to terminate
            SetEvent(g_hStopEvent);
            if (g_hThread) {
                WaitForSingleObject(g_hThread, INFINITE);
                CloseHandle(g_hThread);
                g_hThread = NULL;
            }
            CloseHandle(g_hStopEvent);
            g_hStopEvent = NULL;
        }
        break;
    }
    return TRUE;
}