cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(inspector)

set(
        sources
        src/libs/picohttpparser.c
        src/libs/base64.cpp
        src/libs/sha1.cpp
        src/libs/url.cpp
        src/util.cpp
        src/Socket.cpp
        src/HttpServer.cpp
        src/Inspector.cpp
        src/Database.cpp
        src/DatabasePlugin.cpp
        src/NetworkPlugin.cpp
        src/CustomPlugin.cpp)


add_library(inspector-cpp STATIC ${sources})

target_link_libraries(inspector-cpp z)


if (NOT DEFINED ANDROID_NDK)
    find_library(LIBSQLITE sqlcipher)
    find_package(Threads REQUIRED)

    target_link_libraries(inspector-cpp ${LIBSQLITE} Threads::Threads)

    if (NOT DEFINED UNIT_TEST)
        add_executable(inspector-demo src/main.cpp)
        target_link_libraries(inspector-demo inspector-cpp)
    endif ()
endif ()
