# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(inspector)

set(
        sources
        src/sqlite3.c
        src/picohttpparser.c
        src/util.cpp
        src/Socket.cpp
        src/HttpServer.cpp
        src/Inspector.cpp
        src/Database.cpp
        src/DatabasePlugin.cpp
)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        inspector-cpp

        # Sets the library as a shared library.
        STATIC

        # Provides a relative path to your source file(s).
        ${sources})

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

if (NOT DEFINED ANDROID_NDK)
    add_executable(inspector-test src/main.cpp)

    target_link_libraries(inspector-test inspector-cpp z)
endif ()