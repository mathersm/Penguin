# Create a library
add_library (Penguin SHARED
    Exception.cpp
    Exception.h
    Language.h
    Monitor.cpp
    Monitor.h
    Penguin_export.h
    Profiler.cpp
    Profiler.h
    Semaphore.cpp
    Semaphore.h
    Unbounded_Queue.h
    Version.h)


set_target_properties(Penguin PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Penguin PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Penguin PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Penguin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Penguin PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Penguin PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Penguin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Penguin PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(Penguin PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)

include (GenerateExportHeader)
generate_export_header(Penguin
    BASE_NAME Penguin
    EXPORT_MACRO_NAME Penguin_Export
    EXPORT_FILE_NAME Penguin_export.h
    STATIC_DEFINE Penguin_BUILT_AS_STATIC)

if (UNIX)
target_link_libraries(Penguin pthread)
endif (UNIX)