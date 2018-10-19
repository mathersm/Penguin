# Add an executable
add_executable (Sample_Container
    Sample_Container.cpp
)

# Dependencies
add_dependencies (Sample_Container Penguin)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (Sample_Container LINK_PUBLIC Penguin)




