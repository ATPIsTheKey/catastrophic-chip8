cmake_minimum_required(VERSION 3.15)
project(catastrophic_chip8 C)

set(CMAKE_C_STANDARD 99)

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_C_FLAGS "-Wall --pedantic -g -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-all -Werror=format-security -Werror=implicit-function-declaration")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(catastrophic_chip8 main.c
        src/vm.c src/vm.h
        rf/mystdlib.c rf/mystdlib.h
        src/instructions.c src/instructions.h
        src/debug.c src/debug.h
        libs/argtable3.c libs/argtable3.h
        )

target_link_libraries(catastrophic_chip8 ${SDL2_LIBRARIES} m)
#target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)