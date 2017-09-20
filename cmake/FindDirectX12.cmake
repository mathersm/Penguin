
# There can be problems with CMake and finding paths that have spaces in them...
set(ProgFilesX86 "ProgramFiles(x86)")

find_path (
  DirectX12_D3D12_DIR
  NAMES "d3d12.h"
  PATHS "$ENV{${ProgFilesX86}}/Windows Kits/10/Include/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
  PATH_SUFFIXES "shared" "um"
  DOC "DirectX12 d3d12.h include directory"
)

find_path (
  DirectX12_DXGI_DIR
  NAMES "dxgi.h"
  PATHS "$ENV{${ProgFilesX86}}/Windows Kits/10/Include/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
  PATH_SUFFIXES "shared" "um"
  DOC "DirectX12 dxgi.h include directory"
)

message("DirectX12 include directories = " "${DirectX12_D3D12_DIR}" " and " "${DirectX12_DXGI_DIR}")


find_library (
  DirectX12_D3D12_LIBRARY
  NAMES d3d12
  PATHS "$ENV{${ProgFilesX86}}/Windows Kits/10/Lib/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
  PATH_SUFFIXES "/um/x64"
  DOC "DirectX12 D3D12 library"
)

find_library (
  DirectX12_DXGI_LIBRARY
  NAMES dxgi
  PATHS "$ENV{${ProgFilesX86}}/Windows Kits/10/Lib/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
  PATH_SUFFIXES "/um/x64"
  DOC "DirectX12 DXGI library"
)

message("DirectX12 libraries = " "${DirectX12_D3D12_LIBRARY}" " and " "${DirectX12_DXGI_LIBRARY}")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  DirectX12
  FOUND_VAR DirectX12_FOUND
  REQUIRED_VARS
    DirectX12_D3D12_DIR
    DirectX12_DXGI_DIR
    DirectX12_D3D12_LIBRARY
    DirectX12_DXGI_LIBRARY
)

if (DirectX12_FOUND)
  set(
    DirectX12_LIBRARIES
    ${DirectX12_D3D12_LIBRARY}
    ${DirectX12_DXGI_LIBRARY}
  )
  set(
    DirectX12_INCLUDE_DIRS
    ${DirectX12_D3D12_DIR}
    ${DirectX12_DXGI_DIR}
  )
endif (DirectX12_FOUND)

# Make sure CMake GUIs ignore 'local' variables
mark_as_advanced(
  DirectX12_D3D12_DIR
  DirectX12_DXGI_DIR
  DirectX12_D3D12_LIBRARY
  DirectX12_DXGI_LIBRARY
)