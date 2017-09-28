# Add an executable
add_executable (Test_Monitor
    Test_Monitor.cpp)

# Dependencies
add_dependencies (Test_Monitor Penguin)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (Test_Monitor LINK_PUBLIC Penguin)

add_test (
    NAME Test_Monitor
    COMMAND Test_Monitor
)
