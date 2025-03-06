cmake_minimum_required(VERSION 3.12)
project(SokoFold)

message("PROJECT_SOURCE_DIR\t${PROJECT_SOURCE_DIR}")
message("CMAKE_SOURCE_DIR\t${CMAKE_SOURCE_DIR}")

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/deps/glfw/include/)
include_directories(${CMAKE_SOURCE_DIR}/deps/entt/src/)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/ sokoFoldSRC)
add_executable(sokoFold sokoFold.cpp ${sokoFoldSRC})
target_link_libraries(sokoFold glad glfw freetype)

