cmake_minimum_required(VERSION 3.25)
project(7wad)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(zlib)

add_executable(7wad src/main.cpp src/wad/File.h src/wad/File.cpp src/wad/Archive.h src/wad/Archive.cpp)

target_link_libraries(7wad zlib)
