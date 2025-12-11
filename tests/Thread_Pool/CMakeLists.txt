# Add an executable
add_executable (Test_Thread_Pool
    Test_Thread_Pool.cpp)

# Dependencies
add_dependencies (Test_Thread_Pool Penguin)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

# Link the executable to the Penguin library
# link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_directories (Test_Thread_Pool PUBLIC ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
target_link_libraries (Test_Thread_Pool LINK_PUBLIC Penguin)


add_test (
    NAME Test_Thread_Pool
    COMMAND Test_Thread_Pool
)
