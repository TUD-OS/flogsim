cmake_minimum_required(VERSION 3.2)
project(flogsim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fconcepts")

find_package(Boost 1.52 COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/collectives)

file(GLOB FLOGSIM_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB FLOGLIB_SRC ${PROJECT_SOURCE_DIR}/lib/*.cpp)
file(GLOB PHASES_SRC ${PROJECT_SOURCE_DIR}/lib/phase/*.cpp)
file(GLOB TOPOLOGY_SRC ${PROJECT_SOURCE_DIR}/lib/topology/*.cpp)

add_library(flog SHARED ${FLOGLIB_SRC} ${PHASES_SRC} ${TOPOLOGY_SRC})

add_executable(flogsim ${FLOGSIM_SRC})
TARGET_LINK_LIBRARIES(flogsim ${Boost_LIBRARIES} flog)

install(DIRECTORY script DESTINATION ${PROJECT_BINARY_BIN}/bin
  FILES_MATCHING PATTERN "*.R")

install(TARGETS flogsim RUNTIME DESTINATION ${PROJECT_BINARY_BIN}/bin)
install(TARGETS flog DESTINATION ${PROJECT_BINARY_BIN}/lib)

option(UNIT_TESTS "Enable compilation of unit tests" OFF)

if(UNIT_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
