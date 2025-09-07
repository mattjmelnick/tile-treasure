cmake_minimum_required(VERSION 3.10)
project(tile-treasure)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")

add_executable(tile-treasure ${SOURCES})

target_include_directories(tile-treasure PRIVATE ${CMAKE_SOURCE_DIR}/lib)

if (WIN32)
    target_link_libraries(tile-treasure PRIVATE ${CMAKE_SOURCE_DIR}/lib/windows/raylib.lib)
    target_link_libraries(tile-treasure PRIVATE winmm.lib gdi32.lib opengl32.lib)
endif()