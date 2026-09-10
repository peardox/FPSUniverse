#ifndef WINDOWS_CHROME_H
#define WINDOWS_CHROME_H
#include <windows.h>

#ifdef BUILDING_DLL
    #define CHROME_API __declspec(dllexport)
#else
    #define CHROME_API __declspec(dllimport)
#endif

extern "C" {
    CHROME_API double screen_ext_get_window_chrome(char* buf, char* handle);
}

#endif // WINDOWS_CHROME_H