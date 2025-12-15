# Check if the build type is Debug
if(CMAKE_BUILD_TYPE STREQUAL "debug_thread" )
    message("Compiling with sanitizer")
    # Set the sanitizer flags for Debug builds
    set(SANITIZER_FLAGS "-fsanitize=thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
endif()

# Add the executable for the files in this folder
add_executable(concurrency_problem concurrency_problem.cpp)