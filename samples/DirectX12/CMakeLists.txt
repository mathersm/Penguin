find_package(DirectX12 REQUIRED)
if (DirectX12_FOUND)

  add_executable (Sample_DirectX12
    Sample_DirectX12.cpp)

  add_dependencies (Sample_DirectX12 Penguin)

  include_directories(
    ${CMAKE_SOURCE_DIR}
    ${DirectX12_INCLUDE_DIRS}
  )

  link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  target_link_libraries (
    Sample_DirectX12
    LINK_PUBLIC
      Penguin
      ${DirectX12_LIBRARIES}
  )

else (DirectX12_FOUND)
  message (FATAL_ERROR "Failed to find DirectX12!")
endif (DirectX12_FOUND)


