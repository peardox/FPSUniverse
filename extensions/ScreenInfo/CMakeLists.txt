# CMakeList.txt : CMake project for GMSVirtualScreen, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 4.0)
project("ScreenInfo"  C CXX)
project("ScreenInfo"  VERSION 0.0.3)

if (POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Add source to this project's executable.

add_subdirectory("src")
