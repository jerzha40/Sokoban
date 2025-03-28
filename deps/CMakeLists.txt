cmake_minimum_required(VERSION 3.12)
project(deps)
message("PROJECT_SOURCE_DIR\t${PROJECT_SOURCE_DIR}")
message("CMAKE_SOURCE_DIR\t${CMAKE_SOURCE_DIR}")

set(GLAD_SOURCES_DIR "${PROJECT_SOURCE_DIR}/glad")
add_subdirectory("${GLAD_SOURCES_DIR}/cmake" glad_cmake)
glad_add_library(glad REPRODUCIBLE LOADER API egl=1.5 gles2=2.0)
