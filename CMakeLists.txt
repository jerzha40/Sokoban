cmake_minimum_required(VERSION 3.12)
project(windowWithSDL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -mwindows")

message("PROJECT_SOURCE_DIR\t${PROJECT_SOURCE_DIR}")
message("CMAKE_SOURCE_DIR\t${CMAKE_SOURCE_DIR}")

# 添加 SDL 子模块
set(SDL_SHARED OFF CACHE BOOL "Build SDL as a static library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL as a static library" FORCE)
set(SDL_DIR "${CMAKE_SOURCE_DIR}/deps/SDL")
add_subdirectory(${SDL_DIR})

# 添加可执行文件
add_executable(windowWithSDL main.cpp)
add_executable(test test.cpp)

# 链接 SDL3
target_link_libraries(windowWithSDL PRIVATE SDL3::SDL3)
target_link_libraries(test PRIVATE SDL3::SDL3)
