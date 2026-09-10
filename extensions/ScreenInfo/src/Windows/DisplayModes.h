#ifndef DISPLAY_MODES_H
#define DISPLAY_MODES_H
#include <windows.h>

#ifdef BUILDING_DLL
    #define RES_API __declspec(dllexport)
#else
    #define RES_API __declspec(dllimport)
#endif

extern "C" {
    RES_API double screen_ext_get_monitor_resolutions(char* inbuf, char* disp, double pageNum);
}

#endif // COMMON_H