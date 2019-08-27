# Add an executable
add_executable (
  Test_Timer
  Test_Timer.cpp
)

# Dependencies
add_dependencies (
  Test_Timer
    Penguin
)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (Test_Timer LINK_PUBLIC Penguin)

add_test (
    NAME Test_Timer
    COMMAND Test_Timer
)
