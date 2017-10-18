cmake_minimum_required(VERSION 3.2)
project(flogsim)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.52 COMPONENTS program_options REQUIRED)

file(GLOB FLOGSIM_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(flogsim ${FLOGSIM_SRC})
target_link_libraries(flogsim ${Boost_PROGRAM_OPTIONS_LIBRARY})

install(DIRECTORY script DESTINATION ${PROJECT_BINARY_BIN}/bin
  FILES_MATCHING PATTERN "*.R")

install(TARGETS flogsim RUNTIME DESTINATION ${PROJECT_BINARY_BIN}/bin)
