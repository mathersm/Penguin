# Add an executable
add_executable (Test_Component_Container
    Test_Component_Container.cpp)

# Dependencies
add_dependencies (Test_Component_Container Penguin)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (Test_Component_Container LINK_PUBLIC Penguin)

add_test (
    NAME Test_Component_Container
    COMMAND Test_Component_Container
)