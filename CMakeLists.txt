cmake_minimum_required(VERSION 3.10)
project(tile-treasure)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")

include_directories(lib/include)

if (WIN32)
    set(RAYLIB_LIB lib/windows/raylib.lib)
elseif (APPLE)
    set(RAYLIB_LIB lib/macos/libraylib.a)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(COREVIDEO CoreVideo)
    find_library(OPENGL OpenGL)
    set(EXTRA_LIBS ${COCOA} ${IOKIT} ${COREVIDEO} ${OPENGL})
elseif (UNIX AND NOT APPLE)
    set(RAYLIB_LIB lib/linux/libraylib.a)
    set(EXTRA_LIBS m pthread dl rt X11 GL)
endif()

add_executable(tile-treasure ${SOURCES})
target_link_libraries(tile-treasure ${RAYLIB_LIB} ${EXTRA_LIBS})
