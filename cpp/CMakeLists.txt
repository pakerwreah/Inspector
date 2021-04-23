cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(inspector)

include_directories(src src/utils src/libs)

file(GLOB_RECURSE sources src/*.cpp src/*.c)

# Create base object with sources
add_library(inspector-cpp OBJECT ${sources})
target_compile_options(inspector-cpp PRIVATE -pedantic-errors -Werror -Wall -Wextra -Wno-reorder-ctor -Wno-sign-compare)

# Configure code coverage
include(CodeCoverage.cmake)
target_link_libraries(inspector-cpp coverage_config)

# Link with system libraries
find_library(LIBSQLITE sqlcipher)
find_package(Threads REQUIRED)
target_link_libraries(inspector-cpp ${LIBSQLITE} Threads::Threads z)

# Create demo application
file(GLOB_RECURSE ext ext/*.cpp)
add_executable(inspector-demo main.cpp ${ext})
target_include_directories(inspector-demo PRIVATE ext/utils)
target_link_libraries(inspector-demo inspector-cpp)

# Create tests runner
file(GLOB_RECURSE tests tests/*.cpp)
add_executable(inspector-tests ${tests})
target_include_directories(inspector-tests PRIVATE tests/utils)
target_link_libraries(inspector-tests inspector-cpp)
add_custom_target(test inspector-tests)
