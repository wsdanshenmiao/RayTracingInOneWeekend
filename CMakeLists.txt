cmake_minimum_required(VERSION 3.10)

project(RayTracing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCE_FILES Src/*.cpp)
file(GLOB_RECURSE HEAD_FILES Src/*.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEAD_FILES})