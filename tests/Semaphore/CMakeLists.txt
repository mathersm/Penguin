# Add an executable
add_executable (Test_Semaphore
    Test_Semaphore.cpp)

# Dependencies
add_dependencies (Test_Semaphore Penguin)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (Test_Semaphore LINK_PUBLIC Penguin)

