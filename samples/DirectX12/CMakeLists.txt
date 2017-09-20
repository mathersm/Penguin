# Find the include directories and libraries for DirectX12
find_package(DirectX12 REQUIRED)
if (NOT DirectX12_FOUND)
  message (FATAL_ERROR "Failed to find DirectX12!")
endif (NOT DirectX12_FOUND)


# Add an executable
add_executable (Sample_DirectX12
    Sample_DirectX12.cpp)

# Dependencies
add_dependencies (Sample_DirectX12 Penguin)

# Include files
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${DirectX12_INCLUDE_DIRS}
)

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (
  Sample_DirectX12
  LINK_PUBLIC
    Penguin
    ${DirectX12_LIBRARIES}
)

