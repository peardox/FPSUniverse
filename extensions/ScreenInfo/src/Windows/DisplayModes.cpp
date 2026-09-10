#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <vector>
#include <set>
#include "Screens.h"
#include "Common.h"
#include "DisplayModes.h"

// Alternative function that returns the data instead of printing
std::vector<DisplayMode> GetScreenResolutionsAndRefreshRates(LPCSTR DeviceName) {
    DEVMODE devMode;
    devMode.dmSize = sizeof(DEVMODE);
    DWORD modeIndex = 0;
    std::set<DisplayMode> uniqueModes;
    
    while (EnumDisplaySettings(DeviceName, modeIndex, &devMode)) {
        DisplayMode mode;
        mode.width = devMode.dmPelsWidth;
        mode.height = devMode.dmPelsHeight;
        mode.frequency = devMode.dmDisplayFrequency;
        mode.bitsPerPixel = devMode.dmBitsPerPel;
        
        uniqueModes.insert(mode);
        modeIndex++;
    }
    
    std::vector<DisplayMode> deviceModes(uniqueModes.begin(), uniqueModes.end());
    return deviceModes;
}

#ifdef BUILDING_DLL
template<typename T>
inline char* GMSWrite(char* buf, const T& val, size_t forced_len = sizeof(T)) {
    // Test for current or impending buf ovverflow and return nullptr
    if((buf == nullptr) || ((buf + forced_len) > (buf + rezol_get_buffer_size(RESOLUTIONINFO)))) {
        return nullptr;
    }
    std::memcpy(buf, &val, forced_len);
    return buf + forced_len;
}

BOOL __internal_get_monitor_resolutions(ResolutionInfo* info, char* disp, int32_t pageNum) {
	auto modes = GetScreenResolutionsAndRefreshRates(disp);
    if (modes.empty() || ((pageNum * MAX_RESOLUTIONS)>modes.size())) {
		return false;
	}	
	size_t startIndex = pageNum * MAX_RESOLUTIONS;
	size_t endIndex = std::min(startIndex + MAX_RESOLUTIONS, modes.size());
    
    info->header.count = endIndex-startIndex;
    info->header.maxCount = MAX_RESOLUTIONS;
	info->header.fromResolution = startIndex;
    info->header.pageNum = pageNum;
    if(endIndex > modes.size()) {
        info->header.more = 1;
    } else {
        info->header.more = 0;
    }
    
	for (size_t i = 0; i < endIndex-startIndex; ++i) {
		const auto& mode = modes[i+startIndex];
		info->res[i].width = mode.width;
		info->res[i].height = mode.height;
		info->res[i].frequency = mode.frequency;
		info->res[i].bitsPerPixel = mode.bitsPerPixel;
	}

	return true;
}

double screen_ext_get_monitor_resolutions(char* inbuf, char* disp, double pageNum) {
    char* buf = getGMSBuffAddress(inbuf);//Interpret the string address form GMS so it can be managed by C++
    ResolutionInfo info = {};
    
//    info.fourcc = GMEX;
    
    // Call the function from the DLL
    if(__internal_get_monitor_resolutions(&info, disp, pageNum)) {
        buf = GMSWrite(buf, info.header.count);
        buf = GMSWrite(buf, info.header.maxCount);
        buf = GMSWrite(buf, info.header.fromResolution);
        buf = GMSWrite(buf, info.header.pageNum);
        buf = GMSWrite(buf, info.header.more);
        buf = GMSWrite(buf, info.header.reserved, RESOLUTION_EXPANSION_SPACE);
        for (size_t i = 0; i < info.header.count; ++i) {
            buf = GMSWrite(buf, info.res[i].width);
            buf = GMSWrite(buf, info.res[i].height);
            buf = GMSWrite(buf, info.res[i].frequency);
            buf = GMSWrite(buf, info.res[i].bitsPerPixel);
        }
        if (info.header.count < MAX_RESOLUTIONS) {
            DisplayMode empty = {};
            for(int i = info.header.count; i < MAX_RESOLUTIONS; i++) {
                buf = GMSWrite(buf, empty);
            }
        }
        buf = GMSWrite(buf, info.fourcc);
        // buf will be a nullptr if overflow occurred
        if(buf != nullptr) {
            return 0;
        }
        
        return 0;
    }

    return 1;
}
#else

int main() {
	auto modes = GetScreenResolutionsAndRefreshRates("\\\\.\\DISPLAY1");
//    auto modes = GetScreenResolutionsAndRefreshRates("\\\\.\\DISPLAY2");
    
    if (modes.empty()) {
        std::cout << "No display modes found for the specified device.\n";
        return 1;
    }
    
    std::cout << "Found " << modes.size() << " display modes:\n\n";
    
    const size_t pageSize = 10;
    size_t totalPages = (modes.size() + pageSize - 1) / pageSize; // Ceiling division
    
    for (size_t page = 0; page < totalPages; ++page) {
        size_t startIndex = page * pageSize;
        size_t endIndex = std::min(startIndex + pageSize, modes.size());
        
        std::cout << "Page " << (page + 1) << " of " << totalPages 
                  << " (showing " << (startIndex + 1) << "-" << endIndex 
                  << " of " << modes.size() << "):\n";
        std::cout << "----------------------------------------\n";
        
        for (size_t i = startIndex; i < endIndex; ++i) {
            const auto& mode = modes[i];
            std::cout << (i + 1) << ". Resolution: " << mode.width << "x" << mode.height 
                      << " @ " << mode.frequency << "Hz"
                      << " (" << mode.bitsPerPixel << " bits per pixel)\n";
        }
        
    }
    
    return 0;
}
#endif
