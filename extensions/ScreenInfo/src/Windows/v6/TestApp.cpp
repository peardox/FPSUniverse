#include <windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <conio.h>
#include "WindowHook.h"

// Create a simple window for testing
HWND CreateTestWindow() {
    const char* className = "TestWindowClass";
    
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    if (!RegisterClassA(&wc)) {
        return NULL;
    }
    
    return CreateWindowA(
        className,
        "Test Window for Move/Resize Events",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );
}

void PrintEvent(const WindowEventInfo& event) {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "MOST RECENT EVENT: " << event.changeType << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "Position:   (" << event.windowRect.left << ", " << event.windowRect.top << ")" << std::endl;
    std::cout << "Size:       " << (event.windowRect.right - event.windowRect.left) 
              << " x " << (event.windowRect.bottom - event.windowRect.top) << std::endl;
    std::cout << "Monitor:    " << event.monitorName << std::endl;
    std::cout << "Handle:     0x" << std::hex << event.hwnd << std::dec << std::endl;
    std::cout << "Thread:     " << event.dwEventThread << std::endl;
    std::cout << "Time:       " << event.dwmsEventTime << " ms" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
}

void ShowMenu() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "CURRENT PROCESS WINDOW EVENT MONITOR (RECENT EVENTS ONLY)" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  [S] - Show most recent events (by category)" << std::endl;
    std::cout << "  [C] - Clear event buffer" << std::endl;
    std::cout << "  [N] - Show event category count" << std::endl;
    std::cout << "  [R] - Show specific recent event type" << std::endl;
    std::cout << "  [A] - Auto-show new events (toggle)" << std::endl;
    std::cout << "  [T] - Create test window" << std::endl;
    std::cout << "  [Q] - Quit" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "Note: Only the most recent event of each type is kept." << std::endl;
}

int main() {
    std::cout << "Current Process Window Event Monitor (Recent Events Only)" << std::endl;
    std::cout << "=========================================================" << std::endl;
    std::cout << "Process ID: " << GetCurrentProcessId() << std::endl;
    std::cout << "This will ONLY monitor windows belonging to this process." << std::endl;
    std::cout << "Only the most recent event in each category will be stored." << std::endl;
    
    // Load DLL
    HMODULE hDll = LoadLibraryA("WindowHook.dll");
    if (!hDll) {
        std::cerr << "ERROR: Failed to load WindowHook.dll" << std::endl;
        return 1;
    }

    // Get function pointers
    auto StartHook = (BOOL(__cdecl *)(DWORD, DWORD))GetProcAddress(hDll, "StartHook");
    auto StopHook = (BOOL(__cdecl *)())GetProcAddress(hDll, "StopHook");
    auto EnableBuffering = (BOOL(__cdecl *)(BOOL))GetProcAddress(hDll, "EnableBuffering");
    auto GetBufferedEventCount = (int(__cdecl *)())GetProcAddress(hDll, "GetBufferedEventCount");
    auto GetBufferedEvent = (BOOL(__cdecl *)(int, WindowEventInfo*))GetProcAddress(hDll, "GetBufferedEvent");
    auto ClearEventBuffer = (void(__cdecl *)())GetProcAddress(hDll, "ClearEventBuffer");
    auto GetMonitoredProcessId = (DWORD(__cdecl *)())GetProcAddress(hDll, "GetMonitoredProcessId");
    auto GetMostRecentEvent = (BOOL(__cdecl *)(DWORD, WindowEventInfo*))GetProcAddress(hDll, "GetMostRecentEvent");
    auto GetAllRecentEvents = (int(__cdecl *)(WindowEventInfo*, int))GetProcAddress(hDll, "GetAllRecentEvents");

    if (!StartHook || !StopHook || !EnableBuffering || !GetBufferedEventCount || 
        !GetBufferedEvent || !ClearEventBuffer || !GetAllRecentEvents) {
        std::cerr << "ERROR: Failed to get function addresses" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    // Enable buffering and start hook
    EnableBuffering(TRUE);
    
    if (!StartHook(0, 0)) {
        std::cerr << "ERROR: Failed to start hook" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    std::cout << "Hook started successfully!" << std::endl;
    if (GetMonitoredProcessId) {
        std::cout << "Monitoring process ID: " << GetMonitoredProcessId() << std::endl;
    }
    std::cout << "Most recent events are being tracked for THIS PROCESS ONLY..." << std::endl;
    
    // Create a test window automatically
    HWND testWindow = CreateTestWindow();
    if (testWindow) {
        std::cout << "Test window created! Try moving/resizing it." << std::endl;
    }
    
    bool autoShow = false;
    int lastEventCount = 0;
    
    ShowMenu();
    
    // Main command loop
    while (true) {
        // Check for auto-show mode
        if (autoShow) {
            int currentCount = GetBufferedEventCount();
            if (currentCount != lastEventCount) {
                // Show all current recent events
                WindowEventInfo events[10];
                int count = GetAllRecentEvents ? GetAllRecentEvents(events, 10) : 0;
                
                std::cout << "\n*** AUTO-UPDATE: Current Recent Events ***" << std::endl;
                for (int i = 0; i < count; i++) {
                    PrintEvent(events[i]);
                }
                lastEventCount = currentCount;
                std::cout << "\nPress any key for menu...";
            }
        }
        
        // Check for user input
        if (_kbhit()) {
            char cmd = toupper(_getch());
            
            switch (cmd) {
                case 'S': // Show all recent events
                {
                    WindowEventInfo events[10];
                    int count = GetAllRecentEvents ? GetAllRecentEvents(events, 10) : 0;
                    
                    if (count == 0) {
                        std::cout << "\nNo recent events available." << std::endl;
                    } else {
                        std::cout << "\nShowing " << count << " most recent events (by category):" << std::endl;
                        for (int i = 0; i < count; i++) {
                            PrintEvent(events[i]);
                        }
                    }
                    ShowMenu();
                    break;
                }
                
                case 'C': // Clear buffer
                    ClearEventBuffer();
                    lastEventCount = 0;
                    std::cout << "\nEvent buffer cleared." << std::endl;
                    ShowMenu();
                    break;
                
                case 'N': // Show count
                {
                    int count = GetBufferedEventCount();
                    std::cout << "\nCurrent buffer contains " << count << " event categories with data." << std::endl;
                    ShowMenu();
                    break;
                }
                
                case 'R': // Show specific event type
                {
                    std::cout << "\nSelect event type:" << std::endl;
                    std::cout << "1 - Move/Resize Start" << std::endl;
                    std::cout << "2 - Move/Resize End" << std::endl;
                    std::cout << "3 - Position/Size Change" << std::endl;
                    std::cout << "4 - Z-Order Change" << std::endl;
                    std::cout << "Choice (1-4): ";
                    
                    char choice = _getch();
                    DWORD eventType = 0;
                    
                    switch (choice) {
                        case '1': eventType = 0x000A; break; // EVENT_SYSTEM_MOVESIZESTART
                        case '2': eventType = 0x000B; break; // EVENT_SYSTEM_MOVESIZEEND
                        case '3': eventType = 0x800B; break; // EVENT_OBJECT_LOCATIONCHANGE
                        case '4': eventType = 0x8004; break; // EVENT_OBJECT_REORDER
                        default: 
                            std::cout << "\nInvalid choice." << std::endl;
                            ShowMenu();
                            continue;
                    }
                    
                    if (GetMostRecentEvent) {
                        WindowEventInfo event;
                        if (GetMostRecentEvent(eventType, &event)) {
                            std::cout << std::endl;
                            PrintEvent(event);
                        } else {
                            std::cout << "\nNo recent event of this type found." << std::endl;
                        }
                    }
                    ShowMenu();
                    break;
                }
                
                case 'A': // Toggle auto-show
                    autoShow = !autoShow;
                    if (autoShow) {
                        lastEventCount = GetBufferedEventCount();
                        std::cout << "\nAuto-show mode ENABLED. Recent events will be displayed when updated." << std::endl;
                    } else {
                        std::cout << "\nAuto-show mode DISABLED." << std::endl;
                    }
                    ShowMenu();
                    break;
                
                case 'T': // Create test window
                {
                    HWND newWindow = CreateTestWindow();
                    if (newWindow) {
                        std::cout << "\nNew test window created! Try moving/resizing it." << std::endl;
                    } else {
                        std::cout << "\nFailed to create test window." << std::endl;
                    }
                    ShowMenu();
                    break;
                }
                
                case 'Q': // Quit
                    goto exit_loop;
                
                default:
                    ShowMenu();
                    break;
            }
        }
        
        // Process Windows messages (important for window events)
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto exit_loop;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        Sleep(50);
    }

exit_loop:
    std::cout << "\nStopping hook..." << std::endl;
    StopHook();
    
    // Show final statistics
    int finalCount = GetBufferedEventCount();
    std::cout << "Final buffer contained " << finalCount << " event categories." << std::endl;
    
    FreeLibrary(hDll);
    std::cout << "Application terminated." << std::endl;
    return 0;
}