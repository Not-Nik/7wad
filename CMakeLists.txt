cmake_minimum_required(VERSION 3.25)
project(7wad)

set(CMAKE_CXX_STANDARD 20)

find_package(ZLIB REQUIRED)
add_subdirectory(raylib)

add_library(tinyfiledialogs STATIC tinyfiledialogs/tinyfiledialogs.c tinyfiledialogs/tinyfiledialogs.h)
target_include_directories(tinyfiledialogs PUBLIC tinyfiledialogs)

add_executable(7wad src/main.cpp src/wad/File.h src/wad/File.cpp src/wad/Archive.h src/wad/Archive.cpp)

target_link_libraries(7wad ZLIB::ZLIB raylib tinyfiledialogs)
