#include "Screens.h"
#include "Common.h"
#include <string> // For stoull
#include <math.h>
#include <stdio.h>
#include <shellscalingapi.h>
#include <utility>
#include <vector>
#include <iostream>

using namespace std;

// Helper functions

GMSRect RectToGMSRect(LPRECT lprcMonitor) {
    GMSRect rect;
    rect.left = lprcMonitor->left;
    rect.top = lprcMonitor->top;
    rect.right = lprcMonitor->right;
    rect.bottom = lprcMonitor->bottom;
    return rect;
}

GMSRect RectToGMSRect(RECT rcMonitor) {
    GMSRect rect;
    rect.left = rcMonitor.left;
    rect.top = rcMonitor.top;
    rect.right = rcMonitor.right;
    rect.bottom = rcMonitor.bottom;
    return rect;
}

size_t rezol_get_buffer_size(int32_t which) {
    size_t buff_size;
    
    buff_size = 0;
    
    switch(which) {
        case SCREENINFOHEADER:
            buff_size = sizeof(ScreenInfoHeader);
            break;
        case SCREENINFOHEADERRESERVED:
            buff_size = EXPANSION_SPACE;
            break;
        case SCREENINFO:
            buff_size = sizeof(ScreenInfoHeader) + (sizeof(PhysicalScreen) * MAX_SCREENS) + sizeof(uint32_t);
            break;
        case PHYSICALSCREEN:
            buff_size = sizeof(PhysicalScreen);
            break;
        case WINDOWCHROME:
            buff_size = sizeof(WindowChrome);
            break;
		case MONITORNAMEBUFFER:
            buff_size = MONITOR_NAME_BUFFER_SIZE;
            break;
		case MONITORDEVICEBUFFER:
            buff_size = MONITOR_DEVICE_BUFFER_SIZE;
            break;
		case RESOLUTIONHEADER:
			buff_size = sizeof(ResolutionInfoHeader);
			break;
		case RESOLUTIONHEADERRESERVED:
			buff_size = RESOLUTION_EXPANSION_SPACE;
			break;
		case RESOLUTIONINFO:
			buff_size = sizeof(ResolutionInfo);
			break;
        case DISPLAYMODE:
            buff_size = sizeof(DisplayMode);
            break;

    }
    
    return buff_size;
}

double screen_ext_get_buffer_size(double which) {
    return rezol_get_buffer_size(which);
}

char* getGMSBuffAddress(char* _GMSBuffPtrStr) {
    /*
        @description    Converts a GMS buffer address string to a usable pointer in C++.
        @params         {char*} _GMSBuffPtrStr - The ptr to a GMS buffer as a string.
        @return         {char*} The pointer to the buffer. Now functions like memcpy will work.
    */
    size_t GMSBuffLongPointer = stoull(_GMSBuffPtrStr, NULL, 16);//Gets the ptr string into and int64_t.
    return (char*)GMSBuffLongPointer;//Casts the int64_t pointer to char* and returns it so the buffer can be now operated in C++.
}

