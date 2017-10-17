cmake_minimum_required(VERSION 3.2)
project(flogsim)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB FLOGSIM_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(flogsim ${FLOGSIM_SRC})
