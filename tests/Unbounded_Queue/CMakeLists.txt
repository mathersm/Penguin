# Add an executable
add_executable (Test_Unbounded_Queue
    Test_Unbounded_Queue.cpp)

# Dependencies
add_dependencies (Test_Unbounded_Queue Penguin)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (Test_Unbounded_Queue LINK_PUBLIC Penguin)

