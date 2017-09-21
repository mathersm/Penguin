
# There can be problems with CMake and finding paths that have spaces in them...
set(ProgFilesX86 "ProgramFiles(x86)")

find_path (
  DXGI_DIR
  NAMES "dxgi.h"
  PATHS "$ENV{${ProgFilesX86}}/Windows Kits/10/Include/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
  PATH_SUFFIXES "shared" "um"
  DOC "DXGI include directory"
)

find_library (
  DXGI_LIBRARY
  NAMES dxgi
  PATHS "$ENV{${ProgFilesX86}}/Windows Kits/10/Lib/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
  PATH_SUFFIXES "/um/x64"
  DOC "DXGI library"
)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  DXGI
  REQUIRED_VARS
    DXGI_DIR
    DXGI_LIBRARY
)

if (DXGI_FOUND)
  set(
    DXGI_LIBRARIES
    ${DXGI_LIBRARY}
  )
  set(
    DXGI_INCLUDE_DIRS
    ${DXGI_DIR}
  )
endif (DXGI_FOUND)

# Make sure CMake GUIs ignore 'local' variables
mark_as_advanced(
  DXGI_DIR
  DXGI_LIBRARY
)