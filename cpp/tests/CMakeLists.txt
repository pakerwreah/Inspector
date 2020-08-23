cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(inspector-tests)

option(UNIT_TEST ON)

set(sources ..)

add_subdirectory(${sources} inspector-build)
include_directories("${sources}/src")

file(GLOB tests "*.cpp")

add_executable(inspector-tests ${tests})

target_link_libraries(inspector-tests inspector-cpp)
