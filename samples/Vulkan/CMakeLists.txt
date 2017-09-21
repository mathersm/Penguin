# Find the include directories and libraries for Vulkan
find_package(Vulkan)

if (Vulkan_FOUND)

  add_executable (Sample_Vulkan
    Sample_Vulkan.cpp)

  add_dependencies (Sample_Vulkan Penguin)

  include_directories(
    ${CMAKE_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
  )

  link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  target_link_libraries (
    Sample_Vulkan
    LINK_PUBLIC
      Penguin
      ${Vulkan_LIBRARIES}
  )
else (Vulkan_FOUND)
  message (FATAL_ERROR "Failed to find Vulkan!")
endif (Vulkan_FOUND)




