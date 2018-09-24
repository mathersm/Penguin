# Add an executable
add_executable (Sample_Eclipse_Calculator
    main.cpp)

# Dependencies
add_dependencies (Sample_Eclipse_Calculator Penguin)

# Penguin library
target_include_directories (Sample_Eclipse_Calculator PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries (Sample_Eclipse_Calculator LINK_PUBLIC Penguin)

