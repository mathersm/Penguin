# Add an executable
add_executable (Test_Dynamic_Library
    Test_Dynamic_Library.cpp)

# Dependencies
add_dependencies (Test_Dynamic_Library Penguin)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (Test_Dynamic_Library LINK_PUBLIC Penguin)

add_test (
    NAME Test_Dynamic_Library
    COMMAND Test_Dynamic_Library
)