cmake_minimum_required(VERSION 3.12)
project(InfermonSAGA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -mwindows")

message("PROJECT_SOURCE_DIR\t${PROJECT_SOURCE_DIR}")
message("CMAKE_SOURCE_DIR\t${CMAKE_SOURCE_DIR}")

add_subdirectory(deps)
# add_subdirectory(features/topologyMap)

include_directories(${PROJECT_SOURCE_DIR}/deps/imgui/)
include_directories(${PROJECT_SOURCE_DIR}/deps/imgui/backends)
aux_source_directory(${PROJECT_SOURCE_DIR}/deps/imgui/ imghcpp)
set(backends
    ${PROJECT_SOURCE_DIR}/deps/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/backends/imgui_impl_opengl3.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src/)
include_directories(${PROJECT_SOURCE_DIR}/deps/glm/)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/ SokobanSRC)
add_executable(Sokoban src/main.cpp ${SokobanSRC})
target_link_libraries(Sokoban glad glfw imgui)

