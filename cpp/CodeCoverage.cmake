# Code Coverage Configuration
option(CODE_COVERAGE "Enable coverage reporting" ON)

if(CODE_COVERAGE)
    # Add required flags (GCC & LLVM/Clang)
    target_compile_options(inspector-cpp PRIVATE
        -O0        # no optimization
        -g         # generate debug info
        --coverage # sets all required flags
        -fprofile-instr-generate
        -fcoverage-mapping
    )
    target_link_libraries(inspector-cpp -fprofile-instr-generate -fcoverage-mapping)
endif(CODE_COVERAGE)
