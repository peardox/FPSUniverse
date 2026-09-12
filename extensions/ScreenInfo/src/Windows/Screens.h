#ifndef SCREEN_UTILS_H
#define SCREEN_UTILS_H

#include <windows.h>
#include <cstdint> // For int32_t
#include <shellscalingapi.h>

// This macro handles the keywords for exporting from a DLL
// and importing into an executable.
#ifdef BUILDING_DLL
    #define SCREEN_API __declspec(dllexport)
#else
    #define SCREEN_API __declspec(dllimport)
#endif

// Custom type definition used in the struct

constexpr int     MAX_SCREENS = 8;
constexpr int     MAX_RESOLUTIONS = 128;
constexpr uint8_t GMSVersionMajor = 0;
constexpr uint8_t GMSVersionMinor = 1;
constexpr uint8_t GMSVersionBuild = 2;
constexpr uint32_t GMEX = 0x474D4558; // "GMEX"
constexpr size_t MONITOR_NAME_BUFFER_SIZE = 64;
constexpr size_t MONITOR_DEVICE_BUFFER_SIZE = 64;
constexpr size_t EXPANSION_SPACE = 92; // Pad ScreenInfoHeader to 128 bytes
constexpr size_t RESOLUTION_EXPANSION_SPACE = 12; // Pad ScreenInfoHeader to 64 bytes

struct DisplayMode {
    DWORD width;
    DWORD height;
    DWORD frequency;
    DWORD bitsPerPixel;
    
    bool operator<(const DisplayMode& other) const {
        if (width != other.width) return width < other.width;
        if (height != other.height) return height < other.height;
        if (frequency != other.frequency) return frequency < other.frequency;
        return bitsPerPixel < other.bitsPerPixel;
    }
};

extern "C" {

    enum REZOL_DATA_BUFFER {
        SCREENINFOHEADER,
        SCREENINFOHEADERRESERVED,
        SCREENINFO,
        PHYSICALSCREEN,
        WINDOWCHROME,
        MONITORNAMEBUFFER,
        MONITORDEVICEBUFFER,
        RESOLUTIONHEADER,
        RESOLUTIONHEADERRESERVED,
        RESOLUTIONINFO,
        DISPLAYMODE
        
    };

    // Struct definitions that are part of the public API
    struct GMSRect {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;
    };

    struct GMSBox {
        int32_t width;
        int32_t height;
    };

    struct PhysicalSize {
        int32_t width;
        int32_t height;
        int32_t diagonal;
    };

    typedef struct {
        int32_t         errorCode;
        int32_t         refreshRate;
        int32_t         bitsPerPixel;
        int32_t         isPrimary;
        int32_t         scaleFactor;
        GMSBox          pixelBox;
        GMSRect         virtualRect;
        GMSRect         workingRect;
        PhysicalSize    physSize;
        char   			name[MONITOR_NAME_BUFFER_SIZE];
        char   			device[MONITOR_DEVICE_BUFFER_SIZE];
    } PhysicalScreen;

    typedef struct {
        int32_t count;
        int32_t maxCount;
        int32_t fromScreen;
        int32_t pageNum;
        int32_t autoHideTaskbar;
        int32_t more;
        int32_t versionMajor;
        int32_t versionMinor;
        int32_t versionBuild;
        char    reserved[EXPANSION_SPACE];
    } ScreenInfoHeader;
        
    struct ScreenInfo {
        ScreenInfoHeader    header;
        PhysicalScreen*     screen;
        uint32_t fourcc     = GMEX;
    };

    struct ResolutionInfoHeader {
        int32_t    			count;
        int32_t 			maxCount;
        int32_t 			fromResolution;
        int32_t 			pageNum;
        int32_t 			more;
        char    			reserved[RESOLUTION_EXPANSION_SPACE];
    };

    struct ResolutionInfo {
        ResolutionInfoHeader	header;
        DisplayMode     		res[MAX_RESOLUTIONS];
        uint32_t fourcc     	= GMEX;
    };

    struct WindowChrome {
        GMSRect  outerRect;
        GMSRect  innerRect;
        uint32_t fourcc = GMEX;
    };

    SCREEN_API double screen_ext_get_screen_info(char* buf);
    SCREEN_API double screen_ext_get_screen_info_paged(char* buf, double pageNum);
    SCREEN_API double screen_ext_get_dpi_awareness();
}
#endif // SCREEN_UTILS_H
