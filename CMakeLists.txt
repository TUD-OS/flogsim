cmake_minimum_required(VERSION 3.2)
project(flogsim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.52 COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

file(GLOB FLOGSIM_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB COLLECTIVES_SRC ${PROJECT_SOURCE_DIR}/collectives/*.cpp)

add_executable(flogsim ${FLOGSIM_SRC} ${COLLECTIVES_SRC})
TARGET_LINK_LIBRARIES(flogsim ${Boost_LIBRARIES})

install(DIRECTORY script DESTINATION ${PROJECT_BINARY_BIN}/bin
  FILES_MATCHING PATTERN "*.R")

install(TARGETS flogsim RUNTIME DESTINATION ${PROJECT_BINARY_BIN}/bin)
