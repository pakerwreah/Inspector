# Code Coverage Configuration
add_library(coverage_config INTERFACE)

option(CODE_COVERAGE "Enable coverage reporting" ON)

if(CODE_COVERAGE)
    file(GLOB coverage_exclude main.cpp tests/* ext/* src/libs/*)
    string(REPLACE ";" "|" coverage_exclude "${coverage_exclude}")

    # Add required flags (GCC & LLVM/Clang)
    target_compile_options(coverage_config INTERFACE
            -O0        # no optimization
            -g         # generate debug info
            --coverage # sets all required flags
            -fprofile-exclude-files=\(${coverage_exclude}\)
            )
    target_link_options(coverage_config INTERFACE --coverage)
endif(CODE_COVERAGE)
