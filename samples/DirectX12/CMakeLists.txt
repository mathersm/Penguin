find_package(D3D12)
find_package(DXGI)

if (D3D12_FOUND AND DXGI_FOUND)

  add_executable (Sample_DirectX12
    Sample_DirectX12.cpp)

  add_dependencies (Sample_DirectX12 Penguin)

  include_directories(
    ${CMAKE_SOURCE_DIR}
    ${D3D12_INCLUDE_DIRS}
    ${DXGI_INCLUDE_DIRS}
  )

  link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  target_link_libraries (
    Sample_DirectX12
    LINK_PUBLIC
      Penguin
      ${D3D12_LIBRARIES}
      ${DXGI_LIBRARIES}
  )

else (D3D12_FOUND AND DXGI_FOUND)
  message (WARNING "Failed to find DXGI and/or D3D12!")
endif (D3D12_FOUND AND DXGI_FOUND)


