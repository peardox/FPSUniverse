# Set the minimum required version of CMake.
cmake_minimum_required(VERSION 3.15)

# Set the C++ standard to C++17 for modern features.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This project uses the Windows API, so it will only build on Windows.
if(NOT WIN32)
  message(FATAL_ERROR "This project is Windows-only.")
endif()

set(DLLName "ScreenInfo_x64")

# Place all runtime executables (.exe) and shared libraries (.dll)
# into a 'bin' subdirectory inside the build folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # For .lib files

# --- 1. Define the Shared Library (DLL) ---

# Create a library target named ${DLLName} from its source files.
# The 'SHARED' keyword specifies that we are building a DLL.
set(DLL_SOURCES 
    Screens.cpp 
    WindowsChrome.cpp 
    DisplayModes.cpp 
    Common.cpp 
    )

add_library(${DLLName} SHARED ${DLL_SOURCES})
#  WindowEventCapture.cpp
#  WindowEventCapture.h

#  Screens.h
#  WindowsChrome.h
#  DisplayModes.h
#  Common.h

# Test code
# add_executable(dmodes DisplayModes.cpp)
# target_compile_definitions(dmodes PRIVATE STANDALONE)

# Define the include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add the preprocessor definition needed to export symbols from the DLL.
# This definition is PRIVATE, meaning it only applies when compiling
# 'screen_utils' itself. Any target that links to this library will NOT
# inherit this definition, which is exactly what we need for the
# __declspec(dllimport) macro to work correctly in the executable.
target_compile_definitions(${DLLName} PRIVATE BUILDING_DLL)

set_target_properties(${DLLName} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Link the library against the Windows User32 library, which is required
# for the EnumDisplayMonitors function.
target_link_libraries(${DLLName} PUBLIC user32)

