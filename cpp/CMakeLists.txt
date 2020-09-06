if (DEFINED ANDROID_NDK)
    cmake_minimum_required(VERSION 3.10.2)
else ()
    cmake_minimum_required(VERSION 3.13.0)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(inspector)

include_directories(src src/utils src/libs)

file(GLOB_RECURSE sources src/*.cpp src/*.c)

add_library(inspector-cpp STATIC ${sources})

target_link_libraries(inspector-cpp z)

if (NOT DEFINED ANDROID_NDK)
    include(CodeCoverage.cmake)

    find_library(LIBSQLITE sqlcipher)
    find_package(Threads REQUIRED)

    target_link_libraries(inspector-cpp ${LIBSQLITE} Threads::Threads)

    add_executable(inspector-demo main.cpp ext/explorer/explorer.cpp)
    target_link_libraries(inspector-demo inspector-cpp)

    file(GLOB tests tests/*.cpp)
    add_executable(inspector-tests ${tests})
    target_link_libraries(inspector-tests inspector-cpp)
    add_custom_target(test inspector-tests)
endif ()
