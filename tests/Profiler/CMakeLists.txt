# Add an executable
add_executable (Test_Profiler
    Test_Profiler.cpp)

# Dependencies
add_dependencies (Test_Profiler Penguin)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (Test_Profiler LINK_PUBLIC Penguin)

