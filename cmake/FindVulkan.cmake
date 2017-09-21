
find_path (
  Vulkan_DIR
  NAMES "vulkan/vulkan.h"
  PATHS "$ENV{VK_SDK_PATH}/Include"
  DOC "Vulkan vulkan/vulkan.h include directory"
)

message("Vulkan include directories = " "${Vulkan_DIR}")

find_library (
  Vulkan_LIBRARY
  NAMES vulkan-1
  PATHS "$ENV{VK_SDK_PATH}/Bin"
  DOC "Vulkan library"
)

message("Vulkan libraries = " "${Vulkan_LIBRARY}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Vulkan
  FOUND_VAR Vulkan_FOUND
  REQUIRED_VARS
    Vulkan_DIR
    Vulkan_LIBRARY
)

if (Vulkan_FOUND)
  set(
    Vulkan_LIBRARIES
    ${Vulkan_LIBRARY}
  )
  set(
    Vulkan_INCLUDE_DIRS
    ${Vulkan_DIR}
  )
endif (Vulkan_FOUND)

# Make sure CMake GUIs ignore 'local' variables
mark_as_advanced(
  Vulkan_DIR
  Vulkan_LIBRARY
)