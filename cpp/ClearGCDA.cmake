file(GLOB_RECURSE files "${CMAKE_BINARY_DIR}/*.gcda")

if(files)
    file(REMOVE ${files})
endif()
