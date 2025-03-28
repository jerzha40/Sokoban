cmake_minimum_required(VERSION 3.16)
project(SokobanSDL3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -mwindows")

message("PROJECT_SOURCE_DIR\t${PROJECT_SOURCE_DIR}")
message("CMAKE_SOURCE_DIR\t${CMAKE_SOURCE_DIR}")
