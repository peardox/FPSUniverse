#include "WindowsChrome.h"
#include "Screens.h"
#include <string> // For stoull
#include <math.h>
#include <stdio.h>
#include <shellscalingapi.h>
#include <utility>
#include <vector>
#include <iostream>
#include "Common.h"

#pragma comment(lib, "shcore.lib")

using namespace std;

// Write a value of type T into buf, advance buf by sizeof(T)
template<typename T>
inline char* GMSWriteWC(char* buf, const T& val) {
    // Test for current or impending buf ovverflow and return nullptr
    if((buf == nullptr) || ((buf + sizeof(T)) > (buf + rezol_get_buffer_size(WINDOWCHROME)))) {
        return nullptr;
    }
    std::memcpy(buf, &val, sizeof(T));
    return buf + sizeof(T);
}

bool rezol_get_window_chrome(WindowChrome *wc, HWND handle) {
    RECT rc;
    bool res = TRUE;
    
    if(GetWindowRect(handle, &rc) == 0) {
        res = FALSE;
    } else {
        wc->outerRect = RectToGMSRect(rc);
    }
    
    if(GetClientRect(handle, &rc) == 0) {
        res = FALSE;
    } else {
        wc->innerRect = RectToGMSRect(rc);
    }
    
    return res;
}

double screen_ext_get_window_chrome(char* inbuf, char* handle) {
    WindowChrome wc;
    HWND hwnd = (HWND)(uintptr_t)getGMSBuffAddress(handle);
    
    auto res = rezol_get_window_chrome(&wc, hwnd);
    if(res) {
        char* buf = getGMSBuffAddress(inbuf);
        buf = GMSWriteWC(buf, wc.outerRect);
        buf = GMSWriteWC(buf, wc.innerRect);
        buf = GMSWriteWC(buf, wc.fourcc);
        if(buf != nullptr) {
            // buf is good, return 0
            return 0;
        }
    }

    // buf is bad, return 1
    return 1;
}

BOOL __internal_get_window_chrome(WindowChrome* wc, HWND handle) {
    return rezol_get_window_chrome(wc, handle);
}

