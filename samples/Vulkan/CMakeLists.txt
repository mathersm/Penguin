# Find the include directories and libraries for Vulkan
find_package(Vulkan REQUIRED)
if (NOT Vulkan_FOUND)
  message (FATAL_ERROR "Failed to find Vulkan!")
endif (NOT Vulkan_FOUND)


# Add an executable
add_executable (Sample_Vulkan
    Sample_Vulkan.cpp)

# Dependencies
add_dependencies (Sample_Vulkan Penguin)

# Include files
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${Vulkan_INCLUDE_DIRS}
)

# Link the executable to the Penguin library
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries (
  Sample_Vulkan
  LINK_PUBLIC
    Penguin
    ${Vulkan_LIBRARIES}
)

