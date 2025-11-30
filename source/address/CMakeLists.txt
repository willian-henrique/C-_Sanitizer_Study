# Check if the build type is Debug
if(CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug" )
    # Set the sanitizer flags for Debug builds
    set(SANITIZER_FLAGS "-fsanitize=address")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
endif()

# Add the executable for the files in this folder
add_executable(stack_usage_out_scope stack_usage_out_scope.cpp)