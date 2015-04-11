cmake_minimum_required(VERSION 2.8)

project(spookyhash)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast -fomit-frame-pointer -flto -std=c99")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -flto")

add_library (
    spookyhash
    src/context.c
    src/spookyhash.c
)