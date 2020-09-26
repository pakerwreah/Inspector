cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(src ../../../../../../cpp/src)

include_directories(${src} ${src}/utils ${src}/libs)

file(GLOB_RECURSE sources ${src}/*.cpp ${src}/*.c)

add_library(inspector SHARED bridge.cpp ${sources})

set(LIBSQLITE ${PROJECT_SOURCE_DIR}/../sqlcipher/${ANDROID_ABI}/libsqlcipher-inspector.so)

target_link_libraries(inspector ${LIBSQLITE} z)
