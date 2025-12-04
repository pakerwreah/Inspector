# Code Coverage Configuration
add_library(coverage_config INTERFACE)

option(CODE_COVERAGE "Enable coverage reporting" ON)

if (CODE_COVERAGE)
    # Add required flags (GCC & LLVM/Clang)
    target_compile_options(coverage_config INTERFACE
            -O0        # no optimization
            -g         # generate debug info
            --coverage # sets all required flags
            -fprofile-instr-generate
            -fcoverage-mapping
            )
    target_link_options(coverage_config INTERFACE -fprofile-instr-generate)

    # Clear old GCDA files
    add_custom_target(clear_gcda COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/ClearGCDA.cmake")
    add_dependencies(coverage_config clear_gcda)
endif (CODE_COVERAGE)
