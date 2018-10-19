# Create a library
add_library (Test_Component SHARED
    Test_Component.cpp
    Test_Component.h
    Test_Component_export.h
)

set_target_properties(Test_Component PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Test_Component PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Test_Component PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Test_Component PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Test_Component PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Test_Component PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Test_Component PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Test_Component PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Test_Component PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)

include (GenerateExportHeader)
generate_export_header(Test_Component
    BASE_NAME Test_Component
    EXPORT_MACRO_NAME Test_Component_Export
    EXPORT_FILE_NAME Test_Component_export.h
    STATIC_DEFINE Test_Component_BUILT_AS_STATIC)

# Include files
include_directories(${CMAKE_SOURCE_DIR})

target_link_libraries(
    Test_Component
    LINK_PUBLIC
      Penguin
)
