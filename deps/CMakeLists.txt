cmake_minimum_required(VERSION 3.12)
project(deps)

message("PROJECT_SOURCE_DIR\t${PROJECT_SOURCE_DIR}")
message("CMAKE_SOURCE_DIR\t${CMAKE_SOURCE_DIR}")

set(GLAD_SOURCES_DIR "${PROJECT_SOURCE_DIR}/glad")
add_subdirectory("${GLAD_SOURCES_DIR}/cmake" glad_cmake)
glad_add_library(glad REPRODUCIBLE MX API gl:core=4.3)

add_subdirectory(glfw)
include_directories(glfw/include)

set(SDL_TEST OFF)
add_subdirectory(SDL)

include_directories(${PROJECT_SOURCE_DIR}/imgui/)
include_directories(${PROJECT_SOURCE_DIR}/imgui/backends)
aux_source_directory(${PROJECT_SOURCE_DIR}/imgui/ imghcpp)
set(backends
    ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${backends} ${imghcpp})

add_executable(depTes depsTest.cpp ${backends} ${imghcpp})
target_link_libraries(depTes glad glfw)

