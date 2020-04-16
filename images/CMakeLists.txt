# cmake_minimum_required(VERSION <specify CMake version here>)
project(images C)

set(CMAKE_C_STANDARD 99)

add_executable(images main.c
        mario.bmp
        parrots.bmp
        parrots2.bmp
        parrot2.bmp)