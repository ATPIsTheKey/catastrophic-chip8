cmake_minimum_required(VERSION 3.15)
project(catastrophic_chip8 C)

set(CMAKE_C_STANDARD 99)

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(catastrophic_chip8 main.c
        catastrophic-chip8/vm.h catastrophic-chip8/vm.c
        rf/stdutils.c rf/stdutils.h catastrophic-chip8/instructions.c catastrophic-chip8/instructions.h)
target_link_libraries(catastrophic_chip8 ${SDL2_LIBRARIES})