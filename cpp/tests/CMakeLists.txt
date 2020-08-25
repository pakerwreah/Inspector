cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(inspector-tests)

option(UNIT_TEST ON)

add_subdirectory(.. inspector-build)

include_directories(../src ../src/utils ../src/libs)

file(GLOB tests *.cpp)

add_executable(inspector-tests ${tests})

target_link_libraries(inspector-tests inspector-cpp)
