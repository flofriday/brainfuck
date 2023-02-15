cmake_minimum_required(VERSION 3.22)
project(brainfuck)

# Configure the compiler
set(CXX "clang++")
set(CC "clang")
set(CMAKE_CXX_STANDARD 20)

# LLD is not available on Apple Darwin
if (NOT APPLE)
    add_link_options("-fuse-ld=lld")
endif ()

set(CMAKE_CXX_FLAGS "-Ofast -flto -march=native -Wall -W -pedantic")

# Include the source code
add_subdirectory(src)