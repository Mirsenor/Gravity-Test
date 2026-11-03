cmake_minimum_required(VERSION 3.16)
project(GravityTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(GravityTest WIN32 src/main.cpp)

target_link_libraries(GravityTest PRIVATE SDL3::SDL3)