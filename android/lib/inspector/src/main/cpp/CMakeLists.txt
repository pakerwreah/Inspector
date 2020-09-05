cmake_minimum_required(VERSION 3.10.2)

set(sources ../../../../../../cpp)

add_subdirectory(${sources} build)

include_directories(
        ${sources}/src
        ${sources}/src/utils
        ${sources}/src/libs
)

add_library(inspector SHARED bridge.cpp)

# Searches for a specified public NDK library
find_library(log-lib log)

set(LIBSQLITE ${PROJECT_SOURCE_DIR}/../sqlcipher/${ANDROID_ABI}/libsqlcipher-inspector.so)

target_link_libraries(
        inspector
        inspector-cpp
        ${LIBSQLITE}
        ${log-lib}
)
