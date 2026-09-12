#include "Screens.h"
#include <string> // For stoull
#include <math.h>
#include <stdio.h>
#include <shellscalingapi.h>
#include <utility>
#include <vector>
#include <iostream>
#include "Common.h"
// #include "DisplayModes.h"
// #include "WindowsChrome.h"


#pragma comment(lib, "shcore.lib")

using namespace std;

// Function to get monitor friendly name for a specific HMONITORINFOEX
std::string GetMonitorFriendlyName(MONITORINFOEX monitorInfo, bool& okflag)
{
	okflag = false;
	
    vector<DISPLAYCONFIG_PATH_INFO> paths;
    vector<DISPLAYCONFIG_MODE_INFO> modes;
    UINT32 flags = QDC_ONLY_ACTIVE_PATHS | QDC_VIRTUAL_MODE_AWARE;
    LONG result = ERROR_SUCCESS;

    do
    {
        UINT32 pathCount, modeCount;
        result = GetDisplayConfigBufferSizes(flags, &pathCount, &modeCount);

        if (result != ERROR_SUCCESS)
        {
            return "Unknown Monitor";
        }

        paths.resize(pathCount);
        modes.resize(modeCount);

        result = QueryDisplayConfig(flags, &pathCount, paths.data(), &modeCount, modes.data(), nullptr);

        paths.resize(pathCount);
        modes.resize(modeCount);

    } while (result == ERROR_INSUFFICIENT_BUFFER);

    if (result != ERROR_SUCCESS)
    {
        return "Unknown Monitor";
    }

    // Convert the narrow device name to wide string for comparison
    int wideLength = MultiByteToWideChar(CP_ACP, 0, monitorInfo.szDevice, -1, nullptr, 0);
    if (wideLength <= 0)
    {
        return "Unknown Monitor";
    }
    
    wstring monitorDeviceName(wideLength - 1, L'\0'); // -1 to exclude null terminator
    MultiByteToWideChar(CP_ACP, 0, monitorInfo.szDevice, -1, &monitorDeviceName[0], wideLength);

    // Find the matching path by comparing device names
    for (auto& path : paths)
    {
        // Get the source device name for this path
        DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName = {};
        sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        sourceName.header.size = sizeof(sourceName);
        sourceName.header.adapterId = path.sourceInfo.adapterId;
        sourceName.header.id = path.sourceInfo.id;

        result = DisplayConfigGetDeviceInfo(&sourceName.header);
        if (result == ERROR_SUCCESS)
        {
            // Compare the source device name with our monitor's device name
            if (monitorDeviceName == sourceName.viewGdiDeviceName)
            {
                // Found matching path, now get the target (monitor) friendly name
                DISPLAYCONFIG_TARGET_DEVICE_NAME targetName = {};
                targetName.header.adapterId = path.targetInfo.adapterId;
                targetName.header.id = path.targetInfo.id;
                targetName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
                targetName.header.size = sizeof(targetName);
                
                result = DisplayConfigGetDeviceInfo(&targetName.header);
                if (result != ERROR_SUCCESS)
                {
                    return "Unknown Monitor";
                }

                // Prefer EDID friendly name, fallback to monitor friendly device name
                const wchar_t* nameToUse = nullptr;
                if (targetName.flags.friendlyNameFromEdid && wcslen(targetName.monitorFriendlyDeviceName) > 0)
                {
                    nameToUse = targetName.monitorFriendlyDeviceName;
                }
                else if (wcslen(targetName.monitorFriendlyDeviceName) > 0)
                {
                    nameToUse = targetName.monitorFriendlyDeviceName;
                }

                if (nameToUse == nullptr)
                {
					okflag = true;
                    return "Internal Display";
                }

                // Convert wide string to UTF-8 string
                int utf8Length = WideCharToMultiByte(
                    CP_UTF8, 0, nameToUse, -1, nullptr, 0, nullptr, nullptr
                );

                if (utf8Length <= 0)
                {
                    return "Unknown Monitor";
                }

                string friendlyName(utf8Length - 1, '\0'); // -1 to exclude null terminator
                WideCharToMultiByte(
                    CP_UTF8, 0, nameToUse, -1, 
                    &friendlyName[0], utf8Length, nullptr, nullptr
                );

			    okflag = true;
                return friendlyName;
            }
        }
    }

    return "Unknown Monitor";
}

static BOOL CALLBACK MonitorEnum(
    HMONITOR hMonitor, // Monitor Handle
    HDC hdc, // Unused
    LPRECT lprcMonitor, // Scaled rect of this screen
    LPARAM pData // For passing data around
    ) {
    ScreenInfo* info = reinterpret_cast<ScreenInfo*>(pData);

    if (info->header.count < info->header.fromScreen) {
        info->header.count++;
        return true;
    }
    
    uint32_t screen_index = info->header.count - info->header.fromScreen;
    
    info->screen[screen_index].virtualRect = RectToGMSRect(lprcMonitor);
    info->screen[screen_index].workingRect = { 0,0,0,0 };
    info->screen[screen_index].errorCode = 0;

    info->header.autoHideTaskbar = 0;
    
    MONITORINFOEX monitorInfo; // Used to get Primary + Display Name
  
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    if (GetMonitorInfo(hMonitor, &monitorInfo)) {
		bool nameok;
 		std:string mn = GetMonitorFriendlyName(monitorInfo, nameok);
		if(!nameok) {
            info->screen[screen_index].errorCode |= 8;
		}
		
		/*
        DEVICE_SCALE_FACTOR mScale;
        
        if(GetScaleFactorForMonitor(hMonitor, &mScale)) {
            info->screen[screen_index].errorCode |= 16;
        }
        info->screen[screen_index].scaleFactor = int32_t(actualScale);
        */
        UINT dpiX, dpiY;
        GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        float actualScale = (dpiX / 96.0f) * 100; // 96 DPI = 100% scale
        info->screen[screen_index].scaleFactor = int32_t(actualScale + 0.5);
        
		std::strncpy(info->screen[screen_index].name, mn.c_str(), MONITOR_NAME_BUFFER_SIZE - 1);
		info->screen[screen_index].name[MONITOR_NAME_BUFFER_SIZE - 1] = '\0';
		
		std::strncpy(info->screen[screen_index].device, monitorInfo.szDevice, MONITOR_DEVICE_BUFFER_SIZE - 1);
		info->screen[screen_index].device[MONITOR_NAME_BUFFER_SIZE - 1] = '\0';

        info->screen[screen_index].isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY);
		info->screen[screen_index].workingRect = RectToGMSRect(monitorInfo.rcWork);

        // --- Get Native/Physical Pixel Resolution using EnumDisplaySettingsEx ---
        // This gives the true resolution of the monitor's current display mode.
        DEVMODE devMode;
        devMode.dmSize = sizeof(DEVMODE);
        devMode.dmDriverExtra = 0; // Must be 0 for EnumDisplaySettingsEx

        if (EnumDisplaySettingsEx(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS,
                                  &devMode, 0)) {
            info->screen[screen_index].pixelBox.width   = devMode.dmPelsWidth;
            info->screen[screen_index].pixelBox.height  = devMode.dmPelsHeight;
            info->screen[screen_index].refreshRate      = devMode.dmDisplayFrequency;
			info->screen[screen_index].bitsPerPixel     = devMode.dmBitsPerPel;

            // --- Get physical dimensions (mm) using GetDeviceCaps ---
            HDC hdc = CreateDC(monitorInfo.szDevice, nullptr, nullptr, nullptr);
            if (hdc) {
                int32_t pwidth = GetDeviceCaps(hdc, HORZSIZE); // Physical width in mm
                info->screen[screen_index].physSize.width = pwidth;
                int32_t pheight = GetDeviceCaps(hdc, VERTSIZE); // Physical height in mm
                info->screen[screen_index].physSize.height = pheight;
                info->screen[screen_index].physSize.diagonal = lround(sqrt((pheight * pheight) + (pwidth * pwidth)));
                
                DeleteDC(hdc); // Always release the DC
            } else {
                info->screen[screen_index].errorCode |= 4;
                info->screen[screen_index].physSize = { 0, 0, 0 };
            }


        } else {
            info->screen[screen_index].errorCode |= 2;
            info->screen[screen_index].pixelBox = { 0,0 };
            info->screen[screen_index].refreshRate = 0;
			info->screen[screen_index].bitsPerPixel = 0;
        }
        
    } else {
        info->screen[screen_index].errorCode |= 1;
        info->screen[screen_index].isPrimary = false;
    }
    
    info->header.count++;

    if ((info->header.count - info->header.fromScreen) == info->header.maxCount) {
        info->header.more = true;
        return false;
    }
    
    return true;
}

// Write a value of type T into buf, advance buf by sizeof(T)
template<typename T>
inline char* GMSWrite(char* buf, const T& val, size_t forced_len = sizeof(T)) {
    // Test for current or impending buf ovverflow and return nullptr
    if((buf == nullptr) || ((buf + forced_len) > (buf + rezol_get_buffer_size(SCREENINFO)))) {
        return nullptr;
    }
    std::memcpy(buf, &val, forced_len);
    return buf + forced_len;
}

// Specialize bool so it always writes 1 byte (0 or 1)
inline char* GMSWrite(char* buf, bool val) {
    uint8_t b = val ? 1 : 0;
    // Test for current or impending buf ovverflow and return nullptr
    if((buf == nullptr) || ((buf + sizeof(b)) > (buf + rezol_get_buffer_size(SCREENINFO)))) {
        return nullptr;
    }
    std::memcpy(buf, &b, sizeof(b));
    return buf + sizeof(b);
}

// --- Implementation of Exported Functions ---

BOOL __internal_get_virtual_screens_paged(ScreenInfo* info, uint32_t pageNum) {
    // Initialize the struct to pass to the library function
    info->header.count = 0;
    info->header.maxCount = MAX_SCREENS;
    info->header.fromScreen = pageNum * MAX_SCREENS; 
    info->header.pageNum = pageNum;
    info->header.more = false;
    info->header.versionMajor    = GMSVersionMajor;
    info->header.versionMinor    = GMSVersionMinor;
    info->header.versionBuild    = GMSVersionBuild;
    // Clear reserved
    for (size_t i = 0; i < EXPANSION_SPACE; ++i) {
        info->header.reserved[i] = 0;
    }

    BOOL rval = EnumDisplayMonitors(NULL, NULL, &MonitorEnum, reinterpret_cast<LPARAM>(info));

    info->header.count = info->header.count - info->header.fromScreen;

    return rval;
}

BOOL __internal_get_virtual_screens(ScreenInfo* info) {
    return __internal_get_virtual_screens_paged(info, 0);
}

double get_screen_info(char* inbuf, uint32_t pageNum) {
    PhysicalScreen screenArray[MAX_SCREENS];

    ScreenInfoHeader header = {}; 
    ScreenInfo info;
    
    // Initialize the struct to pass to the library function
    info.header = header;
    info.screen = screenArray;

    char* buf = getGMSBuffAddress(inbuf);//Interpret the string address form GMS so it can be managed by C++
    
    // Call the function from the DLL
    __internal_get_virtual_screens_paged(&info, pageNum);

    buf = GMSWrite(buf, info.header.count);
    buf = GMSWrite(buf, info.header.maxCount);
    buf = GMSWrite(buf, info.header.fromScreen);
    buf = GMSWrite(buf, info.header.pageNum);
    buf = GMSWrite(buf, info.header.autoHideTaskbar);
    buf = GMSWrite(buf, info.header.more);
    buf = GMSWrite(buf, info.header.versionMajor);
    buf = GMSWrite(buf, info.header.versionMinor);
    buf = GMSWrite(buf, info.header.versionBuild);
    buf = GMSWrite(buf, info.header.reserved);
    if(info.header.count) {
        for(int i = 0; i < info.header.count; i++) {
            buf = GMSWrite(buf, info.screen[i].errorCode);
            buf = GMSWrite(buf, info.screen[i].refreshRate);
            buf = GMSWrite(buf, info.screen[i].bitsPerPixel);
            buf = GMSWrite(buf, info.screen[i].isPrimary);
            buf = GMSWrite(buf, info.screen[i].scaleFactor);

            buf = GMSWrite(buf, info.screen[i].pixelBox.width);
            buf = GMSWrite(buf, info.screen[i].pixelBox.height);

            buf = GMSWrite(buf, info.screen[i].virtualRect.left);
            buf = GMSWrite(buf, info.screen[i].virtualRect.top);
            buf = GMSWrite(buf, info.screen[i].virtualRect.right);
            buf = GMSWrite(buf, info.screen[i].virtualRect.bottom);

            buf = GMSWrite(buf, info.screen[i].workingRect.left);
            buf = GMSWrite(buf, info.screen[i].workingRect.top);
            buf = GMSWrite(buf, info.screen[i].workingRect.right);
            buf = GMSWrite(buf, info.screen[i].workingRect.bottom);

            buf = GMSWrite(buf, info.screen[i].physSize.width);
            buf = GMSWrite(buf, info.screen[i].physSize.height);
            buf = GMSWrite(buf, info.screen[i].physSize.diagonal);

            buf = GMSWrite(buf, info.screen[i].name, MONITOR_NAME_BUFFER_SIZE);
            buf = GMSWrite(buf, info.screen[i].device, MONITOR_DEVICE_BUFFER_SIZE);
        }
    }
    if (info.header.count < MAX_SCREENS) {
        PhysicalScreen empty = {};
        for(int i = info.header.count; i < MAX_SCREENS; i++) {
            buf = GMSWrite(buf, empty);
        }
    }
    buf = GMSWrite(buf, info.fourcc);
    // buf will be a nullptr if overflow occurred
    if(buf != nullptr) {
        return 0;
    }

    
    // buf is bad, return 1
    return 1;
}

double screen_ext_get_screen_info(char* inbuf) {
    return get_screen_info(inbuf, 0);
}

double screen_ext_get_screen_info_paged(char* buf, double pageNum) {
    return get_screen_info(buf, pageNum);
}

int32_t __internal_get_dpi_awareness()
{
    PROCESS_DPI_AWARENESS dpiAwareness = PROCESS_DPI_UNAWARE;
    
    HRESULT hr = GetProcessDpiAwareness(GetCurrentProcess(), &dpiAwareness);
    
    if (FAILED(hr))
    {
        return -1; // Default fallback
    }
    
    return dpiAwareness;
}


double screen_ext_get_dpi_awareness() {
    return __internal_get_dpi_awareness();
}
