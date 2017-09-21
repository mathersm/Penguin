
# There can be problems with CMake and finding paths that have spaces in them...
set(ProgFilesX86 "ProgramFiles(x86)")

find_path (
  D3D12_DIR
  NAMES "d3d12.h"
  PATHS "$ENV{${ProgFilesX86}}/Windows Kits/10/Include/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
  PATH_SUFFIXES "shared" "um"
  DOC "D3D12 include directory"
)

find_library (
  D3D12_LIBRARY
  NAMES d3d12
  PATHS "$ENV{${ProgFilesX86}}/Windows Kits/10/Lib/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
  PATH_SUFFIXES "/um/x64"
  DOC "D3D12 library"
)


include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  D3D12
  REQUIRED_VARS
    D3D12_DIR
    D3D12_LIBRARY
)

if (D3D12_FOUND)
  set(
    D3D12_LIBRARIES
    ${D3D12_LIBRARY}
  )
  set(
    D3D12_INCLUDE_DIRS
    ${D3D12_DIR}
  )
endif (D3D12_FOUND)

# Make sure CMake GUIs ignore 'local' variables
mark_as_advanced(
  D3D12_DIR
  D3D12_LIBRARY
)