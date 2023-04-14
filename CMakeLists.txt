cmake_minimum_required(VERSION 3.25)
project(7wad)

set(CMAKE_CXX_STANDARD 20)
set(ZLIB_USE_STATIC_LIBS ON)

find_package(ZLIB REQUIRED)
add_subdirectory(raylib)

add_library(tinyfiledialogs STATIC tinyfiledialogs/tinyfiledialogs.c tinyfiledialogs/tinyfiledialogs.h)
target_include_directories(tinyfiledialogs PUBLIC tinyfiledialogs)

set(SOURCES src/main.cpp src/wad/File.h src/wad/File.cpp src/wad/Archive.h src/wad/Archive.cpp)

if (WIN32 AND MSVC)
    set(SOURCES ${SOURCES} 7wad.rc)
endif()

add_executable(7wad ${SOURCES})

target_link_libraries(7wad ZLIB::ZLIB raylib tinyfiledialogs)

if (WIN32)
    if (MINGW)
        target_link_options(7wad PRIVATE -mwindows)
    elseif (MSVC)
        target_link_options(7wad PRIVATE /SUBSYSTEM:WINDOWS /NODEFAULTLIB:library)
    endif ()
endif ()
