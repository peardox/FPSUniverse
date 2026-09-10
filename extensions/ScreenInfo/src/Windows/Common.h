#ifndef COMMON_H
#define COMMON_H
#include <windows.h>

#ifdef BUILDING_DLL
    #define COMMON_API __declspec(dllexport)
#else
    #define COMMON_API __declspec(dllimport)
#endif


GMSRect RectToGMSRect(LPRECT lprcMonitor);
GMSRect RectToGMSRect(RECT rcMonitor);
char* getGMSBuffAddress(char* _GMSBuffPtrStr);
size_t rezol_get_buffer_size(int32_t which);

extern "C" {
    COMMON_API double screen_ext_get_buffer_size(double which);
}

#endif // COMMON_H