
find_path (
  Vulkan_DIR
  NAMES "vulkan/vulkan.h"
  PATHS "$ENV{VK_SDK_PATH}/Include"
  DOC "Vulkan vulkan/vulkan.h include directory"
)

find_library (
  Vulkan_LIBRARY
  NAMES vulkan-1
  PATHS "$ENV{VK_SDK_PATH}/Bin"
  DOC "Vulkan library"
)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Vulkan
  FOUND_VAR Vulkan_FOUND
  REQUIRED_VARS
    Vulkan_DIR
    Vulkan_LIBRARY
  FAIL_MESSAGE "Vulkan SDK not found"
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